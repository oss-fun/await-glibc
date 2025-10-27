#!/bin/bash
# rootfsに対してglibcの適用を行う

# ログディレクトリの作成
source /app/scripts/log_controller.sh
log_init glibc_install

TIMESTAMP=$(date +"%Y%m%d_%H%M%S")

# glibcのインストール
cd /app/glibc-2.35/build

run_silent "glibc_install" "#1 install glibc" make -j 16 install DESTDIR=/output/await-rootfs
log_error_check "#1 install glibc"

run_silent "glibc_install" "#2 create glibc dir" chroot /output/await-rootfs mkdir -p glibc-old
log_error_check "#2 create glibc dir"

run_silent "glibc_install" "#3 copy old glibc" chroot /output/await-rootfs bash -c "cp -a /lib/x86_64-linux-gnu/*  glibc-old"
log_error_check "#3 copy old glibc"

run_silent "glibc_install" "#4 copy old glibc 2" chroot /output/await-rootfs bash -c "cp -a /usr/local/lib/x86_64-linux-gnu/* glibc-old/"
log_error_check "#4 copy old glibc 2"

chroot /output/await-rootfs bash -c "ls -al glibc-old"

run_silent "glibc_install" "#5 merge old glibc" chroot /output/await-rootfs bash -c "cp -a glibc-old/* /usr/local/lib/x86_64-linux-gnu"
log_error_check "#5 merge old glibc"

run_silent "glibc_install" "#6 add ldconf " chroot /output/await-rootfs bash -c 'echo "/usr/local/lib/x86_64-linux-gnu" > ld_conf'
log_error_check "#6 setting ldconf"

run_silent "glibc_install" "#7 merge ldconf" chroot /output/await-rootfs bash -c "cat /etc/ld.so.conf >> ld_conf"
log_error_check "#7 merge ldconf"

run_silent "glibc_install" "#8 ldconf setting" chroot /output/await-rootfs mv ld_conf /etc/ld.so.conf
log_error_check "#8 ldconf setting"

run_silent "glibc_install" "#9 do ldconfig" chroot /output/await-rootfs ldconfig
log_error_check "#9 do ldconfig"

run_silent "glibc_install" "#10 copy busybox" cp /app/busybox_LN /output/await-rootfs/
log_error_check "#10 copy busybox"

run_silent "glibc_install" "#11 set link to new glibc" chroot /output/await-rootfs /busybox_LN -fs /usr/local/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2 /lib64/ld-linux-x86-64.so.2
log_error_check "#11 set link to new glibc"

exit 0
