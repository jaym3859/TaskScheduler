#pragma once
#include <chrono>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip> // For formatting output
#include <mutex>

class Clock {
public:
    // Default constructor - Starts with the clock paused
    Clock();
    // Destructor
    ~Clock();
    // Copy constructor - Keeps the clock paused
    Clock(const Clock& other);
    // Move constructor - Keeps the clock paused after moving
    Clock(Clock&& other) noexcept;
    // Move assignment operator
    Clock& operator=(Clock&& other) noexcept;
    // Copy assignment operator
    Clock& operator=(const Clock& other);
    // Convert the current time duration to a string formatted as "HH:MM:SS.MS"
    std::string to_string() const;
    // Stop the clock (accumulate the time)
    void stop();
    // Resume the clock without restart 
    void resume();
    // Start the clock (begin measuring time)
    void start();
    // Reset the clock (clears the accumulated time)
    void reset();
    // Get the elapsed time in milliseconds
    long long elapsed_ms() const;

private:
    std::chrono::time_point<std::chrono::steady_clock> m_start_; // start timepoint
    std::chrono::time_point<std::chrono::steady_clock> m_end_;  // end timepoint
    std::chrono::steady_clock::duration m_duration_;           // Using high_resolution_clock::duration for simplicity
    bool m_paused_;                                           // clock timer pause flag
    mutable std::mutex timer_mutex_;                         // Mutex for thread-safety
};