#ifndef UI_H
#define UI_H

#include "types.h"

// 入力関数
void ui_input_string(const char *prompt, char *buffer, int max_len);
int ui_input_int(const char *prompt);
int ui_confirm(const char *message);

// メニュー表示
void ui_show_main_menu(void);
int ui_get_menu_choice(void);

// サブスク表示
void ui_show_subscription_list(SubscriptionList *list);
void ui_show_categories(CategoryList *list);

// メッセージ表示
void ui_show_success(const char *message);
void ui_show_error(const char *message);

#endif