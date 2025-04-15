#include "Tasks.h"

void BaseTask::pause_task() {
    std::lock_guard<std::mutex> lock(task_mutex_);
    paused_ = true;
}
void BaseTask::resume_task() {
    std::lock_guard<std::mutex> lock(task_mutex_);
    paused_ = false;
}
void BaseTask::set_priority(const PriorityLevel& priority_in) {
    std::lock_guard<std::mutex> lock(task_mutex_);
    priority_ = priority_in;
}; //set priority level
PriorityLevel BaseTask::get_priority() {
    std::lock_guard<std::mutex> lock(task_mutex_);
    return priority_;
}; //return the tasks priority level
void BaseTask::set_completed() {
    std::lock_guard<std::mutex> lock(task_mutex_);
    completed_ = true;
}; //set the task as completed
bool BaseTask::is_completed() {
    std::lock_guard<std::mutex> lock(task_mutex_);
    return completed_;
} //return if the task is completed or not
bool BaseTask::is_paused() {
    std::lock_guard<std::mutex> lock(task_mutex_);
    return paused_;
} //return if the task is paused

Task::Task(std::function<void()> task_fn)
    : task_fn_(task_fn) {
}

void Task::execute() {
    try {
        task_fn_();
        set_completed();
    }
    catch (const std::exception& e) {
        std::string tmp = e.what();
        Logger::Get()->log(log_level::Error, "Error executing task: " + tmp);
    }
};



