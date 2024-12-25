import os
import psutil
import time
import yaml
from flask import Flask, render_template, request, jsonify
from flask_caching import Cache
import subprocess
import datetime
import threading
import signal
import secrets
import json
from scapy.all import sniff, IP, TCP
import bcrypt
import pyotp
from flask import Flask, render_template, request, redirect, url_for, flash, jsonify, session
from flask_login import LoginManager, UserMixin, login_user, login_required, logout_user, current_user

app = Flask(__name__)
app.secret_key = os.urandom(24)
cache = Cache(app, config={'CACHE_TYPE': 'simple'})

with open("config.yaml", "r") as file:
    config = yaml.safe_load(file)


login_manager = LoginManager()
login_manager.init_app(app)
login_manager.login_view = "login"

user_db = "user_data.json" 

if os.path.exists(user_db):
    with open(user_db, "r") as file:
        users = json.load(file)
else:
    users = {}

class User(UserMixin):
    def __init__(self, username):
        self.id = username
        self.username = username

    @staticmethod
    def get(username):
        if username in users:
            return User(username)
        return None

    @staticmethod
    def verify_pass(username, password):
        if username in users:
            return bcrypt.checkpw(password.encode(), users[username]["password"].encode())
        return False

    @staticmethod
    def gen_totpsecret(username):
        return users[username]["totp_secret"]

@login_manager.user_loader
def load_user(user_id):
    return User.get(user_id)

@app.route('/register', methods=['GET', 'POST'])
def register():
    if request.method == 'POST':
        username = request.form.get('username')
        password = request.form.get('password')

        if not username or not password:
            flash("Username and password are required.", "danger")  
            return redirect(url_for('register'))

        if username in users:
            flash("Username already exists.", "danger")  
            return redirect(url_for('register'))

        hashed_password = bcrypt.hashpw(password.encode(), bcrypt.gensalt()).decode()
        totp_secret = pyotp.random_base32() 

        users[username] = {
            "password": hashed_password,
            "totp_secret": totp_secret
        }

        with open(user_db, "w") as file:
            json.dump(users, file)

        flash("User registered successfully. Please log in.", "success") 
        return redirect(url_for('login'))

    return render_template('register.html')

@app.route('/signup', methods=['GET', 'POST'])
def signup():
    if request.method == 'POST':
        username = request.form.get('username')
        password = request.form.get('password')
        if username in users:
            flash("Username already exists.", "danger")
            return redirect(url_for('signup'))

        hashed_password = bcrypt.hashpw(password.encode(), bcrypt.gensalt())
        totp_secret = pyotp.random_base32() 

        users[username] = {
            "password": hashed_password.decode(),
            "totp_secret": totp_secret
        }
        with open(user_db, "w") as file:
            json.dump(users, file)
        flash("User registered successfully. Please log in.", "success")
        return redirect(url_for('login'))
    return render_template('signup.html')

@app.route('/login', methods=['GET', 'POST'])
def login():
    if request.method == 'POST':
        username = request.form.get('username')
        password = request.form.get('password')

        if not username or not password:
            flash("Username and password are required.", "danger") 
            return redirect(url_for('login'))

        user = User.get(username)
        if user and User.verify_pass(username, password):
            login_user(user)
            flash("Login successful.", "success")  
            return redirect(url_for('home'))
        else:
            flash("Invalid username or password.", "danger") 
    return render_template('login.html')


@app.route('/logout')
@login_required
def logout():
    logout_user()
    return redirect(url_for('login'))

@app.route('/')
@login_required
def home():
    cpu_usage = psutil.cpu_percent(interval=1)
    ram_usage = psutil.virtual_memory().percent
    uptime = system_uptime()

    return render_template("index.html", 
                           cpu_usage=cpu_usage, 
                           ram_usage=ram_usage, 
                           uptime=uptime, 
                           ports=ports)

