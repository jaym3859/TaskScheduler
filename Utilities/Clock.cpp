#include "Clock.h"
    // Default constructor - Starts with the clock paused
    Clock::Clock()
        : m_start_(std::chrono::steady_clock::now()),
        m_end_(m_start_),
        m_duration_(0),  // Start with no accumulated duration
        m_paused_(true) { // Start in paused state
    }

    // Copy constructor - Keeps the clock paused
    Clock::Clock(const Clock& other)
        : m_start_(other.m_start_),
        m_end_(other.m_end_),
        m_duration_(other.m_duration_),
        m_paused_(true) { // Copy constructor keeps the clock paused
    }

    // Move constructor - Keeps the clock paused after moving
    Clock::Clock(Clock&& other) noexcept
        : m_start_(std::move(other.m_start_)),
        m_end_(std::move(other.m_end_)),
        m_duration_(other.m_duration_),  // Move the duration
        m_paused_(other.m_paused_) {     // Move the pause state
        other.m_duration_ = std::chrono::steady_clock::duration::zero();
        other.m_paused_ = true;
        other.m_start_ = std::chrono::steady_clock::time_point();
        other.m_end_ = std::chrono::steady_clock::time_point();
    }

    Clock& Clock::operator=(Clock&& other) noexcept {
        if (this != &other) {
            std::lock_guard<std::mutex> lock(timer_mutex_);

            m_start_ = std::move(other.m_start_);
            m_end_ = std::move(other.m_end_);
            m_duration_ = other.m_duration_;
            m_paused_ = other.m_paused_;

            // Reset the moved-from object to a valid state
            other.m_duration_ = std::chrono::steady_clock::duration::zero();
            other.m_paused_ = true;
            other.m_start_ = std::chrono::steady_clock::time_point();
            other.m_end_ = std::chrono::steady_clock::time_point();
        }
        return *this;
    }

    // Copy assignment operator
    Clock& Clock::operator=(const Clock& other) {
        if (this != &other) {
            std::lock_guard<std::mutex> lock(timer_mutex_);
            m_start_ = other.m_start_;
            m_end_ = other.m_end_;
            m_duration_ = other.m_duration_;
            m_paused_ = other.m_paused_;
        }
        return *this;
    }

    // Destructor
    Clock::~Clock() {}

   void Clock::stop() {
        std::lock_guard<std::mutex> lock(timer_mutex_);
        if (!m_paused_) {
            m_end_ = std::chrono::steady_clock::now();
            auto time_elapsed = m_end_ - m_start_;
            m_duration_ += time_elapsed;
            m_paused_ = true;

            // Optionally reset m_duration if you want to clear the accumulated time on each stop
            // m_duration = std::chrono::high_resolution_clock::duration::zero();
        }
    }
    // Resume the clock (continue from the last start time without resetting start)
    void Clock::resume() {
        std::lock_guard<std::mutex> lock(timer_mutex_);
        if (m_paused_) {
            m_start_ = std::chrono::steady_clock::now() - (m_end_ - m_start_); // Calculate time difference
            m_paused_ = false;
        }
    }

    // Start the clock (begin measuring time)
    void Clock::start() {
        std::lock_guard<std::mutex> lock(timer_mutex_);
        if (m_paused_) {
            m_start_ = std::chrono::steady_clock::now();
            m_end_ = m_start_; // Ensure m_end is reset
            m_paused_ = false;
        }
    }

    // Reset the clock (clears the accumulated time)
    void Clock::reset() {
        std::lock_guard<std::mutex> lock(timer_mutex_);
        m_end_ = std::chrono::steady_clock::now();
        m_start_ = m_end_;
        m_duration_ = std::chrono::steady_clock::duration::zero();
        m_paused_ = true;
    }

    // Get the elapsed time in milliseconds
    long long Clock::elapsed_ms() const {
        std::lock_guard<std::mutex> lock(timer_mutex_);

        if (m_paused_) {
            return std::chrono::duration_cast<std::chrono::milliseconds>(m_duration_).count();
        }
        else {
            auto now = std::chrono::steady_clock::now();
            auto total_elapsed = m_duration_ + (now - m_start_);
            return std::chrono::duration_cast<std::chrono::milliseconds>(total_elapsed).count();
        }
    }


