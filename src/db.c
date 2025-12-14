#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db.h"

// ========== 接続管理 ==========

PGconn* db_connect(void) {
    PGconn *conn = PQconnectdb("dbname=submanager");
    
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "接続失敗: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        return NULL;
    }
    
    // 文字化け対策
    PQexec(conn, "SET client_encoding TO 'UTF8'");
    
    return conn;
}

void db_disconnect(PGconn *conn) {
    if (conn != NULL) {
        PQfinish(conn);
    }
}

// ========== カテゴリ操作 ==========

CategoryList* db_get_categories(PGconn *conn) {
    PGresult *res = PQexec(conn, "SELECT id, name FROM categories ORDER BY id");
    
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        PQclear(res);
        return NULL;
    }
    
    int rows = PQntuples(res);
    
    CategoryList *list = malloc(sizeof(CategoryList));
    if (list == NULL) {
        PQclear(res);
        return NULL;
    }
    
    list->items = malloc(sizeof(Category) * rows);
    if (list->items == NULL) {
        free(list);
        PQclear(res);
        return NULL;
    }
    
    list->count = rows;
    
    for (int i = 0; i < rows; i++) {
        list->items[i].id = atoi(PQgetvalue(res, i, 0));
        strncpy(list->items[i].name, PQgetvalue(res, i, 1), 50);
        list->items[i].name[50] = '\0';
    }
    
    PQclear(res);
    return list;
}

void db_free_category_list(CategoryList *list) {
    if (list == NULL) return;
    if (list->items != NULL) free(list->items);
    free(list);
}

// ========== サブスク取得 ==========

SubscriptionList* db_get_subscriptions(PGconn *conn) {
    const char *sql = 
        "SELECT s.id, s.service_name, s.price, s.billing_cycle, "
        "       s.category_id, c.name, s.next_billing_date, "
        "       COALESCE(s.memo, '') "
        "FROM subscriptions s "
        "JOIN categories c ON s.category_id = c.id "
        "WHERE s.is_active = true "
        "ORDER BY s.next_billing_date";
    
    PGresult *res = PQexec(conn, sql);
    
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        PQclear(res);
        return NULL;
    }
    
    int rows = PQntuples(res);
    
    SubscriptionList *list = malloc(sizeof(SubscriptionList));
    if (list == NULL) {
        PQclear(res);
        return NULL;
    }
    
    list->items = malloc(sizeof(Subscription) * (rows > 0 ? rows : 1));
    if (list->items == NULL) {
        free(list);
        PQclear(res);
        return NULL;
    }
    
    list->count = rows;
    list->capacity = rows > 0 ? rows : 1;
    
    for (int i = 0; i < rows; i++) {
        list->items[i].id = atoi(PQgetvalue(res, i, 0));
        strncpy(list->items[i].service_name, PQgetvalue(res, i, 1), 100);
        list->items[i].price = atoi(PQgetvalue(res, i, 2));
        strncpy(list->items[i].billing_cycle, PQgetvalue(res, i, 3), 10);
        list->items[i].category_id = atoi(PQgetvalue(res, i, 4));
        strncpy(list->items[i].category_name, PQgetvalue(res, i, 5), 50);
        strncpy(list->items[i].next_billing_date, PQgetvalue(res, i, 6), 10);
        strncpy(list->items[i].memo, PQgetvalue(res, i, 7), 255);
        list->items[i].is_active = 1;
    }
    
    PQclear(res);
    return list;
}


Subscription* db_get_subscription_by_id(PGconn *conn, int id) {
    char sql[500];
    snprintf(sql, sizeof(sql),
        "SELECT s.id, s.service_name, s.price, s.billing_cycle, "
        "       s.category_id, c.name, s.next_billing_date, "
        "       COALESCE(s.memo, '') "
        "FROM subscriptions s "
        "JOIN categories c ON s.category_id = c.id "
        "WHERE s.id = %d AND s.is_active = true", id);
    
    PGresult *res = PQexec(conn, sql);
    
    if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
        PQclear(res);
        return NULL;
    }
    
    Subscription *sub = malloc(sizeof(Subscription));
    if (sub == NULL) {
        PQclear(res);
        return NULL;
    }
    
    sub->id = atoi(PQgetvalue(res, 0, 0));
    strncpy(sub->service_name, PQgetvalue(res, 0, 1), 100);
    sub->price = atoi(PQgetvalue(res, 0, 2));
    strncpy(sub->billing_cycle, PQgetvalue(res, 0, 3), 10);
    sub->category_id = atoi(PQgetvalue(res, 0, 4));
    strncpy(sub->category_name, PQgetvalue(res, 0, 5), 50);
    strncpy(sub->next_billing_date, PQgetvalue(res, 0, 6), 10);
    strncpy(sub->memo, PQgetvalue(res, 0, 7), 255);
    sub->is_active = 1;
    
    PQclear(res);
    return sub;
}

void db_free_subscription_list(SubscriptionList *list) {
    if (list == NULL) return;
    if (list->items != NULL) free(list->items);
    free(list);
}

void db_free_subscription(Subscription *sub) {
    if (sub != NULL) free(sub);
}

// ========== サブスク操作 ==========

int db_insert_subscription(PGconn *conn, Subscription *sub) {
    const char *sql = 
        "INSERT INTO subscriptions "
        "(service_name, price, billing_cycle, category_id, next_billing_date, memo) "
        "VALUES ($1, $2, $3, $4, $5, $6) RETURNING id";
    
    char price_str[20];
    char category_str[10];
    snprintf(price_str, sizeof(price_str), "%d", sub->price);
    snprintf(category_str, sizeof(category_str), "%d", sub->category_id);
    
    const char *params[6] = {
        sub->service_name,
        price_str,
        sub->billing_cycle,
        category_str,
        sub->next_billing_date,
        strlen(sub->memo) > 0 ? sub->memo : NULL
    };
    
    PGresult *res = PQexecParams(conn, sql, 6, NULL, params, NULL, NULL, 0);
    
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "INSERT失敗: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return -1;
    }
    
    int new_id = atoi(PQgetvalue(res, 0, 0));
    PQclear(res);
    return new_id;
}

int db_update_subscription(PGconn *conn, Subscription *sub) {
    const char *sql = 
        "UPDATE subscriptions "
        "SET service_name = $1, price = $2, billing_cycle = $3, "
        "    category_id = $4, next_billing_date = $5, memo = $6 "
        "WHERE id = $7";
    
    char price_str[20];
    char category_str[10];
    char id_str[10];
    snprintf(price_str, sizeof(price_str), "%d", sub->price);
    snprintf(category_str, sizeof(category_str), "%d", sub->category_id);
    snprintf(id_str, sizeof(id_str), "%d", sub->id);
    
    const char *params[7] = {
        sub->service_name,
        price_str,
        sub->billing_cycle,
        category_str,
        sub->next_billing_date,
        strlen(sub->memo) > 0 ? sub->memo : NULL,
        id_str
    };
    
    PGresult *res = PQexecParams(conn, sql, 7, NULL, params, NULL, NULL, 0);
    
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "UPDATE失敗: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return -1;
    }
    
    PQclear(res);
    return 0;
}

int db_delete_subscription(PGconn *conn, int id) {
    char sql[100];
    snprintf(sql, sizeof(sql),
             "UPDATE subscriptions SET is_active = false WHERE id = %d", id);
    
    PGresult *res = PQexec(conn, sql);
    
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "DELETE失敗: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return -1;
    }
    
    PQclear(res);
    return 0;
}