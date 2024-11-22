#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <iomanip>
#include <ctime>
#include <atomic>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/steady_timer.hpp>  
#include <yaml-cpp/yaml.h>
#include <sstream>
#include <unordered_map>

using boost::asio::ip::tcp;

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
            queue_cv_.wait(lock, [this]()
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

void help()
{
    const std::string reset = "\033[0m";
    const std::string bold = "\033[1m";
    const std::string underline = "\033[4m";
    const std::string yellow = "\033[33m";
    const std::string cyan = "\033[36m";
    const std::string green = "\033[32m";
    const std::string red = "\033[31m";
    const std::string magenta = "\033[35m";

    std::cout << bold << yellow << "\n=========================================\n"
              << reset;
    std::cout << bold << cyan << "           TCP Forwarder Help            \n"
              << reset;
    std::cout << bold << yellow << "=========================================\n\n"
              << reset;

    std::cout << bold << "Usage:\n"
              << reset;
    std::cout << green << "  tcp_forwarder <config_file>\n\n"
              << reset;

    std::cout << bold << "Description:\n"
              << reset;
    std::cout << "  " << cyan << "This application forwards TCP traffic from a local port to a target address and port.\n";
    std::cout << "  It supports both IPv4 and IPv6 addresses and provides options for logging, connection retries, health checks, and more.\n\n"
              << reset;

    std::cout << bold << "Configuration File Format (YAML):\n"
              << reset;
    std::cout << "  The configuration file must be provided in " << magenta << "YAML format" << reset << ". Below is an explanation of each setting:\n\n";

    std::cout << bold << "  forwarders:\n"
              << reset;
    std::cout << "    - A list of forwarder configurations. Each forwarder must include:\n";
    std::cout << "      * " << green << "listen_address" << reset << ": The address to listen on.\n";
    std::cout << "      * " << green << "listen_port" << reset << ": The port to listen on.\n";
    std::cout << "      * " << green << "target_address" << reset << ": The address to forward traffic to.\n";
    std::cout << "      * " << green << "target_port" << reset << ": The port to forward traffic to.\n";
    std::cout << "      * " << green << "port_range" << reset << " (optional): Specify a start and end port for forwarding a range of ports.\n\n";

    std::cout << bold << "  buffer_size: " << reset << "(Optional) Size of the buffer in bytes for data forwarding. Default: 8192.\n";
    std::cout << bold << "  tcp_no_delay: " << reset << "(Optional) Boolean to disable Nagle's algorithm (for low latency). Default: true.\n";
    std::cout << bold << "  retry_attempts: " << reset << "(Optional) Number of retry attempts if a connection fails. Default: 3.\n";
    std::cout << bold << "  retry_delay: " << reset << "(Optional) Delay between retry attempts, in seconds. Default: 2.\n";
    std::cout << bold << "  max_connections: " << reset << "(Optional) Maximum number of simultaneous connections. Default: 100.\n\n";

    std::cout << bold << "  thread_pool:\n"
              << reset;
    std::cout << "    - " << green << "threads" << reset << ": Number of threads to handle connections. Recommended: At least the number of CPU cores.\n\n";

    std::cout << bold << "  logging:\n"
              << reset;
    std::cout << "    - " << green << "enabled" << reset << ": Boolean to enable or disable logging.\n";
    std::cout << "    - " << green << "file" << reset << ": The file name for saving log output.\n\n";

    std::cout << bold << "  health_check:\n"
              << reset;
    std::cout << "    - " << green << "enabled" << reset << ": Boolean to enable or disable health checks.\n";
    std::cout << "    - " << green << "interval" << reset << ": Interval in seconds between health checks.\n\n";

    std::cout << bold << underline << "Example Configuration (config.yaml):\n"
              << reset;
    std::cout << "--------------------------------------\n";
    std::cout << green << "forwarders:\n";
    std::cout << "  - listen_address: '::'\n";
    std::cout << "    listen_port: 8080\n";
    std::cout << "    target_address: '2001:db8::1'\n";
    std::cout << "    target_port: 9090\n\n";
    std::cout << "buffer_size: 8192\n";
    std::cout << "tcp_no_delay: true\n";
    std::cout << "retry_attempts: 3\n";
    std::cout << "retry_delay: 2\n";
    std::cout << "max_connections: 100\n\n";

    std::cout << "thread_pool:\n";
    std::cout << "  threads: 4\n\n";

    std::cout << "logging:\n";
    std::cout << "  enabled: true\n";
    std::cout << "  file: 'logfile.log'\n\n";

    std::cout << "health_check:\n";
    std::cout << "  enabled: true\n";
    std::cout << "  interval: 10\n";
    std::cout << "--------------------------------------\n\n"
              << reset;

    std::cout << bold << yellow << "=========================================\n"
              << reset;
    std::cout << bold << cyan << "         End of Help Information         \n"
              << reset;
    std::cout << bold << yellow << "=========================================\n\n"
              << reset;
}

void validate_and_set_defaults(YAML::Node &config)
{
    if (!config["forwarders"] || !config["forwarders"].IsSequence())
    {
        throw std::runtime_error("Error: 'forwarders' must be specified and must be a list.");
    }

    if (!config["thread_pool"] || !config["thread_pool"]["threads"])
    {
        throw std::runtime_error("Error: 'thread_pool.threads' must be specified.");
    }

    if (!config["buffer_size"])
    {
        config["buffer_size"] = 8192;
    }

    if (!config["tcp_no_delay"])
    {
        config["tcp_no_delay"] = true;
    }

    if (!config["retry_attempts"])
    {
        config["retry_attempts"] = 3;
    }

    if (!config["retry_delay"])
    {
        config["retry_delay"] = 2;
    }

    if (!config["max_connections"])
    {
        config["max_connections"] = 100;
    }

    if (!config["logging"] || !config["logging"]["enabled"] || !config["logging"]["file"])
    {
        throw std::runtime_error("Error: 'logging.enabled' and 'logging.file' must be specified.");
    }

    if (!config["health_check"] || !config["health_check"]["enabled"] || !config["health_check"]["interval"])
    {
        throw std::runtime_error("Error: 'health_check.enabled' and 'health_check.interval' must be specified.");
    }

    if (!config["tcp_keep_alive"])
    {
        config["tcp_keep_alive"]["enabled"] = false;
    }
    else
    {
        if (!config["tcp_keep_alive"]["idle_time"])
        {
            config["tcp_keep_alive"]["idle_time"] = 30;
        }
        if (!config["tcp_keep_alive"]["interval"])
        {
            config["tcp_keep_alive"]["interval"] = 10;
        }
        if (!config["tcp_keep_alive"]["count"])
        {
            config["tcp_keep_alive"]["count"] = 5;
        }
    }
}

class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(boost::asio::io_context &io_context, tcp::socket in_socket, const tcp::endpoint &target_endpoint,
            std::size_t buffer_size, bool tcp_no_delay, int retry_attempts, int retry_delay, Logger &logger,
            std::atomic<int> &active_connections, const YAML::Node &tcp_keep_alive)
        : io_context_(io_context),
          in_socket_(std::move(in_socket)),
          out_socket_(io_context),
          target_endpoint_(target_endpoint),
          buffer_size_(buffer_size),
          tcp_no_delay_(tcp_no_delay),
          retry_attempts_(retry_attempts),
          retry_delay_(retry_delay),
          current_attempt_(0),
          timer_(io_context),
          logger_(logger),
          active_connections_(active_connections),
          tcp_keep_alive_(tcp_keep_alive),
          data_in_(buffer_size),
          data_out_(buffer_size)
    {
        ++active_connections_;
        logger_.debug("Session created. Active connections: " + std::to_string(active_connections_));

    boost::system::error_code ec;
    in_socket_.set_option(tcp::no_delay(tcp_no_delay_), ec);
    if (ec)
    {
        logger_.error("seting up TCP nodelay on incoming socket failed: " + ec.message());
    }
    else
    {
        logger_.info("TCP nodelay has been set on incoming socket");
    }
}

~Session()
    {
        --active_connections_;
        logger_.debug("Session destroyed. Active connections: " + std::to_string(active_connections_));
    }
    void set_keep_alive_options(tcp::socket &socket)
    {
        if (tcp_keep_alive_["enabled"].as<bool>())
        {
            boost::system::error_code ec;
            socket.set_option(boost::asio::socket_base::keep_alive(true), ec);
            if (ec)
            {
                logger_.warn("enabling TCP keepalive failed: " + ec.message());
                return;
            }

#ifdef _WIN32
            logger_.info("Keepalive enabled but not configurable on Windows");
#else
            int idle = tcp_keep_alive_["idle_time"].as<int>();
            int interval = tcp_keep_alive_["interval"].as<int>();
            int count = tcp_keep_alive_["count"].as<int>();

            if (setsockopt(socket.native_handle(), SOL_TCP, TCP_KEEPIDLE, &idle, sizeof(idle)) < 0 ||
                setsockopt(socket.native_handle(), SOL_TCP, TCP_KEEPINTVL, &interval, sizeof(interval)) < 0 ||
                setsockopt(socket.native_handle(), SOL_TCP, TCP_KEEPCNT, &count, sizeof(count)) < 0)
            {
                logger_.warn("seting up TCP keepalive parameters failed");
            }
            else
            {
                logger_.info("TCP keepalive parameters set: idle=" + std::to_string(idle) +
                             ", interval=" + std::to_string(interval) +
                             ", count=" + std::to_string(count));
            }
#endif
        }
    }

    void start()
    {
        logger_.trace("Starting session...");
        set_keep_alive_options(in_socket_);
        attempt_connection();
    }

private:
    void attempt_connection()
    {
        if (current_attempt_ >= retry_attempts_)
        {
            logger_.error("Max retry attempts reached. Connection failed.");
            return;
        }

        auto self(shared_from_this());
        out_socket_.async_connect(target_endpoint_, [this, self](boost::system::error_code ec)
                                  {
        if (!ec)
        {
            logger_.info("Connected to target endpoint.");
            set_keep_alive_options(out_socket_);
            plz_forward();
        }
        else
        {
            logger_.warn("Connection attempt failed: " + ec.message());
            ++current_attempt_;
            timer_.expires_after(std::chrono::seconds(retry_delay_));
            timer_.async_wait([this, self](boost::system::error_code) { attempt_connection(); });
        } });
    }

    void plz_forward()
    {
        logger_.trace("Starting data forwarding...");
        forward_data(in_socket_, out_socket_, data_in_);
        forward_data(out_socket_, in_socket_, data_out_);
    }

    void forward_data(tcp::socket &source, tcp::socket &destination, std::vector<char> &buffer)
    {
        auto self(shared_from_this());
        source.async_read_some(boost::asio::buffer(buffer), [this, self, &source, &destination, &buffer](boost::system::error_code ec, std::size_t length)
                               {
        if (!ec)
        {
            logger_.debug("Data read from source. Length: " + std::to_string(length));
            boost::asio::async_write(destination, boost::asio::buffer(buffer, length),
                                     [this, self, &source, &destination, &buffer](boost::system::error_code write_ec, std::size_t /*bytes_transferred*/)
                                     {
                                         if (!write_ec)
                                         {
                                             logger_.trace("Data forwarded successfully.");
                                             forward_data(source, destination, buffer); 
                                         }
                                         else
                                         {
                                             logger_.warn("Write error: " + write_ec.message());
                                             clean_up(); 
                                         }
                                     });
        }
        else if (ec == boost::asio::error::eof)
        {
            logger_.info("EOF received.. closing connection.");
            clean_up(); 
        }
        else
        {
            logger_.error("Read error: " + ec.message());
            clean_up(); 
        } });
    }

    void clean_up()
    {
        boost::system::error_code ec;
        if (in_socket_.is_open())
        {
            in_socket_.shutdown(tcp::socket::shutdown_both, ec);
            in_socket_.close(ec);
        }
        if (out_socket_.is_open())
        {
            out_socket_.shutdown(tcp::socket::shutdown_both, ec);
            out_socket_.close(ec);
        }
        logger_.info("Session cleaned up. Sockets closed.");
    }

    void close_sockets()
    {
        boost::system::error_code ec;
        in_socket_.close(ec);
        out_socket_.close(ec);
        logger_.info("Sockets closed");
    }

    boost::asio::io_context &io_context_;
    tcp::socket in_socket_;
    tcp::socket out_socket_;
    tcp::endpoint target_endpoint_;
    std::size_t buffer_size_;
    bool tcp_no_delay_;
    int retry_attempts_;
    int retry_delay_;
    int current_attempt_;
    boost::asio::steady_timer timer_;
    Logger &logger_;
    std::atomic<int> &active_connections_;
    std::vector<char> data_in_;
    std::vector<char> data_out_;
    YAML::Node tcp_keep_alive_;
};

