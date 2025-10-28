# glibc-2.35 open64.c 改変ドキュメント

## 概要
このドキュメントは、glibc-2.35の`sysdeps/unix/sysv/linux/open64.c`に対して実装されたPreopenメカニズムの改変について説明します。

## 改変の目的
ファイルオープンの際にPreopenメカニズムを実装することで、特定のファイルやディレクトリパスに対して事前に開かれたファイルディスクリプタを使用できるようにします。これにより、実行時のファイルアクセスを制限・制御することが可能になります。

## 主な機能

### 1. デバッグ機能
- **DEBUG_PRINTF**: `PREOPEN_DEBUG`が定義されている場合にデバッグメッセージを出力
- **debug_log**: デバッグログをファイルに出力する機能
- **get_debug_fd**: デバッグ出力先のファイルディスクリプタを取得

### 2. 環境変数による設定
- **PREOPEN_DEBUG_FD**: デバッグ出力先のファイルディスクリプタを指定
- **PREOPEN_DEBUG_FILE**: デバッグログファイルのパスを指定
- **PREOPEN_FILES**: プリオープンするファイルパスのリスト（コロン区切り）
- **PREOPEN_FILE_FDS**: プリオープンファイルに対応するファイルディスクリプタのリスト（コロン区切り）
- **PREOPEN_PATHS**: プリオープンするディレクトリパスのリスト（コロン区切り）
- **PREOPEN_PATH_FDS**: プリオープンディレクトリに対応するファイルディスクリプタのリスト（コロン区切り）

### 3. 主要な関数

#### `preopen_file(const char *file, int oflag, int mode)`
- 指定されたファイルパスが`PREOPEN_FILES`に含まれているかチェック
- 一致する場合は対応する`PREOPEN_FILE_FDS`のファイルディスクリプタを返す

#### `preopen_from_dir(const char *file, int oflag, int mode)`
- ファイルの親ディレクトリが`PREOPEN_PATHS`に含まれているかチェック
- 一致する場合は対応する`PREOPEN_PATH_FDS`のファイルディスクリプタを使用して`openat`システムコールを実行

#### `preopen(const char *file, int oflag, int mode)`
- メインのpreopen機能
- 最初に`preopen_file`を試行し、失敗した場合は`preopen_from_dir`を試行

#### `find_matching_preopen_path(const char *file_path, char **preopen_paths, int path_count)`
- ファイルパスとプリオープンパスのマッチングを行う
- パスコンポーネントごとに比較して最適なマッチを見つける

### 4. ユーティリティ関数

#### `get_absolute_path(const char *path)`
- 相対パスを絶対パスに変換

#### `remove_last_component(const char *path)`
- パスの最後のコンポーネント（ファイル名）を削除してディレクトリパスを取得

#### `get_path_component(const char *path, int n)`
- パスのn番目のコンポーネントを取得

#### `get_string_list(char* env_str)`
- 環境変数の値をコロン区切りで分割して配列に変換

## 処理フロー

1. `__libc_open64`関数が呼び出される
2. デバッグログのファイルディスクリプタを取得
3. `preopen`関数を呼び出してプリオープンを試行
4. プリオープンが成功した場合、そのファイルディスクリプタを返す
5. 失敗した場合は通常の`openat`システムコールを実行
6. デバッグログを出力して結果を返す

## 使用例

```bash
# 環境変数の設定例
export PREOPEN_FILES="/etc/passwd:/etc/hosts"
export PREOPEN_FILE_FDS="3:4"
export PREOPEN_PATHS="/tmp:/var/log"
export PREOPEN_PATH_FDS="5:6"
export PREOPEN_DEBUG_FILE="/tmp/preopen.log"
```

## 制限事項

- 最大128個のプリオープンエントリまで対応
- 最大256文字のパス長まで対応
- 環境変数による設定が必要

## デバッグ機能

`PREOPEN_DEBUG`を定義してコンパイルすると、詳細なデバッグ情報が出力されます。デバッグログは`PREOPEN_DEBUG_FILE`で指定したファイルに記録されます。

## ファイル場所

- 改変されたファイル: `glibc-2.35/sysdeps/unix/sysv/linux/open64.c`
- 主要関数: `__libc_open64` (494-520行)
- Preopen機能: `preopen` (471-492行)