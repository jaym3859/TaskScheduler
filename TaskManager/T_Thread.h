#pragma once
#include "../Utilities/Logger.h"
#include "MessageQueue.h"
#include "Tasks.h"

/// <T_Thread>
/// T_Thread is an std::thread wrapper with a single Worker pool loop 
/// tthread is not an object since we want to use thread_ID rather than uuid
/// </T_Thread>
class T_Thread : public TaskQueue{
public:
    static std::vector<std::shared_ptr<T_Thread>> thread_pool;

    // Constructor accepts a shared pointer to the message queue
    T_Thread();
    // Non-movable
    T_Thread(const T_Thread& other) = delete;
    T_Thread& operator=(const T_Thread& other) = delete;
    // Destructor
    ~T_Thread();
    // Set the task_
    void set_task(std::shared_ptr<BaseTask> task_);
    //set the message
    void SetMessage(Message msg);
    // Stop the thread
    void stop();
    //pause the thread
    void pushMsg(const Message& messageIn);
    // Get the thread's status
    Message GetMsg();
    //get the thread id
    std::thread::id GetID() const; 
private:
    bool is_any_thread_idle_or_paused();
    // Worker loop that listens for tasks and messages
    void Worker();
    MessageQueue msgQ;
    std::mutex threadMutex;  // Mutex for locking
    std::mutex queueMutex;
    std::condition_variable queueCV;  // Condition variable to Notify the Worker thread
    std::condition_variable pauseCV;  // Condition variable for pause
    std::shared_ptr<BaseTask> task_;  // Task assigned to this thread
    Message message;  // Thread status
    std::thread t_thread;  // The actual thread
    std::any result_;  //the last result 
};
