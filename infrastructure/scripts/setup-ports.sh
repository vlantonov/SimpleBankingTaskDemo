#!/bin/bash
# Generate port configuration
REPO_INDEX=${REPO_INDEX:-0}
BACKEND_PORT=$((8080 + REPO_INDEX))
cat > "$(dirname "$0")/../.env" <<EOF
BACKEND_PORT=$BACKEND_PORT
EOF
echo "Ports configured: BACKEND_PORT=$BACKEND_PORT"
