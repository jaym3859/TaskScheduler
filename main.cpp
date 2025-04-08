#include <iostream>
#include <memory>
#include <functional>
#include <chrono>
#include <thread>
#include <string>
#include "TaskScheduler.hpp" // Assuming the code is in TaskScheduler.hpp

int main() {
    // Create a TaskScheduler with 2 threads
    TaskScheduler scheduler(2);

    // Define a simple print task
    auto task1 = std::make_shared<Task>([]() {
        std::this_thread::sleep_for(std::chrono::seconds(1)); // Simulate work
        std::cout << "Task 1 executed!" << std::endl;
        return std::any{};
        });

    auto task2 = std::make_shared<Task>([]() {
        std::this_thread::sleep_for(std::chrono::seconds(2)); // Simulate work
        std::cout << "Task 2 executed!" << std::endl;
        return std::any{};
        });

    // Add the tasks to the scheduler
    scheduler.add_task(task1);
    scheduler.add_task(task2);

    // Sleep to allow tasks to execute
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Stop the scheduler after execution
    scheduler.stop_all();
    return 0;
}
