#pragma once
#include "Entity.h"
#include <mutex>
#include <queue>
#include <string>
#include <optional>
#include "Message.h"


// MessageQueue that holds messages to be processed by the main thread
class MessageQueue : public Entity {
public:
    MessageQueue() = default;
    MessageQueue(const MessageQueue&);
    MessageQueue& operator=(const MessageQueue&);
    bool operator==(const MessageQueue& other);
    bool operator!=(const MessageQueue& other);
    void push(Message msg);
    std::optional<Message> pop();
    bool empty() const;
private:
    std::queue<Message> queue_;
    std::condition_variable cv_;
    std::mutex mtx_;
};