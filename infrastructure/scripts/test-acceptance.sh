#!/bin/bash
set -e
source "$(dirname "$0")/../.env"
export BACKEND_PORT
cd "$(dirname "$0")/../.."
cmake -B build -DCMAKE_BUILD_TYPE=Debug 2>&1
cmake --build build --target acceptance_tests --parallel 2>&1
./build/acceptance/acceptance_tests "$@"
