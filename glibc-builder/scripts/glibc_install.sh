#!/bin/bash
# rootfsに対してglibcの適用を行う

# ログディレクトリの作成
source /app/scripts/log_controller.sh
log_init glibc_install

TIMESTAMP=$(date +"%Y%m%d_%H%M%S")

# glibcのインストール
cd /app/glibc-2.35/build

log_message "glibc_install_1" make -j 16 install DESTDIR=/output/await-rootfs
if [ $? -eq 0 ]; then
    print_success "glibc install completed successfully"
else
    print_error "glibc install failed"
    exit 1
fi

log_message "glibc_install_2" chroot /output/await-rootfs mkdir -p glibc-old
if [ $? -eq 0 ]; then
    print_success "glibc-old directory created successfully"
else
    print_error "failed to create glibc-old directory"
    exit 1
fi

log_message "glibc_install_3" chroot /output/await-rootfs bash -c "cp -a /lib/x86_64-linux-gnu/*  glibc-old"
if [ $? -eq 0 ]; then
    print_success "glibc libraries copied to glibc-old successfully"
else
    print_error "failed to copy glibc libraries to glibc-old"
    exit 1
fi

log_message "glibc_install_4" chroot /output/await-rootfs bash -c "cp -a /usr/local/lib/x86_64-linux-gnu/* glibc-old/"
if [ $? -eq 0 ]; then
    print_success "local glibc libraries copied to glibc-old successfully"
else
    print_error "failed to copy local glibc libraries to glibc-old"
    exit 1
fi

log_message "glibc_install_5" chroot /output/await-rootfs bash -c "cp -a glibc-old/* /usr/local/lib/x86_64-linux-gnu"
if [ $? -eq 0 ]; then
    print_success "glibc libraries copied to local directory successfully"
else
    print_error "failed to copy glibc libraries to local directory"
    exit 1
fi

log_message "glibc_install_6" chroot /output/await-rootfs bash -c 'echo "/usr/local/lib/x86_64-linux-gnu" > ld_conf'
if [ $? -eq 0 ]; then
    print_success "ld_conf created successfully"
else
    print_error "failed to create ld_conf"
    exit 1
fi

log_message "glibc_install_7" chroot /output/await-rootfs bash -c "cat /etc/ld.so.conf >> ld_conf"
if [ $? -eq 0 ]; then
    print_success "ld.so.conf appended to ld_conf successfully"
else
    print_error "failed to append ld.so.conf to ld_conf"
    exit 1
fi

log_message "glibc_install_8" chroot /output/await-rootfs mv ld_conf /etc/ld.so.conf
if [ $? -eq 0 ]; then
    print_success "ld_conf moved to /etc/ld.so.conf successfully"
else
    print_error "failed to move ld_conf to /etc/ld.so.conf"
    exit 1
fi

log_message "glibc_install_9" chroot /output/await-rootfs ldconfig
if [ $? -eq 0 ]; then
    print_success "ldconfig completed successfully"
else
    print_error "ldconfig failed"
    exit 1
fi

log_message "glibc_install_10" cp /app/busybox_LN /output/await-rootfs/
if [ $? -eq 0 ]; then
    print_success "busybox_LN copied successfully"
else
    print_error "failed to copy busybox_LN"
    exit 1
fi

log_message "glibc_install_11" chroot /output/await-rootfs /busybox_LN -fs /usr/local/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2 /lib64/ld-linux-x86-64.so.2
if [ $? -eq 0 ]; then
    print_success "symbolic link created successfully"
else
    print_error "failed to create symbolic link"
    exit 1
fi
