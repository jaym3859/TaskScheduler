#pragma once
#include "TaskScheduler.h"

class TaskManager {
public:
    // Delete the constructor so no one else can create instances of this manager
    TaskManager() = delete;
    TaskManager(const TaskManager& other) = delete;
    TaskManager& operator=(const TaskManager& other) = delete;
    // Delete the constructor so no one else can create instances of this manager
    ~TaskManager() = default;

    // Singleton accessor
    static std::shared_ptr<TaskScheduler> GetScheduler();
    static std::shared_ptr<std::unordered_map<std::thread::id, std::shared_ptr<T_Thread>>>  GetThreadMap();
private:
    static std::shared_ptr<TaskScheduler> task_scheduler_;  // The singleton TaskScheduler instance
};

