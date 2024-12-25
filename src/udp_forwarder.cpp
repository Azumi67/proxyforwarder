#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <iomanip>
#include <ctime>
#include <atomic>
#include <mutex>
#include <queue>
#include <cstring>
#include <condition_variable>
#include <yaml-cpp/yaml.h>
#include <unordered_map>
#include <thread>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <algorithm>
#include <list>

class Logger
{
public:
    enum class LogLevel
    {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        ALL
    };

    Logger(bool enabled, const std::string &file, const std::string &level)
        : enabled_(enabled), stop_worker_(false)
    {
        if (level == "TRACE")
            log_level_ = LogLevel::TRACE;
        else if (level == "DEBUG")
            log_level_ = LogLevel::DEBUG;
        else if (level == "INFO")
            log_level_ = LogLevel::INFO;
        else if (level == "WARN")
            log_level_ = LogLevel::WARN;
        else if (level == "ERROR")
            log_level_ = LogLevel::ERROR;
        else
            log_level_ = LogLevel::ALL;

        if (enabled_)
        {
            logfile_.open(file, std::ios::app);
            if (!logfile_)
            {
                std::cerr << "opennig log file failed: " << file << std::endl;
                enabled_ = false;
            }
        }

        if (enabled_)
        {
            worker_thread_ = std::thread(&Logger::process_queue, this);
        }
    }

    ~Logger()
    {
        if (enabled_)
        {
            {
                std::lock_guard<std::mutex> lock(queue_mutex_);
                stop_worker_ = true;
                queue_cv_.notify_all();
            }
            if (worker_thread_.joinable())
            {
                worker_thread_.join();
            }

            if (logfile_.is_open())
            {
                logfile_.close();
            }
        }
    }

    void log(const std::string &level, const std::string &message, LogLevel msg_level)
    {
        if (enabled_ && must_log(msg_level))
        {
            std::time_t now = std::time(nullptr);
            std::tm *ltm = std::localtime(&now);

            std::ostringstream log_entry;
            log_entry << "[" << std::put_time(ltm, "%Y-%m-%d %H:%M:%S") << "] "
                      << "[" << level << "] " << message << std::endl;

            {
                std::lock_guard<std::mutex> lock(queue_mutex_);
                log_queue_.emplace(log_entry.str());
            }
            queue_cv_.notify_one();
        }
    }

    void trace(const std::string &message) { log("TRACE", message, LogLevel::TRACE); }
    void debug(const std::string &message) { log("DEBUG", message, LogLevel::DEBUG); }
    void info(const std::string &message) { log("INFO", message, LogLevel::INFO); }
    void warn(const std::string &message) { log("WARN", message, LogLevel::WARN); }
    void error(const std::string &message) { log("ERROR", message, LogLevel::ERROR); }

private:
    bool must_log(LogLevel msg_level)
    {
        return log_level_ <= msg_level || log_level_ == LogLevel::ALL;
    }

    void process_queue()
    {
        while (true)
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            queue_cv_.wait(lock, [this]
                           { return !log_queue_.empty() || stop_worker_; });

            while (!log_queue_.empty())
            {
                try
                {
                    logfile_ << log_queue_.front();
                    logfile_.flush();
                }
                catch (const std::exception &e)
                {
                    std::cerr << "Logging error: " << e.what() << std::endl;
                }
                log_queue_.pop();
            }

            if (stop_worker_)
                break;
        }
    }

    bool enabled_;
    std::ofstream logfile_;
    std::mutex queue_mutex_;
    std::queue<std::string> log_queue_;
    std::condition_variable queue_cv_;
    std::thread worker_thread_;
    std::atomic<bool> stop_worker_;
    LogLevel log_level_;
};

struct sockaddr_inx
{
    union
    {
        struct sockaddr sa;
        struct sockaddr_in in;
        struct sockaddr_in6 in6;
    };
};

