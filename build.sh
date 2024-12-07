#/bin/bash
# FOR DOCKERFILE

cd /app/glibc-2.35/build

../configure --prefix=/output --libdir=/lib --sysconfdir=/etc
make -j 16 
make -j 16 install
