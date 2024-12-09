FROM ubuntu:22.04 AS base

RUN apt update \
		&& apt install -y wget debootstrap

RUN mkdir /app
WORKDIR /app
COPY rootfs-builder/config /app/config
COPY rootfs-builder/scripts /app/scripts

FROM base AS first_stage
RUN chmod +x /app/scripts/download_base_rootfs.sh 
RUN /app/scripts/download_base_rootfs.sh

FROM first_stage AS second_stage
RUN chmod +x /app/scripts/create_rootfs.sh
