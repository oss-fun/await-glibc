#!/bin/bash

echo "=========================================="
echo "Starting rootfs-server..."
echo "=========================================="

# Get container IP address
CONTAINER_IP=$(hostname -I | awk '{print $1}')

# Start nginx in background
nginx -g "daemon off;" &
NGINX_PID=$!

# Give nginx time to start
sleep 2

# Display server information
echo "Server Configuration:"
echo "  Container IP: $CONTAINER_IP"
echo "  Internal Port: 80"
echo "  External Port: 8101 (mapped via docker-compose)"
echo "  Protocol: HTTP"
echo

# Check and display listening ports
echo "Listening Ports:"
if command -v ss > /dev/null 2>&1; then
    ss -tulpn | grep LISTEN | while read line; do
        echo "  $line"
    done
elif command -v netstat > /dev/null 2>&1; then
    netstat -tulpn | grep LISTEN | while read line; do
        echo "  $line"
    done
else
    echo "  Network tools not available, checking /proc/net/tcp..."
    if [ -f /proc/net/tcp ]; then
        # Convert hex to decimal for port display
        awk 'NR>1 {split($2,a,":"); printf "  TCP *:%d LISTEN\n", strtonum("0x" a[2])}' /proc/net/tcp
    fi
fi

echo
echo "Server URL: http://localhost:8101"
echo "Server is ready to serve rootfs files."
echo "=========================================="

# Keep nginx running
wait $NGINX_PID