struct ProxyConn
{
    sockaddr_inx cli_addr;
    int svr_sock;
    time_t last_active;
};

class UDPProxy
{
public:
    UDPProxy(const std::string &srcAddrPort, const std::string &dstAddrPort, int timeout, int buffer_size, Logger &logger)
        : timeout(timeout), buffer_size(buffer_size), connTblHashSize(256), logger(logger)
    {
        pAddress(srcAddrPort, srcAddr);
        pAddress(dstAddrPort, dstAddr);
        setupSocket();
        initiateConnectionTable();
    }

    ~UDPProxy()
    {
        if (srcSocket != -1)
            close(srcSocket);
    }

    void processConnections();

private:
    int timeout;
    int buffer_size;
    sockaddr_inx srcAddr, dstAddr;
    int srcSocket = -1;
    int epollFd = -1;
    int connTblHashSize;
    Logger &logger;

    std::vector<std::shared_ptr<ProxyConn>> connTable[256];
    std::unordered_map<int, ProxyConn *> connMap;
    std::mutex connMutex;

    void pAddress(const std::string &addrPort, sockaddr_inx &sockAddr);
    void setupSocket();
    void initiateConnectionTable();
    void recycleConnections();
    int hashAddress(sockaddr_inx *addr);
    ProxyConn *tOrCreateConnection(sockaddr_inx &cliAddr);
    void rlsConnection(ProxyConn *conn);
    static void setNonBlocking(int sockfd);
    static bool compareAddresses(sockaddr_inx *a, sockaddr_inx *b);
};

void UDPProxy::pAddress(const std::string &addrPort, sockaddr_inx &sockAddr)
{
    std::string ip = addrPort.substr(0, addrPort.find(':'));
    int port = std::stoi(addrPort.substr(addrPort.find_last_of(':') + 1));

    memset(&sockAddr, 0, sizeof(sockAddr));
    if (ip.find(':') != std::string::npos)
    { // IPv6 address
        sockAddr.in6.sin6_family = AF_INET6;
        inet_pton(AF_INET6, ip.c_str(), &sockAddr.in6.sin6_addr);
        sockAddr.in6.sin6_port = htons(port);
    }
    else
    { // IPv4 address
        sockAddr.in.sin_family = AF_INET;
        inet_pton(AF_INET, ip.c_str(), &sockAddr.in.sin_addr);
        sockAddr.in.sin_port = htons(port);
    }

    logger.debug("Parsed address: " + ip + ":" + std::to_string(port));
}

void UDPProxy::setupSocket()
{
    int addrFamily = srcAddr.sa.sa_family;

    srcSocket = socket(addrFamily, SOCK_DGRAM, 0);
    if (srcSocket < 0)
    {
        logger.error("Socket creation failed: " + std::string(strerror(errno)));
        throw std::runtime_error("Socket creation failed");
    }

    int reuse = 1;
    setsockopt(srcSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    if (bind(srcSocket, (struct sockaddr *)&srcAddr,
             (addrFamily == AF_INET6) ? sizeof(srcAddr.in6) : sizeof(srcAddr.in)) < 0)
    {
        logger.error("Binding socket failed for address: " +
                     std::string((addrFamily == AF_INET6) ? "[IPv6]" : "[IPv4]") +
                     " Error: " + std::string(strerror(errno)));
        throw std::runtime_error("Binding socket failed");
    }

    setNonBlocking(srcSocket);

    epollFd = epoll_create1(0);
    if (epollFd < 0)
    {
        logger.error("Creating epoll file descriptor failed: " + std::string(strerror(errno)));
        throw std::runtime_error("Creating epoll file descriptor failed");
    }

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = srcSocket;
    epoll_ctl(epollFd, EPOLL_CTL_ADD, srcSocket, &ev);

    logger.info("Socket successfully set and bound to address");
}

void UDPProxy::setNonBlocking(int sockfd)
{
    fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK);
}

