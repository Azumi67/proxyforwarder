![R (2)](https://github.com/Azumi67/PrivateIP-Tunnel/assets/119934376/a064577c-9302-4f43-b3bf-3d4f84245a6f)
نام پروژه : پورت فوروارد با مانیتورینگ و ربات
--
- این پروژه در راستای یادگیری زبان C++ بوده است و از زبان های html. js و پایتون و فریم ورک flask هم استفاده شده است. در این برنامه به صورت realtime از مانیتورینگ و ربات تلگرام پشتیبانی میشود. میتوان مقدار رم و cpu مصرفی و ترافیک هر پورت را مشاهده کرد. سیستم لاگ و لاگ های تانل هم قابل مشاهده هستند. ایپی های متصل به این برنامه قابل مشاهده است و میتوان ان ها را بست و از API هم پشتیبانی میکند
- از تجربه به دست امده در این برنامه برای نوشتن پنل وایرگارد فارسی با حجم و زمان و ربات استفاده خواهد شد که base کد ها‌ در دست خودم باشد.
- باید به یک سری نکات توجه داشته باشید که اگر در udp سرور شما محدودیت داشت باید توسط لوکال ایپی geneve این مشکل را حل کنید. پرایوت ایپی شما باید ورژن 4 باشد
---------------------------------------------------------------

![check](https://github.com/Azumi67/PrivateIP-Tunnel/assets/119934376/13de8d36-dcfe-498b-9d99-440049c0cf14)
**امکانات**
- پورت فوروارد 
- پشتیبانی از مانیتورینگ ( مقدار cpu و ram، حجم ترافیکی هر پورت، اپتایم سیستم، سیستم لاگ، لاگ تانل، ایپی های متصل به برنامه)
- دارای Api و telegram bot برای مانیتورینگ و سایر موارد
- ترافیک مصرفی هر پورت در json file ذخیره میشود
- بستن یا باز کردن ایپی های متصل به برنامه
- داری thread pool
- دارای پورت رنج و پورت به صورت single
- پشتیبانی از tcp و udp
- دارای tcpnodelay و tcp keepalive
- دارای tcp health check
- دارای لاگ های info,debug,trace,error,warn و ذخیره آن در لاگ فایل
- دارای max connection برای tcp
- دارای buffer size برای tcp و udp
- دارای retry و delay in between برای tcp
- دارای timeout برای stale connections
- دارای اسکریپت نصب پایتون , flask و کامپایل پروژه
- امکان استفاده از برنامه با و بدون مانیتورینگ
- دارای لاگ سیستم و flask لاگ
- پشتیبانی از arm64 / amd64

-----------------------
![images](https://github.com/user-attachments/assets/f50ecb83-2194-4b91-9594-00d310dc506a)
اسکرین شات:
<details>
  <summary align="right">ربات تلگرام</summary>

  <p align="right">
    <img src="https://github.com/user-attachments/assets/b1f92f84-b53b-4fa5-907e-0fca1f0f358e" alt="menu screen" />
  </p>
</details>

<details>
  <summary align="right">صفحه لاگین</summary>

  <p align="right">
    <img src="https://github.com/user-attachments/assets/838180ee-d49b-4370-9eda-3ca81bd6a766" alt="menu screen" />
  </p>
</details>

<details>
  <summary align="right">صفحه اصلی</summary>

  <p align="right">
    <img src="https://github.com/user-attachments/assets/45c43bd3-ae5b-4d39-8870-15b193cb14f9" alt="menu screen" />
  </p>
</details>

---------------------------------------------------------------
<div align="right">
  <details>
    <summary><strong><img src="https://github.com/Azumi67/Rathole_reverseTunnel/assets/119934376/3cfd920d-30da-4085-8234-1eec16a67460" alt="Image"> نکات</strong></summary>
    
------------------------------------ 


    
- ادرس لاگ ها و backup در همان داخل پروژه میباشد
- لاگ های flask و forwarder داخل پروژه میباشد
- فایل config.yaml هم در داخل پروژه میباشد
- اگر سرور شما منابع خوبی دارد میتوانید حتی buffer size را بر روی 65535 قراز دهید. این مورد را باید خود شما تست نمایید.


</details>
</div>
  
------------------------------------ 

  ![6348248](https://github.com/Azumi67/PrivateIP-Tunnel/assets/119934376/398f8b07-65be-472e-9821-631f7b70f783)
**آموزش استفاده از برنامه با و بدون اسکریپت**

 <div align="right">
  <details>
    <summary><strong><img src="https://github.com/Azumi67/Rathole_reverseTunnel/assets/119934376/fcbbdc62-2de5-48aa-bbdd-e323e96a62b5" alt="Image"> </strong>نمونه config.yaml</summary>

------------------

- نمونه کانفیگ tcp
 <div align="left">
   
```
#TCP USAGE
forwarders:
  - listen_address: "0.0.0.0"         #ادرسی لوکال سرور که به همین صورت وارد میکنید
    listen_port: 8080                # پورتی که در لوکال سرور باید انتخاب کنید
    target_address: "192.168.1.10"   # ادرس سرور خارج
    target_port: 8080                # پورت سرور خارج

  - listen_address: "::"             # این همان نمونه برای ایپی 6 میباشد
    listen_port: 7070                # پورت ایران
    target_address: "2001:db8::1"    # ادرس سرور خارج ایپی 6
    target_port: 7070                 # پورت سرور خارج

# port range
  - listen_address: "0.0.0.0"    # الوکال سرور که باید به همین صورت وارد نمایید
    target_address: "192.168.1.10"  # ایپی سرور خارج
    port_range:
      start: 8080   # پورت شروع
      end: 8085     # پورت پایان

  - listen_address: "::"  # IPv6 address
    target_address: "fe80::1"  # IPv6 سرور خارج
    port_range:
      start: 9090  پورت شروع
      end: 9095   پورت پایانی

thread_pool:
  threads: 2    # threads for cpu cores  بستگی به تعداد هسته پردارشگر شما دارد

max_connections: 200  # تعداد نهایی کانکشن هم زمان
retry_attempts: 5   # مقدار تلاش دوباره برای برقرار ارتباط
retry_delay: 10      # وقفه به ثانیه بین هر تلاش مجدد برای برقراری ارتباط
tcp_no_delay: false  # Disable Nagle's algorithm for low latency
buffer_size: 8092  #بافر سایز . میتوانید حتی بیشترین مقدار 65535 بذارید. باید بررسی کنید

monitoring_port: 8080  # پورت مانیتور 

timeout:
  connection: 3000  # Timeout for connections in seconds

health_check:
  enabled: true  #true or false
  interval: 300  # Interval for performing health checks in seconds

tcp_keep_alive:
  enabled: true          # enable or disable TCP keepalive
  idle: 60               # time in seconds the connection is idle before keepalive goods are sent
  interval: 10           # time in seconds between individual keep-alive probes
  count: 5               # number of keepalive goods sent before the connection is dropped

logging:
  enabled: true   # Enable or disable logging (true/false)
  file: "logfile.log" # Name of the file
  level: "INFO"  # Options: "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "ALL"
```
 <div align="right">
- نمونه کانفیگ udp
 <div align="left">
   
```
#UDP USAGE
srcAddrPorts:
  - "0.0.0.0:1150"  #only ipv4 USE Geneve local ip if your server is limited
  - "0.0.0.0:1151"
dstAddrPorts:
  - "66.200.1.1:1150"
  - "66.200.1.2:1151"

timeout: 3000   # Timeout for idle connections (in seconds)
buffer_size: 8092   #buffer size or max 65530
thread_pool:
  threads: 2

logging:
  enabled: true  # Enable/disable logging
  file: "logfile.log" #log file directory
  level: "INFO"  # Log level: TRACE, DEBUG, INFO, WARN, ERROR
monitroing_port: 8080 # or whatever port you want
```

------------------

  </details>
</div>
 <div align="right">
  <details>
    <summary><strong><img src="https://github.com/Azumi67/Rathole_reverseTunnel/assets/119934376/fcbbdc62-2de5-48aa-bbdd-e323e96a62b5" alt="Image"> </strong>نحوه استفاده از اسکریپت برای tcp یا Udp</summary>

------------------

<p align="right">
  <img src="https://github.com/user-attachments/assets/35b7f906-ada3-4b72-947a-c0cf8834a73d" alt="Image" />
</p>

- نخست دستورات پایین را اجرا میکنم
<div align="left">
  
```
apt update -y
sudo apt-get install libpcap-dev
apt install git -y
git clone https://github.com/Azumi67/proxyforwarder.git
cd proxyforwarder/src
```
 <div align="right">
   
- سپس فایل config.yaml را طبق اموزش اماده میکنم و سپس پیش نیاز ها را نصب میکنم و بسته به نیاز tcp یا udp را start میکنم. برنامه اجرا میشود و سپس میتوانم از طریق ipserveriran:port به مانیتورینگ دسترسی پیدا کنم و بعد از ساختن یوزر نیم و پسورد به داخل صفحه اصلی مانیتورینگ میشوم
<div align="left">
  
```
chmod +x forwarder.sh
./forwarder.sh /root/proxyforwarder/src/config.yaml
```
 <div align="right">
   
- برای اینکه هر دفعه برای اجرای این برنامه وارد این اسکریپت نشوم، یک سرویس درست میکنم و مسیر اسکریپت tcp.sh یا udp.sh را در داخلش قرار میدهم. مانند نمونه زیر
 <div align="left">
   
```
chmod +x /root/proxyforwarder/src/tcp.sh
nano /etc/systemd/system/tcpforwarder.service
```
 <div align="right">
   
- برای tcp
 <div align="left">
     
```
[Unit]
Description=TCP Forwarder and Flask Server
After=network.target

[Service]
Type=simple
User=root
WorkingDirectory=/root/proxyforwarder/src
ExecStart=/root/proxyforwarder/src/tcp.sh /root/proxyforwarder/src/config.yaml
Restart=on-failure
Environment="PATH=/root/proxyforwarder/src/venv/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"

[Install]
WantedBy=multi-user.target
```
```
sudo systemctl daemon-reload

sudo systemctl enable tcpforwarder.service

sudo systemctl start tcpforwarder.service

sudo systemctl status tcpforwarder.service
```
 <div align="right">
   
- برای udp

 <div align="left">
   
```
chmod +x /root/proxyforwarder/src/udp.sh
nano /etc/systemd/system/udpforwarder.service
```
```
[Unit]
Description=UDP Forwarder and Flask Server
After=network.target

[Service]
Type=simple
User=root
WorkingDirectory=/root/proxyforwarder/src
ExecStart=/root/proxyforwarder/src/udp.sh /root/proxyforwarder/src/config.yaml
Restart=on-failure
Environment="PATH=/root/proxyforwarder/src/venv/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"

[Install]
WantedBy=multi-user.target
```
```
sudo systemctl daemon-reload

sudo systemctl enable udpforwarder.service

sudo systemctl start udpforwarder.service

sudo systemctl status udpforwarder.service
```
  </details>
</div>
 <div align="right">
  <details>
    <summary><strong><img src="https://github.com/Azumi67/Rathole_reverseTunnel/assets/119934376/fcbbdc62-2de5-48aa-bbdd-e323e96a62b5" alt="Image"> </strong>اجرای برنامه بدون مانیتورینگ</summary>

------------------

- نخست این دستورات را اجرا کنید تا بعدا binary های arch های مختلف را اماده کنم

<div align="left">
  
```
apt update -y
sudo apt-get install libpcap-dev
apt install git -y
git clone https://github.com/Azumi67/proxyforwarder.git
cd proxyforwarder/src
sudo apt install -y build-essential g++ cmake libboost-all-dev libyaml-cpp-dev
#amd64
g++ tcp_forwarder.cpp -o tcp_forwarder -std=c++17 -pthread -lboost_system -lyaml-cpp
#arm64
g++ tcp_forwarder.cpp -o tcp_forwarder -std=c++17 -pthread -lboost_system -lyaml-cpp
```
<div align="right">
  
- برای udp

<div align="left">
  
```
apt update -y
sudo apt-get install libpcap-dev
apt install git -y
git clone https://github.com/Azumi67/proxyforwarder.git
cd proxyforwarder/src
sudo apt install -y build-essential g++ libboost-system-dev libyaml-cpp-dev
#amd64
g++ udp_forwarder.cpp -o udp_forwarder -std=c++17 -pthread -lboost_system -lyaml-cpp
#arm64
g++ udp_forwarder.cpp -o udp_forwarder -std=c++17 -pthread -lboost_system -lyaml-cpp
```
<div align="right">


- سپس طبق اموزش فایل config.yaml را ویرایش میکنم
<div align="left">
  
```
nano /root/proxyforwarder/src/config.yaml
```
<div align="right">

- سرویس برای برنامه

<div align="left">

```
nano /etc/systemd/system/tcpforwarder.service
```
```
[Unit]
Description=TCP Forwarder Service
After=network.target

[Service]
Type=simple
ExecStart=/root/proxyforwarder/src/tcp_forwarder /root/proxyforwarder/src/config.yaml
Restart=always
User=root
WorkingDirectory=/root/proxyforwarder/src
StandardOutput=syslog
StandardError=syslog
SyslogIdentifier=tcp_forwarder
LimitNOFILE=65536

[Install]
WantedBy=multi-user.target


```
```
sudo systemctl daemon-reload
sudo systemctl start tcpforwarder.service
sudo systemctl enable tcpforwarder.service
sudo systemctl status tcpforwarder.service
```
```
ulimit -n 65536
sudo nano /etc/security/limits.conf
root    hard    nofile    65536
root    soft    nofile    65536
```
```
sudo nano /etc/sysctl.conf
net.ipv4.ip_forward = 1
net.ipv6.conf.all.forwarding = 1
CTRL+X و  Y
sudo sysctl -p
```

  </details>
</div>
 <div align="right">
  <details>
    <summary><strong><img src="https://github.com/Azumi67/Rathole_reverseTunnel/assets/119934376/fcbbdc62-2de5-48aa-bbdd-e323e96a62b5" alt="Image"> </strong>نحوه استفاده از ربات</summary>

------------------

- نخست داخل یک سرور خارج، ربات را دانلود میکنم

 <div align="left">
   
```
#not externally managed
-----------------------
apt update -y
sudo apt-get install libpcap-dev
apt install git -y
git clone https://github.com/Azumi67/proxyforwarder.git
cd proxyforwarder/telegramBot
sudo apt install -y python3 python3-pip python3-venv
python3 -m venv venv
source venv/bin/activate
pip install --upgrade pip
pip install python-telegram-bot requests pyyaml
pip freeze

#externally managed
-----------------------
apt update -y
sudo apt-get install libpcap-dev
apt install git -y
git clone https://github.com/Azumi67/proxyforwarder.git
cd proxyforwarder/telegramBot
sudo apt install -y python3 python3-pip python3-venv
apt install python3.11-venv -y
python3 -m venv ~/telegram_bot_env
source ~/telegram_bot_env/bin/activate
pip install -r requirements.txt
deactivate
python3 robot.py
```
 <div align="right">
   
- سپس از شما توکن بات و صفحه مانیتورینگ را میخواهد. به طور مثال ایپی ایران شما 2.2.2.2 میباشد و پورت مانیتورینگ 8080 است . پس url برای شما 2.2.2.2:8080 است
- سپس از شما api key را میخواهد که از قبل باید داخل قسمت api key management در داخل 2.2.2.2:8080 ساخته باشید و paste کنید
- سپس میتوانید از ربات برای مانیتورینگ استفاده نمایید
- دقت نمایید api key management را از قبل بسازید و قبلا ربات خود را از botfather داخل تلگرام دریافت کرده باشید.
- میتوانید ربات را داخل سرویس قرار بدید که برای همیشه فعال باشد

 <div align="left">

```
nano /etc/systemd/system/telegram_bot.service
-------------------------------
[Unit]
Description=Telegram Bot Service
After=network.target

[Service]
Type=simple
User=root
WorkingDirectory=/root/proxyforwarder/telegramBot
ExecStart=/root/telegram_bot_env/bin/python /root/proxyforwarder/telegramBot/robot.py
Restart=always
RestartSec=5
Environment="PYTHONUNBUFFERED=1"
StandardOutput=syslog
StandardError=syslog
SyslogIdentifier=telegram_bot

[Install]
WantedBy=multi-user.target

---------------
sudo systemctl daemon-reload
sudo systemctl enable telegram_bot
sudo systemctl start telegram_bot
sudo systemctl status telegram_bot

```

  </details>
</div>
