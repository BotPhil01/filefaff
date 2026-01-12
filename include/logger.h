#pragma once
#include <iostream>
#include <mutex>
#include <fstream>

class logger {
    private:
        inline static std::mutex accessMutex;
        inline static std::ofstream stream{"log/log.txt"};

        logger() = default;
        
    public:
        logger(logger& copy) = delete;
        logger& operator=(logger& copy) = delete;

        static logger& getInstance() {
            // static std::ofstream stream("log/log.txt");
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
