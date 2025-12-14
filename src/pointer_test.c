#include <stdio.h>

int main(void) {
    int x = 42;
    int *p;          // ポインタ変数を宣言（int型の値を指すポインタ）

    p = &x;          //　ｐにⅹのアドレスを代入（＆はアドレスを取得）

    printf("xの値: %d\n", x);
    printf("xのアドレス: %p\n", (void*)&x);
    printf("pの値(=xのアドレス) : %p\n", (void*)p);
    printf("pが指す先の値: %d\n", *p);

    return 0 ;
}