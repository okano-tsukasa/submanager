#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>

// ========== 入力関数 ==========

void input_string(const char *prompt, char *buffer, int max_len) {
    printf("%s: ", prompt);
    fflush(stdout);
    if (fgets(buffer, max_len, stdin) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
    }
}

int input_int(const char *prompt) {
    char buffer[32];
    int value;
    while (1) {
        printf("%s:", prompt);
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
        "WHERE s.is_active = true ORDER BY s.id";
    
    PGresult *res = PQexec(conn, sql);
    
    if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
        printf("\n登録されているサブスクはありません\n");
        PQclear(res);
        return;
    }
    
    printf("\n【サブスク一覧】\n");
    printf("%-4s %-20s %10s %s\n", "ID", "サービス名", "料金", "カテゴリ");
    printf("------------------------------------------------\n");
    
    int rows = PQntuples(res);
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


// ========== 編集機能 ==========

int edit_subscription(PGconn *conn) {
    show_subscriptions(conn);

    int id = input_int("編集するサブスクのID(0でキャンセル) ");

    if (id == 0) {
        printf("キャンセルしました\n") ;
        return 0;
    }

    // 現在の情報を取得
    char sql_select[300];
    snprintf(sql_select, sizeof(sql_select),
            "SELECT service_name, price, billing_cycle, "
            "next_billing_date "
            "FROM subscriptions WHERE id = %d AND is_active = true", id);
     
    PGresult *res = PQexec(conn, sql_select);
    
    if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
        printf("ID %d のサブスクが見つかりません\n", id);
        PQclear(res);
        return -1;
    }

    // 現在の値を取得
    char current_name[101];
    int current_price;
    char current_cycle[11];
    char current_date[11];

    strncpy(current_name, PQgetvalue(res, 0, 0), 100);
    current_price = atoi(PQgetvalue(res, 0, 1));
    strncpy(current_cycle, PQgetvalue(res, 0, 2), 10);
    strncpy(current_date, PQgetvalue(res, 0, 3), 10);

    PQclear(res);

    // 編集画面
    printf("\n===== 編集（空Enterで変更なし）=====\n");
    
    char new_name[101];
    char new_price_str[20];
    char new_cycle[11];
    char new_date[11];

    // サービス名
    printf("\n現在のサービス名: %s\n", current_name);
    input_string("新しいサービス名", new_name, sizeof(new_name));
    if (strlen(new_name) == 0) strcpy(new_name, current_name);

    // 料金
    printf("\n現在の料金: %d\n", current_price);
    input_string("新しい料金", new_price_str, sizeof(new_price_str));
    int new_price;
    if (strlen(new_price_str) == 0) {
        new_price = current_price;
    } else {
        new_price = atoi(new_price_str);
    }

    // 請求サイクル
    printf("\n現在のサイクル: %s\n", current_cycle);
    printf("新しいサイクル (monthly/yearly): ");
    input_string("", new_cycle, sizeof(new_cycle));
    if (strlen(new_cycle) == 0) strcpy(new_cycle, current_cycle);

    // 次回請求日
    printf("\n現在の次回請求日: %s\n", current_date);
    input_string("新しい次回請求日 (YYYY-MM-DD)", new_date, sizeof(new_date));
    if (strlen(new_date) == 0) strcpy(new_date, current_date);


    // 確認
    printf("\n===== 変更内容 =====\n");
    printf("サービス名: %s → %s\n", current_name, new_name);
    printf("料金: %d → %d\n", current_price, new_price);
    printf("サイクル: %s → %s\n", current_cycle, new_cycle);
    printf("次回請求日: %s → %s\n", current_date, new_date);
    
    printf("\nこの内容で更新しますか？ (y/n): ");
    char confirm[10];
    // fgets(confirm, sizeof(confirm), stdin);
    input_string("", confirm, sizeof(confirm));

    if (confirm[0] != 'y' && confirm[0] != 'Y') {
        printf("キャンセルしました\n");
        return 0;
    }
    
    // UPDATEを実行（パラメータクエリ）
    const char *sql_update = 
        "UPDATE subscriptions "
        "SET service_name = $1, price = $2, billing_cycle = $3, "
        "next_billing_date = $4 "
        "WHERE id = $5";

    char price_str[20];
    char id_str[10];
    snprintf(price_str, sizeof(price_str), "%d", new_price);
    snprintf(id_str, sizeof(id_str), "%d", id);

    const char *params[5] = {
        new_name,
        price_str,
        new_cycle,
        new_date,
        id_str
    };

    res = PQexecParams(conn, sql_update, 5, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        printf("更新失敗: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return -1;
    }

    PQclear(res);
    printf("\n更新が完了しました！\n");

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
    
    edit_subscription(conn);
    
    PQfinish(conn);
    return 0;
}