tcp_ports = []
for forwarder in config.get("forwarders", []):
    if "port_range" in forwarder:
        port_range = forwarder.get("port_range", {})
        start_port = port_range.get("start")
        end_port = port_range.get("end")
        if start_port and end_port:
            try:
                start = int(start_port)
                end = int(end_port)
                if start > end:
                    print(f"Invalid port range: start port {start} is greater than end port {end}.")
                    continue
                tcp_ports.extend(range(start, end + 1))
            except ValueError:
                print(f"Invalid port numbers in port_range: {port_range}")
    elif "listen_port" in forwarder:
        listen_port = forwarder.get("listen_port")
        if listen_port:
            try:
                tcp_ports.append(int(listen_port))
            except ValueError:
                print(f"Invalid listen_port: {listen_port}")
    else:
        print(f"Forwarder missing 'listen_port' or 'port_range': {forwarder}")

udp_ports = []
for addr in config.get("srcAddrPorts", []):
    try:
        udp_port = int(addr.split(":")[-1])
        udp_ports.append(udp_port)
    except (ValueError, IndexError):
        print(f"Invalid srcAddrPort format: {addr}")

for addr in config.get("dstAddrPorts", []):
    try:
        udp_port = int(addr.split(":")[-1])
        udp_ports.append(udp_port)
    except (ValueError, IndexError):
        print(f"Invalid dstAddrPort format: {addr}")

ports = list(set(tcp_ports + udp_ports))
print(f"Initialized Ports: {ports}")

monitoring_port = config.get("monitoring_port", 8080)

banned_ips_file = "banned_ips.txt"
log_file = "app_log.txt"
traffic_data_file = "traffic_data.json"
traffic_data_backup_file = "traffic_data_backup.json" 
tunnel_log_file = "logfile.log"
api_keys_file = "api_keys.txt"

def write_log(message):
    timestamp = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    with open(log_file, "a") as file:
        file.write(f"[{timestamp}] {message}\n")

def save_traffic():
    try:
        with open(traffic_data_file, "w") as file:
            json.dump(traffic_data, file)

        with open(traffic_data_backup_file, "a") as backup_file:
            timestamp = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            backup_entry = {"timestamp": timestamp, "traffic_data": traffic_data}
            json.dump(backup_entry, backup_file)
            backup_file.write("\n")

        with open(traffic_data_backup_file, "r") as backup_file:
            lines = backup_file.readlines()
        if len(lines) > 100:
            with open(traffic_data_backup_file, "w") as backup_file:
                backup_file.writelines(lines[-100:]) 
    except Exception as e:
        write_log(f"saving traffic data failed: {str(e)}")

def load_recent_traffic_data():
    global traffic_data
    if os.path.exists(traffic_data_backup_file):
        try:
            with open(traffic_data_backup_file, "r") as backup_file:
                lines = backup_file.readlines()
                if lines:
                    latest_entry = json.loads(lines[-1])
                    traffic_data = latest_entry["traffic_data"]
                    save_traffic()
                    write_log("Successfully loaded recent traffic data from backup.")
        except (json.JSONDecodeError, FileNotFoundError, Exception) as e:
            write_log(f"loading recent traffic data failed: {str(e)}")
            
traffic_data = {str(port): {"bytes_sent": 0, "bytes_received": 0, "packets_sent": 0, "packets_received": 0} for port in ports}

print(f"Initialized Traffic Data: {traffic_data}")

load_recent_traffic_data()
if os.path.exists(traffic_data_file):
    try:
        with open(traffic_data_file, "r") as file:
            saved_data = json.load(file)
            for port in saved_data:
                if port in traffic_data:
                    traffic_data[port].update(saved_data[port])
    except (json.JSONDecodeError, FileNotFoundError):
        write_log("loading traffic data failed, Using initial values.")


@app.route('/ban-ip', methods=['POST'])
def ban_ip():
    ip = request.json.get('ip')
    if ip:
        ban_ip_w_iptables(ip)
        banned_ips = rcv_banned_ips()
        banned_ips.add(ip)
        save_banned_ips(banned_ips)
        return jsonify({"message": f"IP {ip} has been banned."}), 200
    return jsonify({"error": "Invalid IP address."}), 400


@app.route('/unban-ip', methods=['POST'])
def unban_ip():
    ip = request.json.get('ip')
    if ip:
        unban_ip_w_iptables(ip)
        banned_ips = rcv_banned_ips()
        banned_ips.discard(ip)
        save_banned_ips(banned_ips)
        return jsonify({"message": f"IP {ip} has been unbanned."}), 200
    return jsonify({"error": "Invalid IP address."}), 400