void UDPProxy::initiateConnectionTable()
{
    for (int i = 0; i < connTblHashSize; ++i)
    {
        connTable[i].clear();
    }
    logger.debug("Connection table initialized");
}

void UDPProxy::processConnections()
{
    struct epoll_event events[10];
    sockaddr_inx clientAddr{};
    socklen_t clientLen = sizeof(clientAddr);
    std::vector<char> buffer(buffer_size);

    while (true)
    {
        int nfds = epoll_wait(epollFd, events, 10, 2000);
        recycleConnections();

        if (nfds < 0)
        {
            if (errno == EINTR)
                continue;
            logger.error("epoll wait failed");
            throw std::runtime_error("epoll wait failed");
        }

        for (int i = 0; i < nfds; ++i)
        {
            int sockfd = events[i].data.fd;

            if (sockfd == srcSocket)
            {
                int len = recvfrom(srcSocket, buffer.data(), buffer.size(), 0, (struct sockaddr *)&clientAddr, &clientLen);
                if (len > 0)
                {
                    logger.debug("Received data from client");

                    std::lock_guard<std::mutex> lock(connMutex);
                    ProxyConn *conn = tOrCreateConnection(clientAddr);
                    if (conn)
                    {
                        send(conn->svr_sock, buffer.data(), len, 0);
                        logger.trace("Data sent to server");
                    }
                }
            }
            else
            {
                std::lock_guard<std::mutex> lock(connMutex);
                ProxyConn *conn = connMap[sockfd];
                if (conn)
                {
                    int len = recv(conn->svr_sock, buffer.data(), buffer.size(), 0);
                    if (len > 0)
                    {
                        sendto(srcSocket, buffer.data(), len, 0, (struct sockaddr *)&conn->cli_addr, sizeof(conn->cli_addr.in));
                        logger.trace("Data sent to client");
                    }
                    else
                    {
                        rlsConnection(conn);
                        logger.warn("Connection released due to reading error ");
                    }
                }
            }
        }
    }
}

void UDPProxy::recycleConnections()
{
    time_t now = time(nullptr);
    std::lock_guard<std::mutex> lock(connMutex);

    for (int i = 0; i < connTblHashSize; ++i)
    {
        auto &bucket = connTable[i];

        auto new_end = std::remove_if(bucket.begin(), bucket.end(), [now, this](const std::shared_ptr<ProxyConn> &conn)
                                      {
                                          if (now - conn->last_active > timeout)
                                          {
                                              logger.info("Recycling idle connection for client.");
                                              logger.debug("Releasing connection: Client IP = " + std::string(inet_ntoa(conn->cli_addr.in.sin_addr)) +
                                                           ", Port = " + std::to_string(ntohs(conn->cli_addr.in.sin_port)) +
                                                           ", Last active = " + std::to_string(conn->last_active));

                                              rlsConnection(conn.get()); 
                                              return true;              
                                          }
                                          return false; });

        bucket.erase(new_end, bucket.end());

        logger.debug("Recycled idle connections, new list size: " + std::to_string(bucket.size()));
    }
}

int UDPProxy::hashAddress(sockaddr_inx *addr)
{
    return ntohl(addr->in.sin_addr.s_addr) % connTblHashSize;
}

bool UDPProxy::compareAddresses(sockaddr_inx *a, sockaddr_inx *b)
{
    if (a->sa.sa_family != b->sa.sa_family)
        return false;

    if (a->sa.sa_family == AF_INET)
        return (a->in.sin_addr.s_addr == b->in.sin_addr.s_addr &&
                a->in.sin_port == b->in.sin_port);

    if (a->sa.sa_family == AF_INET6)
        return (memcmp(&a->in6.sin6_addr, &b->in6.sin6_addr, sizeof(a->in6.sin6_addr)) == 0 &&
                a->in6.sin6_port == b->in6.sin6_port);

    return false;
}

