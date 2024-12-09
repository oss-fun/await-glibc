#!/bin/bash

cd /output

CONF=/app/config/base_rootfs.conf
conf=($(cat "$CONF"))
echo "${conf[0]}" "${conf[1]}"
debootstrap --arch=amd64 "${conf[0]}" "${conf[1]}"

