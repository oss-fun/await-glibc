# glibc-builder

This directory contains the Docker environment and scripts for building a custom glibc (GNU C Library) version 2.35 with specific modifications.

## Contents

- `Dockerfile` - Docker image definition for the glibc build environment
- `glibc-2.35/` - Modified glibc 2.35 source code
- `scripts/` - Build and installation scripts
- `test_code/` - Test programs for validation

## Build Environment

The Docker image is based on Ubuntu 22.04 and includes:
- Build tools: gcc, make, gawk, bison, python3, git
- Configuration tools: autoconf, automake, libtool, pkg-config
- Utilities: wget, perl, curl
- BusyBox for testing

## Scripts

### `scripts/exec_all.sh`
Main orchestration script that executes the entire build process:
1. Builds glibc using `glibc_builder.sh`
2. Installs glibc using `glibc_install.sh`
3. Builds and installs libxcrypt using `libxcrypt_builder.sh`

Each step is logged with timestamps to `/logs/` directory.

### `scripts/glibc_builder.sh`
Configures and builds the glibc source code.

### `scripts/glibc_install.sh`
Installs the built glibc into the target rootfs.

### `scripts/libxcrypt_builder.sh`
Builds and installs libxcrypt, which is required for modern glibc.

### `scripts/test_preopen.sh`
Test script for validating the preopen functionality.

## Usage

This component is used as part of the Docker Compose workflow:

```bash
docker-compose up await-glibc-builder
```

The build process will:
1. Configure glibc with custom modifications
2. Build the library
3. Install it into the target rootfs
4. Build and install libxcrypt
5. Generate logs for each step

## Output

The built glibc is installed into the shared volume `await-glibc-builder-output` which is then used by subsequent services in the pipeline.

## Logs

All build logs are stored in the `logs/` directory with timestamps:
- `glibc_builder_YYYYMMDD_HHMMSS.log` - Build logs
- `glibc_install_YYYYMMDD_HHMMSS.log` - Installation logs
- `libxcrypt_builder_YYYYMMDD_HHMMSS.log` - libxcrypt build logs