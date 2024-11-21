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
    existing_pid=$(pgrep -f "tcp_forwarder")
    if [ -n "$existing_pid" ]; then
        print_info "Killing existing TCP forwarder process (PID: $existing_pid)..."
        kill -9 "$existing_pid"
        print_success "Existing TCP forwarder process terminated."
    else
        print_info "No existing TCP forwarder process found."
    fi
}

function start_tcp_forwarder() {
    kill_existing_forwarder

    if [ ! -f "./tcp_forwarder" ]; then
        print_error "TCP forwarder binary not found. Please compile it first."
        exit 1
    fi

    print_info "Starting TCP Forwarder..."
    ./tcp_forwarder "$CONFIG_FILE" > tcp_forwarder.log 2>&1 &
    FORWARDER_PID=$!
    print_success "TCP Forwarder started (PID: $FORWARDER_PID). Logs: tcp_forwarder.log"
    wait $FORWARDER_PID
}

if [ "$#" -ne 1 ]; then
    print_error "No configuration file provided."
    echo -e "\nUsage: ./run_tcp.sh <config_file>"
    exit 1
else
    CONFIG_FILE=$1
    export CONFIG_FILE
fi

start_tcp_forwarder
