#!/bin/bash

# Azumi art logo
logo=$(cat << "EOF"
\033[1;96m          
                 
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\033[1;93m⠀⠀⢀⣀⣀⣀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\033[1;93m⠀⠀⡀⠤⠒⠊⠉⠀⠀⠀⠀⠈⠁⠢⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⠀\033[1;93m⠀⢀⠔⠉⠀⠀⠀⠀⢀⡠⠤⠐⠒⠀⠀⠀⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\033[1;93m⠀⠀⣀⡠⠤⠤⠀⠀⠂⠐\033[1;96m⠀⠠⢤⠎⢑⡭⣽⣳⠶⣖⡶⣤⣖⣬⡽⡭⣥⣄\033[1;93m⠒⠒⠀⠐⠁⠑⢄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\033[1;93m⠀⢀⠴⠊⠁⠀⠀⠀⠀⡀⠀\033[1;96m⣠⣴⡶⣿⢏⡿⣝⡳⢧⡻⣟⡻⣞⠿⣾⡽⣳⣯⣳⣞⡻⣦⡀⠀⠀\033[1;93m⠀⠈⡆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\033[1;93m⢨⠀⠀⠀⢀⠤⠂⠁\033[1;96m⢠⣾⡟⣧⠿⣝⣮⣽⢺⣝⣳⡽⣎⢷⣫⡟⡵⡿⣵⢫⡷⣾⢷⣭⢻⣦⡄\033[1;93m⠤⡸⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\033[1;93m⠘⡄⠀⠀⠓⠂⠀\033[1;96m⣴⣿⢷⡿⣝⣻⣏⡷⣾⣟⡼⣣⢟⣼⣣⢟⣯⢗⣻⣽⣏⡾⡽⣟⣧⠿⡼⣿⣦\033[1;93m⣃⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\033[1;93m⢀⠇⠀⠀⠀⠀\033[1;96m⣼⣿⢿⣼⡻⣼⡟⣼⣧⢿⣿⣸⡧⠿⠃⢿⣜⣻⢿⣤⣛⣿⢧⣻⢻⢿⡿⢧⣛⣿⣧⠀\033[1;93m⠛⠤⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\033[1;93m⠀⢸⠁⠀⠀⠀⠀\033[1;96m⣼⣻⡿⣾⣳⡽⣾⣽⡷⣻⣞⢿⣫⠕⣫⣫⣸⢮⣝⡇⠱⣏⣾⣻⡽⣻⣮⣿⣻⡜⣞⡿⣷\033[1;93m⢀⠀⠀⠑⠢⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\033[1;93m⠘⣧⠀⠀⠀\033[1;96m⣼⣳⢯⣿⣗⣿⣏⣿⠆⣟⣿⣵⢛⣵⡿⣿⣏⣟⡾⣜⣻⠀⢻⡖⣷⢳⣏⡶⣻⡧⣟⡼⣻⡽⣇\033[1;93m⠁⠢⡀⠠⡀⠑⡄⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\033[1;93m⠀⠈⢦⠀\033[1;96m⣰⣯⣟⢯⣿⢾⣹⢾⡟⠰⣏⡾⣾⣟⡷⣿⣻⣽⣷⡶⣟⠿⡆⠀⢻⣝⣯⢷⣹⢧⣿⢧⡻⣽⣳⢽⡀\033[1;93m⠀⠈⠀⠈⠂⡼⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\033[1;93m⠀⠀⡀⢵\033[1;96m⣟⣾⡟⣾⣿⣻⢽⣺⠇⠀⣿⡱⢿⡞⣵⡳⣭⣿⡜⣿⣭⣻⣷⠲⠤⢿⣾⢯⢯⣛⢿⣳⡝⣾⣿⢭⡇⠀\033[1;93m⠀⠀⠀⡰⠁⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\033[1;93m⢀⠤⠊⠀\033[1;96m⣼⢻⣿⢞⣯⢿⡽⣸⣹⡆⠀⢷⣏⢯⣿⣧⣛⠶⣯⢿⣽⣷⣧⣛⣦⠀⠀⠙⢿⣳⣽⣿⣣⢟⡶⣿⣫⡇⠀⠀\033[1;93m⠀⠰⠁⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀\033[1;93m⣠⠖⠁⠀⠀⡄\033[1;96m⡿⣯⣷⣻⡽⣞⡟⣿⣿⣟⠉⠈⢯⣗⣻⣕⢯⣛⡞⣯⢮⣷⣭⡚⠓⠋⠀⠀⠀⠈⠉⣿⡽⣎⠷⡏⡷⣷⠀⠀⠀\033[1;93m⠀⡇⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀\033[1;93m⠐⣇⠀⠀⢀⠊\033[1;96m⣼⣇⣿⡗⣿⣽⣷⡿⣿⣱⡿⣆⠀⠀⠙⠒⠛⠓⠋⠉⠉⠀⠀⠀\033[1;91m⢠⣴⣯⣶⣶⣤⡀\033[1;96m ⠀⣿⣟⡼⣛⡇⣟⣿⡆\033[1;93m⡀⠀⢀⠇⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀\033[1;93m⠀⠘⢤⠀⠃⠌\033[1;96m⣸⣿⢾⡽⣹⣾⠹⣞⡵⣳⣽⡽⣖⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\033[1;91m⣤⣖⣻⣾⣝⢿⡄\033[1;96m ⢸⣯⢳⣏⡿⣏⣾⢧\033[1;93m⠈⠉⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀\033[1;93m⠀⠘⠀⠈⠀\033[1;96m⡿⣿⣻⡽⣽⣿⢧⠌⠉\033[1;91m⠉⣴⣿⣿⣫⣅⡀⠀⠀⠀⠀⠀⠀⠀⠀⣸⣛⠿⠿⢟⢙⡄⠙\033[1;96m ⠘⣯⢳⣞⡟⣯⢾⣻⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀\033[1;93m⠀⡇⠀⠀⠀\033[1;96m⡿⣿⣿⢵⣫⣿⣆⠁⠂\033[1;91m⣼⡿⢹⣿⡿⠽⠟⢢⠀⠀⠀⠀⠀⠀⠀⢹⠀⢄⢀⠀⡿⠀⠀\033[1;96m ⢰⣯⢷⣺⣏⣯⢻⡽⡆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀\033[1;93m⠀⡇⠀⢀⠠\033[1;96m⣿⣿⢾⣛⡶⣽⠈⢓⠀\033[1;91m⢻⠁⢸⠇⠀⠀⠀⢸⠀⠀⠀⠀⠀⠀⠀⠀⠑⠠⠤⠔⠂⠀⠀\033[1;96m ⢸⣿⢮⣽⠿⣜⣻⡝⣧⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀\033[1;93m⠀⠑⠊⠁\033[1;96m⢠⡷⡇⣿⣿⢼⣹⡀⠀⠑⢄⠀\033[1;91m⠀⠃⠌⣁⠦⠟⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠂⠀⠀\033[1;96m⢀⣿⢾⡝⣾⡽⣺⢽⣹⣽⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣻⢽⣻⡟⣮⣝⡷⢦⣄⣄⣢⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⣾⣯⢿⡺⣟⢷⡹⢾⣷⡞⣧⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣟⡿⣎⢿⡽⣳⢮⣿⣹⣾⣯⡝⣷⣄⠀⠀⠀⠀⠀⠀⠀⠀⠃⠀⠀⠀⠀⠀⠀⣀⣴⡟⣿⢧⣏⢷⡟⣮⠝⢿⣹⣯⡽⣆⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢰⣯⡷⣏⣾⡳⣽⢺⣷⡹⣟⢶⡹⣾⡽⣷⣤⣀⡀⠀⠀⠀⠀⠀⠀⠀⠀⡀⠔⣾⢯⣷⡇⣿⢳⣎⢿⡞⣽⢦⣼⡽⣧⢻⡽⣆⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣟⢾⡷⣭⣿⢳⣭⢻⣷⡻⣜⣻⡵⣻⡼⣿⠾⠫\033[1;96m⣽⣟⣶⣶⣶⠒⠒⠂⠉⠀\033[1;96m⢸⣽⢺⡷⣷⣯⢗⣮⣟⢾⢧⣻⠼⡿⣿⢣⡟⣼⣆⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⡾⣝⣾⢳⢧⣟⡳⣎⣿⣿⣱⢏⣾⣽⣳⠟\033[1;92m⠁⠀⡌⠈\033[1;96m⢹⡯⠟⠛⠀⠀⠀⠀⠀⠈\033[1;96m⣷⢻⣼⣽⣿⡾⣼⣏⣾⣻⡜⣯⣷⢿⣟⣼⡳⣞⣦⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⢀⣾⢿⡸⣎⠿⣾⡏⣷⣉⣷⣿⢹⣎⡿\033[1;92m⠎⡎⠀⠀⠀⡇⠀⣾⠱⡀⠀⠀⠀⠀⠀⠀⠀⠈⣹⠉⡏⠀\033[1;96m⠹⣾⣏⢹⣶⢹⣶⢿⡾⣿⢶⣿⣸⠾⣇⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⣠⣾⢫⣞⡽⣯⢿⣹⡟⣶⣹⢷⣻\033[1;92m⡷⠊⠀⡜⠀⠀⠀⠀⢱⠀⣿⡀⠈⠢⢀⣀⣀⠠⠄⠒⢈⡏⡰⠀⠀⠀\033[1;96m⠀⣿⡜⣮⢟⡼⣻⡵⣻⣗⠾⣟⣯⢻⣆⠀⠀⠀⠀
⠀⠀⠀⠀⠀⢀⣴⣿⢣⣟⡾⣽⣯⢳⣿⡹⣖⣿⡳\033[1;92m⠋⠀⠀⡸⠀⠀⠀⠀⠀⢸⠀⢺⢂⠀⠀⠀⠀⠀⠀⠀⢠⡺⡱⠁⠀⠀⠀⠀\033[1;96m⢹⣧⣻⢮⡳⣝⡷⢧⣻⢯⢿⣻⣳⢞⡆⠀⠀⠀
⠀⠀⠀⠀⢀⡾⣽⣣⡿⣼⣏⡿⣼⣳⡯⢷⣹⣯⠇\033[1;92m⠀⠀⢠⠁⠀⠀⠀⠀⠀⠈⡆⠈⢹⡰⠤⡀⠀⠀⠀⢠⡼⢱⠁⠀⠀⠀⠀⠀⠀\033[1;96m⠹⣿⣿⣱⣻⣼⣏⢷⣯⣿⡳⣿⣎⢿⡀⠀⠀
⠀⠀⠀⠀⣾⣽⠷⣿⣵⡿⣼⡟⣭⣷⡟⣿⢯⡏⠀\033[1;92m⠀⠀⠘⠀⠀⠒⠈⢡⠀⠀⢗⢄⠀⠃⠀⠺⢁⢈⠥⠋⣀⠇⠀⠀⠀⠀⠀⠀⡀⠀\033[1;96m⠈⠙⢿⣳⢞⣽⢯⣞⣾⣯⡝⣿⡾⡇⠀
           \033[96m __    \033[1;94m  ________  \033[1;92m ____  ____ \033[1;93m ___      ___  \033[1;91m __     
      \033[96m     /""\   \033[1;94m ("      "\ \033[1;92m("  _||_ " |\033[1;93m|"  \    /"  | \033[1;91m|" \    
      \033[96m    /    \   \033[1;94m \___/   :)\033[1;92m|   (  ) : |\033[1;93m \   \  //   | \033[1;91m||  |   
      \033[96m   /' /\  \   \033[1;94m  /  ___/ \033[1;92m(:  |  | . )\033[1;93m /\   \/.    |\033[1;91m |:  |   
     \033[96m   //  __'  \  \033[1;94m //  \__  \033[1;92m \  \__/  / \033[1;93m|: \.        | \033[1;91m|.  |   
      \033[96m  /  /  \   \ \033[1;94m(:   / "\ \033[1;92m /\  __  /\ \033[1;93m|.  \    /:  |\033[1;91m /\  |\ 
      \033[96m(___/    \___) \033[1;94m\_______)\033[1;92m(__________)\033[1;93m|___|\__/|___|\033[1;91m(__\_|_) \033[1;92mSCRIPT   Author: github.com/Azumi67  \033[0m         
EOF
)

RESET="\033[0m"
BOLD="\033[1m"
CYAN="\033[1;36m"
BLUE="\033[1;34m"
GREEN="\033[1;32m"
YELLOW="\033[1;33m"
RED="\033[1;31m"

function display_logo() {
    echo -e "$logo"
}

function check_ulimits() {
    print_info "Checking and updating file descriptor limits..."
    current_limit=$(ulimit -n)
    if [ "$current_limit" -lt 65536 ]; then
        print_info "Current limit ($current_limit) is less than 65536. Updating..."
        ulimit -n 65536
        print_success "Temporary limit set to 65536."

        print_info "Updating /etc/security/limits.conf for permanent change..."
        if ! grep -q "hard nofile 65536" /etc/security/limits.conf; then
            echo -e "$(whoami)\thard\tnofile\t65536" | sudo tee -a /etc/security/limits.conf
            echo -e "$(whoami)\tsoft\tnofile\t65536" | sudo tee -a /etc/security/limits.conf
            print_success "/etc/security/limits.conf updated."
        else
            print_info "File descriptor limits already set in /etc/security/limits.conf."
        fi
    else
        print_success "File descriptor limit is sufficient ($current_limit)."
    fi
}

function install_yaml_cpp_json() {
    print_info "Installing json.hpp..."

    if [ ! -f "json.hpp" ]; then
        wget -O json.hpp https://github.com/nlohmann/json/releases/latest/download/json.hpp || { print_error "Failed to download json.hpp"; exit 1; }
        print_success "json.hpp downloaded."
    else
        print_info "json.hpp already exists."
    fi
}

function display_menu() {
    echo -e "${CYAN}${BOLD}=========================================${RESET}"
    echo -e "${BOLD}${BLUE}        TCP/UDP Forwarder Setup Script   ${RESET}"
    echo -e "${CYAN}${BOLD}=========================================${RESET}"
    echo -e "1) ${GREEN}Install Dependencies${RESET}"
    echo -e "2) ${GREEN}Set Up Virtual Environment${RESET}"
    echo -e "3) ${GREEN}Compile ${YELLOW}TCP${GREEN} Forwarder${RESET}"
    echo -e "4) ${GREEN}Compile ${YELLOW}UDP${GREEN} Forwarder${RESET}"
    echo -e "5) ${GREEN}Run Forwarder & Flask Server${RESET}"
    echo -e "6) ${GREEN}Run All Steps${RESET}"
    echo -e "7) ${RED}Quit${RESET}"
    echo -e "${CYAN}${BOLD}=========================================${RESET}"
}

function print_info() { echo -e "${BLUE}[INFO]${RESET} $1"; }
function print_success() { echo -e "${GREEN}[SUCCESS]${RESET} $1"; }
function print_warning() { echo -e "${YELLOW}[WARNING]${RESET} $1"; }
function print_error() { echo -e "${RED}[ERROR]${RESET} $1"; }

function install_stuff() {
    print_info "Detecting Linux & architecture..."
    . /etc/os-release
    OS_TYPE=$NAME
    ARCH=$(uname -m)

    print_info "Detected OS: $OS_TYPE, Architecture: $ARCH"

    if [ "$EUID" -ne 0 ]; then
        print_error "Please run as root."
        exit 1
    fi

    print_info "Updating Stuff..."
    sudo apt-get update -y

    print_info "Installing required stuff..."
    sudo apt-get install -y g++ libboost-all-dev libyaml-cpp-dev python3 python3-venv python3-pip net-tools iptables-persistent
    print_success "Packages installed successfully."
}

function setup_virtualenv() {
    local venv_dir="$(pwd)/venv"
    
    if [ ! -d "$venv_dir" ]; then
        print_info "Creating Python virtual environment..."
        python3 -m venv "$venv_dir" || { print_error "Couldn't create virtual environment."; exit 1; }
        print_success "Virtual environment created."
    else
        print_success "Virtual environment already exists."
    fi

    print_info "Activating virtual environment..."
    source "$venv_dir/bin/activate" || { print_error "Couldn't activate virtual environment."; exit 1; }
    print_success "Virtual environment activated."

    print_info "Installing Python packages..."
    pip install --upgrade pip
    pip install Flask Flask-Caching Flask-Login bcrypt pyotp pyyaml psutil scapy python-telegram-bot || {
        print_error "Couldn't install some Python packages."; exit 1;
    }
    print_success "Python packages installed successfully."
}


function install_python_stuff() {
    print_info "Installing Python packages..."
    pip install --upgrade pip
    pip install Flask Flask-Caching Flask-Login bcrypt pyotp pyyaml psutil scapy python-telegram-bot || {
        print_error "Coudln't install some Python packages."; exit 1; }
    print_success "Python packages installed successfully."
}

function compile_tcp_forwarder() {
    if [ ! -f "tcp_forwarder" ] || [ main.cpp -nt tcp_forwarder ]; then
        print_info "Compiling the TCP forwarder..."
        g++ tcp_forwarder.cpp -o tcp_forwarder -lboost_system -lyaml-cpp -pthread
        if [ $? -eq 0 ]; then
            print_success "TCP forwarder compiled successfully."
        else
            print_error "Failed to compile TCP forwarder."
            exit 1
        fi
    else
        print_success "TCP forwarder is already compiled."
    fi
}

function compile_udp_forwarder() {
    if [ ! -f "udp_forwarder" ] || [ main.cpp -nt udp_forwarder ]; then
        print_info "Compiling the UDP forwarder..."
        g++ udp_forwarder.cpp -o udp_forwarder -lboost_system -lyaml-cpp -pthread
        if [ $? -eq 0 ]; then
            print_success "UDP forwarder compiled successfully."
        else
            print_error "Failed to compile UDP forwarder."
            exit 1
        fi
    else
        print_success "UDP forwarder is already compiled."
    fi
}

function kill_forwarder() {
    print_info "Checking for existing forwarder processes..."
    existing_pid=$(pgrep -f "tcp_forwarder|udp_forwarder")
    if [ -n "$existing_pid" ]; then
        print_warning "Existing forwarder process found (PID: $existing_pid). Killing it..."
        kill -9 "$existing_pid"
        print_success "Existing forwarder process terminated."
    else
        print_info "No existing forwarder process found."
    fi
}

function start_services() {
    echo -e "${CYAN}${BOLD}=========================================${RESET}"
    echo -e "${BOLD}${BLUE}      Choose Forwarder to Run            ${RESET}"
    echo -e "${CYAN}${BOLD}=========================================${RESET}"
    echo -e "1) ${GREEN}Run TCP Forwarder${RESET}"
    echo -e "2) ${GREEN}Run UDP Forwarder${RESET}"
    echo -e "3) ${YELLOW}Back to main menu${RESET}"
    echo -e "${CYAN}${BOLD}=========================================${RESET}"

    while true; do
        read -p "Select an option: " forwarder_choice
        case $forwarder_choice in
            1)
                FORWARDER_EXEC="./tcp_forwarder"
                print_info "Selected TCP forwarder."
                break
                ;;
            2)
                FORWARDER_EXEC="./udp_forwarder"
                print_info "Selected UDP forwarder."
                break
                ;;
            3)
                print_info "Cancelled. Returning to main menu."
                return
                ;;
            *)
                print_error "Invalid choice. Please select 1, 2, or 3."
                ;;
        esac
    done

    kill_forwarder

    print_info "Activating virtual environment..."
    source "$(pwd)/venv/bin/activate" || { print_error "Couldn't activate virtual environment."; exit 1; }

    print_info "Starting Flask server..."
    "$(pwd)/venv/bin/python" app.py > flask.log 2>&1 &
    FLASK_PID=$!

    print_info "Starting Forwarder..."
    $FORWARDER_EXEC "$CONFIG_FILE" > forwarder.log 2>&1 &
    FORWARDER_PID=$!

    print_success "Both services are now running. Logs: flask.log, forwarder.log"
    print_info "Flask PID: $FLASK_PID, Forwarder PID: $FORWARDER_PID"

    sleep 2  
    wait $FORWARDER_PID $FLASK_PID
}


function run_all_steps() {
    check_ulimits
    install_stuff
    install_yaml_cpp_json
    setup_virtualenv
    install_python_stuff
    compile_tcp_forwarder
    compile_udp_forwarder
    start_services
}

function main() {
    while true; do
        display_menu
        read -p "Select an option: " choice
        case $choice in
            1) check_ulimits && install_stuff && install_yaml_cpp_json ;;
            2)
                setup_virtualenv
                install_python_stuff
                ;;
            3) compile_tcp_forwarder ;;
            4) compile_udp_forwarder ;;
            5)
                setup_virtualenv
                start_services
                ;;
            6)
                run_all_steps
                ;;
            7)
                print_info "quiting.."
                exit 0
                ;;
            *)
                print_error "Wrong choice. Plz try again."
                ;;
        esac
    done
}

if [ "$#" -ne 1 ]; then
    print_error "No configuration file provided."
    echo -e "\nUsage: ./script.sh <config_file>"
    exit 1
else
    CONFIG_FILE=$1
    export CONFIG_FILE
fi


display_logo
main
