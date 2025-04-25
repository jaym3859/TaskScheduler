#include "Tasks.h"

void BaseTask::PauseTask() {
    std::lock_guard<std::mutex> lock(task_mutex_);
    paused_ = true;
}
void BaseTask::ResumeTask() {
    std::lock_guard<std::mutex> lock(task_mutex_);
    paused_ = false;
}
void BaseTask::SetPriority(const PriorityLevel& priority_in) {
    std::lock_guard<std::mutex> lock(task_mutex_);
    priority_ = priority_in;
}; //set priority level
PriorityLevel BaseTask::GetPriority() {
    std::lock_guard<std::mutex> lock(task_mutex_);
    return priority_;
}; //return the tasks priority level
void BaseTask::SetCompleted() {
    std::lock_guard<std::mutex> lock(task_mutex_);
    completed_ = true;
}; //set the task_ as completed
bool BaseTask::IsCompleted() {
    std::lock_guard<std::mutex> lock(task_mutex_);
    return completed_;
} //return if the task_ is completed or not
bool BaseTask::IsPaused() {
    std::lock_guard<std::mutex> lock(task_mutex_);
    return paused_;
} //return if the task_ is paused

Task::Task(std::function<void()> task_fn)
    : task_fn_(task_fn) {
}

void Task::Execute() {
    try {
        task_fn_();
        SetCompleted();
    }
    catch (const std::exception& e) {
        std::string tmp = e.what();
        Logger::Get()->LogInfo(Log_Level::Error, "Error executing task: " + tmp);
    }
};



