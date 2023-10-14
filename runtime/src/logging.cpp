#include "logging.hpp"

#include <iostream>

void Logger::info(std::string_view msg)
{
    log(INFO, msg);
}

void Logger::warning(std::string_view msg)
{
    log(WARNING, msg);
}

void Logger::error(std::string_view msg)
{
    log(ERROR, msg);
}

void Logger::fatal(std::string_view msg)
{
    log(FATAL, msg);
}

std::string Logger::prefix(Severity severity)
{
    std::string res;

    static const char* labels[4] = { "[INFO]   ", "[WARNING]", "[ERROR]  ", "[FATAL]  " };
    res += labels[severity];

    return res;
}

void Logger::log(Severity severity, std::string_view msg)
{
    std::unique_lock lock(mutex);

    if (severity > INFO) {
        std::cerr << prefix(severity) << msg << "\n";
    } else {
        std::cout << prefix(severity) << msg << "\n";
    }

    if (severity == FATAL) {
        exit(1);
    }
}
