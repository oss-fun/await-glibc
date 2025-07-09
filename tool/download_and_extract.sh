#!/bin/bash

# Download and extract await-rootfs.cpio.gz from localhost:8101

set -e

URL="http://localhost:8101/await-rootfs.cpio.gz"
FILENAME="await-rootfs.cpio.gz"

echo "Downloading ${FILENAME} from ${URL}..."
curl -O "${URL}"

echo "Extracting ${FILENAME}..."
gunzip -c "${FILENAME}" | cpio -i

echo "Download and extraction completed successfully."