#!/bin/bash
source "$(dirname "$0")/../.env"
if [ -f /tmp/banking_backend.pid ]; then
  PID=$(cat /tmp/banking_backend.pid)
  kill $PID 2>/dev/null && echo "Backend stopped (PID $PID)" || echo "Backend not running"
  rm -f /tmp/banking_backend.pid
else
  echo "No PID file found"
fi
