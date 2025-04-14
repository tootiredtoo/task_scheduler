#include "TaskScheduler.h"
#include <gtest/gtest.h>
#include <atomic>
#include <chrono>
#include <thread>

TEST(TaskSchedulerTest, ExecutesTasksInPriorityOrder) {
    TaskScheduler scheduler(2);

    std::vector<int> executionOrder;
    std::mutex orderMutex;

    scheduler.submit([&] {
        std::lock_guard<std::mutex> lock(orderMutex);
        executionOrder.push_back(3);
    }, 3);

    scheduler.submit([&] {
        std::lock_guard<std::mutex> lock(orderMutex);
        executionOrder.push_back(5);
    }, 5);

    scheduler.submit([&] {
        std::lock_guard<std::mutex> lock(orderMutex);
        executionOrder.push_back(1);
    }, 1);

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    scheduler.shutdown();

    ASSERT_EQ(executionOrder.size(), 3);
    EXPECT_EQ(executionOrder[0], 5);
    EXPECT_EQ(executionOrder[1], 3);
    EXPECT_EQ(executionOrder[2], 1);
}

TEST(TaskSchedulerTest, HandlesConcurrentSubmissions) {
    TaskScheduler scheduler(4);
    std::atomic<int> counter{0};

    auto submitTasks = [&]() {
        for (int i = 0; i < 100; ++i) {
            scheduler.submit([&] { counter++; }, i % 10);
        }
    };

    std::thread t1(submitTasks);
    std::thread t2(submitTasks);
    t1.join();
    t2.join();

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    scheduler.shutdown();

    EXPECT_EQ(counter, 200);
}

TEST(TaskSchedulerTest, ShutdownStopsGracefully) {
    TaskScheduler scheduler(2);
    std::atomic<int> counter{0};

    scheduler.submit([&] {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        counter++;
    }, 1);

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    scheduler.shutdown();

    EXPECT_EQ(counter, 1);  // Task should have completed before shutdown
}

TEST(TaskSchedulerTest, CanSubmitAfterShortPause) {
    TaskScheduler scheduler(2);
    std::atomic<int> value{0};

    scheduler.submit([&] { value = 42; }, 5);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    scheduler.submit([&] { value = 99; }, 1);

    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    scheduler.shutdown();

    EXPECT_EQ(value, 99);
}

TEST(TaskSchedulerTest, HandlesEmptyQueueOnShutdown) {
    TaskScheduler scheduler(2);

    scheduler.shutdown();

    SUCCEED();  // No exceptions should be thrown, no deadlocks
}

TEST(TaskSchedulerTest, TaskExceptionsDoNotCrashScheduler) {
    TaskScheduler scheduler(2);
    std::atomic<bool> safeExecuted = false;

    scheduler.submit([] {
        throw std::runtime_error("Boom");
    }, 5);

    scheduler.submit([&] {
        safeExecuted = true;
    }, 1);

    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    scheduler.shutdown();

    EXPECT_TRUE(safeExecuted); // The second task should have executed even if the first one failed
}

TEST(TaskSchedulerTest, ThreadSafetyUnderStress) {
    constexpr int numTasks = 1000;
    constexpr int numThreads = 8;
    TaskScheduler scheduler(4);
    std::atomic<int> counter{0};

    auto submitter = [&scheduler, &counter]() {
        for (int i = 0; i < numTasks; ++i) {
            scheduler.submit([&counter]() {
                counter.fetch_add(1, std::memory_order_relaxed);
            }, i % 10);
        }
    };

    std::vector<std::thread> submitThreads;
    for (int i = 0; i < numThreads; ++i) {
        submitThreads.emplace_back(submitter);
    }

    for (auto& t : submitThreads) {
        t.join();
    }

    std::this_thread::sleep_for(std::chrono::seconds(3));
    scheduler.shutdown();

    EXPECT_EQ(counter, numTasks * numThreads);
}

