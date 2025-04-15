#include "Log.h"


Log::Log() {
    log_file_.open("log.txt", std::ios::out | std::ios::trunc);
    if (!log_file_.is_open()) {
        std::cerr << "Failed to open log.txt for writing!" << std::endl;
    }
}

Log::~Log() {
    if (log_file_.is_open()) {
        log_file_.flush();  // Ensure the data is written
        log_file_.close();  // Close the file when done
    }
}

void Log::log(log_level level, const std::string& message, std::source_location location) {
    if (!Log::log_file_.is_open()) {
        std::cerr << "Log file not open, skipping log entry." << std::endl;
        return;
    }

    std::lock_guard<std::mutex> lock(log_mutex_);  // Ensure thread safety

    // Get current time in a readable format
    auto tp = std::chrono::system_clock::now();
    auto local_tp = std::chrono::zoned_time{ std::chrono::current_zone(), tp };
    std::string level_str;

    // Determine the log level string
    switch (level) {
    case log_level::Info:
        level_str = "INFO: ";
        break;
    case log_level::Warning:
        level_str = "WARNING: ";
        break;
    case log_level::Error:
        level_str = "ERROR: ";
        break;
    }

    try {
        // Write to the log file
        log_file_ << "[" << level_str << "] "
            << std::format("{:%F %T %Z}", local_tp)  // Format time
            << " | " << location.file_name()
            << ":" << location.function_name()
            << ":" << location.line()
            << " | " << message
            << std::endl;

        // Flush after writing to ensure it's written immediately
        log_file_.flush();
    }
    catch (const std::exception& e) {
        std::cerr << "Logger format error: " << e.what() << std::endl;
    }
}