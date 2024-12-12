#!/bin/bash
# rootfsに対してglibcの適用を行う

chroot /output/await-rootfs mkdir glibc-old
chroot /output/await-rootfs bash -c "cp -a /lib/x86_64-linux-gnu/*  glibc-old"
chroot /output/await-rootfs bash -c "cp -a /usr/local/lib/x86_64-linux-gnu/* glibc-old/"
chroot /output/await-rootfs bash -c "cp -a glibc-old/* /usr/local/lib/x86_64-linux-gnu"

chroot /output/await-rootfs bash -c 'echo "/usr/local/lib/x86_64-linux-gnu" > ld_conf'
chroot /output/await-rootfs bash -c "cat /etc/ld.so.conf >> ld_conf"
chroot /output/await-rootfs mv ld_conf /etc/ld.so.conf
chroot /output/await-rootfs ldconfig
cp /app/busybox_LN /output/await-rootfs/

chroot /output/await-rootfs /busybox_LN -fs /usr/local/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2 /lib64/ld-linux-x86-64.so.2
