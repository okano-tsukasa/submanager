#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char name[100];
    int price;
} Subscription;

int main(void) {
    int count = 3;  //三件分確保

    // 配列を動的に確保
    Subscription *sub = malloc(sizeof(Subscription) * count);

    if (sub == NULL) {
        printf("メモリ確保に失敗しました。\n");
        return 1;
    }    


    // データを設定 (配列としてアクセス)
    strcpy(sub[0].name, "Netflix");
    sub[0].price = 1490;

    strcpy(sub[1].name, "Amazon Prime");
    sub[1].price = 550;

    strcpy(sub[2].name, "Google Cloud");
    sub[2].price = 250;

    // 全部表示
    printf("=== サブスク一覧 ===\n");
    for (int i = 0; i < count; i++) {
        printf("%d. %s: %d円/月\n", i + 1, sub[i].name, sub[i].price);
    }

    // 合計を計算
    int total = 0;
    for (int i = 0; i < count; i ++){
        total += sub[i].price;
    }
    printf("--------------------\n");
    printf("合計: %d円/月\n", total);

    // メモリ解放
    free(sub);


    return 0;



}
