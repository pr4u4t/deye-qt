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
DATA_BITS=$(bashio::config 'data_bits')
STOP_BITS=$(bashio::config 'stop_bits')
RESPONSE_TIME=$(bashio::config 'response_time')
RETRIES=$(bashio::config 'number_of_retries')
HTTP_PORT=$(bashio::config 'http_port')
INSTANCE_NAME=$(bashio::config 'instance_name')
INTERVAL=$(bashio::config 'interval')
HTTP_SERVER=$(bashio::config 'http_server')

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
bashio::log.info "[CONFIGURATION] HTTP Server: ${HTTP_SERVER}"

bashio::log.green "Configuration validated successfully!"

# ==============================================================================
# Pass arguments to the application
# ==============================================================================

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
    --interval "${INTERVAL}" \
    --http-server "${HTTP_SERVER}" \
    --loop
