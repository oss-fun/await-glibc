# rootfs-builder

This directory contains the Docker environment and scripts for creating a base Ubuntu rootfs that will be used as the foundation for the custom glibc installation.

## Contents

- `Dockerfile` - Docker image definition for the rootfs creation environment
- `scripts/` - Scripts for downloading and preparing the rootfs

## Build Environment

The Docker image is based on Ubuntu 22.04 and includes:
- `wget` - For downloading packages
- `debootstrap` - For creating the base Ubuntu rootfs

## Scripts

### `scripts/download_rootfs.sh`
Creates a minimal Ubuntu rootfs using debootstrap:
1. Creates an Ubuntu jammy (22.04) rootfs in `/output/await-rootfs`
2. Removes the default glibc packages to prepare for custom installation:
   - `libc-bin_2.35-0ubuntu3_amd64.deb`
   - `libc6_2.35-0ubuntu3_amd64.deb`
3. Cleans up `/dev/*` to prepare for containerization

## Usage

This component is used as the first step in the Docker Compose workflow:

```bash
docker-compose up await-rootfs-builder
```

## Configuration

The rootfs creation is configured through environment variables:
- `DIST=jammy` - Ubuntu distribution codename
- `NAME=await-rootfs` - Directory name for the rootfs

## Output

The created rootfs is stored in the shared volume `await-glibc-builder-output` at `/output/await-rootfs/`, where it will be used by the subsequent glibc-builder service.

## Purpose

This base rootfs serves as the foundation where the custom-built glibc will be installed, creating a complete environment with the modified glibc library that can then be packaged and served.