class HealthChecker
{
public:
    HealthChecker(boost::asio::io_context &io_context, int interval, Logger &logger)
        : timer_(io_context),
          interval_(interval),
          logger_(logger) {}

    void start()
    {
    
        logger_.trace("Starting health checks...");
        schedule_check();
    }

private:
    void schedule_check()
    {
        timer_.expires_after(std::chrono::seconds(interval_));
        timer_.async_wait([this](boost::system::error_code ec)
                          {
            if (!ec) {
                logger_.info("Health check: System is operational");
                schedule_check();
            } });
    }

    boost::asio::steady_timer timer_;
    int interval_;
    Logger &logger_;
};

class TCPForwarder
{
public:
    TCPForwarder(boost::asio::io_context &io_context, const YAML::Node &config, Logger &logger)
        : io_context_(io_context),
          logger_(logger),
          config_(config),
          active_connections_(0)
    {
        logger_.trace("Initializing TCP Forwarder...");

        std::size_t buffer_size = config["buffer_size"].as<std::size_t>();
        bool tcp_no_delay = config["tcp_no_delay"].as<bool>();
        int retry_attempts = config["retry_attempts"].as<int>();
        int retry_delay = config["retry_delay"].as<int>();
        int max_connections = config["max_connections"].as<int>();

        if (!config["forwarders"] || !config["forwarders"].IsSequence())
        {
            throw std::runtime_error("Error: 'forwarders' must be specified and must be a sequence.");
        }

        for (const auto &forwarder : config["forwarders"])
        {
            if (!forwarder["listen_address"] || !forwarder["target_address"])
            {
                logger_.error("Forwarder configuration must include 'listen_address' and 'target_address'. Skipping this entry.");
                continue;
            }

            std::string listen_address = forwarder["listen_address"].as<std::string>();
            std::string target_address = forwarder["target_address"].as<std::string>();

            try
            {
                if (forwarder["port_range"])
                {
                    int start_port = forwarder["port_range"]["start"].as<int>();
                    int end_port = forwarder["port_range"]["end"].as<int>();
                    for (int port = start_port; port <= end_port; ++port)
                    {
                        tcp::endpoint listen_endpoint(boost::asio::ip::make_address(listen_address), port);
                        tcp::endpoint target_endpoint(boost::asio::ip::make_address(target_address), port);
                        start_con(listen_endpoint, target_endpoint, buffer_size, tcp_no_delay, retry_attempts, retry_delay, max_connections);
                    }
                }
                else
                {

                    if (!forwarder["listen_port"] || !forwarder["target_port"])
                    {
                        logger_.error("Forwarder configuration must include 'listen_port' and 'target_port' if 'port_range' is not specified.");
                        continue;
                    }

                    int listen_port = forwarder["listen_port"].as<int>();
                    int target_port = forwarder["target_port"].as<int>();

                    tcp::endpoint listen_endpoint(boost::asio::ip::make_address(listen_address), listen_port);
                    tcp::endpoint target_endpoint(boost::asio::ip::make_address(target_address), target_port);
                    start_con(listen_endpoint, target_endpoint, buffer_size, tcp_no_delay, retry_attempts, retry_delay, max_connections);
                }
            }
            catch (const std::exception &e)
            {
                logger_.error("initializing forwarder failed: " + std::string(e.what()));
            }
        }
    }

private:
    void start_con(const tcp::endpoint &listen_endpoint, const tcp::endpoint &target_endpoint, std::size_t buffer_size,
                   bool tcp_no_delay, int retry_attempts, int retry_delay, int max_connections)
    {
        try
        {
            auto acceptor = std::make_shared<tcp::acceptor>(io_context_, listen_endpoint);
            logger_.info("Listening on " + listen_endpoint.address().to_string() + ":" + std::to_string(listen_endpoint.port()));

            YAML::Node tcp_keep_alive_config = config_["tcp_keep_alive"];

            plz_accept(acceptor, target_endpoint, buffer_size, tcp_no_delay, retry_attempts, retry_delay, max_connections, tcp_keep_alive_config);
        }
        catch (const std::exception &e)
        {
            logger_.error("couldn't start listener on " + listen_endpoint.address().to_string() + ":" +
                          std::to_string(listen_endpoint.port()) + ". error: " + e.what());
        }
    }

