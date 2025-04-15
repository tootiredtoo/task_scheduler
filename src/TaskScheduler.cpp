#include "TaskScheduler.h"

TaskScheduler::TaskScheduler(size_t threadCount, std::shared_ptr<Logger> logger)
    : stop_(false), logger_(std::move(logger)) {
    if (logger_) logger_->info("Starting TaskScheduler with " + std::to_string(threadCount) + " threads.");
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
        taskQueue_.push(Task{priority, std::move(task)});
    }
    if (logger_) logger_->info("Task submitted with priority " + std::to_string(priority));
    cv_.notify_one();
}

void TaskScheduler::shutdown() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        stop_ = true;
    }
    if (logger_) logger_->info("Shutdown initiated.");
    cv_.notify_all();
    for (auto& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
    if (logger_) logger_->info("All worker threads have terminated.");
}

void TaskScheduler::workerThread() {
    while (true) {
        Task task;
        {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait(lock, [this] { return stop_ || !taskQueue_.empty(); });
            if (stop_ && taskQueue_.empty())
                return;
            task = taskQueue_.top();
            taskQueue_.pop();
        }
        if (logger_) logger_->info("Executing task with priority " + std::to_string(task.priority));
        try {
            task.func();
        } catch (const std::exception& e) {
            if (logger_) logger_->error(std::string("Exception in task: ") + e.what());
        } catch (...) {
            if (logger_) logger_->error("Unknown exception in task.");
        }
    }
}
