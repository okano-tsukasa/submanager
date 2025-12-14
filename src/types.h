#ifndef TYPES_H
#define  TYPES_H

// カテゴリ構造体
typedef struct {
    int id;
    char name[51];
} Category;

// サブスクリプション構造体
typedef struct {
    int id;
    char service_name[101];
    int price;
    char billing_cycle[11];
    int category_id;
    char category_name[51];
    char next_billing_date[11];
    char memo[256];
    int is_active;
} Subscription;

// サブスクリプションリスト
typedef struct {
    Subscription *items;
    int count;
    int capacity;
} SubscriptionList;

// カテゴリリスト
typedef struct {
    Category *items;
    int count;
} CategoryList;

#endif