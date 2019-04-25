#pragma once

#include <queue>
#include <sstream>
#include <functional>

#define SRV_LOG_STACK_OFFSET_STEP    3

namespace infra {

    typedef std::queue<std::string> queue_t;
    typedef std::function<void(const std::string&)> log_func;

    class LogWriter;
    class Logger
    {
    public:
        Logger()
            : _file("")
            , _line(0)
            , _function("")
            , _module("")
            , _is_real(false)
        {}

        Logger(const std::string file, const int line, const std::string function, const std::string module)
            : _file(file)
            , _line(line)
            , _function(function)
            , _module(module)
            , _is_real(true)
        {
            std::string msg = "scope " + _function + " {";
            Logger::log(msg, _module, _file, _line);
            Logger::_stack_size++;
        }

        ~Logger() {
            if (_is_real) {
                Logger::_stack_size--;
                std::string msg = "}";
                Logger::log(msg, _module, _file, _line);
                Logger::log("", _module, _file, _line);
            }
        }

    private:
        const std::string _file;
        const int _line;
        const std::string _function;
        const std::string _module;
        const bool _is_real;

    // Static members and methods
    public:
        static void log(const std::string&, const std::string&, const std::string, const int) noexcept;

    private:
        // Defines loging method
        static void _write(const std::string&);
        static void _normalize(std::string&, std::string&);

        static std::size_t _stack_size;
        static std::string _stack_offset() {
            std::ostringstream oss;
            for (std::size_t step = 0; step < _stack_size; step++) {
                oss << "|" + std::string(SRV_LOG_STACK_OFFSET_STEP, ' ');
            }
            return oss.str();
        }

        static queue_t _msg_queue;
        static LogWriter _log_writer;
    };

    class LogWriter {
        friend class Logger;

        LogWriter(queue_t&, log_func);
        ~LogWriter();

        int _main(); // Log writer thread
        void _create();

        queue_t& _queue_ref;
        log_func _write;
        bool _killed;
    };
}

#ifdef _LOG_ON
    class Loggable {
    protected:
        virtual std::string module()  { return std::string(' ', 3); }
        void log_msg(std::string msg) { infra::Logger::log(msg, module(), __FILE__, __LINE__); }
        void log_exp(std::string msg) { infra::Logger::log(" {EXP} " + msg, module(), __FILE__, __LINE__); }
    #ifdef _SCOPE_LOG_ON
        infra::Logger log_scp(std::string scp) { return infra::Logger(__FILE__, __LINE__, scp, module()); }
    #else
        infra::Logger log_scp(std::string scp) { return infra::Logger(); }
    #endif
    };

    #define LOG_GLOBAL_MSG(msg) infra::Logger::log(msg, "GLB", __FILE__, __LINE__);
    #define LOG_GLOBAL_EXP(msg) infra::Logger::log(" {EXP} " + (msg), "GLB", __FILE__, __LINE__);
#else
    class Loggable {
    protected:
        virtual std::string module() { return std::string(' ', 3); }
        void log_msg(std::string msg) {}
        void log_exp(std::string msg) {}
        infra::Logger log_scp(std::string scp) { return infra::Logger(); }
    };

    #define LOG_GLOBAL_MSG(msg) {}
    #define LOG_GLOBAL_EXP(msg) {}
#endif

#define LOG_MODULE(mdl)     std::string module() override { return (mdl); }
#define LOG_SCOPE			infra::Logger _scp_obj = log_scp(__FUNCTION__);
#define LOG_MSG(msg)		log_msg(msg);
#define LOG_EXP(exp)		log_exp(exp);

#undef SRV_LOG_STACK_OFFSET_STEP
