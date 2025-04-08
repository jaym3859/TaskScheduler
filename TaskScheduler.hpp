#include <any>
#include <future>
#include <queue>
#include <iostream>
#include <thread>
#include <condition_variable>
#include <vector>
#include <mutex>
#include <chrono>
#include "Clock.h"

enum class PriorityLevel { SCHEDULED, EXCLUSIVE, VERY_HIGH, HIGH, MEDIUM, NORMAL, LOW, BLOCKED };
enum class Status { RUNNING, POOLING, STOPPING };

class BaseTask : public Object {
public:
    virtual ~BaseTask() = default;
    virtual std::future<std::any> execute() = 0;
    virtual void set_priority(const PriorityLevel& priority_in) = 0;
    virtual PriorityLevel get_priority() const = 0;
    virtual void set_completed() = 0;
    virtual bool is_completed() const = 0;

protected:
    PriorityLevel priority = PriorityLevel::NORMAL;
    bool completed = false;
};

class Task : public BaseTask {
public:
    Task(std::function<std::any()> task_fn) : task_fn_(task_fn) {}

    virtual std::future<std::any> execute() override {
        return std::async(std::launch::async, [this]() {
            try {
                std::any result = task_fn_();
                set_data(result);
                set_completed();
                return result;
            }
            catch (const std::exception& e) {
                std::cerr << "Error executing task: " << e.what() << std::endl;
                return std::any{};
            }
            });
    }

    virtual void set_priority(const PriorityLevel& priority_in) override { priority = priority_in; }
    virtual PriorityLevel get_priority() const override { return priority; }
    virtual void set_completed() override { completed = true; }
    virtual bool is_completed() const override { return completed; }
    virtual std::any get_data() { return data; }
    virtual void set_data(std::any data_in) { data = data_in; }
private:
    std::function<std::any()> task_fn_;
    std::any data;
};

class T_Thread : public Object {
public:
    T_Thread() : status_(Status::POOLING), t_thread(&T_Thread::worker, this) {}

    ~T_Thread() {
        stop();
        if (t_thread.joinable()) {
            t_thread.join();
        }
    }

    void set_task(std::shared_ptr<BaseTask> task) {
        std::lock_guard<std::mutex> lock(thread_mutex);
        task_ = task;
        cv.notify_one();
    }

    void stop() {
        std::lock_guard<std::mutex> lock(thread_mutex);
        status_ = Status::STOPPING;
        cv.notify_one();
    }

    Status get_status() {
        std::lock_guard<std::mutex> lock(thread_mutex);
        return status_;
    }

private:
    void worker() {
        while (status_ != Status::STOPPING) {
            std::shared_ptr<BaseTask> current_task;
            {
                std::unique_lock<std::mutex> lock(thread_mutex);
                cv.wait(lock, [this]() { return task_ != nullptr || status_ == Status::STOPPING; });

                if (status_ == Status::STOPPING) break;

                current_task = task_;
                task_ = nullptr;
            }

            if (current_task) {
                auto result = current_task->execute();
                result.wait();
                std::cout << "Task completed with result of type: " << result.get().type().name() << std::endl;
                current_task->set_completed();
            }
        }
    }

    std::mutex thread_mutex;
    std::condition_variable cv;
    std::shared_ptr<BaseTask> task_;
    Status status_;
    std::thread t_thread;
};

class TaskScheduler : public Object {
public:
    TaskScheduler(size_t num_threads = std::thread::hardware_concurrency()) {
        // Resize the priority bins to accommodate the number of priority levels
        priority_bins_.resize(static_cast<size_t>(PriorityLevel::BLOCKED) + 1);  // Ensure correct number of bins

        // Create threads at the start
        for (size_t i = 0; i < num_threads; ++i) {
            task_threads_.emplace_back(std::make_shared<T_Thread>());
        }

        // Start worker loop to dispatch tasks
        worker_thread = std::thread(&TaskScheduler::worker, this);
    }

    ~TaskScheduler() {
        stop_all();
        if (worker_thread.joinable()) {
            worker_thread.join();  // Ensure worker thread finishes
        }
    }

    void add_task(std::shared_ptr<BaseTask> task) {
        size_t bin_index = static_cast<size_t>(task->get_priority());

        if (bin_index < priority_bins_.size()) {
            std::lock_guard<std::mutex> lock(bins_mutex);  // Lock only when adding tasks to the queue
            priority_bins_[bin_index].push(task);  // Add task to the bin
            std::cout << "Task added to bin: " << static_cast<int>(task->get_priority()) << std::endl; // Debugging
            cv.notify_one();  // Notify the worker thread to check for new tasks
        }
        else {
            std::cerr << "Invalid priority level!" << std::endl;
        }
    }
    // Add a periodic task that executes at fixed intervals
    void add_periodic_task(std::shared_ptr<BaseTask> task, size_t interval_ms) {
        // Add periodic task with an initial last_execution_time
        periodic_tasks_.emplace_back(TaskInfo{ task, std::chrono::steady_clock::now(), std::chrono::milliseconds(interval_ms) });
    }

