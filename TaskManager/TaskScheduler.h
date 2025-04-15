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
#include "Logger.h"
#include "Clock.h"
#include "T_Thread.h"
#include "MessageQueue.h"
#include "Tasks.h"

class TaskScheduler {
public:
    //a periodic task 
    struct Periodic_Task {
        std::shared_ptr<BaseTask> task;
        std::chrono::steady_clock::time_point next_execution_time;  // When to run the task next
        std::chrono::milliseconds interval;
        Periodic_Task() : task(nullptr), next_execution_time(std::chrono::steady_clock::now()),
            interval(0) {
        };
        Periodic_Task(std::shared_ptr<BaseTask> task_) : task(task_), next_execution_time(std::chrono::steady_clock::now()),
            interval(0) {
        };
        Periodic_Task(std::shared_ptr<BaseTask> task_, std::chrono::milliseconds interval_) : task(task_), next_execution_time(std::chrono::steady_clock::now()),
            interval(interval_) {
        };
        Periodic_Task(std::shared_ptr<BaseTask> task_, std::chrono::steady_clock::time_point time_point_, std::chrono::milliseconds interval_) : task(task_), next_execution_time(time_point_), interval(interval_) {
        };
        ~Periodic_Task() = default;
        // Check if it's time to trigger the task
        bool is_time_to_run() const {
            return std::chrono::steady_clock::now() >= next_execution_time;
        }

        // Update next execution time after running the task
        void update_execution_time() {
            next_execution_time += interval;
        }
    };

    // Constructor 
    TaskScheduler();
    //destructor 
    ~TaskScheduler();
    //add a task
    void add_task(std::shared_ptr<BaseTask> task);
    // Add a periodic task that executes at fixed intervals
    void schedule_task(std::shared_ptr<BaseTask> task, size_t interval_ms);
    //stop all threads
    void stop_all();
    //stop a task
    void stop_task(const std::string& id);

    //pause a task
    void pause_task(std::string id);
    //resume a task
    void resume_task(std::string id);
    //return a pointer to the system scheduler clock to use for timings
    std::shared_ptr<Clock> get_clock();
    std::shared_ptr<std::unordered_map<std::thread::id, std::shared_ptr<T_Thread>>> get_thread_map();

    //return a pointer to the message queue
    //posts a message to all threads in the pool
    void PostMessage(const Message& msg);
private:
    void worker();
    bool all_queues_empty();


    //handle regular tasks
    void handle_regular_tasks();
    //handle periodic tasks
    void handle_periodic_tasks();
    //return a thread thats pooling available for a task
    std::shared_ptr<T_Thread> get_available_thread();
   
    std::unordered_map<std::string, Periodic_Task> scheduled_tasks_; //scheduled tasks mapped
    std::unordered_map<std::thread::id, std::shared_ptr<T_Thread>> thread_pool_; //the thread pool mapped
    std::shared_ptr<Clock> clock_;  // Add clock to track time
    std::vector<std::queue<std::shared_ptr<BaseTask>>> priority_bins_;  // priority bins of tasks
   
    std::mutex scheduled_tasks_mutex_;      // Mutex for safe task handling
    std::mutex bins_mutex_;                 // Mutex for priority bins
    std::condition_variable cv_;            // Condition variable for thread synchronization
    bool stop_flag_ = false;                // stop flag 
    std::thread worker_thread_;             // Worker thread
};
