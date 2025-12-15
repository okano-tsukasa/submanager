# SubManager

サブスクリプション管理CLIアプリケーション

## 概要

月額サービスの契約を一元管理するコマンドラインツールです。

## 技術スタック

- **言語**: C言語
- **データベース**: PostgreSQL 15
- **インフラ**: AWS EC2 (Amazon Linux 2023)
- **ビルドツール**: Make

## 機能

- サブスクリプションの登録・編集・削除
- カテゴリ別管理
- 月額合計の表示
- 一覧表示

## ローカル環境での実行

### 前提条件

- gcc
- PostgreSQL 15
- libpq-dev（PostgreSQL開発ライブラリ）

### セットアップ

```bash
# リポジトリをクローン
git clone https://github.com/okano-tsukasa/submanager.git
cd submanager

# PostgreSQLでデータベース作成
sudo -u postgres psql
CREATE USER submanager WITH PASSWORD 'your_password';
CREATE DATABASE submanager_db OWNER submanager;
\q

# テーブル作成
psql -U submanager -d submanager_db -f sql/init.sql

# ビルド
make

# 実行
./submanager
```

## AWSへのデプロイ

本アプリケーションはAWS EC2上で動作確認済みです。

詳細な構築手順は [docs/aws_setup.md](docs/aws_setup.md) を参照してください。

### 構成図

```
AWS Cloud
└── EC2 (t2.micro) - Amazon Linux 2023
    ├── gcc (Cコンパイラ)
    ├── PostgreSQL 15 (データベースサーバー)
    └── SubManager (アプリケーション)
```

## ディレクトリ構成

```
submanager/
├── src/           # ソースコード
│   ├── main.c     # エントリーポイント
│   ├── db.c       # データベース操作
│   ├── db.h
│   ├── ui.c       # ユーザーインターフェース
│   └── ui.h
├── sql/           # SQLスクリプト
│   └── init.sql   # テーブル初期化
├── docs/          # ドキュメント
│   └── aws_setup.md
├── Makefile
└── README.md
```

## 学習目的

このプロジェクトは以下の学習を目的として作成しました：

- C言語でのポインタ操作
- 構造体とメモリ管理
- PostgreSQLとの連携（libpq）
- Makefileによるビルド管理
- AWS EC2でのLinuxサーバー構築

## 作者

岡野司
