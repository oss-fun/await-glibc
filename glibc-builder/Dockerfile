FROM ubuntu:22.04

RUN apt update \
		&& apt install -y wget perl gcc make gawk bison python3 git autoconf automake libtool pkg-config curl 

# glibcのビルド環境の作成
RUN mkdir /app
WORKDIR /app
RUN curl -L https://www.busybox.net/downloads/binaries/1.35.0-x86_64-linux-musl/busybox_LN -o busybox_LN
RUN chmod +x /app/busybox_LN

# libcryptのビルド環境の作成
WORKDIR /app
RUN git clone https://github.com/besser82/libxcrypt.git

# ソースやスクリプト類のコピー
COPY ./glibc-2.35 /app/glibc-2.35
COPY ./glibc-builder/scripts /app/scripts
RUN mkdir /app/glibc-2.35/build
RUN chmod +x /app/scripts/*.sh
