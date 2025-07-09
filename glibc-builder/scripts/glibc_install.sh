#!/bin/bash
# rootfsに対してglibcの適用を行う

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

# glibcのインストール
cd /app/glibc-2.35/build

make -j 16 install DESTDIR=/output/await-rootfs >> "$LOG_DIR/glibc_install_make.log" 2>&1
#cp "$LOG_DIR/glibc_install_make.log" "$HOST_LOG_DIR/"
if [ $? -eq 0 ]; then
    print_success "glibc install completed successfully"
else
    print_error "glibc install failed"
    exit 1
fi

chroot /output/await-rootfs mkdir glibc-old >> "$LOG_DIR/glibc_install_mkdir.log" 2>&1
#cp "$LOG_DIR/glibc_install_mkdir.log" "$HOST_LOG_DIR/"
if [ $? -eq 0 ]; then
    print_success "glibc-old directory created successfully"
else
    print_error "failed to create glibc-old directory"
    exit 1
fi

chroot /output/await-rootfs bash -c "cp -a /lib/x86_64-linux-gnu/*  glibc-old" >> "$LOG_DIR/glibc_install_copy1.log" 2>&1
#cp "$LOG_DIR/glibc_install_copy1.log" "$HOST_LOG_DIR/"
if [ $? -eq 0 ]; then
    print_success "glibc libraries copied to glibc-old successfully"
else
    print_error "failed to copy glibc libraries to glibc-old"
    exit 1
fi

chroot /output/await-rootfs bash -c "cp -a /usr/local/lib/x86_64-linux-gnu/* glibc-old/" >> "$LOG_DIR/glibc_install_copy2.log" 2>&1
#cp "$LOG_DIR/glibc_install_copy2.log" "$HOST_LOG_DIR/"
if [ $? -eq 0 ]; then
    print_success "local glibc libraries copied to glibc-old successfully"
else
    print_error "failed to copy local glibc libraries to glibc-old"
    exit 1
fi

chroot /output/await-rootfs bash -c "cp -a glibc-old/* /usr/local/lib/x86_64-linux-gnu" >> "$LOG_DIR/glibc_install_copy3.log" 2>&1
#cp "$LOG_DIR/glibc_install_copy3.log" "$HOST_LOG_DIR/"
if [ $? -eq 0 ]; then
    print_success "glibc libraries copied to local directory successfully"
else
    print_error "failed to copy glibc libraries to local directory"
    exit 1
fi

chroot /output/await-rootfs bash -c 'echo "/usr/local/lib/x86_64-linux-gnu" > ld_conf' >> "$LOG_DIR/glibc_install_ldconf1.log" 2>&1
#cp "$LOG_DIR/glibc_install_ldconf1.log" "$HOST_LOG_DIR/"
if [ $? -eq 0 ]; then
    print_success "ld_conf created successfully"
else
    print_error "failed to create ld_conf"
    exit 1
fi

chroot /output/await-rootfs bash -c "cat /etc/ld.so.conf >> ld_conf" >> "$LOG_DIR/glibc_install_ldconf2.log" 2>&1
#cp "$LOG_DIR/glibc_install_ldconf2.log" "$HOST_LOG_DIR/"
if [ $? -eq 0 ]; then
    print_success "ld.so.conf appended to ld_conf successfully"
else
    print_error "failed to append ld.so.conf to ld_conf"
    exit 1
fi

chroot /output/await-rootfs mv ld_conf /etc/ld.so.conf >> "$LOG_DIR/glibc_install_ldconf3.log" 2>&1
#cp "$LOG_DIR/glibc_install_ldconf3.log" "$HOST_LOG_DIR/"
if [ $? -eq 0 ]; then
    print_success "ld_conf moved to /etc/ld.so.conf successfully"
else
    print_error "failed to move ld_conf to /etc/ld.so.conf"
    exit 1
fi
chroot /output/await-rootfs ldconfig >> "$LOG_DIR/glibc_install_ldconfig.log" 2>&1
cp "$LOG_DIR/glibc_install_ldconfig.log" "$HOST_LOG_DIR/"
if [ $? -eq 0 ]; then
    print_success "ldconfig completed successfully"
else
    print_error "ldconfig failed"
    exit 1
fi

cp /app/busybox_LN /output/await-rootfs/ >> "$LOG_DIR/glibc_install_busybox.log" 2>&1
#cp "$LOG_DIR/glibc_install_busybox.log" "$HOST_LOG_DIR/"
if [ $? -eq 0 ]; then
    print_success "busybox_LN copied successfully"
else
    print_error "failed to copy busybox_LN"
    exit 1
fi

chroot /output/await-rootfs /busybox_LN -fs /usr/local/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2 /lib64/ld-linux-x86-64.so.2 >> "$LOG_DIR/glibc_install_symlink.log" 2>&1
#cp "$LOG_DIR/glibc_install_symlink.log" "$HOST_LOG_DIR/"
if [ $? -eq 0 ]; then
    print_success "symbolic link created successfully"
else
    print_error "failed to create symbolic link"
    exit 1
fi
