FROM ubuntu:22.04

RUN apt update \
		&& apt install -y wget perl gcc make gawk bison python3

RUN mkdir /app
COPY ./glibc-2.35 /app/glibc-2.35
COPY ./build.sh /app/build.sh
RUN chmod +x /app/build.sh

VOLUME /output

RUN mkdir /app/glibc-2.35/build

ENTRYPOINT ["/bin/bash", "/app/build.sh"]
