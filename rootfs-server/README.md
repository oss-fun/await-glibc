# rootfs-server

This directory contains the Docker environment and configuration for serving the built custom glibc rootfs via HTTP.

## Contents

- `Dockerfile` - Docker image definition for the HTTP server
- `conf/` - Nginx configuration files
- `entrypoint.sh` - Server startup script

## Build Environment

The Docker image is based on Ubuntu 22.04 and includes:
- `nginx` - HTTP server for serving files
- `wget` - For downloading utilities
- `net-tools` and `iproute2` - For network diagnostics

## Configuration

### `conf/rootfs-server.conf`
Nginx configuration optimized for serving binary files:
- **Document Root**: `/app/data` (where the packaged rootfs is stored)
- **Port**: 80 (internal), mapped to 8101 externally
- **Directory Listing**: Enabled with human-readable sizes and timestamps
- **Binary Transfer Optimization**: 
  - Special handling for `.tar.gz` files
  - Proper Content-Type headers
  - Gzip compression disabled for binary files
  - Large file support (1000MB max)
  - Optimized TCP settings

### `entrypoint.sh`
Server startup script that:
1. Starts nginx in the background
2. Displays server configuration information
3. Shows listening ports and network status
4. Provides access URL information
5. Keeps the server running

## Usage

This component is used as the final step in the Docker Compose workflow:

```bash
docker-compose up await-rootfs-server
```

The server will be available at: `http://localhost:8101`

## Data Source

The server serves files from the `await-glibc-builder-data` volume, which contains:
- `await-rootfs.cpio.gz` - The packaged rootfs with custom glibc

## Purpose

This HTTP server provides a convenient way to download the custom-built glibc rootfs. The rootfs can be used to:
1. Create containers with the custom glibc
2. Extract and analyze the modified glibc installation
3. Serve as a base for further development

## Features

- **Auto-indexing**: Browse files through the web interface
- **Optimized for binary files**: Proper headers and transfer optimization
- **Large file support**: Can handle rootfs archives up to 1GB
- **Network diagnostics**: Startup script shows network configuration
- **Logging**: Access and error logs available via nginx