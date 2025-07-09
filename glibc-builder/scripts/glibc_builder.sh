#!/bin/bash
# FOR DOCKERFILE

# ログディレクトリの作成（コンテナ内とホスト側両方）
mkdir -p /logs
LOG_DIR="/logs"
#HOST_LOG_DIR="logs"
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")

# 色付きメッセージ用の関数
print_success() {
    echo -e "\033[32m✓\033[0m $1"
}

print_error() {
    echo -e "\033[31m✗\033[0m $1"
}

cd /app/glibc-2.35/build

mkdir -p /output/glibc >> "$LOG_DIR/glibc_builder_mkdir_${TIMESTAMP}.log" 2>&1
#cp "$LOG_DIR/glibc_builder_mkdir_${TIMESTAMP}.log" "$HOST_LOG_DIR/"
if [ $? -eq 0 ]; then
    print_success "output directory created successfully"
else
    print_error "failed to create output directory"
    exit 1
fi

../configure --exec_prefix="/usr/local" --prefix="/usr/local" --libdir=/lib/x86_64-linux-gnu libc_cv_slibdir=/usr/local/lib/x86_64-linux-gnu --enable-crypt --enable-math --enable-nss --enable-ipc --enable-locales --enable-pthread --disable-sanity-checks >> "$LOG_DIR/glibc_builder_configure_${TIMESTAMP}.log" 2>&1
#cp "$LOG_DIR/glibc_builder_configure_${TIMESTAMP}.log" "$HOST_LOG_DIR/"
if [ $? -eq 0 ]; then
    print_success "glibc configure completed successfully"
else
    print_error "glibc configure failed"
    exit 1
fi

make -j 16 >> "$LOG_DIR/glibc_builder_make_${TIMESTAMP}.log" 2>&1
#cp "$LOG_DIR/glibc_builder_make_${TIMESTAMP}.log" "$HOST_LOG_DIR/"
if [ $? -eq 0 ]; then
    print_success "glibc make completed successfully"
else
    print_error "glibc make failed"
    exit 1
fi 
