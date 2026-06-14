// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Utils/NonCopyable.h"

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

/**
 * @class ThreadPool
 * @brief A foundational class that manages worker threads for asynchronous task execution and data parallelism (ParallelFor).
 *
 * To avoid the heavy overhead of creating and destroying threads on the fly, this pool
 * pre-allocates a specified number of worker threads upon construction and puts them into a
 * wait state. It serves as the core of the engine's multi-threading architecture (e.g., ECS System Updates),
 * efficiently distributing work via a thread-safe task queue.
 *
 * Additionally, each thread is assigned a unique, thread-local index (`s_workerIndex`).
 * This is highly effective for lock-free data structures and thread-local memory allocations.
 */
class ThreadPool final : NonCopyable
{
public:
    static inline thread_local int s_workerIndex = -1;

    /**
     * @brief Constructs the thread pool and spins up the specified number of worker threads.
     */
    explicit ThreadPool(size_t threads)
        : stop(false)
    {
        for (size_t i = 0; i < threads; ++i) {
            workers.emplace_back([this, i] {
                s_workerIndex = static_cast<int>(i);

                for (;;) {
                    std::function<void()> task;
                    {
                        // Lock the queue to safely retrieve a task
                        std::unique_lock<std::mutex> lock(this->queue_mutex);

                        // Wait (sleep) until a task is enqueued or a stop signal is received
                        this->condition.wait(lock, [this] {
                            return this->stop || !this->tasks.empty();
                        });

                        if (this->stop && this->tasks.empty()) {
                            return;
                        }

                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }
                    // Execute the task completely in parallel (outside the lock)
                    task();
                }
            });
        }
    }

    /**
     * @brief Destructor. Issues a stop signal to all threads and joins them.
     */
    ~ThreadPool()
    {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread& worker : workers) {
            worker.join();
        }
    }

public:
    /**
     * @brief Enqueues a task and returns a std::future to retrieve the result.
     */
    template<class F, class... Args>
    [[nodiscard]] auto Enqueue(F&& f, Args&&... args)
        -> std::future<typename std::invoke_result<F, Args...>::type>
    {
        using return_type = typename std::invoke_result<F, Args...>::type;

        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            if (stop) throw std::runtime_error("enqueue on stopped ThreadPool");

            tasks.emplace([task]() { (*task)(); });
        }
        // Wake up one waiting thread
        condition.notify_one();
        return res;
    }

    /**
     * @brief Data parallelism utility that splits a workload evenly across available worker threads.
     */
    template<typename Func>
    void ParallelFor(int totalElements, Func&& task)
    {
        if (totalElements <= 0) {
            return;
        }
        // If the workload is too small, execute synchronously to avoid threading overhead
        if (totalElements < 4) {
            task(0, totalElements);
            return;
        }

        int numThreads = GetWorkerCount();
        if (numThreads <= 0) {
            numThreads = 4;
        }
        if (totalElements < numThreads) {
            numThreads = totalElements;
        }

        int chunkSize = totalElements / numThreads;
        int remainder = totalElements % numThreads;

        std::vector<std::future<void>> futures;
        futures.reserve(numThreads);

        int currentStart = 0;

        for (int i = 0; i < numThreads; ++i) {
            int currentEnd = currentStart + chunkSize + (i < remainder ? 1 : 0);

            futures.emplace_back(
                Enqueue([task, currentStart, currentEnd]() {
                    task(currentStart, currentEnd);
                })
            );
            currentStart = currentEnd;
        }

        // Block the calling thread until all chunks are completed
        for (auto& future : futures) {
            future.wait();
        }
    }

    [[nodiscard]] int GetWorkerCount() const
    {
        return static_cast<int>(workers.size());
    }

    [[nodiscard]] static int GetCurrentWorkerIndex()
    {
        return s_workerIndex;
    }

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop = false;
};