    void plz_accept(std::shared_ptr<tcp::acceptor> acceptor, const tcp::endpoint &target_endpoint, std::size_t buffer_size,
                    bool tcp_no_delay, int retry_attempts, int retry_delay, int max_connections, const YAML::Node &tcp_keep_alive_config)
    {
        acceptor->async_accept([this, acceptor, target_endpoint, buffer_size, tcp_no_delay, retry_attempts, retry_delay, max_connections, tcp_keep_alive_config](boost::system::error_code ec, tcp::socket in_socket)
                               {
            if (!ec)
            {
                if (active_connections_ >= max_connections)
                {
                    logger_.warn("Max connections reached. Rejecting new connection.");
                    in_socket.close();
                }
                else
                {
                    logger_.info("Accepted new connection");
                    std::make_shared<Session>(io_context_, std::move(in_socket), target_endpoint, buffer_size, tcp_no_delay,
                                              retry_attempts, retry_delay, logger_, active_connections_, tcp_keep_alive_config)
                        ->start();
                }
            }
            else
            {
                logger_.error("Accept error: " + ec.message());
            }

            plz_accept(acceptor, target_endpoint, buffer_size, tcp_no_delay, retry_attempts, retry_delay, max_connections, tcp_keep_alive_config); });
    }

    boost::asio::io_context &io_context_;
    Logger &logger_;
    YAML::Node config_;
    std::atomic<int> active_connections_;
};

