#include "Clock.h"
    // Default constructor - Starts with the clock paused
    Clock::Clock()
        : m_start(std::chrono::steady_clock::now()),
        m_end(m_start),
        m_duration(0),  // Start with no accumulated duration
        m_paused(true) { // Start in paused state
    }

    // Copy constructor - Keeps the clock paused
    Clock::Clock(const Clock& other)
        : m_start(other.m_start),
        m_end(other.m_end),
        m_duration(other.m_duration),
        m_paused(true) { // Copy constructor keeps the clock paused
    }

    // Move constructor - Keeps the clock paused after moving
    Clock::Clock(Clock&& other) noexcept
        : m_start(std::move(other.m_start)),
        m_end(std::move(other.m_end)),
        m_duration(other.m_duration),
        m_paused(true) { // Set to paused when moved
    }

    Clock& Clock::operator=(Clock&& other) noexcept { // Notice Clock:: here
        if (this != &other) {
            std::lock_guard<std::mutex> lock(timer_mutex);

            m_start = std::move(other.m_start);
            m_end = std::move(other.m_end);
            m_duration = other.m_duration;
            m_paused = other.m_paused;

            // Optionally reset the moved-from object
            other.m_duration = m_duration;
            other.m_paused = false;
            other.m_start = std::chrono::steady_clock::time_point();
            other.m_end = std::chrono::steady_clock::time_point();
        }
        return *this;
    }

    // Copy assignment operator
    Clock& Clock::operator=(const Clock& other) {
        if (this != &other) {
            std::lock_guard<std::mutex> lock(timer_mutex);
            m_start = other.m_start;
            m_end = other.m_end;
            m_duration = other.m_duration;
            m_paused = other.m_paused;
        }
        return *this;
    }

    // Destructor
    Clock::~Clock() {}

    // Convert the current time duration to a string formatted as "HH:MM:SS.MS"
    std::string Clock::to_string() const {
        std::lock_guard<std::mutex> lock(timer_mutex);

        auto total_seconds = std::chrono::duration_cast<std::chrono::seconds>(m_duration);
        auto hours = std::chrono::duration_cast<std::chrono::hours>(total_seconds);
        total_seconds -= hours;
        auto minutes = std::chrono::duration_cast<std::chrono::minutes>(total_seconds);
        total_seconds -= minutes;
        auto seconds = total_seconds;
        long milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(m_duration).count() % 1000;

        // Format the time as "HH:MM:SS.MS"
        std::stringstream ss;
        ss << std::setw(2) << std::setfill('0') << hours.count() << ":"
            << std::setw(2) << std::setfill('0') << minutes.count() << ":"
            << std::setw(2) << std::setfill('0') << seconds.count() << "."
            << std::setw(3) << std::setfill('0') << milliseconds;

        return ss.str();
    }

    // Stop the clock (accumulate the time)
    void Clock::stop() {
        std::lock_guard<std::mutex> lock(timer_mutex);
        if (!m_paused) {
            m_end = std::chrono::steady_clock::now();
            auto time_elapsed = m_end - m_start;
            m_duration += time_elapsed;
            m_paused = true;
        }
    }

    // Start the clock (begin measuring time)
    void Clock::start() {
        std::lock_guard<std::mutex> lock(timer_mutex);
        if (m_paused) {
            m_start = std::chrono::steady_clock::now();
            m_end = m_start; // Ensure m_end is reset
            m_paused = false;
        }
    }

    // Reset the clock (clears the accumulated time)
    void Clock::reset() {
        std::lock_guard<std::mutex> lock(timer_mutex);
        m_end = std::chrono::steady_clock::now();
        m_start = m_end;
        m_duration = std::chrono::steady_clock::duration::zero();
        m_paused = true;
    }

    // Get the elapsed time in milliseconds
    long long Clock::elapsed_ms() const {
        std::lock_guard<std::mutex> lock(timer_mutex);  // Lock for thread-safety

        if (m_paused) {
            // If paused, return the duration accumulated so far in milliseconds
            return std::chrono::duration_cast<std::chrono::milliseconds>(m_duration).count();
        }
        else {
            // If running, return the total elapsed time from start to now, plus the accumulated duration
            auto now = std::chrono::steady_clock::now();
            auto total_elapsed = m_duration + (now - m_start);  // Total duration so far
            return std::chrono::duration_cast<std::chrono::milliseconds>(total_elapsed).count();
        }
    }

