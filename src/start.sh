#!/bin/bash

# Colors for stylish output
RESET="\033[0m"
BLUE="\033[1;34m"
GREEN="\033[1;32m"
RED="\033[1;31m"

# Function to print messages with styles
function print_info() {
    echo -e "${BLUE}[INFO]${RESET} $1"
}

function print_success() {
    echo -e "${GREEN}[SUCCESS]${RESET} $1"
}

function print_error() {
    echo -e "${RED}[ERROR]${RESET} $1"
}

# Path to your project directory
PROJECT_DIR="/path/to/your/script/directory"
CONFIG_FILE="$PROJECT_DIR/config.yaml"

# Function to start the services
function start_services() {
    # Activate the virtual environment
    print_info "Activating virtual environment..."
    source "$PROJECT_DIR/venv/bin/activate"

    # Start the Flask server in the background
    print_info "Starting Flask server..."
    python "$PROJECT_DIR/app.py" &
    FLASK_PID=$!

    # Start the TCP Forwarder in the background
    print_info "Starting TCP Forwarder..."
    "$PROJECT_DIR/tcp_forwarder" "$CONFIG_FILE" &
    TCP_FORWARDER_PID=$!

    # Wait for the TCP Forwarder to finish
    wait $TCP_FORWARDER_PID

    # Clean up by killing the Flask server
    print_info "Stopping Flask server..."
    kill $FLASK_PID 2>/dev/null

    print_success "Services stopped successfully."
}

# Run the start_services function
start_services


service : 
[Unit]
Description=TCP Forwarder and Flask Server
After=network.target

[Service]
Type=simple
User=your-username
WorkingDirectory=/path/to/your/script/directory
ExecStart=/path/to/your/script/directory/start_services.sh
Restart=on-failure
Environment="PATH=/path/to/your/script/directory/venv/bin:$PATH"

[Install]
WantedBy=multi-user.target

chmod +x /path/to/your/script/directory/start_services.sh

sudo systemctl daemon-reload

sudo systemctl enable tcp_forwarder.service

sudo systemctl start tcp_forwarder.service
