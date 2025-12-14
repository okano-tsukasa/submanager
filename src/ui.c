#include <stdio.h>
#include <string.h>
#include "ui.h"

// 入力関数

void ui_input_string(const char *prompt, char *buffer, int max_len) {
    printf("%s: ", prompt);
    fflush(stdout);

    if (fgets(buffer, max_len, stdin) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len -1] = '\0';
        }
    }
}

int ui_input_int(const char *prompt) {
    char buffer[32];
    int value;

    while(1) {
        printf("%s: ", prompt);
        fflush(stdout);

        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            continue;
        }

        if (sscanf(buffer, "%d", &value) == 1) {
            return value;
        }

        printf("整数を入力してください\n");
    }
}

int ui_confirm(const char *message) {
    printf("%s (y/n): ", message) ;
    fflush(stdout);

    char buffer[10];
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        return 0;
    }
    return (buffer[0] == 'y' || buffer[0] =='Y');
}

// メニュー表示

void ui_show_main_menu(void) {
    printf("\n");
    printf("╔════════════════════════╗\n");
    printf("║     SubManager         ║\n");
    printf("╠════════════════════════╣\n");
    printf("║ 1. サブスク一覧        ║\n");
    printf("║ 2. 新規登録            ║\n");
    printf("║ 3. 編集                ║\n");
    printf("║ 4. 削除                ║\n");
    printf("║ 5. サマリー表示        ║\n");
    printf("║ 0. 終了                ║\n");
    printf("╚════════════════════════╝\n");
}

int ui_get_menu_choice(void) {
    return ui_input_int("選択");
}


// サブスク表示

void ui_show_subscription_list(SubscriptionList *list) {
    if (list == NULL || list->count == 0) {
        printf("\n登録されているサブスクはありません\n");
        return;
    }

    printf("\n【サブスク一覧】（%d件）\n", list->count);
    printf("%-4s %-18s %10s %-12s %s\n", 
           "ID", "サービス名", "料金", "カテゴリ", "次回請求日");
    printf("--------------------------------------------------------------\n");


    int total_monthly = 0;
    int yearly_as_monthly = 0;

    for (int i = 0; i < list->count; i++) {
        Subscription *sub = &list->items[i];

        int is_yearly = (strcmp(sub->billing_cycle, "yearly") == 0);
        const char *cycle_label = is_yearly ? "年" : "月";
        
        printf("%-4d %-18s ¥%8d/%s %-12s %s\n",
               sub->id,
               sub->service_name,
               sub->price,
               cycle_label,
               sub->category_name,
               sub->next_billing_date);

        if (is_yearly) {
            yearly_as_monthly += sub->price / 12;
        } else {
            total_monthly += sub->price;
        }        
    }


     printf("--------------------------------------------------------------\n");
     printf("月額: ¥%d", total_monthly);
     if (yearly_as_monthly > 0) {
        printf("  + 年額月換算: ¥%d", yearly_as_monthly);
     }
     printf("   【合計: ¥%d/月】\n", total_monthly + yearly_as_monthly);
}

void ui_show_categories(CategoryList *list) {
    if (list == NULL || list->count == 0) {
        printf("カテゴリがありません\n");
        return;
    }

    printf("\n【カテゴリ一覧】\n");
    for (int i = 0; i < list->count; i++) {
        printf("  %d. %s\n", list->items[i].id, list->items[i].name);
    }

    printf("\n");

}

// メッセージ表示


void ui_show_success(const char *message) {
    printf("\n✓ %s\n", message);
}

void ui_show_error(const char *message) {
    printf("\n✗ エラー: %s", message);
}