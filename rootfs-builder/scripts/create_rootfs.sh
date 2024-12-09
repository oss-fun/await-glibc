#!/bin/sh

# rootfs作成時にlibcが更新されないようにする
#rm -rf /app/await-rootfs/var/cache/apt/archives/libc6_2.35-0ubuntu3_amd64.deb

# カスタムglibcのインストール
#cp -r /output/glibc/usr/lib/x86_64-linux-gnu/* /app/await-rootfs/usr/lib/x86_64-linux-gnu/
#ln -sf /lib/x86_64-linux-gnu/libcrypt.so.1 /usr/lib/x86_64-linux-gnu/libcrypt.so