@app.route('/retrieve-traffic-data', methods=['POST'])
def retrieve_traffic_data():
    global traffic_data
    try:
        if os.path.exists(traffic_data_backup_file):
            with open(traffic_data_backup_file, "r") as backup_file:
                lines = backup_file.readlines()
                if lines:
                    latest_entry = json.loads(lines[-1])
                    traffic_data = latest_entry["traffic_data"]
                    save_traffic() 
                    return "Traffic data successfully restored from backup.", 200
                else:
                    return "backup data not found.", 404
        else:
            return "Backup file not found.", 404
    except Exception as e:
        write_log(f"restoring traffic data failed: {str(e)}")
        return f"error restoring traffic data: {str(e)}", 500


def monitor_traffic(packet):
    try:
        if IP in packet and TCP in packet:
            for port in ports:
                port_str = str(port)
                if port_str not in traffic_data:
                    traffic_data[port_str] = {"bytes_sent": 0, "bytes_received": 0, "packets_sent": 0, "packets_received": 0}
                
                if packet[TCP].sport == port:
                    traffic_data[port_str]["bytes_sent"] += len(packet)
                    traffic_data[port_str]["packets_sent"] += 1
                elif packet[TCP].dport == port:
                    traffic_data[port_str]["bytes_received"] += len(packet)
                    traffic_data[port_str]["packets_received"] += 1

        if IP in packet and packet.haslayer("UDP"):
            for port in ports:
                port_str = str(port)
                if port_str not in traffic_data:
                    traffic_data[port_str] = {"bytes_sent": 0, "bytes_received": 0, "packets_sent": 0, "packets_received": 0}
                
                if packet["UDP"].sport == port:
                    traffic_data[port_str]["bytes_sent"] += len(packet)
                    traffic_data[port_str]["packets_sent"] += 1
                elif packet["UDP"].dport == port:
                    traffic_data[port_str]["bytes_received"] += len(packet)
                    traffic_data[port_str]["packets_received"] += 1

        save_traffic()
    except Exception as e:
        write_log(f"Error in traffic monitoring: {str(e)}")


def start_sniffing():
    filter_expression = " or ".join([f"tcp port {port}" for port in ports] + [f"udp port {port}" for port in ports])
    sniff(filter=filter_expression, prn=monitor_traffic, store=0, count=0)


sniff_thread = threading.Thread(target=start_sniffing)
sniff_thread.daemon = True
sniff_thread.start()

def cleanup_n_exit():
    global tcp_forwarder_process
    write_log("Shutting down...")

    if tcp_forwarder_process and tcp_forwarder_process.poll() is None:
        tcp_forwarder_process.terminate()
        tcp_forwarder_process.wait()
        write_log("tcp_forwarder process terminated.")

    save_traffic()
    write_log("Cleaning complete. exiting..")
    os._exit(0)

def signal_handler(signum, frame):
    cleanup_n_exit()

signal.signal(signal.SIGINT, signal_handler)
signal.signal(signal.SIGTERM, signal_handler)


uptime_cache = {"last_valid_uptime": "Calculating.."}

def system_uptime():
    try:
        boot_time = psutil.boot_time()
        current_time = time.time()

        if boot_time > current_time:
            write_log(f"time ({boot_time}) is in the future compared to the current time ({current_time}).")
            return uptime_cache.get("last_valid_uptime", "Calculating..")

        uptime_seconds = current_time - boot_time
        if uptime_seconds < 0:
            write_log(f"negative uptime. time = {boot_time}, Current time = {current_time}.")
            return uptime_cache.get("last_valid_uptime", "Calculating..")

        uptime_string = time.strftime("%H:%M:%S", time.gmtime(uptime_seconds))
        uptime_cache["last_valid_uptime"] = uptime_string
        return uptime_string

    except Exception as e:
        write_log(f"Unhandled error in system uptime: {str(e)}")
        return uptime_cache.get("last_valid_uptime", "Calculating..")

@app.route('/uptime')
@login_required
def uptime():
    uptime_value = system_uptime()  
    return jsonify({"uptime": uptime_value})  

def generate_api_key():
    return secrets.token_hex(16)

@app.route('/api/generate-key', methods=['POST'])
def generate_key():
    new_key = generate_api_key()
    with open(api_keys_file, "a") as file:
        file.write(f"{new_key}\n")
    return jsonify({"api_key": new_key})

