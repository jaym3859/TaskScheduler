#include "TaskScheduler.h"

TaskScheduler::TaskScheduler() {
    clock_ = std::make_shared<GameTimer>();
    priority_bins_.resize(static_cast<size_t>(PriorityLevel::BLOCKED)+1);

    // Create threads with the shared message queue
    for (size_t i = 0; i < std::thread::hardware_concurrency() - 1; ++i) {  // num_threads-1? This seems odd, unless there's a reason
        std::shared_ptr<T_Thread> new_thread = std::make_shared<T_Thread>();
        thread_pool_.insert({ new_thread->GetID(), new_thread });
    }

    workerThread = std::thread(&TaskScheduler::Worker, this);
}


TaskScheduler::~TaskScheduler() {
    if (stopFlag == false)
        StopAll(); // stop all workers and join if possible
}

void TaskScheduler::AddTask(std::shared_ptr<BaseTask> task_) {
    size_t bin_index = static_cast<size_t>(task_->GetPriority());
    if (bin_index < priority_bins_.size()) {
        std::lock_guard<std::mutex> lock(binsMutex);  // Lock only when adding tasks to the queue
        priority_bins_[bin_index].push(task_); // Add task_ to the bin
        Logger::Get()->LogInfo(Log_Level::Info, "Task added to bin: "); 
        cv.notify_one();  // Notify the Worker thread to check for new tasks
    }
    else {
        Logger::Get()->LogInfo(Log_Level::Error, "Invalid priority level!");
    }
};

void TaskScheduler::ScheduleTask(std::shared_ptr<BaseTask> task_, float interval) {
    std::string id = task_->GetID();
    Periodic_Task pt(task_, interval,get_clock());
    {
        std::lock_guard<std::mutex> lock(scheduledTasksMutex);
        scheduled_tasks_[id] = pt;
    }
}

void TaskScheduler::StopAll() {
    stopFlag = true;
    cv.notify_all();

    // Stop Worker threads first
    for (auto& thread : thread_pool_) {
        thread.second->stop();
    }

    {
        // Lock and drain the task_ queues instead of clearing containers
        std::lock_guard<std::mutex> lock(binsMutex);

        for (auto& bin : priority_bins_) {
            std::queue<std::shared_ptr<BaseTask>> empty;
            std::swap(bin, empty);  // clears bin without invalidating vector
        }

        scheduled_tasks_.clear();  // okay to clear now that stop_flag is set
    }

    if (workerThread.joinable()) {
        workerThread.join();  // now it's safe to wait for the Worker
    }

    // Now safe to clear containers
    thread_pool_.clear();
    priority_bins_.clear();  // container itself can be destroyed
    Logger::Get()->LogInfo(Log_Level::Info, "All tasks and threads have been cleared.");
}

std::shared_ptr<GameTimer> TaskScheduler::get_clock() {
    return clock_;
};
//stop a task_
void TaskScheduler::StopTask(const std::string& id) {
    std::lock_guard<std::mutex> lock(scheduledTasksMutex);
    scheduled_tasks_.erase(id);
}
//pause a task_
void TaskScheduler::PauseTask(std::string id) {
    std::lock_guard<std::mutex> lock(scheduledTasksMutex);
    auto it = scheduled_tasks_.find(id);
    if (it != scheduled_tasks_.end()) {
        it->second.task_->PauseTask();
    }
    else {
        Logger::Get()->LogInfo(Log_Level::Warning, "Pause failed: task not found with id: " + id);
    }
}

void TaskScheduler::ResumeTask(std::string id) {
    std::lock_guard<std::mutex> lock(scheduledTasksMutex);
    auto it = scheduled_tasks_.find(id);
    if (it != scheduled_tasks_.end()) {
        it->second.task_->ResumeTask();
    }
    else {
        Logger::Get()->LogInfo(Log_Level::Warning, "Resume failed: task not found with id: " + id);
    }
}

