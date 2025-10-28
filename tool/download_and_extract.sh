#!/bin/bash

# Download and extract await-rootfs.cpio.gz from localhost:8101

set -e

if [ $# -ne 1 ]; then
    echo "Usage: $0 <filename>"
    exit 1
fi

FILENAME="$1"
URL="http://localhost:8101/${FILENAME}"

echo "Downloading ${FILENAME} from ${URL}..."
curl -O "${URL}"

echo "Extracting ${FILENAME}..."
gunzip -c "${FILENAME}" | cpio -i

echo "Download and extraction completed successfully."