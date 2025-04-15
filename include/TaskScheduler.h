#pragma once

#include "ConsoleLogger.h"

#include <functional>
#include <queue>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <iostream>

struct Task {
    int priority;
    std::function<void()> func;

    bool operator<(const Task& other) const {
        return priority < other.priority;
    }
};

class TaskScheduler {
    public:
        explicit TaskScheduler(size_t threadCount = std::thread::hardware_concurrency(),
                               std::shared_ptr<Logger> logger = nullptr);
        ~TaskScheduler();

        void submit(std::function<void()> task, int priority);
        void shutdown();

    private:
        void workerThread();

        std::vector<std::thread> workers_;
        std::priority_queue<Task> taskQueue_;
        std::mutex mutex_;
        std::condition_variable cv_;
        std::atomic<bool> stop_;
        std::shared_ptr<Logger> logger_;
    };