ProxyConn *UDPProxy::tOrCreateConnection(sockaddr_inx &cliAddr)
{
    int bucket = hashAddress(&cliAddr);
    auto &list = connTable[bucket];

    logger.debug("Checking connection table for client address...");
    for (auto &conn : list)
    {
        if (compareAddresses(&conn->cli_addr, &cliAddr))
        {
            conn->last_active = time(nullptr);
            logger.debug("Reused existing connection for client.");
            return conn.get();
        }
    }

    logger.debug("No existing connection found. Creating a new one.");

    int svrSock = (dstAddr.sa.sa_family == AF_INET6) ? socket(AF_INET6, SOCK_DGRAM, 0) : socket(AF_INET, SOCK_DGRAM, 0);
    if (svrSock < 0)
    {
        logger.error("Creating server socket failed: " + std::string(strerror(errno)));
        return nullptr;
    }

    if (connect(svrSock, (struct sockaddr *)&dstAddr,
                (dstAddr.sa.sa_family == AF_INET6) ? sizeof(dstAddr.in6) : sizeof(dstAddr.in)) < 0)
    {
        logger.error("Connecting to server socket failed: " + std::string(strerror(errno)));
        close(svrSock);
        return nullptr;
    }

    setNonBlocking(svrSock);

    auto newConn = std::make_shared<ProxyConn>(ProxyConn{cliAddr, svrSock, time(nullptr)});
    list.push_back(newConn);
    connMap[svrSock] = newConn.get();

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = svrSock;
    epoll_ctl(epollFd, EPOLL_CTL_ADD, svrSock, &ev);

    return newConn.get();
}

void UDPProxy::rlsConnection(ProxyConn *conn)
{
    if (conn->svr_sock != -1)
    {
        epoll_ctl(epollFd, EPOLL_CTL_DEL, conn->svr_sock, nullptr);
        close(conn->svr_sock);
        conn->svr_sock = -1;
    }

    int bucket = hashAddress(&conn->cli_addr);
    auto &bucketVec = connTable[bucket];

    bucketVec.erase(
        std::remove_if(bucketVec.begin(), bucketVec.end(), [&](const std::shared_ptr<ProxyConn> &item)
                       { return item.get() == conn; }),
        bucketVec.end());

    connMap.erase(conn->svr_sock);

    logger.info("Released & closed connection");
}

int main()
{
    try
    {
        YAML::Node config = YAML::LoadFile("config.yaml");
        std::vector<std::string> srcAddrPorts = config["srcAddrPorts"].as<std::vector<std::string>>();
        std::vector<std::string> dstAddrPorts = config["dstAddrPorts"].as<std::vector<std::string>>();
        int timeout = config["timeout"].as<int>();
        int buffer_size = config["buffer_size"].as<int>();
        bool loggingEnabled = config["logging"]["enabled"].as<bool>();
        std::string logFile = config["logging"]["file"].as<std::string>();
        std::string logLevel = config["logging"]["level"].as<std::string>();
        int threadCount = config["thread_pool"]["threads"].as<int>();

        Logger logger(loggingEnabled, logFile, logLevel);

        if (srcAddrPorts.size() != dstAddrPorts.size())
        {
            logger.error("Mismatch in the number of source and destination addresses");
            throw std::runtime_error("Mismatch in the number of source and destination addresses");
        }

        std::vector<std::unique_ptr<UDPProxy>> proxies;
        for (size_t i = 0; i < srcAddrPorts.size(); ++i)
        {
            proxies.push_back(std::make_unique<UDPProxy>(srcAddrPorts[i], dstAddrPorts[i], timeout, buffer_size, logger));
        }

        std::vector<std::thread> threads;
        for (int i = 0; i < threadCount; ++i)
        {
            threads.emplace_back([&proxies, i, threadCount]()
                                 {
                                     for (size_t j = i; j < proxies.size(); j += threadCount)
                                     {
                                         proxies[j]->processConnections();
                                     } });
        }

        for (auto &thread : threads)
        {
            thread.join();
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
