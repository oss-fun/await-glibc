#/bin/bash
# FOR DOCKERFILE

cd /app/glibc-2.35/build

mkdir /output/glibc
../configure --exec_prefix="/usr/local" --prefix="/usr/local" --libdir=/lib/x86_64-linux-gnu libc_cv_slibdir=/usr/local/lib/x86_64-linux-gnu --enable-crypt --enable-math --enable-nss --enable-ipc --enable-locales --enable-pthread --disable-sanity-checks
make -j 16 
make -j 16 install DESTDIR=/output/await-rootfs
