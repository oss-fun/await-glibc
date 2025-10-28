#!/bin/bash
# FOR DOCKERFILE

# ログディレクトリの作成（コンテナ内とホスト側両方）
source /app/scripts/log_controller.sh
log_init glibc_builder
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")

cd /app/glibc-2.35/build

export CFLAGS="-O2 -Wno-error"

run_silent "glibc_builder" "#1 configure glibc" bash -c '../configure --exec-prefix="/usr/local" --prefix="/usr/local" --libdir=/lib/x86_64-linux-gnu libc_cv_slibdir=/usr/local/lib/x86_64-linux-gnu --enable-crypt --enable-math --enable-nss --enable-ipc --enable-locales --enable-pthread --disable-sanity-checks'
log_error_check "#1 configure glibc"
#cp "$LOG_DIR/glibc_builder_configure.log" "$HOST_LOG_DIR/"

run_silent "glibc_builder" "#2 build glibc" make -j 16
log_error_check "#2 build glibc"
#cp "$LOG_DIR/glibc_builder_make.log" "$HOST_LOG_DIR/"

exit 0

