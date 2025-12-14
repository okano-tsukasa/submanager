#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>

// 構造体定義

typedef struct {
    int id;
    char service_name[101];
    int price;
    char billing_cycle[11];
    char next_billing_date[11];
    char category_name[51];
    char next_blling[11];
} Subscription;

typedef struct {
    Subscription *items;
    int count;
} SubscriptionList;



// データ取得関数

SubscriptionList* get_subscriptions(PGconn *conn) {
    const char *sql =
        "SELECT "
        "   s.id, s.service_name, s.price, s.billing_cycle, "
        "   c.name, s.next_billing_date "
        "FROM subscriptions s "
        "JOIN categories c ON s.category_id = c.id "
        "WHERE s.is_active = true "
        "ORDER BY s.next_billing_date";

    PGresult *res = PQexec(conn, sql);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        printf("クエリ失敗: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return NULL;
    }

    int rows = PQntuples(res);

    // リスト構造体を確保
    SubscriptionList *list = malloc(sizeof(SubscriptionList));
    if (list == NULL) {
        PQclear(res);
        return NULL;
    }

    // 配列を確保
    list->items = malloc(sizeof(Subscription) *rows);
    if (list->items == NULL) {
        free(list);
        PQclear(res);
        return NULL;
    }

    list->count = rows;

    // データ構造体にコピー
    for (int i = 0; i < rows; i++) {
        list->items[i].id = atoi(PQgetvalue(res, i, 0));
        strncpy(list->items[i].service_name, PQgetvalue(res, i, 1), 100);
        list->items[i].price = atoi(PQgetvalue(res, i, 2));
        strncpy(list->items[i].billing_cycle, PQgetvalue(res, i, 3), 10);
        strncpy(list->items[i].category_name, PQgetvalue(res, i, 4), 50);
        strncpy(list->items[i].next_billing_date, PQgetvalue(res, i, 5), 10);        
    }

    PQclear(res);
    return list;
}

// メモリ解放
void free_subscription_list(SubscriptionList *list) {
    if (list == NULL) return;
    if (list->items != NULL) {
        free(list->items);
    }
    free(list);
}

// 表示関数

void show_subscriptions(SubscriptionList *list) {
    if (list == NULL || list->count == 0) {
        printf("\n登録されているサブスクはありません\n");
        return;
    }

    int total_monthly = 0;
    int yearly_as_monthly = 0;

    printf("\n【サブスク一覧】（%d件）\n", list->count);
    printf("%-4s %-20s %10s %-12s %s\n", 
           "ID", "サービス名", "料金", "カテゴリ", "次回請求日");
    printf("------------------------------------------------------------\n");

    for (int i = 0; i < list->count; i++) {
        Subscription *sub = &list->items[i];    //  ポインタで参照
        
        int is_yearly = (strcmp(sub->billing_cycle,"yearly") == 0);
        const char *cycle_label = is_yearly ? "年" : "月";

        printf("%-4d %-20s ¥%8d/%s %-12s %s\n",
               sub->id,
               sub->service_name,
               sub->price,
               cycle_label,
               sub->category_name,
               sub->next_billing_date);
        
        if (is_yearly) {
            yearly_as_monthly += sub->price / 12;
        } else {
            total_monthly += sub->price;
        }
    }

    printf("------------------------------------------------------------\n");
    printf("月額合計: ¥%d", total_monthly);
    if (yearly_as_monthly > 0) {
        printf("  + 年額の月換算: ¥%d", yearly_as_monthly);
    }
    printf("\n総合計（月額換算）: ¥%d\n", total_monthly + yearly_as_monthly);

}




// メイン

int main(void) {
    PGconn *conn = PQconnectdb("dbname=submanager");
    
    if (PQstatus(conn) != CONNECTION_OK) {
        printf("接続失敗: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        return 1;
    }
    
    PQexec(conn, "SET client_encoding TO 'UTF8'");
    
    // データを取得
    SubscriptionList *list = get_subscriptions(conn);
    
    // 表示
    show_subscriptions(list);
    
    // 後片付け
    free_subscription_list(list);
    PQfinish(conn);
    
    return 0;
}