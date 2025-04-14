#include "TaskScheduler.h"

TaskScheduler::TaskScheduler(size_t threadCount)
    : stop_(false)
{
    for (size_t i = 0; i < threadCount; ++i) {
        workers_.emplace_back(&TaskScheduler::workerThread, this);
    }
}

TaskScheduler::~TaskScheduler() {
    shutdown();
}

void TaskScheduler::submit(std::function<void()> task, int priority) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        taskQueue_.push({priority, std::move(task)});
    }
    cv_.notify_one();
}

void TaskScheduler::shutdown() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        stop_ = true;
    }
    cv_.notify_all();

    for (auto& thread : workers_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

void TaskScheduler::workerThread() {
    while (true) {
        Task task;

        {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait(lock, [&] { return stop_ || !taskQueue_.empty(); });

            if (stop_ && taskQueue_.empty()) return;

            task = taskQueue_.top();
            taskQueue_.pop();
        }

        try {
            task.func();
        } catch (...) {
            std::cout  << "Task execution failed" << std::endl;
        }
    }
}
