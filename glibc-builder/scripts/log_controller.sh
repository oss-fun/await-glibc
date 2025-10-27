#!/bin/bash
#各スクリプトファイルでログを制御するためのコンポーネント

# ログ初期化
log_init() {
	BASE_LOGDIR="/logs"
	LOG_DIR="/logs/$1"
  mkdir -p ${LOG_DIR}
	INIT_TIMESTAMP=$(date '+%Y%m%d_%H%M%S')
}

# ログリセット
log_reset() {
	rm -rf /logs/*
}

# 第1引数はセクション名
# それ以降は任意の数の引数を受け取る
# [時間] [呼び出し元スクリプト] [引数]
# ↑この形式でファイルに書き出す
log_message() {
	local section_name="$1"
	local log_path="${LOG_DIR}/${section_name}.log"
	shift

	"$@" 2>&1 | ts '[%Y-%m-%d %H:%M:%S]' >>"$log_path"
	return $?
}

log_error_check() {
	if [ $? -eq 0 ]; then
			print_success "$2 completed successfully"
	else
			print_error "$1 failed"
			exit 1
	fi
}

# 色付きメッセージ用の関数
print_success() {
	echo -e "\033[32m✓\033[0m $1"
}

print_error() {
	echo -e "\033[31m✗\033[0m $1"
	exit -1
}

