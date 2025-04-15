#include "TaskScheduler.h"
#include "ConsoleLogger.h"

#include <iostream>
#include <chrono>
#include <thread>

#define MAX_THREADS 3

void simulateWork(const std::string& label, int durationMs) {
    std::cout << "[Start] " << label << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(durationMs));
    std::cout << "[Done ] " << label << std::endl;
}

int main() {
    auto logger = std::make_shared<ConsoleLogger>();
    TaskScheduler scheduler(MAX_THREADS, logger);

    scheduler.submit([] { simulateWork("Task A (priority 1)", 300); }, 1);
    scheduler.submit([] { simulateWork("Task B (priority 5)", 100); }, 5);
    scheduler.submit([] { simulateWork("Task C (priority 3)", 200); }, 3);
    scheduler.submit([] { simulateWork("Task D (priority 10)", 50); }, 10);
    scheduler.submit([] { simulateWork("Task E (priority 7)", 150); }, 7);

    std::this_thread::sleep_for(std::chrono::seconds(2));
    scheduler.shutdown();

    std::cout << "All tasks done. Scheduler shutdown.\n";
    return 0;
}
