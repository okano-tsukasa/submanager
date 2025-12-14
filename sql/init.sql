-- 既存テーブルを削除（やり直し用）
DROP TABLE IF EXISTS subscriptions;
DROP TABLE IF EXISTS categories;

-- カテゴリテーブル
CREATE TABLE categories (
	id SERIAL PRIMARY KEY,
	name VARCHAR(50) NOT NULL UNIQUE
);	
	
-- サブスクリプションテーブル
CREATE TABLE subscriptions (
	id SERIAL PRIMARY KEY,
	service_name VARCHAR(100) NOT NULL,
	price INTEGER NOT NULL,
	billing_cycle VARCHAR(10) NOT NULL,
	category_id INTEGER NOT NULL REFERENCES categories(id),
	next_billing_date DATE NOT NULL,
	memo TEXT,
	is_active BOOLEAN NOT NULL DEFAULT TRUE,
	created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP
);	
-- 初期カテゴリ 
INSERT INTO categories (name) VALUES
	('動画配信'),
	('音楽配信'),
	('クラウドストレージ'),
	('ソフトウェア'),
	('ゲーム'),
	('学習'),
	('その他');