#!/bin/bash

# ログディレクトリの作成（コンテナ内とホスト側両方）
mkdir -p /output/logs
mkdir -p /app/logs
LOG_DIR="/output/logs"
HOST_LOG_DIR="/app/logs"
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")

# 色付きメッセージ用の関数
print_success() {
    echo -e "\033[32m✓\033[0m $1"
}

print_error() {
    echo -e "\033[31m✗\033[0m $1"
}

echo --- build glibc ---
/app/scripts/glibc_builder.sh > "$LOG_DIR/glibc_builder_${TIMESTAMP}.log" 2>&1
cp "$LOG_DIR/glibc_builder_${TIMESTAMP}.log" "$HOST_LOG_DIR/"
if [ $? -eq 0 ]; then
    print_success "glibc build completed successfully"
else
    print_error "glibc build failed"
fi

#/app/scripts/test_preopen.sh


echo --- glibc install ---
/app/scripts/glibc_install.sh > "$LOG_DIR/glibc_install_${TIMESTAMP}.log" 2>&1
cp "$LOG_DIR/glibc_install_${TIMESTAMP}.log" "$HOST_LOG_DIR/"
if [ $? -eq 0 ]; then
    print_success "glibc install completed successfully"
else
    print_error "glibc install failed"
fi

echo --- libxcrypt install ---
/app/scripts/libxcrypt_builder.sh > "$LOG_DIR/libxcrypt_builder_${TIMESTAMP}.log" 2>&1
cp "$LOG_DIR/libxcrypt_builder_${TIMESTAMP}.log" "$HOST_LOG_DIR/"
if [ $? -eq 0 ]; then
    print_success "libxcrypt install completed successfully"
else
    print_error "libxcrypt install failed"
fi
