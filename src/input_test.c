#include <stdio.h>
#include <string.h>

// 文字列を入力する関数

void input_string(const char *prompt, char *buffer, int max_len) {
    printf("%s: ", prompt);
    fflush(stdout);     // 出力を即座に表示

    if (fgets(buffer, max_len, stdin) != NULL) {
        // fgetsは改行を含むので、それを削除
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len -1] = '\0';

        }

    }
}

// 整数を入力する関数
int input_int(const char *prompt) {
    char buffer[32];
    int value;

    while (1) {
        printf("%s: ", prompt);
        fflush(stdout);

        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            continue;

        }

        // 数値に変換を試みる
        if (sscanf(buffer, "%d", &value) == 1) {
            return value;
        }

        printf("整数を入力してください\n");
    }

}

int main(void) {
    char name[100];
    int price;

    printf("=== 入力テスト ===\n\n");
    input_string("サービス名", name, sizeof(name));
    price = input_int("月額料金（円）");

    printf("\n=== 入力結果 ===\n");
    printf("サービス名: %s\n", name);
    printf("月額料金: %d\n", price);

    return 0;

}