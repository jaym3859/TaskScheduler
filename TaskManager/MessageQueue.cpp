#pragma once
#include "MessageQueue.h"
// Copy constructor
MessageQueue::MessageQueue(const MessageQueue& other) {
    std::lock_guard<std::mutex> lock_this(mtx_);  // Lock this queue's mutex

    // Copy the contents of the queue
    queue_ = other.queue_;
}

// Assignment operator
MessageQueue& MessageQueue::operator=(const MessageQueue& other) {
    if (this != &other) {  // Self-assignment check
        std::lock_guard<std::mutex> lock_this(mtx_);  // Lock this queue's mutex

        // Copy the contents of the queue
        queue_ = other.queue_;
    }
    return *this;
}
// Equality operator (compares the contents of the queues)
bool MessageQueue::operator==(const MessageQueue& other) {
    std::lock_guard<std::mutex> lock_this(mtx_);  // Lock this queue's mutex

    // Compare the contents of both queues by size and element-by-element
    if (queue_.size() != other.queue_.size()) {
        return false;
    }

    // Compare element by element
    auto this_copy = queue_;
    auto other_copy = other.queue_;

    while (!this_copy.empty() && !other_copy.empty()) {
        if (this_copy.front() != other_copy.front()) {
            return false;
        }
        this_copy.pop();
        other_copy.pop();
    }

    return true;
}

// Inequality operator
bool MessageQueue::operator!=(const MessageQueue& other) {
    return !(*this == other);  // Calls the equality operator
}


// MessageQueue that holds messages to be processed by the main thread
void MessageQueue::push(Message msg) {
    std::lock_guard<std::mutex> lock(mtx_);
    queue_.push(msg);
    cv_.notify_one();  // Notify one waiting thread
}

std::optional<Message> MessageQueue::pop() {
    std::unique_lock<std::mutex> lock(mtx_);
    cv_.wait(lock, [this] { return !queue_.empty(); });

    if (!queue_.empty()) {
        Message msg = queue_.front();
        queue_.pop();
        return msg;
    }

    return std::nullopt;  // Return an empty optional if the queue is empty
}

bool MessageQueue::empty() const {
    return queue_.empty();
}