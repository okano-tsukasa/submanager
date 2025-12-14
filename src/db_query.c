#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>

int main(void) {
    // 1.接続
    PGconn *conn = PQconnectdb("dbname=submanager");

    if (PQstatus(conn) != CONNECTION_OK) {
        printf("接続失敗: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        return 1;
    }

    printf("接続成功\n\n");

    // 2.SQLを実行
    PGresult *res = PQexec(conn, "SELECT * FROM categories");

    // 実行結果をチェック
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        printf("クエリ失敗: %s\n", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        return 1;

    }

    // 3.結果を取得して表示
    int rows = PQntuples(res);   // 行数

    printf("=== カテゴリ一覧 (%d件)===\n", rows);

    for (int i = 0; i < rows; i++) {
        // PQgetvalue(結果、行番号,列番号)で値を取得
        // 列0 = id, 列1 = name
        char *id = PQgetvalue(res, i, 0);
        char *name = PQgetvalue(res, i, 1);

        printf("ID: %s, 名前: %s\n", id, name);
    }

    // 4.後片付け
    PQclear(res);   // 結果のメモリを解放
    PQfinish(conn); // 接続を終了

    printf("\n処理完了\n");

    return 0;
}