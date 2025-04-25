#pragma once
#pragma once
#include <functional>
#include <any>
#include <future>
#include "..\Utilities\Entity.h"
#include "..\Utilities\Logger.h"


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
///  BaseTask is a partial virtual base class of a Task
/// </BaseTask>
class BaseTask : public Entity {
public:
    BaseTask() = default; //constructor
    virtual BaseTask operator=(const BaseTask& other) = delete; //non movable
    BaseTask(const BaseTask& other) = delete; //non movable
    virtual ~BaseTask() = default; //default destructor 
    virtual void Execute() = 0; //Execute the task_
    virtual void PauseTask(); 
    virtual void ResumeTask();
    virtual void SetPriority(const PriorityLevel& priority_in); //set priority level
    virtual PriorityLevel GetPriority(); //return the tasks priority level
    virtual void SetCompleted(); //set the task_ as completed
    virtual bool IsCompleted(); //return if the task_ is completed or not
    virtual bool IsPaused();//return if the task_ is paused
protected:
    PriorityLevel priority_ = PriorityLevel::NORMAL; //the priority level of the task_
    bool completed_ = false; //flag for whether the taks is completed
    bool paused_ = false;
    std::mutex task_mutex_;
    std::condition_variable cv;
};


//a task_ that returns data
class Task : public BaseTask {
public:
    //default constructor 
    Task(std::function<void()> task_fn);
    Task(const Task& other) = delete;
    Task& operator=(const Task& other) = delete;
    virtual ~Task() = default;
    virtual void Execute() override;

private:

    static std::vector<std::any> results_; //results
    static std::mutex resultsMutex;  // Mutex for thread-safety
    std::function<void()> task_fn_; //the tasks related function
};
