#/bin/bash
# FOR DOCKERFILE

export LD_LIBRARY_PATH=/output/glibc/usr/lib/x86_64-linux-gnu
export CC="gcc -L/output/glibc/usr/lib/x86_64-linux-gnu"
export CFLAGS="-I/output/glibc/usr/include"

cd /app/libxcrypt
./autogen.sh
./configure \
    --prefix=/ \
    --exec_prefix=/ \
    --libdir=/usr/lib/x86_64-linux-gnu \
    --enable-shared \
    --enable-static \
    --enable-xcrypt-compat-files

make
make install DESTDIR=/output/glibc/usr/lib/x86_64-linux-gnu
