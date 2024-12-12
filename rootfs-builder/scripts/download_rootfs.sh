#!/bin/bash

source /app/config/base_rootfs.conf
echo "${DIST}" "${NAME}"

cd /output
if [ ! -d await-rootfs ]; then
	rm -rf await-rootfs
fi

debootstrap --arch=amd64 "${DIST}" "${NAME}"
