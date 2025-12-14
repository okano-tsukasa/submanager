#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>

void show_subscriptions(PGconn *conn) {
    const char *sql =
        "SELECT "
        "   s.id, "
        "   s.service_name, "
        "   s.price, "
        "   s.billing_cycle, "
        "   c.name AS category_name, "
        "   s.next_billing_date "
    "FROM subscriptions s "
    "JOIN categories c ON s.category_id = c.id "
    "WHERE s.is_active = true "
    "ORDER BY s.next_billing_date";

    PGresult *res = PQexec(conn, sql);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        printf("クエリ失敗: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return;

    }


    int rows = PQntuples(res);

    if (rows == 0) {
        printf("\n登録されているサブスクはありません\n");
        PQclear(res);
        return;
    }

    // ヘッダー
    printf("\n【サブスク一覧】 (%d件) \n", rows);
    printf("%-4s %-20s %12s %-14s %s\n",
            "No.", "サービス名", "料金", "カテゴリ", "次回請求日");
    printf("------------------------------------------------------------\n");

    // データ行
    for (int i = 0; i < rows; i++) {
        char *id = PQgetvalue(res, i, 0);
        char *name = PQgetvalue(res, i, 1);
        int price = atoi(PQgetvalue(res, i, 2));
        char *cycle = PQgetvalue(res, i, 3);
        char *category = PQgetvalue(res, i, 4);
        char *next_date = PQgetvalue(res, i, 5);
    
    // 月額/年額の表示を変える
    const char *cycle_label = strcmp(cycle, "yearly") == 0 ? "年" : "月";

    printf("%-4s %-20s ¥%'10d/%s %-14s %s\n",
            id, name, price, cycle_label, category, next_date);
    }
    
    printf("------------------------------------------------------------\n");

    PQclear(res);
}


int main(void) {
    PGconn *conn = PQconnectdb("dbname=submanager");

    if (PQstatus(conn) != CONNECTION_OK) {
        printf("接続失敗: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        return 1;
    }

    PQexec(conn, "SET client_encoding TO 'UTF8'");

    show_subscriptions(conn);

    PQfinish(conn);
    return 0;
}