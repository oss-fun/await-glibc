#!/bin/bash

# ログディレクトリの作成（コンテナ内とホスト側両方）
# ログコントローラをインポート
source /app/scripts/log_controller.sh
log_reset
log_init exec_all


run_silent "glibc_build" "glibc_build" /app/scripts/glibc_builder.sh
if [ $? -eq 0 ]; then
    print_success "glibc build completed successfully"
else
    print_error "glibc build failed"
    exit 1
fi

run_silent "glibc_install" "glibc_install" /app/scripts/glibc_install.sh
if [ $? -eq 0 ]; then
    print_success "glibc install completed successfully"
else
    print_error "glibc install failed"
    exit 1
fi

run_silent "libxcrypt_builder" "libxcrypt_builder" /app/scripts/libxcrypt_builder.sh
if [ $? -eq 0 ]; then
    print_success "libxcrypt build completed successfully"
else
    print_error "libxcrypt build failed"
    exit 1
fi

