#include <stdio.h>

//普通の変数:コピーが渡されるので元の値は変わらない
void add_ten_copy(int n ) {
    n = n + 10;
    printf("関数内: n = %d\n", n);
}

//ポインタを使う変数:元の変数を直接変更できる
void add_ten_pointer(int *p) {
    *p = *p + 10;   //Pが指す先の値に10を足す
    printf("関数内: p = %d\n", *p);
}

int main(void) {
    int x = 5;

    printf("最初: X = %d\n", x);

    printf("\nadd_ten_cpoy(x)を呼ぶ:\n");
    add_ten_copy(x);
    printf("\n呼んだ後: x = %d (変わっていない)\n", x);

    printf("_naddd_ten_pointer(&x)を呼ぶ:\n");
    add_ten_pointer(&x);
    printf("呼んだ後: x = %d (変わっている)\n", x);

    return 0;
}