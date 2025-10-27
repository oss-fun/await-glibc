#!/bin/bash
# FOR DOCKERFILE

# ログディレクトリの作成（コンテナ内とホスト側両方）
source /app/scripts/log_controller.sh
log_init glibc_builder
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")

cd /app/glibc-2.35/build


run_silent "glibc_builder" "glibc_builder_1" CFLAGS="-O2 -Wno-error" ../configure --exec_prefix="/usr/local" --prefix="/usr/local" --libdir=/lib/x86_64-linux-gnu libc_cv_slibdir=/usr/local/lib/x86_64-linux-gnu --enable-crypt --enable-math --enable-nss --enable-ipc --enable-locales --enable-pthread --disable-sanity-checks

#cp "$LOG_DIR/glibc_builder_configure.log" "$HOST_LOG_DIR/"

run_silent "glibc_builder" "glibc_builder_2" CFLAGS="-O2 -Wno-error" make -j 16
#cp "$LOG_DIR/glibc_builder_make.log" "$HOST_LOG_DIR/"

exit 0

