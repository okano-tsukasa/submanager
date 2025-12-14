#include <stdio.h>
#include <string.h>

//構造体の定義
struct Subscription {
    char name[100];
    int price;
    char cycle[10];
};

int main(void) {
    //構造体変数を作成
    struct Subscription sub1;

    //メンバに値を設定(.でアクセス)
    strcpy(sub1.name, "Netflix");   //文字列はstrcpy())
    sub1.price = 1490;
    strcpy(sub1.cycle, "monthly");

    //表示
    printf("サービス名: %s\n", sub1.name);
    printf("料金:%d円\n", sub1.price);
    printf("サイクル:%s\n", sub1.cycle);

    return 0;

}

