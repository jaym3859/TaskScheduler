
#include "T_Thread.h"

T_Thread::T_Thread()
    : t_thread_([this]() { this->worker(); }) {
}

// Destructor
T_Thread::~T_Thread() {
    stop();
    if (t_thread_.joinable()) {
        t_thread_.join();
    }
}

// Set the task
void T_Thread::set_task(std::shared_ptr<BaseTask> task) {
    std::lock_guard<std::mutex> lock(thread_mutex_);
    task_ = task;
    cv_.notify_one();
}

//set the message
void T_Thread::set_message(Message msg) {
    std::lock_guard<std::mutex> lock(thread_mutex_);
    message_.type_ = msg.type_;
}

// Stop the thread
void T_Thread::stop() {
    std::lock_guard<std::mutex> lock(thread_mutex_);
    message_.type_ = MessageType::Stop;
    cv_.notify_one();
}

// Get the thread's status
Message T_Thread::get_message() {
    std::lock_guard<std::mutex> lock(thread_mutex_);
    return message_;
}
std::thread::id T_Thread::get_id() const {
    return t_thread_.get_id();  // Assuming t_thread_ is the actual std::thread object
}

// Worker loop that listens for tasks and messages
void T_Thread::worker() {
    message_.type_ = MessageType::Pool;
    while (true) {
        std::shared_ptr<BaseTask> current_task;

        {
            std::unique_lock<std::mutex> lock(thread_mutex_);
            cv_.wait(lock, [this]() {
                return task_ != nullptr || message_.type_ == MessageType::Stop;
                });

            if (message_.type_ == MessageType::Stop) break;

            current_task = task_;
            task_ = nullptr;
        }

        if (current_task) {
            std::unique_lock<std::mutex> lock(results_mutex_);
            // Execute task if available
            message_.type_ = MessageType::Resume;
            current_task->execute();
            Logger::Get()->log(log_level::Info, "task executed returning to pool");
            current_task->set_completed();
            message_.type_ = MessageType::Pool;
        }
    }
}

