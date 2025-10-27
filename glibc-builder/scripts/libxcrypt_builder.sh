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
run_silent "libxcrypt builder" "libxcrypt_builder_1" ./autogen.sh
if [ $? -eq 0 ]; then
    print_success "autogen.sh completed successfully"
else
    print_error "autogen.sh failed"
    exit 1
fi
run_silent "libxcrypt builder" "libxcript_builder_2" ./configure --prefix="/usr/local" \
  --exec-prefix="/usr/local" \
  --libdir=/usr/local/lib/x86_64-linux-gnu \
  --enable-shared \
  --enable-static \
	--enable-xcrypt-compat-files
if [ $? -eq 0 ]; then
    print_success "configure completed successfully"
else
    print_error "configure failed"
    exit 1
fi

run_silent "libxcrypt builder" "libxcrypt_builder_3" mkdir /output/libxcrypt
if [ $? -eq 0 ]; then
    print_success "output directory created successfully"
else
    print_error "failed to create output directory"
    exit 1
fi

run_silent "libxcrypt builder" "libxcrypt_builder_4" make -j16
if [ $? -eq 0 ]; then
    print_success "make completed successfully"
else
    print_error "make failed"
    exit 1
fi

run_silent "libxcrypt builder" "libxcrypt_builder_5" make -j16 install DESTDIR=/output/libxcrypt
if [ $? -eq 0 ]; then
    print_success "make install completed successfully"
else
    print_error "make install failed"
    exit 1
fi

# ライブラリをrootfsにコピー
run_silent "libxcrypt builder" "libxcrypt_builder_6" cp -r /output/libxcrypt/usr/local/lib/x86_64-linux-gnu/* /output/await-rootfs/usr/local/lib/x86_64-linux-gnu/
if [ $? -eq 0 ]; then
    print_success "libxcrypt libraries copied successfully"
else
    print_error "failed to copy libxcrypt libraries"
    exit 1
fi

run_silent "libxcrypt builder" "libxcrypt_builder_7" cp -r /output/libxcrypt/usr/local/include/* /output/await-rootfs/usr/local/include/
if [ $? -eq 0 ]; then
    print_success "libxcrypt headers copied successfully"
else
    print_error "failed to copy libxcrypt headers"
    exit 1
fi

# debootstrapではman3やman5のディレクトリが生成されないためここで作成する
#mkdir /output/await-rootfs/usr/local/share/man/man3
#mkdir /output/await-rootfs/usr/local/share/man/man5

# manのコピー
#cp -r /output/libxcrypt/usr/local/share/man/man3/*          /output/await-rootfs/usr/local/share/man/man3/
#cp -r /output/libxcrypt/usr/local/share/man/man5/*          /output/await-rootfs/usr/local/share/man/man5/

