#!/bin/bash

echo "${DIST}" "${NAME}"

cd /output
if [ -d /output/await-rootfs ]; then
	rm -rf await-rootfs
	echo "await-rootfs is exist. remove await-rootfs"
fi

debootstrap --arch=amd64 "${DIST}" "${NAME}"
