#include <stdio.h>
#include <string.h>

typedef struct {
    char name[100];
    int price;
    char cycle [10];
} Subscription;

// 構造体のポインタを受け取って表示する関数
void print_subscription(Subscription *sub) {
    // ポインタから構造体メンバにアクセスするには ->を使用
    printf("サービス名: %s\n", sub->name);
    printf("料金: %d円\n", sub->price);
    printf("サイクル: %s\n", sub->cycle);

}

// 構造体のポインタを受けとって値を変更する関数
void apply_discount(Subscription *sub, int percent) {
    sub->price = sub->price * (100 - percent) / 100;
    printf("(%d%%割引を適用しました) \n", percent);

}

int main(void) {
    Subscription netflix;
    strcpy(netflix.name, "Netflix");
    netflix.price = 1490;
    strcpy(netflix.cycle, "monthly");

    printf("=== 割引前 ===\n");
    print_subscription(&netflix); //アドレスを渡す

    printf("\n=== 20%%割引適用 ===\n");
    apply_discount(&netflix, 20);

    printf("\n=== 割引後 ===\n");
    print_subscription(&netflix);

    return 0;
}