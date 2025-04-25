#pragma once
#include <mutex>
#include <queue>
#include <string>
#include <optional>
#include "Tasks.h"

enum class MessageType {
    Pause,
    Run,
    Pool,
    Stop,
    Task
};

// Define the message structure
struct Message {
    MessageType type;  // type of message
    std::shared_ptr<BaseTask> task; // associated task
    std::string msg;  // associated message 
    void* data; //associated data
};


// MessageQueue that holds messages to be processed by the main thread
class MessageQueue {
public:
    MessageQueue() = default;
    MessageQueue(const MessageQueue& other);
    MessageQueue& operator=(const MessageQueue& other);
    bool operator==(const MessageQueue& other);
    bool operator!=(const MessageQueue& other);
    std::optional<Message> pop();
    std::optional<Message> top();
    void push(const Message& msg);
    bool empty() const;
private:
    std::queue<Message> queue;
    std::condition_variable cv;
    std::mutex queueMtx;
};

//this is a base class to inherit from to share the task queue
class TaskQueue {
public:
    static MessageQueue task_queue;
};