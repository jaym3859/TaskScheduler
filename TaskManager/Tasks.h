#pragma once
#pragma once
#include <functional>
#include <any>
#include <future>
#include "Entity.h"
#include "Logger.h"


enum class PriorityLevel {
    SCHEDULED = 0,
    EXCLUSIVE = 1,
    VERY_HIGH = 2,
    HIGH = 3,
    MEDIUM = 4,
    NORMAL = 5,
    LOW = 6,
    BLOCKED = 7
};


/// <BaseTask>
///  BaseTask is a pure virtual base class of a Task
/// </BaseTask>
class BaseTask : public Entity {
public:
    BaseTask() = default; //constructor
    virtual BaseTask operator=(const BaseTask& other) = delete; //non movable
    BaseTask(const BaseTask& other) = delete; //non movable
    virtual ~BaseTask() = default; //default destructor 
    virtual void execute() = 0; //execute the task
    virtual void pause_task(); 
    virtual void resume_task();
    virtual void set_priority(const PriorityLevel& priority_in); //set priority level
    virtual PriorityLevel get_priority(); //return the tasks priority level
    virtual void set_completed(); //set the task as completed
    virtual bool is_completed(); //return if the task is completed or not
    virtual bool is_paused();//return if the task is paused
protected:
    PriorityLevel priority_ = PriorityLevel::NORMAL; //the priority level of the task
    bool completed_ = false; //flag for whether the taks is completed
    bool paused_ = false;
    std::mutex task_mutex_;
    std::condition_variable cv_;
};


//a task that returns data
class Task : public BaseTask {
public:
    //default constructor 
    Task(std::function<void()> task_fn);
    Task(const Task& other) = delete;
    Task& operator=(const Task& other) = delete;
    virtual ~Task() = default;
    virtual void execute() override;

private:

    static std::vector<std::any> results_; //results
    static std::mutex results_mutex_;  // Mutex for thread-safety
    std::function<void()> task_fn_; //the tasks related function
};
