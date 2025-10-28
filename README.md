# await-glibc

This project builds a custom glibc (GNU C Library) version 2.35 with specific modifications for await functionality.

## Directory Structure

- `glibc-2.35/` - Modified glibc 2.35 source code
- `glibc-builder/` - Docker environment and scripts for building glibc
- `rootfs-builder/` - Scripts for building the base rootfs
- `rootfs-server/` - HTTP server for distributing the built rootfs
- `logs/` - Build and installation logs

## Quick Start

```bash
docker-compose up
```

## Manual Build

```bash
mkdir glibc-2.35/build
mkdir local

cd glibc-2.35/build
../configure --prefix=/$HOME/await-glibc/local
make -j16
make install
```


