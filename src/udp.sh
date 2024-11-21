#!/bin/bash

RESET="\033[0m"
CYAN="\033[1;36m"
GREEN="\033[1;32m"
RED="\033[1;31m"
BLUE="\033[1;34m"

function print_info() { echo -e "${BLUE}[INFO]${RESET} $1"; }
function print_success() { echo -e "${GREEN}[SUCCESS]${RESET} $1"; }
function print_error() { echo -e "${RED}[ERROR]${RESET} $1"; }

function kill_existing_forwarder() {
    print_info "Checking for existing forwarder processes..."
    existing_pid=$(pgrep -f "udp_forwarder")
    if [ -n "$existing_pid" ]; then
        print_info "Killing existing UDP forwarder process (PID: $existing_pid)..."
        kill -9 "$existing_pid"
        print_success "Existing UDP forwarder process terminated."
    else
        print_info "No existing UDP forwarder process found."
    fi
}

function start_udp_forwarder() {
    kill_existing_forwarder

    if [ ! -f "./udp_forwarder" ]; then
        print_error "UDP forwarder binary not found. Please compile it first."
        exit 1
    fi

    print_info "Starting UDP Forwarder..."
    ./udp_forwarder "$CONFIG_FILE" > udp_forwarder.log 2>&1 &
    FORWARDER_PID=$!
    print_success "UDP Forwarder started (PID: $FORWARDER_PID). Logs: udp_forwarder.log"
    wait $FORWARDER_PID
}

if [ "$#" -ne 1 ]; then
    print_error "No configuration file provided."
    echo -e "\nUsage: ./run_udp.sh <config_file>"
    exit 1
else
    CONFIG_FILE=$1
    export CONFIG_FILE
fi

start_udp_forwarder
