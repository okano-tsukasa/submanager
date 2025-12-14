#ifndef DB_H
#define DB_H

#include <libpq-fe.h>
#include "types.h"

// 接続管理
PGconn* db_connect(void);
void db_disconnect(PGconn *conn);

// カテゴリ操作
CategoryList* db_get_categories(PGconn *conn);
void db_free_category_list(CategoryList *list);

// サブスク取得
SubscriptionList* db_get_subscriptions(PGconn *conn);
Subscription* db_get_subscription_by_id(PGconn *conn, int id);
void db_free_subscription_list(SubscriptionList *list);
void db_free_subscription(Subscription *sub);

// サブスク操作
int db_insert_subscription(PGconn *conn, Subscription *sub);
int db_update_subscription(PGconn *conn, Subscription *sub);
int db_delete_subscription(PGconn *conn, int id);

#endif