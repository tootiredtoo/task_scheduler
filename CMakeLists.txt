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

# Enable unit testing (to be added)
enable_testing()
