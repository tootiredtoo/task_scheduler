cmake_minimum_required(VERSION 3.14)
project(TaskScheduler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set project structure
set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")
set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")

# Create library
add_library(TaskSchedulerLib
    ${SRC_DIR}/TaskScheduler.cpp
)

target_include_directories(TaskSchedulerLib PUBLIC ${INCLUDE_DIR})

# Demo executable
add_executable(TaskSchedulerDemo ${SRC_DIR}/main.cpp)
target_link_libraries(TaskSchedulerDemo PRIVATE TaskSchedulerLib)

# ========== Google Test ==========
include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/release-1.12.1.zip
)

# For Windows: prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

enable_testing()

# ========== Add Tests ==========
add_executable(TaskSchedulerTests
    tests/TaskSchedulerTests.cpp
    tests/LoggerTests.cpp
)

target_link_libraries(TaskSchedulerTests
    TaskSchedulerLib
    GTest::gtest_main
    GTest::gmock
)

include(GoogleTest)
gtest_discover_tests(TaskSchedulerTests)
