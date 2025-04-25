#include "TaskManager.h"

// Define and initialize the static member
std::shared_ptr<TaskScheduler> TaskManager::task_scheduler_ = nullptr;
std::mutex TaskManager::mutex_;

// Get the singleton instance of TaskScheduler
std::shared_ptr<TaskScheduler> TaskManager::Scheduler() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!task_scheduler_) 
        task_scheduler_ = std::make_shared<TaskScheduler>();  // First-time initialization

    return task_scheduler_;
}
//get threadmap
std::shared_ptr<std::unordered_map<std::thread::id,std::shared_ptr<T_Thread>>> TaskManager::ThreadMap() {
    std::lock_guard<std::mutex> lock(mutex_);
    return Scheduler()->GetThreadMap();
}
std::shared_ptr<GameTimer> TaskManager::Clock() {
    std::lock_guard<std::mutex> lock(mutex_);
    return Scheduler()->get_clock();
}
