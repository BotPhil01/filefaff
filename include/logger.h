#pragma once
#include <iostream>
#include <mutex>
#include <fstream>
#include <string_view>
#include <unordered_map>

enum class LOGMODE {
    NONE,
    CLIENT,
    SERVER,
    TEST
};

namespace {
    using namespace std::string_view_literals;
    const std::unordered_map<LOGMODE, std::string> fileMappings {
        {LOGMODE::NONE, "log"},
        {LOGMODE::CLIENT, "client"},
        {LOGMODE::SERVER, "server"},
        {LOGMODE::TEST, "test"},
    };
}

class logger {
    private:
        inline static std::mutex accessMutex;
        inline static std::ofstream stream{"log/.log"};

        logger() = default;
        
    public:
        logger(logger& copy) = delete;
        logger& operator=(logger& copy) = delete;

        inline static logger& getInstance(const LOGMODE m) {
            stream = std::ofstream{"log/" + fileMappings.at(m) + ".log"};
            static logger instance;
            return instance;
        }

        template<typename T>
        logger& operator<<(T msg) {
            accessMutex.lock();
            stream << msg << std::flush;
            accessMutex.unlock();
            return *this;
        }
};

extern logger& LOG;
