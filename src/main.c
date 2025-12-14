#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "db.h"
#include "ui.h"

// ========== 機能実装 ==========


// 一覧表示
void handle_list(PGconn *conn) {
    SubscriptionList *list = db_get_subscriptions(conn);
    ui_show_subscription_list(list);
    db_free_subscription_list(list);
}

// 新規登録

void handle_register(PGconn *conn)  {
    printf("\n===== 新規登録 =====\n");
    
    Subscription sub = {0};
    
    // サービス名
    ui_input_string("サービス名", sub.service_name, sizeof(sub.service_name));
    
    // 料金
    sub.price = ui_input_int("料金（円）");
    
    // 請求サイクル
    printf("\n請求サイクル: 1.月額  2.年額\n");
    int cycle = ui_input_int("選択");
    strcpy(sub.billing_cycle, cycle == 2 ? "yearly" : "monthly");
    
    // カテゴリ
    CategoryList *categories = db_get_categories(conn);
    ui_show_categories(categories);
    sub.category_id = ui_input_int("カテゴリID");
    db_free_category_list(categories);
    
    // 次回請求日
    ui_input_string("次回請求日 (YYYY-MM-DD)", sub.next_billing_date, sizeof(sub.next_billing_date));
    
    // メモ
    ui_input_string("メモ（任意）", sub.memo, sizeof(sub.memo));
    
    // 確認
    printf("\n--- 登録内容 ---\n");
    printf("サービス名: %s\n", sub.service_name);
    printf("料金: %d円/%s\n", sub.price, sub.billing_cycle);
    printf("次回請求日: %s\n", sub.next_billing_date);
    
    if (ui_confirm("登録しますか？")) {
        int result = db_insert_subscription(conn, &sub);
        if (result > 0) {
            ui_show_success("登録しました");
        } else {
            ui_show_error("登録に失敗しました");
        }
    } else {
        printf("キャンセルしました\n");
    }
}

// 編集
void handle_edit(PGconn *conn) {
    handle_list(conn);

    int id = ui_input_int("編集ID (0でキャンセル)");
    if (id == 0) return;

    Subscription *sub = db_get_subscription_by_id(conn, id);
    if (sub == NULL) {
        ui_show_error("見つかりません");
        return;
    }
    
    printf("\n===== 編集（空Enterで変更なし）=====\n");

    char buffer[256];

    // サービス名
    printf("現在: %s\n", sub->service_name);
    ui_input_string("新しいサービス名", buffer, sizeof(buffer));
    if (strlen(buffer) > 0) strcpy(sub->service_name, buffer);


    // 料金
    printf("現在: %d\n", sub->price);
    ui_input_string("新しい料金", buffer, sizeof(buffer));
    if (strlen(buffer) > 0) sub->price = atoi(buffer);
    
    // 次回請求日"
    printf("現在: %s\n", sub->next_billing_date);
    ui_input_string("新しい次回請求日", buffer, sizeof(buffer));
    if (strlen(buffer) > 0) strcpy(sub->next_billing_date, buffer);

    if (ui_confirm("更新しますか？")) {
        if (db_update_subscription(conn, sub) == 0) {
            ui_show_success("更新しました");
        } else {
            ui_show_error("更新に失敗しました");
        }
    }

    db_free_subscription(sub);

}

// 削除
void handle_delete(PGconn *conn) {
    handle_list(conn);

    int id = ui_input_int("削除するID(0でキャンセル) ");
    if (id == 0) return;

    Subscription *sub = db_get_subscription_by_id(conn, id);
    if (sub == NULL) {
        ui_show_error("見つかりません");
        return;
    }

    printf("\n削除対象: %s (¥%d)\n", sub->service_name, sub->price);

    if (ui_confirm("本当に削除しますか？")) {
        if (db_delete_subscription(conn, id) == 0){
            ui_show_success("削除しました");
        } else {
            ui_show_error("削除に失敗しました");
        }
    }
    db_free_subscription(sub);
}

// サマリー
void handle_summary(PGconn *conn) {
    SubscriptionList *list = db_get_subscriptions(conn);
    
    if (list == NULL || list->count == 0) {
        printf("\nデータがありません\n");
        db_free_subscription_list(list);
        return;
    }
    
    int monthly_total = 0;
    int yearly_total = 0;
    
    for (int i = 0; i < list->count; i++) {
        if (strcmp(list->items[i].billing_cycle, "yearly") == 0) {
            yearly_total += list->items[i].price;
        } else {
            monthly_total += list->items[i].price;
        }
    }
    
    printf("\n╔═══════════════════════════╗\n");
    printf("║       支払いサマリー       ║\n");
    printf("╠═══════════════════════════╣\n");
    printf("║ 登録件数:     %3d 件      ║\n", list->count);
    printf("║ 月額合計:  ¥%6d        ║\n", monthly_total);
    printf("║ 年額合計:  ¥%6d        ║\n", yearly_total);
    printf("║ 年間総額:  ¥%6d        ║\n", monthly_total * 12 + yearly_total);
    printf("╚═══════════════════════════╝\n");
    
    db_free_subscription_list(list);
}

// ========== メイン ==========

int main(void) {
    PGconn *conn = db_connect();
    
    if (conn == NULL) {
        return 1;
    }
    
    int running = 1;
    
    while (running) {
        ui_show_main_menu();
        int choice = ui_get_menu_choice();
        
        switch (choice) {
            case 1: handle_list(conn); break;
            case 2: handle_register(conn); break;
            case 3: handle_edit(conn); break;
            case 4: handle_delete(conn); break;
            case 5: handle_summary(conn); break;
            case 0:
                running = 0;
                printf("\nお疲れさまでした！\n");
                break;
            default:
                printf("\n無効な選択です\n");
        }
    }
    
    db_disconnect(conn);
    return 0;
}
        