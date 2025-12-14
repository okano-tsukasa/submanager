#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>

void show_subscriptions(PGconn *conn) {
    const char *sql =
        "SELECT"
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


    int row = PQntuples(res);

    if (row == 0) {
        printf("\n登録されているサブスクはありません。\n");
        PQclear(res);
        return;
    }

    // 合計計算用
    int total_monthly = 0;
    int yearly_as_monthly = 0;


    // ヘッダー
    printf("\n【サブスク一覧】（%d件）\n", row);
    printf("%-4s %-20s %10s %-12s %s\n", 
           "No.", "サービス名", "料金", "カテゴリ", "次回請求日");
    printf("------------------------------------------------------------\n");


    // データ行
    for (int i = 0; i < row; i++) {
        char *id = PQgetvalue(res, i, 0);
        char *name = PQgetvalue(res, i, 1);
        int price = atoi(PQgetvalue(res, i, 2));
        char *cycle = PQgetvalue(res, i, 3);
        char *category = PQgetvalue(res, i, 4);
        char *next_date = PQgetvalue(res, i, 5);
        
        int is_yearly = (strcmp(cycle, "yearly") == 0);
        const char *cycle_label = is_yearly ? "年" : "月";
        
        printf("%-4s %-20s ¥%8d/%s %-12s %s\n",
               id, name, price, cycle_label, category, next_date);

        // 合計に加算
        if (is_yearly) {
            yearly_as_monthly += price / 12;
        } else {
            total_monthly += price;
        }      
    }

    printf("------------------------------------------------------------\n");


    // 合計表示
    int grand_total = total_monthly + yearly_as_monthly;

    printf("月額合計: ¥%d", total_monthly);
    if (yearly_as_monthly > 0) {
        printf("  + 年額の月換算: ¥%d", yearly_as_monthly);

    }
            printf("\n");
    printf("総合計（月額換算）: ¥%d\n", grand_total);
    
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