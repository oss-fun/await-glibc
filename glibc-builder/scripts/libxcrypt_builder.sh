#!/bin/bash
# FOR DOCKERFILE

# ログディレクトリの作成
source /app/scripts/log_controller.sh
log_init libxcrypt_builder

TIMESTAMP=$(date +"%Y%m%d_%H%M%S")

export LD_LIBRARY_PATH=/output/glibc/usr/lib/x86_64-linux-gnu
export CC="gcc -L/output/glibc/usr/lib/x86_64-linux-gnu"
export CFLAGS="-I/output/glibc/usr/include"

cd /app/libxcrypt

# libxcryptのビルドコンフィグ
run_silent "libxcrypt_builder" "#1 libxcrypt autogen" ./autogen.sh
log_error_check "#1 libxcrypt autogen"

run_silent "libxcrypt_builder" "#2 configure libxcrypt" ./configure --prefix="/usr/local" \
  --exec-prefix="/usr/local" \
  --libdir=/usr/local/lib/x86_64-linux-gnu \
  --enable-shared \
  --enable-static \
	--enable-xcrypt-compat-files
log_error_check "#2 configure libxcrypt"

run_silent "libxcrypt_builder" "#3 mkdir" mkdir /output/libxcrypt
log_error_check "#3 mkdir"

run_silent "libxcrypt_builder" "#4 build libxcrypt" make -j16
log_error_check "#4 build libxcrypt"

run_silent "libxcrypt_builder" "#5 install libxcrypt" make -j16 install DESTDIR=/output/libxcrypt
log_error_check "#5 install libxcrypt"

# ライブラリをrootfsにコピー
run_silent "libxcrypt_builder" "#6 copy libxcrypt" cp -r /output/libxcrypt/usr/local/lib/x86_64-linux-gnu/* /output/await-rootfs/usr/local/lib/x86_64-linux-gnu/
log_error_check "libxcrypt build task #6"

run_silent "libxcrypt_builder" "#7 copy libxcrypt 2" cp -r /output/libxcrypt/usr/local/include/* /output/await-rootfs/usr/local/include/
log_error_check "#7 copy libxcrypt 2"

# debootstrapではman3やman5のディレクトリが生成されないためここで作成する
#mkdir /output/await-rootfs/usr/local/share/man/man3
#mkdir /output/await-rootfs/usr/local/share/man/man5

# manのコピー
#cp -r /output/libxcrypt/usr/local/share/man/man3/*          /output/await-rootfs/usr/local/share/man/man3/
#cp -r /output/libxcrypt/usr/local/share/man/man5/*          /output/await-rootfs/usr/local/share/man/man5/

