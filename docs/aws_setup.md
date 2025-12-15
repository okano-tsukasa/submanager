# AWS EC2 セットアップ手順

本ドキュメントでは、SubManagerをAWS EC2上にデプロイする手順を説明します。

## 前提条件

- AWSアカウント（無料枠利用可能）
- 基本的なLinuxコマンドの知識

## 1. EC2インスタンスの作成

### 1.1 EC2ダッシュボードへ移動

1. AWSマネジメントコンソールにログイン
2. 上部検索バーで「EC2」と入力して選択
3. 「インスタンスを起動」ボタンをクリック

### 1.2 インスタンスの設定

| 項目 | 設定値 |
|------|--------|
| 名前 | submanager-server |
| AMI | Amazon Linux 2023（無料枠対象） |
| インスタンスタイプ | t2.micro（無料枠対象） |
| キーペア | 新規作成（.pem形式） |
| ストレージ | 8 GiB（デフォルト） |

### 1.3 セキュリティグループの設定

- セキュリティグループ名: `SubManager-sg`
- インバウンドルール:
  - タイプ: SSH
  - ポート: 22
  - ソース: 自分のIP（またはAnywhere-IPv4）

### 1.4 インスタンスの起動

「インスタンスを起動」をクリックし、ステータスチェックが「2/2のチェックに合格しました」になるまで待機。

## 2. EC2インスタンスへの接続

### EC2 Instance Connect（ブラウザ経由）

1. EC2ダッシュボード → インスタンス一覧
2. インスタンスを選択 →「接続」ボタン
3. 「EC2 Instance Connect」タブで「接続」

## 3. 開発環境のセットアップ

### 3.1 必要なパッケージのインストール

```bash
# Cコンパイラ
sudo dnf install gcc -y

# PostgreSQL開発ライブラリ
sudo dnf install postgresql15-devel -y

# PostgreSQLサーバー
sudo dnf install postgresql15-server -y

# Git
sudo dnf install git -y
```

### 3.2 PostgreSQLの初期化と起動

```bash
# 初期化
sudo postgresql-setup --initdb

# 起動
sudo systemctl start postgresql

# 自動起動設定
sudo systemctl enable postgresql

# 動作確認
sudo -u postgres psql -c "SELECT version();"
```

## 4. データベースの作成

### 4.1 ユーザーとデータベースの作成

```bash
sudo -u postgres psql
```

```sql
CREATE USER submanager WITH PASSWORD 'your_password_here';
CREATE DATABASE submanager_db OWNER submanager;
\q
```

### 4.2 認証設定の変更

```bash
sudo sed -i 's/local   all             all                                     peer/local   all             all                                     md5/' /var/lib/pgsql/data/pg_hba.conf
sudo systemctl restart postgresql
```

## 5. アプリケーションのデプロイ

### 5.1 リポジトリのクローン

```bash
git clone https://github.com/okano-tsukasa/submanager.git
cd submanager
```

### 5.2 DB接続設定の変更

`src/db.c` の接続文字列を環境に合わせて変更:

```c
PGconn *conn = PQconnectdb("dbname=submanager_db user=submanager password=your_password_here");
```

### 5.3 ビルドと実行

```bash
# ビルド
make clean
make

# テーブル作成
psql -U submanager -d submanager_db -f sql/init.sql

# 実行
./submanager
```

## 6. 構成図

```
AWS Cloud
├── VPC (Default)
│   └── Subnet
│       └── EC2 Instance (t2.micro)
│           ├── Amazon Linux 2023
│           ├── Security Group (SSH: port 22)
│           └── EBS Volume (8 GiB)
│
└── EC2内部構成
    ├── gcc 11.5.0
    ├── PostgreSQL 15.15
    │   ├── Database: submanager_db
    │   └── User: submanager
    └── SubManager Application
```

## 7. 無料枠について

| サービス | 無料枠 | 本構成 |
|---------|-------|--------|
| EC2 | t2.micro 月750時間 | t2.micro ✓ |
| EBS | 30GB/月 | 8GB ✓ |
| データ転送 | 100GB/月（送信） | 微量 ✓ |

**注意**: 無料枠はAWSアカウント作成から12ヶ月間のみ有効。

## 8. 注意事項

- インスタンスを使わないときは停止してコスト削減
- キーペアファイル（.pem）は紛失しないよう保管
- パスワードのハードコーディングは本番環境では避け、環境変数を使用
- セキュリティグループは必要最小限のアクセスに制限

## 9. トラブルシューティング

### EC2 Instance Connectで接続できない

- セキュリティグループでSSH（ポート22）が許可されているか確認
- ソースを「Anywhere-IPv4」に変更して再試行

### PostgreSQL認証エラー

- `pg_hba.conf` で認証方式が `md5` になっているか確認
- PostgreSQLを再起動: `sudo systemctl restart postgresql`

### アプリがDBに接続できない

- `src/db.c` の接続文字列（dbname, user, password）を確認
- 再ビルド: `make clean && make`
