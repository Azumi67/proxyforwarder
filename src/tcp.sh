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
    print_info "Checking for existing TCP forwarder processes..."
    existing_pid=$(pgrep -f "tcp_forwarder")
    if [ -n "$existing_pid" ]; then
        print_info "Killing existing TCP forwarder process (PID: $existing_pid)..."
        kill -9 "$existing_pid"
        if [ $? -eq 0 ]; then
            print_success "Existing TCP forwarder process terminated."
        else
            print_error "Failed to terminate TCP forwarder process (PID: $existing_pid)."
        fi
    else
        print_info "No existing TCP forwarder process found."
    fi
}

function kill_existing_app() {
    print_info "Checking for existing app.py processes..."
    existing_app_pid=$(pgrep -f "python app.py")
    if [ -n "$existing_app_pid" ]; then
        print_info "Killing existing app.py process (PID: $existing_app_pid)..."
        kill -9 "$existing_app_pid"
        if [ $? -eq 0 ]; then
            print_success "Existing app.py process terminated."
        else
            print_error "Failed to terminate app.py process (PID: $existing_app_pid)."
        fi
    else
        print_info "No existing app.py process found."
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
    sleep 1  
    if ps -p $FORWARDER_PID > /dev/null 2>&1; then
        print_success "TCP Forwarder started (PID: $FORWARDER_PID). Logs: tcp_forwarder.log"
    else
        print_error "Failed to start TCP Forwarder. Check tcp_forwarder.log for details."
        exit 1
    fi
}

function start_app_py() {
    kill_existing_app

    VENV_DIR="./venv"

    if [ ! -d "$VENV_DIR" ]; then
        print_error "Virtual environment not found. Please set it up first."
        exit 1
    fi

    print_info "Activating Python virtual environment..."
    source "$VENV_DIR/bin/activate"
    if [ $? -ne 0 ]; then
        print_error "Failed to activate virtual environment."
        exit 1
    fi
    print_success "Virtual environment activated."

    if [ ! -f "./app.py" ]; then
        print_error "app.py not found in the current directory."
        deactivate
        exit 1
    fi

    print_info "Starting app.py..."
    python app.py > app.log 2>&1 &
    APP_PID=$!
    sleep 1  
    if ps -p $APP_PID > /dev/null 2>&1; then
        print_success "app.py started (PID: $APP_PID). Logs: app.log"
    else
        print_error "Failed to start app.py. Check app.log for details."
        deactivate
        exit 1
    fi
}

function cleanup() {
    print_info "Termination signal received. Shutting down processes..."
    kill_existing_forwarder
    kill_existing_app
    print_success "All processes terminated. Exiting."
    exit 0
}

trap cleanup SIGINT SIGTERM

if [ "$#" -ne 1 ]; then
    print_error "No configuration file provided."
    echo -e "\nUsage: ./run_tcp.sh <config_file>"
    exit 1
else
    CONFIG_FILE=$1
    if [ ! -f "$CONFIG_FILE" ]; then
        print_error "Configuration file '$CONFIG_FILE' does not exist."
        exit 1
    fi
    export CONFIG_FILE
fi

start_tcp_forwarder
start_app_py

wait
