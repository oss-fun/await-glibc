#!/bin/bash

echo "${DIST}" "${NAME}"

cd /output


if [ -d /output/await-rootfs ]; then
	exit
	#rm -rf await-rootfs
	#echo "await-rootfs is exist. remove await-rootfs"
fi

debootstrap --arch=amd64 "${DIST}" "${NAME}"
rm await-rootfs/var/cache/apt/archives/libc-bin_2.35-0ubuntu3_amd64.deb 
rm await-rootfs/var/cache/apt/archives/libc6_2.35-0ubuntu3_amd64.deb
rm -rf await-rootfs/dev/*

