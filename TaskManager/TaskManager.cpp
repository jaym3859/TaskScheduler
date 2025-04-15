#include "TaskManager.h"

// Define and initialize the static member
std::shared_ptr<TaskScheduler> TaskManager::task_scheduler_ = nullptr;

// Get the singleton instance of TaskScheduler
std::shared_ptr<TaskScheduler> TaskManager::GetScheduler() {
    if (!task_scheduler_) {
        task_scheduler_ = std::make_shared<TaskScheduler>();  // First-time initialization
    }
    return task_scheduler_;
}
//get threadmap
std::shared_ptr<std::unordered_map<std::thread::id,std::shared_ptr<T_Thread>>> TaskManager::GetThreadMap() {
    return GetScheduler()->get_thread_map();
}
