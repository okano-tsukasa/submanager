#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>

// 入力関数
void input_string(const char *prompt, char *buffer, int max_len) {
    printf("%s:", prompt);
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
    while(1) {
        printf("%s: ", prompt);
        fflush(stdout);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) continue;
        if (sscanf(buffer, "%d", &value) == 1) return value;
        printf("整数を入力してください\n");
    }
}

// メニューを表示
void show_menu(void) {
    printf("\n");
    printf("===== SubManager =====\n");
    printf("1. サブスク一覧\n");
    printf("2. 新規登録\n");
    printf("3. 削除\n");
    printf("0. 終了\n");
    printf("======================\n");
}

// 一覧表示（仮）
void show_subscriptions(PGconn *conn) {
    printf("\n【一覧表示は次のChapterで実装します】\n");
}

// 新規登録（仮）
void register_subscription(PGconn *conn) {
    printf("\n【新規登録は Step 3-3 で実装済み】\n");
    printf("【ここに register_sub.c の中身をコピーする】\n");
}

// 削除（仮）
void delete_subscription(PGconn *conn) {
    printf("\n【削除は Chapter 5 で実装します】\n");
}

int main(void) {
    // 接続
    PGconn *conn = PQconnectdb("dbname=submanager");

    if (PQstatus(conn) != CONNECTION_OK) {
        printf("接続失敗: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        return 1;
    }

    PQexec(conn, "SET client_encoding TO 'UTF8'");

    // メインループ
    int running = 1;

    while (running) {
        show_menu();
        int choice = input_int("選択");

        switch (choice)
        {
        case 1:
            show_subscriptions(conn);
            break;
        case 2:
            register_subscription(conn);
            break;
        case 3:
        delete_subscription(conn);
            break;
        case 0:
            running = 0;
            printf("\nお疲れ様でした\n");
            break;
        default:
            printf("\n無効な選択です\n");
        }
    }

    PQfinish(conn);
    return 0;



}