int main(int argc, char *argv[])
{
    try
    {
        if (argc != 2)
        {
            std::cerr << "Please provide the path to the configuration file." << std::endl;
            help();
            return 1;
        }

        YAML::Node config = YAML::LoadFile(argv[1]);
        validate_and_set_defaults(config);

        bool logging_enabled = config["logging"]["enabled"].as<bool>();
        std::string log_file = config["logging"]["file"].as<std::string>();
        std::string log_level = config["logging"]["level"].as<std::string>();

        Logger logger(logging_enabled, log_file, log_level);

        int num_threads = config["thread_pool"]["threads"].as<int>();
        bool health_check_enabled = config["health_check"]["enabled"].as<bool>();
        int health_check_interval = config["health_check"]["interval"].as<int>();

        boost::asio::io_context io_context;
        boost::asio::thread_pool thread_pool(num_threads);

        TCPForwarder forwarder(io_context, config, logger);

        if (health_check_enabled)
        {
            HealthChecker health_checker(io_context, health_check_interval, logger);
            health_checker.start();
        }

        for (int i = 0; i < num_threads; ++i)
        {
            boost::asio::post(thread_pool, [&io_context]()
                              { io_context.run(); });
        }

        thread_pool.join();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception in the net: " << e.what() << std::endl;
        help();
    }
    catch (...)
    {
        std::cerr << "unknown exception in the net!" << std::endl;
        help();
    }

    return 0;
}
