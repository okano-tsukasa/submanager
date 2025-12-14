#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char name[100];
    int price;
 } Subscription;

int main(void) {
    // 動的にメモリ確保
    Subscription *sub = malloc(sizeof(Subscription));

    // 確保できたか確認
    if (sub == NULL) {
        printf("メモリ確保に失敗しました。");
        return 1;

    }

    // 値を設定 (ポインタなので->　を使う)
    strcpy(sub->name, "Netflix");
    sub->price = 1490;

    // 表示
    printf("サービス名: %s\n", sub->name);
    printf("料金: %d円\n", sub->price);

    // メモリを解放(忘れずに！)
    free(sub);
    sub = NULL;     // 解放後はNULLにしておくと安全

    printf("メモリを解放しました。\n");

    return 0;

}