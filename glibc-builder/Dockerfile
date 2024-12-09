FROM ubuntu:22.04

RUN apt update \
		&& apt install -y wget perl gcc make gawk bison python3 git autoconf automake libtool pkg-config 

# glibcのビルド環境の作成
RUN mkdir /app
COPY ./glibc-2.35 /app/glibc-2.35
COPY ./glibc-builder/scripts /app/scripts
RUN chmod +x /app/scripts/*.sh

RUN mkdir /app/glibc-2.35/build

# libcryptのビルド環境の作成
WORKDIR /app
RUN git clone https://github.com/besser82/libxcrypt.git
