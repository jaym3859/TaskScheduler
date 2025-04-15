#pragma once
#pragma once
#include "MessageQueue.h"
#include "Tasks.h"

/// <T_Thread>
/// T_Thread is an std::thread wrapper with a single worker pool loop 
/// tthread is not an object since we want to use thread_ID rather than uuid
/// </T_Thread>
class T_Thread{
public:
    // Constructor accepts a shared pointer to the message queue
    T_Thread();
    // Non-movable
    T_Thread(const T_Thread& other) = delete;
    T_Thread& operator=(const T_Thread& other) = delete;
    // Destructor
    ~T_Thread();
    // Set the task
    void set_task(std::shared_ptr<BaseTask> task);
    //set the message
    void set_message(Message msg);
    // Stop the thread
    void stop();
    // Get the thread's status
    Message get_message();
    //get the thread id
    std::thread::id get_id() const; 

private:

    // Worker loop that listens for tasks and messages
    void worker();

    std::condition_variable pause_cv_;
    std::mutex results_mutex_;
    std::mutex pause_mutex_;
    std::mutex thread_mutex_;  // Mutex for locking
    std::condition_variable cv_;  // Condition variable to notify the worker thread
    std::shared_ptr<BaseTask> task_;  // Task assigned to this thread
    Message message_;  // Thread status
    std::thread t_thread_;  // The actual thread
    std::any result_;  //the last result 
};
