// task_scheduler_logger_test.cpp
#include "TaskScheduler.h"
#include "MockLogger.h"

#include <gtest/gtest.h>
#include <chrono>
#include <thread>
#include <future>

TEST(TaskSchedulerLoggerTest, LogsTaskSubmissionAndExecution) {
    auto mockLogger = std::make_shared<MockLogger>();
    TaskScheduler scheduler(1, mockLogger);

    std::promise<void> done;
    scheduler.submit([&done] {
        done.set_value();
    }, 5);

    done.get_future().wait();
    scheduler.shutdown();

    bool submitLogged = false;
    bool executeLogged = false;

    for (const auto& msg : mockLogger->infos) {
        if (msg.find("Task submitted") != std::string::npos) submitLogged = true;
        if (msg.find("Executing task") != std::string::npos) executeLogged = true;
    }

    EXPECT_TRUE(submitLogged);
    EXPECT_TRUE(executeLogged);
}
