#include "MessageQueue.h"

MessageQueue::MessageQueue(const MessageQueue& other) {
    std::lock_guard<std::mutex> lock_this(queueMtx);  // Lock this queue's mutex

    // Copy the contents of the queue
    queue = other.queue;
}

MessageQueue& MessageQueue::operator=(const MessageQueue& other) {
    if (this != &other) {  // Self-assignment check
        std::lock_guard<std::mutex> lock_this(queueMtx);  // Lock this queue's mutex

        // Copy the contents of the queue
        queue = other.queue;
    }
    return *this;
}

bool MessageQueue::operator==(const MessageQueue& other) {
    std::lock_guard<std::mutex> lock_this(queueMtx);  // Lock this queue's mutex

    // Compare the contents of both queues by size and element-by-element
    if (queue.size() != other.queue.size()) {
        return false;
    }

    // Compare element by element
    auto this_copy = queue;
    auto other_copy = other.queue;

    while (!this_copy.empty() && !other_copy.empty()) {
        if (this_copy.front().type != other_copy.front().type) {
            return false;
        }

        this_copy.pop();
        other_copy.pop();
    }

    return true;
}

bool MessageQueue::operator!=(const MessageQueue& other) {
    return !(*this ==(other));
}


void MessageQueue::push(const Message& msg) {
    std::lock_guard<std::mutex> lock(queueMtx);
    queue.push(msg);
}
std::optional<Message> MessageQueue::pop() {
    std::unique_lock<std::mutex> lock(queueMtx);
    cv.wait(lock, [this] { return !queue.empty(); });

    if (!queue.empty()) {
        Message msg = queue.front();
        queue.pop();
        return msg;
    }

    return std::nullopt;  // Return an empty optional if the queue is empty
}
std::optional<Message> MessageQueue::top() {
    std::unique_lock<std::mutex> lock(queueMtx);
    cv.wait(lock, [this] { return !queue.empty(); });

    if (!queue.empty()) {
        return queue.front();
    }

    return std::nullopt;  // Return an empty optional if the queue is empty
}
bool MessageQueue::empty() const {
    return queue.empty();
}