#pragma once

#include <mutex>
#include <string_view>

class Logger {
public:
    enum Severity : uint8_t {
        INFO = 0,
        WARNING,
        ERROR,
        FATAL
    };

    void info(std::string_view msg);
    void warning(std::string_view msg);
    void error(std::string_view msg);
    void fatal(std::string_view msg);

private:
    std::mutex mutex;

    std::string prefix(Severity severity);
    void log(Severity severity, std::string_view msg);
};

static inline Logger logger;

#ifdef DEBUG
#define INFO(msg) logger.info(msg)
#define WARNING(msg) logger.warning(msg)
#else
#define INFO(msg)
#define WARNING(msg)
#endif

#define ERROR(msg) logger.error(msg)
#define FATAL(msg) logger.fatal(msg)