    //adds task after duration_ms
    void add_delayed_task(std::shared_ptr<BaseTask> task, size_t duration_ms) {
        std::this_thread::sleep_for(std::chrono::milliseconds(duration_ms));  // Sleep for a short period before checking again
        size_t bin_index = static_cast<size_t>(task->get_priority());

        if (bin_index < priority_bins_.size()) {
            std::lock_guard<std::mutex> lock(bins_mutex);  // Lock only when adding tasks to the queue
            priority_bins_[bin_index].push(task);  // Add task to the bin
            std::cout << "Task added to bin: " << static_cast<int>(task->get_priority()) << std::endl; // Debugging
            cv.notify_one();  // Notify the worker thread to check for new tasks
        }
        else {
            std::cerr << "Invalid priority level!" << std::endl;
        }
    }
    
    void stop_all() {
        stop_flag = true;
        cv.notify_all();  // Notify all threads to stop
        for (auto& thread : task_threads_) {
            thread->stop();
        }
    }



private:

void worker() {
    while (true) {
        std::shared_ptr<BaseTask> current_task = nullptr;

        // Wait until there is a task to assign or we need to stop
        {
            std::unique_lock<std::mutex> lock(bins_mutex);
            cv.wait(lock, [this]() { return stop_flag || !all_queues_empty(); });

            // If we need to stop and all bins are empty, exit the loop
            if (stop_flag && all_queues_empty()) {
                break;
            }
            assign_task_to_thread();
        }
    }
}

bool all_queues_empty() {
        for (const auto& bin : priority_bins_) {
            if (!bin.empty()) return false;
        }
        return true;
    }

 void assign_task_to_thread() {
    // Try to assign a task to an idle thread
        for (auto& thread : task_threads_) {
            // Check if the thread is in POOLING state (idle and ready for tasks)
            if (thread->get_status() == Status::POOLING) {
                std::shared_ptr<BaseTask> task = nullptr;

                // Check for regular tasks in the priority bins
                for (auto& bin : priority_bins_) {
                    if (!bin.empty()) {
                        task = bin.front();
                        bin.pop();  // Remove the task from the bin
                        break;  // Exit once a task has been found
                    }
                }

                // Check periodic tasks and assign if the time interval has elapsed
                auto now = std::chrono::steady_clock::now();
                for (auto& task_info : periodic_tasks_) {
                    // Calculate elapsed time in milliseconds since the last execution
                    auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(now - task_info.last_execution_time);

                    // If the task is due for execution, assign it to the thread
                    if (elapsed_time >= task_info.interval) {
                        task = task_info.task;  // Assign the periodic task
                        task_info.last_execution_time = now;  // Update the last execution time
                        std::cout << "Assigning periodic task to thread." << std::endl;
                        break;  // Exit after assigning one periodic task
                    }
                }

                // If a task was found (either regular or periodic), assign it to the thread
                if (task) {
                    std::cout << "Assigning task to thread." << std::endl;
                    thread->set_task(task);  // Assign the task to the thread
                    return;  // Exit after assigning one task
                }
            }
        }

        // If no threads are available (all are busy), create a new thread
        if (std::none_of(task_threads_.begin(), task_threads_.end(), [](const std::shared_ptr<T_Thread>& t) {
            return t->get_status() == Status::POOLING;  // Look for a thread that is pooling (idle)
            })) {
            std::cout << "All threads are busy, creating a new thread." << std::endl;
            auto new_thread = std::make_shared<T_Thread>();
            task_threads_.emplace_back(new_thread);
        }
    }

private:
    struct TaskInfo {
        std::shared_ptr<BaseTask> task;
        std::chrono::steady_clock::time_point last_execution_time;
        std::chrono::milliseconds interval;
    };

    std::vector<std::queue<std::shared_ptr<BaseTask>>> priority_bins_;  // For regular tasks
    std::vector<std::shared_ptr<T_Thread>> task_threads_;
    std::vector<TaskInfo> periodic_tasks_;  // Holds info for periodic tasks
    std::mutex scheduled_tasks_mutex;      // Mutex for safe task handling
    std::mutex bins_mutex;                 // Mutex for priority bins
    std::condition_variable cv;            // Condition variable for thread synchronization
    bool stop_flag = false;
    std::thread worker_thread;             // Worker thread
};
