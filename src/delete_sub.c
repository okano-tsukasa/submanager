#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>

// ========== 入力関数 ==========

int input_int(const char *prompt) {
    char buffer[32];
    int value;
    while (1) {
        printf("%s: ", prompt);
        fflush(stdout);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) continue;
        if (sscanf(buffer, "%d", &value) == 1) return value;
        printf("整数を入力してください\n");
    }
}

// ========== 一覧表示 ==========

void show_subscriptions(PGconn *conn) {
    const char *sql = 
        "SELECT s.id, s.service_name, s.price, s.billing_cycle, c.name "
        "FROM subscriptions s "
        "JOIN categories c ON s.category_id = c.id "
        "WHERE s.is_active = true "
        "ORDER BY s.id";
    
    PGresult *res = PQexec(conn, sql);
    
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        printf("クエリ失敗\n");
        PQclear(res);
        return;
    }
    
    int rows = PQntuples(res);
    
    if (rows == 0) {
        printf("\n登録されているサブスクはありません\n");
        PQclear(res);
        return;
    }
    
    printf("\n【サブスク一覧】\n");
    printf("%-4s %-20s %10s %s\n", "ID", "サービス名", "料金", "カテゴリ");
    printf("------------------------------------------------\n");
    
    for (int i = 0; i < rows; i++) {
        printf("%-4s %-20s ¥%8s %s\n",
               PQgetvalue(res, i, 0),
               PQgetvalue(res, i, 1),
               PQgetvalue(res, i, 2),
               PQgetvalue(res, i, 4));
    }
    printf("\n");
    
    PQclear(res);
}

// ========== 削除機能 ==========

int delete_subscription(PGconn *conn) {
    // まずは一覧表示
    show_subscriptions(conn);

    // 削除するIDを入力
    int id = input_int("削除するサブスクのID（0でキャンセル）");

    if (id == 0) {
        printf("キャンセルしました\n");
        return 0;
    }

    // 削除対象の情報を取得して確認
    char sql_select[200];
    snprintf(sql_select, sizeof(sql_select),
            "SELECT service_name, price FROM subscriptions WHERE id = %d "
            " AND is_active = true", id);


    PGresult *res = PQexec(conn, sql_select);

    if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
        printf("ID %d のサブスクが見つかりません\n", id);
        PQclear(res);
        return -1;
    }

    char *name = PQgetvalue(res, 0, 0);
    char *price = PQgetvalue(res, 0, 1);

    printf("\n削除対象: %s (¥%s)\n", name, price);
    printf("本当に削除しますか？ (y/n): ");
    
    char confirm[10];
    fgets(confirm, sizeof(confirm), stdin);

    PQclear(res);

    if (confirm[0] != 'y' && confirm[0] != 'Y') {
        printf("キャンセルしました\n");
        return 0;
    }

    // 削除（無効化）を実行
    char sql_delete[100];
    snprintf(sql_delete, sizeof(sql_delete), "UPDATE subscriptions SET is_active = false WHERE id = %d", id);

    res = PQexec(conn, sql_delete);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        printf("削除失敗: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return -1;
    }

    PQclear(res);
    printf("削除が完了しました！\n");

    return 0;

}

// ========== メイン ==========

int main(void) {
    PGconn *conn = PQconnectdb("dbname=submanager");
    
    if (PQstatus(conn) != CONNECTION_OK) {
        printf("接続失敗: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        return 1;
    }
    
    PQexec(conn, "SET client_encoding TO 'UTF8'");
    
    delete_subscription(conn);
    
    PQfinish(conn);
    return 0;
}