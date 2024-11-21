function fetchMetrics() {
    fetch('/metrics')
        .then(response => response.json())
        .then(data => {
            document.getElementById('cpu-usage').innerText = data.cpu_usage;
            document.getElementById('ram-usage').innerText = data.ram_usage;
            document.getElementById('uptime').innerText = data.uptime; 
        })
        .catch(error => console.error('error in fetching metrics:', error));
}

function fetchNetworkStats() {
    fetch('/network-stats')
        .then(response => response.json())
        .then(data => {
            const portSelector = document.getElementById('port-selector');
            const selectedPort = portSelector.value;
            const portData = data[selectedPort];

            if (portData) {
                document.getElementById('bytes-sent').innerText = portData.bytes_sent;
                document.getElementById('bytes-received').innerText = portData.bytes_received;
                document.getElementById('packets-sent').innerText = portData.packets_sent;
                document.getElementById('packets-received').innerText = portData.packets_received;
            }
        })
        .catch(error => console.error('error in fetching network stats:', error));
}

function updateNetworkStats() {
    fetchNetworkStats();
}

function fetchSystemLogs() {
    fetch('/system-logs')
        .then(response => response.json())
        .then(data => {
            document.getElementById('system-logs').innerText = data.logs;
        })
        .catch(error => console.error('error in fetching system logs:', error));
}

function fetchTunnelLogs() {
    fetch('/api/tunnel-logs')
        .then(response => response.json())
        .then(data => {
            const logsContainer = document.getElementById('tunnel-logs');
            logsContainer.innerHTML = '';

            const logLines = data.logs.split('\n');
            logLines.forEach(line => {
                const logLineElement = document.createElement('div');
                logLineElement.classList.add('log-line');

                if (line.includes('[INFO]')) {
                    logLineElement.classList.add('log-info');
                } else if (line.includes('[WARNING]')) {
                    logLineElement.classList.add('log-warning');
                } else if (line.includes('[ERROR]')) {
                    logLineElement.classList.add('log-error');
                } else if (line.includes('[CRITICAL]')) {
                    logLineElement.classList.add('log-critical');
                }

                logLineElement.textContent = line;
                logsContainer.appendChild(logLineElement);
            });
        })
        .catch(error => console.error('error in fetching tunnel logs:', error));
}

function fetchTunnelStatus() {
    fetch('/tunnel-status')
        .then(response => response.json())
        .then(data => {
            document.getElementById('tunnel-status').innerText = `Tunnel Status: ${data.status}`;
        })
        .catch(error => console.error('error in fetching tunnel status:', error));
}

function banIp(ip) {
    fetch('/ban-ip', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({ ip: ip })
    })
    .then(response => response.json())
    .then(data => {
        alert(`IP ${data.ip} has been ${data.status}`);
        document.getElementById(`status-${ip}`).innerText = data.status;
        moveIpToBannedList(ip); 
    })
    .catch(error => console.error('banning IP error:', error));
}

function unbanIp(ip) {
    fetch('/unban-ip', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({ ip: ip })
    })
    .then(response => response.json())
    .then(data => {
        alert(`IP ${data.ip} has been ${data.status}`);
        document.getElementById(`status-${ip}`).innerText = data.status;
        moveIpToConnectedList(ip); 
    })
    .catch(error => console.error('unbanning IP error:', error));
}

function moveIpToBannedList(ip) {
    const ipElement = document.querySelector(`#status-${ip}`).closest('li');
    const bannedList = document.querySelector('.banned-ip-list ul');
    if (ipElement && bannedList) {
        bannedList.appendChild(ipElement);
    }
}

function moveIpToConnectedList(ip) {
    const ipElement = document.querySelector(`#status-${ip}`).closest('li');
    const connectedList = document.querySelector('.connected-ip-list ul');
    if (ipElement && connectedList) {
        connectedList.appendChild(ipElement);
    }
}

setInterval(fetchMetrics, 5000); 
setInterval(fetchNetworkStats, 5000); 
setInterval(fetchSystemLogs, 10000); 
setInterval(fetchTunnelLogs, 10000); 
setInterval(fetchTunnelStatus, 5000); 

fetchMetrics();
fetchNetworkStats();
fetchSystemLogs();
fetchTunnelLogs();
fetchTunnelStatus();