@app.route('/api/keys')
def list_keys():
    if not os.path.exists(api_keys_file):
        return jsonify({"api_keys": []})
    with open(api_keys_file, "r") as file:
        keys = file.read().splitlines()
    return jsonify({"api_keys": keys})

@app.route('/api.html')
def api_page():
    return render_template("api.html")

@app.route('/shutdown', methods=['POST'])
def shutdown():
    cleanup_n_exit()
    return "Program stopped."

forwarder_processes = {
    "tcp_forwarder": None,
    "udp_forwarder": None
}
def start_forwarder(process_name, config_file="config.yaml"):
    global forwarder_processes
    try:
        command = [f"./{process_name}", config_file]
        process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        forwarder_processes[process_name] = process
        write_log(f"{process_name} started successfully.")
    except Exception as e:
        write_log(f"Error starting {process_name}: {str(e)}")

def stop_forwarder(process_name):
    global forwarder_processes
    process = forwarder_processes.get(process_name)
    if process and process.poll() is None:  
        process.terminate()  
        process.wait()  
        write_log(f"{process_name} stopped.")
        forwarder_processes[process_name] = None
    else:
        write_log(f"{process_name} is not running or already stopped.")

def restart_forwarder(process_name):
    stop_forwarder(process_name)  
    start_forwarder(process_name) 

@app.route('/restart-tcp-forwarder', methods=['POST'])
def restart_tcp_forwarder_route():
    restart_forwarder("tcp_forwarder")
    return jsonify({"message": "TCP forwarder restarted successfully."}), 200

@app.route('/restart-udp-forwarder', methods=['POST'])
def restart_udp_forwarder_route():
    restart_forwarder("udp_forwarder")
    return jsonify({"message": "UDP forwarder restarted successfully."}), 200

@app.route('/stop-tcp-forwarder', methods=['POST'])
def stop_tcp_forwarder_route():
    stop_forwarder("tcp_forwarder")
    return jsonify({"message": "TCP forwarder stopped successfully."}), 200

@app.route('/stop-udp-forwarder', methods=['POST'])
def stop_udp_forwarder_route():
    stop_forwarder("udp_forwarder")
    return jsonify({"message": "UDP forwarder stopped successfully."}), 200


@app.route('/public-ip-settings')
def public_ip_settings():
    connected_ips = current_connected_ips()
    banned_ips = rcv_banned_ips()
    ip_status = {ip: ("banned" if ip in banned_ips else "unbanned") for ip in connected_ips}
    return jsonify({"ip_status": ip_status, "banned_ips": list(banned_ips)})

@app.route('/public-ip-settingss', methods=['GET'])
def public_ip_settings_page():
    connected_ips = current_connected_ips()
    banned_ips = rcv_banned_ips()
    ip_status = {ip: ("banned" if ip in banned_ips else "unbanned") for ip in connected_ips}
    return render_template('public_ip_settings.html', ip_status=ip_status, banned_ips=banned_ips)

@app.route('/metrics')
@cache.cached(timeout=5)
def metrics():
    cpu_usage = psutil.cpu_percent(interval=1)  
    ram_usage = psutil.virtual_memory().percent
    uptime_value = system_uptime()  
    return jsonify({"cpu_usage": cpu_usage, "ram_usage": ram_usage, "uptime": uptime_value})


@app.route('/network-stats')
def network_stats():
    bytes_to_gb = 1 / (1024 ** 3)
    try:
        network_data = {
            port: {
                "bytes_sent": f"{traffic_data[port]['bytes_sent'] * bytes_to_gb:.2f} GB",
                "bytes_received": f"{traffic_data[port]['bytes_received'] * bytes_to_gb:.2f} GB",
                "packets_sent": traffic_data[port]["packets_sent"],
                "packets_received": traffic_data[port]["packets_received"]
            }
            for port in traffic_data
        }
        print(f"Network Stats: {network_data}")
        return jsonify(network_data)
    except KeyError as e:
        write_log(f"KeyError in network-stats: {str(e)}")
        return jsonify({"error": "Invalid port or traffic data missing."})



