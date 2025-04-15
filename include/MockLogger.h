#pragma once

#include "Logger.h"

#include <vector>
#include <string>

class MockLogger : public Logger {
public:
    void info(const std::string& msg) override { infos.push_back(msg); }
    void warn(const std::string& msg) override { warns.push_back(msg); }
    void error(const std::string& msg) override { errors.push_back(msg); }

    std::vector<std::string> infos;
    std::vector<std::string> warns;
    std::vector<std::string> errors;
};
