#pragma once
#include "Logger.h"
#include <iostream>

class ConsoleLogger : public Logger {
public:
    void info(const std::string& msg) override {
        std::cout << "[INFO] " << msg << std::endl;
    }

    void warn(const std::string& msg) override {
        std::cout << "[WARN] " << msg << std::endl;
    }

    void error(const std::string& msg) override {
        std::cerr << "[ERROR] " << msg << std::endl;
    }
};
