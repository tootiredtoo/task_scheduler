Implement a simple task scheduler in modern C++. It should allow tasks (functors, lambdas, etc.) to be submitted with an associated priority and execute them using a pool of worker threads.

Key Features:
- Thread-safe task queue with priority ordering
- Thread pool for concurrent task execution
- C++17+ features (e.g., std::variant, std::optional, structured bindings, if constexpr)
- Graceful shutdown of worker threads
- Unit tests using Google Test
- Statistics (e.g., tasks processed, average wait time)


Requirements

- Support for submitting tasks with a std::function<void()>
- Tasks have an associated integer priority (higher = more important)
- Internally maintain a priority queue
- Use std::thread, std::mutex, std::condition_variable, std::priority_queue, etc.
- Use RAII for thread lifecycle management

Interface:
void submit(std::function<void()> task, int priority);
void shutdown();