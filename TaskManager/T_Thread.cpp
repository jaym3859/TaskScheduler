
#include "T_Thread.h"
std::vector<std::shared_ptr<T_Thread>> T_Thread::thread_pool;
MessageQueue TaskQueue::task_queue;

T_Thread::T_Thread()
    : t_thread([this]() { this->Worker(); }) {
}

// Destructor
T_Thread::~T_Thread() {
    stop();
    if (t_thread.joinable()) {
        t_thread.join();
    }
}

// Set the task_
void T_Thread::set_task(std::shared_ptr<BaseTask> task_) {
    std::lock_guard<std::mutex> lock(threadMutex);
    task_ = task_;
    queueCV.notify_one();
}

//set the message
void T_Thread::SetMessage(Message msg) {
    std::lock_guard<std::mutex> lock(threadMutex);
    message.type = msg.type;
}

// Stop the thread
void T_Thread::stop() {
    std::lock_guard<std::mutex> lock(threadMutex);
    message.type = MessageType::Stop;
    queueCV.notify_one();
}
// Get the thread's status
Message T_Thread::GetMsg() {
    std::lock_guard<std::mutex> lock(threadMutex);
    return message;
}
std::thread::id T_Thread::GetID() const {
    return t_thread.get_id();  // Assuming t_thread is the actual std::thread object
}
void T_Thread::pushMsg(const Message& messageIn) {
    std::unique_lock<std::mutex> lock(queueMutex);
    msgQ.push(messageIn);
}

bool T_Thread::is_any_thread_idle_or_paused()
{
    // Iterate through the thread pool and check the status of other threads' queues.
    // If one thread has an empty queue and isn't paused, it's considered idle.
    for (auto& thread : thread_pool) {
        if (thread.get()->msgQ.empty()) {
            return true;  // At least one thread is idle or paused, allow worker to steal
        }
    }
    return false;
}
// Worker loop that listens for tasks and messages
void T_Thread::Worker() {
    while (true) {
        std::shared_ptr<BaseTask> current_task;

        // Locking the queue for access
        {
            std::unique_lock<std::mutex> lock(queueMutex);

            // Wait until there is a task or we need to stop
            queueCV.wait(lock, [this]() {
                return !msgQ.empty() || is_any_thread_idle_or_paused();
                });

           

            if (message.type == MessageType::Pause) {
                // Exit the worker loop or pause until resumed
                continue;  // Skip to next iteration, effectively pausing
            }

            // If stop message is received, exit the loop
            if (message.type == MessageType::Stop) break;

            // Pop a task from the front of the queue
            if (!msgQ.empty()) {
                std::optional<Message> maybe = msgQ.pop();
                if (maybe) {
                    Message msg = *maybe;
                    if (msg.type == MessageType::Task) {
                        current_task = msg.task;
                        current_task->Execute();
                        current_task->SetCompleted();
                    }
                    else if (msg.type == MessageType::Stop) {
                        message.type = MessageType::Stop;
                        break;
                    }
                }
            }
            // Otherwise check the global task queue 
            else if (!task_queue.empty()) {
                std::optional<Message> maybe = task_queue.pop();
                if (maybe) {
                    Message msg = *maybe;
                    if (msg.type == MessageType::Task) {
                        current_task = msg.task;
                        current_task->Execute();
                        current_task->SetCompleted();
                    }
                    else if (msg.type == MessageType::Stop) {
                        message.type = MessageType::Stop;
                        break;
                    }
                }
            }
            else {  // Missing closing bracket for this 'else' block
                // If the queue is empty, try stealing tasks from other threads
                for (auto& thread : thread_pool) {
                    // Ensure the weak pointer is still valid
                      // Lock weak pointer
                    if (!thread->msgQ.empty()) {
                        std::optional<Message> maybe = thread->msgQ.pop();
                        if (maybe) {
                            Message msg = *maybe;
                            if (msg.type == MessageType::Task) {
                                current_task = msg.task;
                                current_task->Execute();
                                current_task->SetCompleted();
                            }
                            else if (msg.type == MessageType::Stop) {
                                message.type = MessageType::Stop;
                                break;
                            }
                            break; // Task stolen and processed, break from the stealing loop
                        }
                    }
                }
            }
            if(task_queue.empty() && msgQ.empty())
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}