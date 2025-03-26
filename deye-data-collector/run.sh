#!/bin/sh
set -euo pipefail

# Mandatory parameter check
DEVICE=${DEVICE:?"FATAL: Device path not configured - check add-on settings"}

# Default values with debug output
echo "=== Deye Data Collector Configuration ==="
echo "Core Parameters:"
echo "  Device path:    ${DEVICE}"
echo "  Baud rate:      ${BAUD:-9600} (default: 9600)"
echo "  Parity:         ${PARITY:-none} (default: none)"
echo "  Data bits:      ${DATA_BITS:-8} (default: 8)"
echo "  Stop bits:      ${STOP_BITS:-1} (default: 1)"
echo "Advanced Settings:"
echo "  Response time:  ${RESPONSE_TIME:-1000}ms (default: 1000)"
echo "  Retry attempts: ${NUMBER_OF_RETRIES:-3} (default: 3)"
echo "  HTTP port:      ${HTTP_PORT:-8080} (default: 8080)"
echo "  HTTP server:    ${HTTP_SERVER:-false} (default: false)"
echo "  Interval:	${INTERVAL:-5000} (default: 5000)"
echo "Instance Info:"
echo "  Instance name:  ${INSTANCE_NAME:-Deye Instance}"
echo "  Add-on version: ${VERSION:-unknown}"
echo "========================================"

# Build argument array with debug logging
ARGS=(
  "--device" "${DEVICE}"
  "--baud" "${BAUD:-9600}"
  "--parity" "${PARITY:-none}"
  "--data-bits" "${DATA_BITS:-8}"
  "--stop-bits" "${STOP_BITS:-1}"
  "--response-time" "${RESPONSE_TIME:-1000}"
  "--number-of-retries" "${NUMBER_OF_RETRIES:-3}"
  "--http-port" "${HTTP_PORT:-8080}"
  "--instance" "${INSTANCE_NAME:-Deye Instance}"
  "--interval" "${INTERVAL:-5000}"
  "--http-server" "${HTTP_SERVER:-false}"
  "--loop"
)


echo "[STARTUP] Initializing Deye collector with ${#ARGS[@]} parameters"

# Execute with proper signal handling
exec /app/src/deye "${ARGS[@]}"
