# tool

This directory contains utility scripts for working with the await-glibc project.

## Contents

- `build_docker_image.sh` - Script for building Docker images
- `download_and_extract.sh` - Script for downloading and extracting the rootfs

## Scripts

### `build_docker_image.sh`
Simple script to build a Docker image from the output directory:
```bash
sudo docker build -t await-custom-image output
```

This script is used to create a Docker image from the final rootfs output.

### `download_and_extract.sh`
Downloads and extracts the custom glibc rootfs from the server:

**Features:**
- Downloads `await-rootfs.cpio.gz` from `http://localhost:8101/`
- Extracts the CPIO archive using gunzip and cpio
- Provides progress feedback during the process

**Usage:**
```bash
./download_and_extract.sh
```

**Prerequisites:**
- The rootfs-server must be running on port 8101
- `curl` and `cpio` must be available in the system

**Process:**
1. Downloads the compressed rootfs archive
2. Decompresses using gunzip
3. Extracts the CPIO archive to the current directory
4. Results in a complete rootfs directory structure

## Purpose

These utilities facilitate:
1. **Distribution**: Easy download and extraction of the custom glibc rootfs
2. **Containerization**: Building Docker images from the extracted rootfs
3. **Development**: Quick access to the built artifacts for testing and development

## Typical Workflow

1. Run the full Docker Compose pipeline to build the rootfs
2. Use `download_and_extract.sh` to get the rootfs locally
3. Use `build_docker_image.sh` to create a container image
4. Deploy or test the custom glibc environment