std::shared_ptr<std::unordered_map<std::thread::id, std::shared_ptr<T_Thread>>> TaskScheduler::GetThreadMap() {
    return std::make_shared<std::unordered_map<std::thread::id, std::shared_ptr<T_Thread>>>(thread_pool_);
}
void TaskScheduler::PostMessage(const Message& msg) {
    for (auto& thread : thread_pool_) {
        thread.second->SetMessage(msg);
    }
}
void TaskScheduler::Worker() {
    clock_->Start();  // Start the clock before entering the loop

    while (true) {
        std::shared_ptr<BaseTask> current_task = nullptr;

        {
            std::unique_lock<std::mutex> lock(binsMutex);

            // Wait for a signal indicating either a new task_ or stop
            cv.wait(lock, [this]() {
                return stopFlag || !AllQueuesEmpty() || !scheduled_tasks_.empty();
                });

            // If stop flag is set and there are no tasks to process, exit the loop
            if (stopFlag && AllQueuesEmpty() && scheduled_tasks_.empty()) {
                break;
            }

            // Process regular tasks if available
            if (!AllQueuesEmpty()) {
                HandleRegularTasks();
            }

            // Process periodic tasks if available
            if (!scheduled_tasks_.empty()) {
                HandlePeriodicTasks();
            }
        } // Lock released here, the Worker can now process tasks

        // If there are no tasks to process and we're not stopping, sleep for a while
        if (AllQueuesEmpty() && scheduled_tasks_.empty()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Prevent busy-waiting
        }
    }
    // Optionally LogInfo when the Worker is stopping
    Logger::Get()->LogInfo(Log_Level::Info, "Worker thread exiting.");
}

bool TaskScheduler::AllQueuesEmpty() {
    for (const auto& bin : priority_bins_) {
        if (!bin.empty()) return false;
    }
    return true;
};

//handle regulat tasks
void TaskScheduler::HandleRegularTasks() {
    for (auto& thread : thread_pool_) {
        if (thread.second->GetMsg().type == MessageType::Pool) {
            // Loop through the priority bins and assign tasks from the bins
            std::shared_ptr<BaseTask> task_ = nullptr;
            for (auto& bin : priority_bins_) {
                if (!bin.empty()) {
                    task_ = bin.front();
                    bin.pop();  // Remove the task from the bin
                    Message task_message{ MessageType::Task, std::shared_ptr<BaseTask>(task_) };  // Package the task in a Message

                    // Add the task to the thread's message queue
                    thread.second->pushMsg(task_message);  // Push the task message to the thread's queue
                    Logger::Get()->LogInfo(Log_Level::Info, "Enqueuing task to thread.");
                    return;  // Stop once a task is added to the queue
                }
            }
        }
    }
    //if we got this far add any tasks to the global queue until threads available

    std::shared_ptr<BaseTask> task_ = nullptr;
    for (auto& bin : priority_bins_) {
        if (!bin.empty()) {
            task_ = bin.front();
            bin.pop();  // Remove the task from the bin
            Message task_message{ MessageType::Task, std::shared_ptr<BaseTask>(task_)};  // Package the task in a Message

            // Add the task to the global saved message queue
            task_queue.push(task_message);  // Push the task message to the thread's queue
            Logger::Get()->LogInfo(Log_Level::Info, "Enqueuing task to thread.");
            return;  // Stop once a task is added to the queue
        }
    }
}
//handle periodic tasks
void TaskScheduler::HandlePeriodicTasks() {
    for (auto& task_info : scheduled_tasks_) {
        if (task_info.second.IsTimeToRun()) {
            if (task_info.second.task_->IsPaused()) {
                Logger::Get()->LogInfo(Log_Level::Info, "Skipping paused task: " + task_info.first);
                continue; // Skip if the task_ is paused
            }
            auto thread = get_available_thread();
            if (thread) {
                  // Push the task message to the thread's queue
                Message task_message{ MessageType::Task, std::shared_ptr<BaseTask>(task_info.second.task_) };  // Package the task in a Message

                thread->pushMsg(task_message);
                Logger::Get()->LogInfo(Log_Level::Info, "Executing periodic task.");
                task_info.second.UpdateExecutionTime();
            }
            else {
                // worst case do the task late when thread becomes available
                Message task_message{ MessageType::Task, std::shared_ptr<BaseTask>(task_info.second.task_) };  // Package the task in a Message

                task_queue.push(task_message);
            }
        }
    }
}

std::shared_ptr<T_Thread> TaskScheduler::get_available_thread() {
    for (auto& thread : thread_pool_) {
        if (thread.second->GetMsg().type == MessageType::Pool) {
            return thread.second;
        }
    }
    return nullptr;  // No available thread
}




