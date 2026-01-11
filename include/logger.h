#pragma once
#include <iostream>
#include <mutex>
#include <filesystem>
#include <fstream>

class logger {
    private:
        inline static std::mutex accessMutex;
        static std::ofstream stream;

        logger() = default;
        
    public:
        logger(logger& copy) = delete;
        logger& operator=(logger& copy) = delete;

        static logger& getInstance() {
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
