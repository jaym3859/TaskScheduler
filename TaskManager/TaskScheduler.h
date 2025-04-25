#pragma once
#include <any>
#include <future>
#include <queue>
#include <iostream>
#include <thread>
#include <condition_variable>
#include <vector>
#include <mutex>
#include <chrono>
#include "../Utilities/Logger.h"
#include "T_Thread.h"
#include "Tasks.h"
#include "../Utilities/GameTimer.h"

class TaskScheduler : public TaskQueue {
public:
                                          
    //a periodic task_ 
    struct Periodic_Task {
        std::shared_ptr<BaseTask> task_;
        float nextExecutionTime;  // When to run the task_ next (in seconds)
        float interval;             // Interval in seconds
        std::shared_ptr<GameTimer> gameTimer_;  // Pointer to the game timer for accurate timing

        // Default constructor
        Periodic_Task()
            : task_(nullptr), nextExecutionTime(0.0f), interval(0.0f), gameTimer_(nullptr) {
        }

        // Parameterized constructor for initializing the task_ with interval and game timer
        Periodic_Task(std::shared_ptr<BaseTask> task_, float interval_, std::shared_ptr<GameTimer> timer)
            : task_(task_), interval(interval_), gameTimer_(timer) {
            nextExecutionTime = gameTimer_->TotalTime();  // Initialize to the current game time
        }

        // Check if it's time to run the task_ based on TotalTime and interval
        bool IsTimeToRun() const {
            float current_time = gameTimer_->TotalTime();
            // If the current time minus the next execution time exceeds the interval, it's time to run
            return (current_time - nextExecutionTime) >= interval;
        }

        // Update the next execution time
        void UpdateExecutionTime() {
            nextExecutionTime += interval;  // Add the interval to the next execution time
        }
    };
    // Constructor 
    TaskScheduler();
    //destructor 
    ~TaskScheduler();
    //add a task_
    void AddTask(std::shared_ptr<BaseTask> task_);
    // Add a periodic task_ that executes at fixed intervals
    void ScheduleTask(std::shared_ptr<BaseTask> task_, float interval);
    //stop all threads
    void StopAll();
    //stop a task_
    void StopTask(const std::string& id);

    //pause a task_
    void PauseTask(std::string id);
    //resume a task_
    void ResumeTask(std::string id);
    //return a pointer to the system scheduler clock to use for timings
    std::shared_ptr<GameTimer> get_clock();
        //return a shared pointer to the threadpool map
    std::shared_ptr<std::unordered_map<std::thread::id, std::shared_ptr<T_Thread>>> GetThreadMap();

    //posts a message to all threads in the pool
    void PostMessage(const Message& msg);
private:
    void Worker();
    bool AllQueuesEmpty();


    //handle regular tasks
    void HandleRegularTasks();
    //handle periodic tasks
    void HandlePeriodicTasks();
    //return a thread thats pooling available for a task_
    std::shared_ptr<T_Thread> get_available_thread();

    std::unordered_map<std::string, Periodic_Task> scheduled_tasks_; //scheduled tasks mapped
    std::unordered_map<std::thread::id, std::shared_ptr<T_Thread>> thread_pool_; //the thread pool mapped
    std::shared_ptr<GameTimer> clock_;  // Add clock to track time
    std::vector<std::queue<std::shared_ptr<BaseTask>>> priority_bins_;  // priority bins of tasks
    MessageQueue global_task_queue;
    std::mutex scheduledTasksMutex;      // Mutex for safe task_ handling
    std::mutex binsMutex;                 // Mutex for priority bins
    std::condition_variable cv;            // Condition variable for thread synchronization
    bool stopFlag = false;                // stop flag 
    std::thread workerThread;             // Worker thread
};
