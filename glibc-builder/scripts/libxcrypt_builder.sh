#!/bin/bash
# FOR DOCKERFILE

# ログディレクトリの作成
mkdir -p /logs
LOG_DIR="/logs"
#HOST_LOG_DIR="/app/logs"
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")

# 色付きメッセージ用の関数
print_success() {
    echo -e "\033[32m✓\033[0m $1"
}

print_error() {
    echo -e "\033[31m✗\033[0m $1"
		exit -1
}

export LD_LIBRARY_PATH=/output/glibc/usr/lib/x86_64-linux-gnu
export CC="gcc -L/output/glibc/usr/lib/x86_64-linux-gnu"
export CFLAGS="-I/output/glibc/usr/include"

cd /app/libxcrypt

./autogen.sh >> "$LOG_DIR/libxcrypt_autogen_${TIMESTAMP}.log" 2>&1
#cp "$LOG_DIR/libxcrypt_autogen_${TIMESTAMP}.log" "$HOST_LOG_DIR/"
if [ $? -eq 0 ]; then
    print_success "autogen.sh completed successfully"
else
    print_error "autogen.sh failed"
    exit 1
fi
./configure --prefix="/usr/local" \
  --exec-prefix="/usr/local" \
  --libdir=/usr/local/lib/x86_64-linux-gnu \
  --enable-shared \
  --enable-static \
  --enable-xcrypt-compat-files >> "$LOG_DIR/libxcrypt_configure_${TIMESTAMP}.log" 2>&1
#cp "$LOG_DIR/libxcrypt_configure_${TIMESTAMP}.log" "$HOST_LOG_DIR/"
if [ $? -eq 0 ]; then
    print_success "configure completed successfully"
else
    print_error "configure failed"
    exit 1
fi

mkdir /output/libxcrypt >> "$LOG_DIR/libxcrypt_mkdir_${TIMESTAMP}.log" 2>&1
#cp "$LOG_DIR/libxcrypt_mkdir_${TIMESTAMP}.log" "$HOST_LOG_DIR/"
if [ $? -eq 0 ]; then
    print_success "output directory created successfully"
else
    print_error "failed to create output directory"
    exit 1
fi
make -j16 >> "$LOG_DIR/libxcrypt_make_${TIMESTAMP}.log" 2>&1
#cp "$LOG_DIR/libxcrypt_make_${TIMESTAMP}.log" "$HOST_LOG_DIR/"
if [ $? -eq 0 ]; then
    print_success "make completed successfully"
else
    print_error "make failed"
    exit 1
fi

make -j16 install DESTDIR=/output/libxcrypt >> "$LOG_DIR/libxcrypt_install_${TIMESTAMP}.log" 2>&1
#cp "$LOG_DIR/libxcrypt_install_${TIMESTAMP}.log" "$HOST_LOG_DIR/"
if [ $? -eq 0 ]; then
    print_success "make install completed successfully"
else
    print_error "make install failed"
    exit 1
fi

# ライブラリをrootfsにコピー
cp -r /output/libxcrypt/usr/local/lib/x86_64-linux-gnu/*    /output/await-rootfs/usr/local/lib/x86_64-linux-gnu/ >> "$LOG_DIR/libxcrypt_copy_libs_${TIMESTAMP}.log" 2>&1
cp "$LOG_DIR/libxcrypt_copy_libs_${TIMESTAMP}.log" "$HOST_LOG_DIR/"
if [ $? -eq 0 ]; then
    print_success "libxcrypt libraries copied successfully"
else
    print_error "failed to copy libxcrypt libraries"
    exit 1
fi
cp -r /output/libxcrypt/usr/local/include/*                 /output/await-rootfs/usr/local/include/ >> "$LOG_DIR/libxcrypt_copy_headers_${TIMESTAMP}.log" 2>&1
cp "$LOG_DIR/libxcrypt_copy_headers_${TIMESTAMP}.log" "$HOST_LOG_DIR/"
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

