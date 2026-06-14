#include "pch.h"
#include "Engine/Utils/ThreadPool.h"

#include <atomic>
#include <numeric>
#include <vector>

// ---------------------------------------------------------
// 1. Basic Task Execution and Future Result Test
// ---------------------------------------------------------
TEST(ThreadPoolTest, ExecuteTaskAndGetResult)
{
    ThreadPool pool(2); // Create a pool with 2 worker threads

    // Task with no arguments, returning a value
    auto future1 = pool.Enqueue([]() { return 42; });

    // Task with arguments, returning a value (Testing std::bind and std::forward)
    auto future2 = pool.Enqueue([](int a, int b) { return a + b; }, 10, 20);

    // .get() blocks the calling thread until the asynchronous computation is complete
    EXPECT_EQ(future1.get(), 42);
    EXPECT_EQ(future2.get(), 30);
}

// ---------------------------------------------------------
// 2. High-Volume Concurrent Task Execution Test
//    Ensures the task queue can handle rapid, massive insertions.
// ---------------------------------------------------------
TEST(ThreadPoolTest, ExecuteMultipleTasks)
{
    ThreadPool pool(4); // 4 worker threads
    std::vector<std::future<int>> results;

    // Rapidly enqueue 100 tasks
    for (int i = 0; i < 100; ++i) {
        results.emplace_back(
            pool.Enqueue([i]() { return i * i; })
        );
    }

    int sum = 0;
    // Wait for all 100 computations to finish and aggregate the results
    for (auto& res : results) {
        sum += res.get();
    }

    // The sum of squares from 0^2 to 99^2 should be 328350
    EXPECT_EQ(sum, 328350);
}

// ---------------------------------------------------------
// 3. Thread Safety and Race Condition Prevention Test
//    Verifies concurrent data access using std::atomic.
// ---------------------------------------------------------
TEST(ThreadPoolTest, ThreadSafetyWithAtomics)
{
    ThreadPool pool(4);

    // Thread-safe atomic counter
    std::atomic<int> counter{ 0 };
    std::vector<std::future<void>> futures;

    // 1000 tasks attempting to increment the same atomic variable simultaneously
    for (int i = 0; i < 1000; ++i) {
        futures.emplace_back(
            pool.Enqueue([&counter]() {
            counter++;
        })
        );
    }

    // Wait for all tasks to complete
    for (auto& f : futures) {
        f.get();
    }

    // Since it was incremented 1000 times, the final value must be exactly 1000
    EXPECT_EQ(counter.load(), 1000);
}

// ---------------------------------------------------------
// 4. Data Parallelism (ParallelFor) Workload Distribution Test
// ---------------------------------------------------------
TEST(ThreadPoolTest, ParallelForSplitsWorkloadCorrectly)
{
    ThreadPool pool(4);

    const int numElements = 1000;
    std::vector<int> data(numElements, 1); // Initialize 1000 elements with value 1

    // Use ParallelFor to multiply each element by 2
    pool.ParallelFor(numElements, [&](int startIdx, int endIdx) {
        for (int i = startIdx; i < endIdx; ++i) {
            data[i] *= 2;
        }
    });

    // Verify the total sum: 1000 elements * 2 = 2000
    int sum = std::accumulate(data.begin(), data.end(), 0);
    EXPECT_EQ(sum, 2000);
}

// ---------------------------------------------------------
// 5. ParallelFor Edge Cases Test
//    Tests behavior when workload is extremely small or zero.
// ---------------------------------------------------------
TEST(ThreadPoolTest, ParallelForEdgeCases)
{
    ThreadPool pool(4);
    std::atomic<int> executionCount{ 0 };

    // Case A: 0 elements (Should return immediately without doing anything)
    pool.ParallelFor(0, [&](int start, int end) {
        executionCount++;
    });
    EXPECT_EQ(executionCount.load(), 0);

    // Case B: Less than 4 elements (Should execute synchronously on the calling thread)
    pool.ParallelFor(3, [&](int start, int end) {
        EXPECT_EQ(start, 0);
        EXPECT_EQ(end, 3);
        executionCount++;
    });
    // Should only execute as a single chunk
    EXPECT_EQ(executionCount.load(), 1);
}

// ---------------------------------------------------------
// 6. Thread-Local Worker Index Verification Test
// ---------------------------------------------------------
TEST(ThreadPoolTest, WorkerIndexIsAssignedCorrectly)
{
    ThreadPool pool(3);

    // The main thread (or any non-worker thread) should return the default -1
    EXPECT_EQ(ThreadPool::GetCurrentWorkerIndex(), -1);

    // Verify that a task executed inside the pool gets a valid worker index
    auto future = pool.Enqueue([]() {
        return ThreadPool::GetCurrentWorkerIndex();
    });

    int workerIndex = future.get();

    // The assigned index must be within the valid range [0, numThreads - 1]
    EXPECT_GE(workerIndex, 0);
    EXPECT_LT(workerIndex, 3);
}
