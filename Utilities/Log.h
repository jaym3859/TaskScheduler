#pragma once
#include <iostream>
#include <chrono>
#include <source_location>
#include <format>
#include <fstream>
#include <string>
#include <mutex>  // For thread safety

enum class log_level : char {
    Info = 'I',
    Warning = 'W',
    Error = 'E'
};

class Log {
public:
    Log();
    Log(const Log&) = delete;
    Log& operator=(const Log&) = delete;
    ~Log();

    void log(log_level level, const std::string& message, std::source_location location = std::source_location::current());

private:
    
    std::ofstream log_file_;  // Declaration of the static member
    std::mutex log_mutex_;    // Mutex for thread safety
};