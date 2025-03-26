#!/usr/bin/with-contenv bashio
# shellcheck shell=bash

# ==============================================================================
# Read configuration options from Home Assistant
# ==============================================================================

bashio::log.info "Loading configuration for Deye Data Collector..."

# Required parameters
DEVICE=$(bashio::config 'device')
BAUD=$(bashio::config 'baud')
PARITY=$(bashio::config 'parity')
DATA_BITS=$(bashio::config 'dataBits')
STOP_BITS=$(bashio::config 'stopBits')
RESPONSE_TIME=$(bashio::config 'responseTime')
RETRIES=$(bashio::config 'numberOfRetries')
HTTP_PORT=$(bashio::config 'http_port')
INSTANCE_NAME=$(bashio::config 'instance_name')
INTERVAL=$(bashio::config 'instance_name')

# ==============================================================================
# Log all configuration parameters
# ==============================================================================

bashio::log.info "[CONFIGURATION] Instance Name: ${INSTANCE_NAME}"
bashio::log.info "[CONFIGURATION] Device Path: ${DEVICE}"
bashio::log.info "[CONFIGURATION] Baud Rate: ${BAUD}"
bashio::log.info "[CONFIGURATION] Parity: ${PARITY}"
bashio::log.info "[CONFIGURATION] Data Bits: ${DATA_BITS}"
bashio::log.info "[CONFIGURATION] Stop Bits: ${STOP_BITS}"
bashio::log.info "[CONFIGURATION] Response Time: ${RESPONSE_TIME} ms"
bashio::log.info "[CONFIGURATION] Max Retries: ${RETRIES}"
bashio::log.info "[CONFIGURATION] HTTP Port: ${HTTP_PORT}"
bashio::log.info "[CONFIGURATION] Interval: ${INTERVAL}"

bashio::log.green "Configuration validated successfully!"

# ==============================================================================
# Pass arguments to the application
# =======================
# Pass options to your application (adjust command as needed)
exec /app/src/deye \
    --device "${DEVICE}" \
    --baud "${BAUD}" \
    --parity "${PARITY}" \
    --data-bits "${DATA_BITS}" \
    --stop-bits "${STOP_BITS}" \
    --response-time "${RESPONSE_TIME}" \
    --retries "${RETRIES}" \
    --http-port "${HTTP_PORT}" \
    --instance "${INSTANCE_NAME}" \
    --interval "${INTERVAL}"
    --loop
