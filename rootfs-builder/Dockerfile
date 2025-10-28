FROM ubuntu:22.04

RUN apt update \
		&& apt install -y wget debootstrap

RUN mkdir /app
WORKDIR /app

COPY ./rootfs-builder/scripts /app/scripts
RUN chmod +x /app/scripts/download_rootfs.sh
