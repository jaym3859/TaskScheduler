#include "TaskScheduler.h"

TaskScheduler::TaskScheduler() {
    clock_ = std::make_shared<Clock>();
    priority_bins_.resize(static_cast<size_t>(PriorityLevel::BLOCKED)+1);

    // Create threads with the shared message queue
    for (size_t i = 0; i < std::thread::hardware_concurrency() - 1; ++i) {  // num_threads-1? This seems odd, unless there's a reason
        std::shared_ptr<T_Thread> new_thread = std::make_shared<T_Thread>();
        thread_pool_.insert({ new_thread->get_id(), new_thread });
    }

    worker_thread_ = std::thread(&TaskScheduler::worker, this);
}


TaskScheduler::~TaskScheduler() {
    if (stop_flag_ == false)
        stop_all(); // stop all workers and join if possible
}

void TaskScheduler::add_task(std::shared_ptr<BaseTask> task) {
    size_t bin_index = static_cast<size_t>(task->get_priority());
    if (bin_index < priority_bins_.size()) {
        std::lock_guard<std::mutex> lock(bins_mutex_);  // Lock only when adding tasks to the queue
        priority_bins_[bin_index].push(task); // Add task to the bin
        Logger::Get()->log(log_level::Info, "Task added to bin: "); 
        cv_.notify_one();  // Notify the worker thread to check for new tasks
    }
    else {
        Logger::Get()->log(log_level::Error, "Invalid priority level!");
    }
};

void TaskScheduler::schedule_task(std::shared_ptr<BaseTask> task, size_t interval_ms) {
    std::string id = task->get_id();
    Periodic_Task pt(task, std::chrono::milliseconds(interval_ms));
    {
        std::lock_guard<std::mutex> lock(scheduled_tasks_mutex_);
        scheduled_tasks_[id] = pt;
    }
}

void TaskScheduler::stop_all() {
    stop_flag_ = true;
    cv_.notify_all();

    // Stop worker threads first
    for (auto& thread : thread_pool_) {
        thread.second->stop();
    }

    {
        // Lock and drain the task queues instead of clearing containers
        std::lock_guard<std::mutex> lock(bins_mutex_);

        for (auto& bin : priority_bins_) {
            std::queue<std::shared_ptr<BaseTask>> empty;
            std::swap(bin, empty);  // clears bin without invalidating vector
        }

        scheduled_tasks_.clear();  // okay to clear now that stop_flag is set
    }

    if (worker_thread_.joinable()) {
        worker_thread_.join();  // now it's safe to wait for the worker
    }

    // Now safe to clear containers
    thread_pool_.clear();
    priority_bins_.clear();  // container itself can be destroyed
    Logger::Get()->log(log_level::Info, "All tasks and threads have been cleared.");
}

std::shared_ptr<Clock> TaskScheduler::get_clock() {
    return clock_;
};
//stop a task
void TaskScheduler::stop_task(const std::string& id) {
    std::lock_guard<std::mutex> lock(scheduled_tasks_mutex_);
    scheduled_tasks_.erase(id);
}
//pause a task
void TaskScheduler::pause_task(std::string id) {
    std::lock_guard<std::mutex> lock(scheduled_tasks_mutex_);
    auto it = scheduled_tasks_.find(id);
    if (it != scheduled_tasks_.end()) {
        it->second.task->pause_task();
    }
    else {
        Logger::Get()->log(log_level::Warning, "Pause failed: task not found with id: " + id);
    }
}

void TaskScheduler::resume_task(std::string id) {
    std::lock_guard<std::mutex> lock(scheduled_tasks_mutex_);
    auto it = scheduled_tasks_.find(id);
    if (it != scheduled_tasks_.end()) {
        it->second.task->resume_task();
    }
    else {
        Logger::Get()->log(log_level::Warning, "Resume failed: task not found with id: " + id);
    }
}

std::shared_ptr<std::unordered_map<std::thread::id, std::shared_ptr<T_Thread>>> TaskScheduler::get_thread_map() {
    return std::make_shared<std::unordered_map<std::thread::id, std::shared_ptr<T_Thread>>>(thread_pool_);
}
void TaskScheduler::PostMessage(const Message& msg) {
    for (auto& thread : thread_pool_) {
        thread.second->set_message(msg);
    }
}
void TaskScheduler::worker() {
    clock_->start();  // Start the clock before entering the loop

    while (true) {
        std::shared_ptr<BaseTask> current_task = nullptr;

        {
            std::unique_lock<std::mutex> lock(bins_mutex_);

            // Wait for a signal indicating either a new task or stop
            cv_.wait(lock, [this]() {
                return stop_flag_ || !all_queues_empty() || !scheduled_tasks_.empty();
                });

            // If stop flag is set and there are no tasks to process, exit the loop
            if (stop_flag_ && all_queues_empty() && scheduled_tasks_.empty()) {
                break;
            }

            // Process regular tasks if available
            if (!all_queues_empty()) {
                handle_regular_tasks();
            }

            // Process periodic tasks if available
            if (!scheduled_tasks_.empty()) {
                handle_periodic_tasks();
            }
        } // Lock released here, the worker can now process tasks

        // If there are no tasks to process and we're not stopping, sleep for a while
        if (all_queues_empty() && scheduled_tasks_.empty()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Prevent busy-waiting
        }
    }
    // Optionally log when the worker is stopping
    Logger::Get()->log(log_level::Info, "Worker thread exiting.");
}

bool TaskScheduler::all_queues_empty() {
    for (const auto& bin : priority_bins_) {
        if (!bin.empty()) return false;
    }
    return true;
};

//handle regulat tasks
void TaskScheduler::handle_regular_tasks() {
    for (auto& thread : thread_pool_) {
        if (thread.second->get_message().type_ == MessageType::Pool) {
            std::shared_ptr<BaseTask> task = nullptr;
            for (auto& bin : priority_bins_) {
                if (!bin.empty()) {
                    task = bin.front();
                    bin.pop();  // Remove the task from the bin
                    thread.second->set_task(task);  // Assign the task to the thread
                    Logger::Get()->log(log_level::Info, "Assigning task to thread.");
                    return;  // Stop once a task is assigned
                }
            }
        }
    }
}
//handle periodic tasks
void TaskScheduler::handle_periodic_tasks() {
    for (auto& task_info : scheduled_tasks_) {
        if (task_info.second.is_time_to_run()) {
            if (task_info.second.task->is_paused()) {
                Logger::Get()->log(log_level::Info, "Skipping paused task: " + task_info.first);
                continue; // Skip if the task is paused
            }
            auto thread = get_available_thread();
            if (thread) {
                thread->set_task(task_info.second.task);
                Logger::Get()->log(log_level::Info, "Executing periodic task.");
                task_info.second.update_execution_time();
            }
        }
    }
}

std::shared_ptr<T_Thread> TaskScheduler::get_available_thread() {
    for (auto& thread : thread_pool_) {
        if (thread.second->get_message().type_ == MessageType::Pool) {
            return thread.second;
        }
    }
    return nullptr;  // No available thread
}




