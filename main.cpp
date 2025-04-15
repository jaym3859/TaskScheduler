#include <iostream>
#include <memory>
#include <functional>
#include <chrono>
#include <thread>
#include <string>
#include "Event.h"
#include "TaskManager.h" 
#include "Logger.h"
#include "fmt/core.h"
#include "xsimd/xsimd.hpp"
#include <any>

class int_task : public BaseTask {
public:
    int x = 5;

    // Execute the task asynchronously and return a future
    virtual void execute() override {
        try {
            task_fn_(x, data);
            std::cout << x << "+" << data << "=" << x + data << std::endl;
            data++;
            set_completed();
        }
        catch (const std::exception& e) {
            std::string tmp = e.what();
            Logger::get().log(log_level::Error, "Error executing task: " + tmp);
        }
    }
    //set priority
    virtual void set_priority(const PriorityLevel& priority_in) override { priority = priority_in; }
    //get priority
    virtual PriorityLevel get_priority() const override { return priority; }
    //set completed
    virtual void set_completed() override { completed = true; }
    //check if is_completed
    virtual bool is_completed() const override { return completed; }
    // return the tasks data
     int get_data() const { 
         return data; 
     }
    // set the tasks data
     void set_data(int data_in) {
         data = data_in;
     }

     void set_task(std::function<void(int, int&)> func) {
         task_fn_ = func;
     }

private:

    std::function<void(int, int&)> task_fn_;  // The task's related function
    int data; //the data the task has for potential inputs 
    bool completed;
    PriorityLevel priority;
};

int add(int x, int y) {
    return x + y;
}

//tests the task scheduler and Events in console mode
int main() {
    Logger logger;

    // Create an int_task object
    std::shared_ptr<int_task> myTask = std::make_shared<int_task>();
    int data = 5;
    // Set a task function to print the data
    myTask->set_task([](int x, int& data) {
        std::cout << "Executing task: x = " << x << ", data = " << data << std::endl;
        });
    myTask->set_task(add);
    // Set the task's data
    myTask->set_data(10);

    // Add the task to the scheduler
    TaskManager::GetScheduler()->add_periodic_task(myTask, 250);

    // Sleep to simulate running tasks for some time
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    // Stop the scheduler after the test duration
    TaskManager::GetScheduler()->stop_all();

    return 0;
}

/*
// Define a simple task that prints Hello, World!


std::shared_ptr<BaseTask> create_hello_world_task() {
    auto task_fn = []() {
        fmt::print("Hello, World!\n");
        };
    return std::make_shared<Task>(task_fn);
};
std::shared_ptr<Task> data_task() {
    auto task_fn = []() {
        fmt::print("data: {}\n", timer.to_string());
        };
    return std::make_shared<Task>(task_fn);
};
//print tiee elapsed
void print_callback() {
    fmt::print("callback\n");
};


std::shared_ptr<Task> data_task() {
    auto task_fn = []() {
        fmt::print("data: {}\n", timer.to_string());
        };
    return std::make_shared<Task>(task_fn);
};
Clock timer;

class TestListener : public Listener {
    void on_event_triggered() override {
        fmt::print("event triggered\n");
    }
};

class TestEvent : public Event {
    std::string get_event_type() const override {
        return "test_event";
    }
};


*/
