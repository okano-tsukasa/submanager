#include <stdio.h>
#include <libpq-fe.h>

int main(void) {
    // データベースに接続
    // "dbname=submanager"でsubmanagerデータベースに接続
    PGconn *conn = PQconnectdb("dbname=submanager");

    // 接続できたかチェック
    if (PQstatus(conn) != CONNECTION_OK) {
        // 失敗した場合、エラーメッセージを表示
        printf("接続失敗: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        return 1;

    }

    printf("データベースに接続しました！\n");

    // 接続を終了
    PQfinish(conn);
    printf("接続を終了しました\n");

    return 0;
}