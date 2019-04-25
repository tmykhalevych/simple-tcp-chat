#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <ctime>
#include "logger.h"

//#define DEBUG_LOGGER

#define SRV_SYNC_OBJECT                 _sync_obj
#define SRV_LOGGING_INTERVAL_MS         50
#define SRV_LOGGING_CHECK_INTERVAL_S    10
#ifdef _WIN32
    #include <windows.h>
    #define SRV_CREATE_SYNC_OBJECT      CRITICAL_SECTION SRV_SYNC_OBJECT;
    #define SRV_INIT_SYNC_OBJECT 		InitializeCriticalSection(&SRV_SYNC_OBJECT);
    #define SRV_DELETE_SYNC_OBJECT      DeleteCriticalSection(&SRV_SYNC_OBJECT);
    #define SRV_ENTER_CRITICAL_SECTION  EnterCriticalSection(&SRV_SYNC_OBJECT);
    #define SRV_EXIT_CRITICAL_SECTION   LeaveCriticalSection(&SRV_SYNC_OBJECT);
#else
    #include <mutex>
    #define SRV_CREATE_SYNC_OBJECT      std::mutex SRV_SYNC_OBJECT;
    #define SRV_INIT_SYNC_OBJECT 		
    #define SRV_DELETE_SYNC_OBJECT      
    #define SRV_ENTER_CRITICAL_SECTION  SRV_SYNC_OBJECT.lock();
    #define SRV_EXIT_CRITICAL_SECTION   SRV_SYNC_OBJECT.unlock();
#endif

#define SRV_FOREVER                     while(true)
#define SRV_RELEASE_THREAD              return 0;
#define SRV_CREATE_THREAD(FUNK)         std::thread _thr(FUNK); \
                                        _thr.detach();

using namespace infra;

SRV_CREATE_SYNC_OBJECT

std::size_t Logger::_stack_size = 0;
queue_t Logger::_msg_queue = queue_t();
LogWriter Logger::_log_writer = LogWriter(Logger::_msg_queue, Logger::_write);

void Logger::log(const std::string& msg, const std::string& mdl, const std::string file, const int line) noexcept {
    if (_log_writer._killed) {
        _log_writer._create();
    }
    std::string message = msg;
    std::string module = mdl;
    _normalize(message, module);
    SRV_ENTER_CRITICAL_SECTION
    _msg_queue.push(message);
    SRV_EXIT_CRITICAL_SECTION
}

void Logger::_normalize(std::string& msg, std::string& mdl) {
    std::ostringstream oss;
    std::stringstream ss(msg);
    std::string line;
    while (std::getline(ss, line, '\n')) {
//#ifdef _TIMED_LOG
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now - std::chrono::hours(24));
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
#pragma warning(disable : 4996) // Disable SECURE WARNINGS to use std::localtime in MS VS
        oss << std::put_time(std::localtime(&now_c), "%H:%M:%S");
#pragma warning(default : 4996)
        oss << '.' << std::setfill('0') << std::setw(3) << ms.count() << " ";
//#endif
#ifdef _LINED_LOG
        // TODO
#endif
        oss << '[';
        oss << std::setfill(' ') << std::setw(3) << mdl;
        oss << "] ";
        oss << Logger::_stack_offset() << line << std::endl;
    }
    msg = oss.str();
}

void Logger::_write(const std::string& message) {
    std::cout << message;
}

LogWriter::LogWriter(queue_t& q_ref, log_func log_f)
    : _queue_ref(q_ref)
    , _write(log_f)
    , _killed(false)
{
    SRV_INIT_SYNC_OBJECT
    _create();
}

void LogWriter::_create() {
    _killed = false;
#ifdef DEBUG_LOGGER
    _write(" --- Writer has been CREATED --- ");
#endif
    SRV_CREATE_THREAD(std::bind(&LogWriter::_main, this))
}

LogWriter::~LogWriter() {
    SRV_DELETE_SYNC_OBJECT
}

// Thread
int LogWriter::_main() {
    std::time_t last_wright_time = std::time(nullptr);

    SRV_FOREVER {
        SRV_ENTER_CRITICAL_SECTION
        if (!_queue_ref.empty()) {
            last_wright_time = std::time(nullptr);
            _write(_queue_ref.front());
            _queue_ref.pop();
        }
        else if (std::difftime(std::time(nullptr), last_wright_time) > SRV_LOGGING_CHECK_INTERVAL_S) {
            _killed = true;
#ifdef DEBUG_LOGGER
            _write(" --- Writer has been KILLED --- ");
#endif
            SRV_EXIT_CRITICAL_SECTION
            SRV_RELEASE_THREAD
        }
        SRV_EXIT_CRITICAL_SECTION
        std::this_thread::sleep_for(std::chrono::milliseconds(SRV_LOGGING_INTERVAL_MS));
    }
}

#undef SRV_SYNC_OBJECT
#undef SRV_LOGGING_INTERVAL_MS
#undef SRV_CREATE_SYNC_OBJECT
#undef SRV_INIT_SYNC_OBJECT
#undef SRV_DELETE_SYNC_OBJECT
#undef SRV_ENTER_CRITICAL_SECTION
#undef SRV_EXIT_CRITICAL_SECTION
