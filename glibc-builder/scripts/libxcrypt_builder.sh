#!/bin/bash
# FOR DOCKERFILE

export LD_LIBRARY_PATH=/output/glibc/usr/lib/x86_64-linux-gnu
export CC="gcc -L/output/glibc/usr/lib/x86_64-linux-gnu"
export CFLAGS="-I/output/glibc/usr/include"

cd /app/libxcrypt
./autogen.sh
./configure --prefix="/usr/local" \
  --exec-prefix="/usr/local" \
  --libdir=/usr/local/lib/x86_64-linux-gnu \
  --enable-shared \
  --enable-static \
  --enable-xcrypt-compat-files

mkdir /output/libxcrypt
make -j16
make -j16 install DESTDIR=/output/libxcrypt

# ライブラリをrootfsにコピー
cp -r /output/libxcrypt/usr/local/lib/x86_64-linux-gnu/*    /output/await-rootfs/usr/local/lib/x86_64-linux-gnu/
cp -r /output/libxcrypt/usr/local/include/*                 /output/await-rootfs/usr/local/include/

# debootstrapではman3やman5のディレクトリが生成されないためここで作成する
#mkdir /output/await-rootfs/usr/local/share/man/man3
#mkdir /output/await-rootfs/usr/local/share/man/man5

# manのコピー
#cp -r /output/libxcrypt/usr/local/share/man/man3/*          /output/await-rootfs/usr/local/share/man/man3/
#cp -r /output/libxcrypt/usr/local/share/man/man5/*          /output/await-rootfs/usr/local/share/man/man5/