def current_connected_ips():
    try:
        result = subprocess.run(["ss", "-t", "-n"], capture_output=True, text=True, timeout=30) 

        
        if result.returncode != 0:
            write_log("error in executing the 'ss' command.")
            return set()

        lines = result.stdout.splitlines()
        ips = set()
        
        for line in lines[1:]:
            parts = line.split()
            if len(parts) >= 5:
                remote_ip = parts[4].split(":")[0]
                
                if remote_ip and remote_ip != "127.0.0.1":
                    ips.add(remote_ip)
        
        if not ips:
            write_log("Warning: No connected IPs found.")
        
        return ips
    except Exception as e:
        write_log(f"error fetching connected IPs: {str(e)}")
        return set()


def rcv_banned_ips():
    if os.path.exists(banned_ips_file):
        with open(banned_ips_file, "r") as file:
            return set(file.read().splitlines())
    return set()

def save_banned_ips(ips):
    with open(banned_ips_file, "w") as file:
        file.write("\n".join(ips))

def ban_ip_w_iptables(ip):
    try:
        subprocess.run(["sudo", "iptables", "-A", "INPUT", "-s", ip, "-j", "DROP"], check=True)
        subprocess.run(["sudo", "iptables", "-A", "FORWARD", "-s", ip, "-j", "DROP"], check=True)
        save_iptables_rules()
        write_log(f"IP {ip} has been banned using iptables and rules saved persistently.")
    except subprocess.CalledProcessError as e:
        write_log(f"Failed to ban IP {ip}: {str(e)}")

def unban_ip_w_iptables(ip):
    try:
        ip = ip.replace("-", ".")
        
        subprocess.run(["sudo", "iptables", "-D", "INPUT", "-s", ip, "-j", "DROP"], check=True)
        subprocess.run(["sudo", "iptables", "-D", "FORWARD", "-s", ip, "-j", "DROP"], check=True)
        save_iptables_rules()
        write_log(f"IP {ip} has been unbanned using iptables and rules saved persistently.")
    except subprocess.CalledProcessError as e:
        write_log(f"Failed to unban IP {ip}: {str(e)}")

def save_iptables_rules():
    try:
        subprocess.run(["sudo", "netfilter-persistent", "save"], check=True)
        write_log("iptables rules saved persistently.")
    except subprocess.CalledProcessError as e:
        write_log(f"Failed to save iptables rules persistently: {str(e)}")

@app.route('/system-logs')
def system_logs():
    logs = obtain_system_logs()
    return jsonify({"logs": logs})

def obtain_system_logs():
    if not os.path.exists(log_file):
        with open(log_file, "w") as file:
            file.write("")
    try:
        with open(log_file, "r") as file:
            lines = file.readlines()[-10:]
        return "".join(lines)
    except Exception as e:
        return f"Error reading logs: {str(e)}"

@app.route('/api/tunnel-logs')
def api_tunnel_logs():
    logs = obtain_tunnel_logs()
    return jsonify({"logs": logs})

def obtain_tunnel_logs():
    try:
        if not os.path.exists(tunnel_log_file):
            return "No tunnel logs found."
        with open(tunnel_log_file, "r") as file:
            lines = file.readlines()[-50:]
        return "".join(lines)
    except Exception as e:
        return f"error reading tunnel logs: {str(e)}"

@app.route('/tunnel-status')
def tunnel_status():
    statuses = {"tcp_forwarder": "Inactive", "udp_forwarder": "Inactive"}
    for process in psutil.process_iter(attrs=["pid", "name"]):
        try:
            if "tcp_forwarder" in process.info["name"]:
                statuses["tcp_forwarder"] = "Active"
            elif "udp_forwarder" in process.info["name"]:
                statuses["udp_forwarder"] = "Active"
        except (psutil.NoSuchProcess, psutil.AccessDenied, psutil.ZombieProcess):
            continue
    return jsonify(statuses)

@app.route('/clear-tunnel-logs', methods=['POST'])
def clear_tunnel_logs():
    try:
        open(tunnel_log_file, "w").close()
        return "Logs cleared successfully.", 200
    except Exception as e:
        return f"Failed to clear logs: {str(e)}", 500

@app.route('/tunnel-logs')
def tunnel_logs():
    return render_template("tunnel_logs.html")

if __name__ == '__main__':
    try:
        app.run(debug=True, host='0.0.0.0', port=monitoring_port)
    except KeyboardInterrupt:
        cleanup_n_exit()
