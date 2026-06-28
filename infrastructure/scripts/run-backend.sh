#!/bin/bash
set -e
source "$(dirname "$0")/../.env"
cd "$(dirname "$0")/../.."
cmake -B build -DCMAKE_BUILD_TYPE=Debug 2>&1
cmake --build build --target banking_server --parallel 2>&1
./build/backend/server/banking_server $BACKEND_PORT &
echo $! > /tmp/banking_backend.pid
echo "Backend started on port $BACKEND_PORT (PID $(cat /tmp/banking_backend.pid))"
