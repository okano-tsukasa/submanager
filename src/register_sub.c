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
        printf("%s: ", prompt);
        fflush(stdout);
        
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            continue;
        }
        
        if (sscanf(buffer, "%d", &value) == 1) {
            return value;
        }
        
        printf("整数を入力してください\n");
    }
}

// ========== カテゴリ表示 ==========

void show_categories(PGconn *conn) {
    PGresult *res = PQexec(conn, "SELECT id, name FROM categories ORDER BY id");
    
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        printf("カテゴリ取得失敗\n");
        PQclear(res);
        return;
    }
    
    int rows = PQntuples(res);
    
    printf("\n【カテゴリ一覧】\n");
    for (int i = 0; i < rows; i++) {
        printf("  %s. %s\n", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1));
    }
    printf("\n");
    
    PQclear(res);
}

// ========== サブスク登録 ==========

int register_subscription(PGconn *conn) {
    char service_name[101];
    char price_str[20];
    char billing_cycle[11];
    char category_id_str[10];
    char next_billing_date[11];
     char memo[256];
    
    printf("\n===== サブスク新規登録 =====\n\n");
    
    // サービス名
    input_string("サービス名", service_name, sizeof(service_name));
    
    // 料金
    int price = input_int("月額/年額料金（円）");
    snprintf(price_str, sizeof(price_str), "%d", price);
    
    // 請求サイクル
    printf("\n請求サイクル:\n");
    printf("  1. 月額 (monthly)\n");
    printf("  2. 年額 (yearly)\n");
    int cycle_choice = input_int("選択 (1 or 2)");
    if (cycle_choice == 2) {
        strcpy(billing_cycle, "yearly");
    } else {
        strcpy(billing_cycle, "monthly");
    }
    
    // カテゴリ
    show_categories(conn);
    int category_id = input_int("カテゴリID");
    snprintf(category_id_str, sizeof(category_id_str), "%d", category_id);
    
    // 次回請求日
    input_string("次回請求日 (YYYY-MM-DD)", next_billing_date, sizeof(next_billing_date));
    
    // メモ（任意）
    input_string("メモ（任意、なければEnter）", memo, sizeof(memo));
    
    // 確認表示
    printf("\n===== 登録内容確認 =====\n");
    printf("サービス名: %s\n", service_name);
    printf("料金: %d円/%s\n", price, billing_cycle);
    printf("カテゴリID: %s\n", category_id_str);
    printf("次回請求日: %s\n", next_billing_date);
    if (strlen(memo) > 0) {
        printf("メモ: %s\n", memo);
    }
    
    printf("\nこの内容で登録しますか？ (y/n): ");
    char confirm[10];
    fgets(confirm, sizeof(confirm), stdin);
    
    if (confirm[0] != 'y' && confirm[0] != 'Y') {
        printf("登録をキャンセルしました\n");
        return 0;
    }
    
    // INSERTを実行
    const char *sql = 
        "INSERT INTO subscriptions "
        "(service_name, price, billing_cycle, category_id, next_billing_date, memo) "
        "VALUES ($1, $2, $3, $4, $5, $6)";
    
    const char *params[6] = {
        service_name,
        price_str,
        billing_cycle,
        category_id_str,
        next_billing_date,
        strlen(memo) > 0 ? memo : NULL  // 空ならNULL
    };
    
    PGresult *res = PQexecParams(conn, sql, 6, NULL, params, NULL, NULL, 0);
    
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        printf("登録失敗: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return -1;
    }
    
    PQclear(res);
    printf("\n登録が完了しました！\n");
    
    return 0;
}

// ========== メイン ==========

int main(void) {
    // 接続
    PGconn *conn = PQconnectdb("dbname=submanager");
    
    if (PQstatus(conn) != CONNECTION_OK) {
        printf("接続失敗: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        return 1;
    }
    
    // 文字化け対策
    PQexec(conn, "SET client_encoding TO 'UTF8'");
    
    // 登録処理
    register_subscription(conn);
    
    // 切断
    PQfinish(conn);
    
    return 0;
}