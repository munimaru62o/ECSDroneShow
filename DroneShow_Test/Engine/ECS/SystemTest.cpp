#include "pch.h"
#include "Engine/ECS/System.h"
#include "Engine/ECS/Coordinator.h"
#include <vector>
#include <atomic>

// ---------------------------------------------------------
// Dummy System for Testing
// (A wrapper class to expose and invoke the protected ParallelFor)
// ---------------------------------------------------------
class ParallelTestSystem : public System
{
public:
    // Executes a data-parallel task that doubles all values in an array
    void ProcessDataParallel(std::vector<int>& data)
    {
        ParallelFor(static_cast<int>(data.size()), [&data](int start, int end) {
            for (int i = start; i < end; ++i) {
                data[i] *= 2;
            }
        });
    }

    // Used for Race Condition checking
    void IncrementAtomicCounter(int totalElements, std::atomic<int>& counter)
    {
        ParallelFor(totalElements, [&counter](int start, int end) {
            for (int i = start; i < end; ++i) {
                counter++;
            }
        });
    }
};

// ---------------------------------------------------------
// 1. Failsafe Test: Execution without a ThreadPool (Standalone instantiation)
// ---------------------------------------------------------
TEST(SystemTest, ParallelForWithoutThreadPool)
{
    // Instantiate directly without a Coordinator (m_threadPool remains nullptr)
    ParallelTestSystem system;

    const int DATA_SIZE = 1000;
    std::vector<int> data(DATA_SIZE, 1);

    // Should safely process synchronously (single-threaded) without crashing!
    system.ProcessDataParallel(data);

    for (int i = 0; i < DATA_SIZE; ++i) {
        EXPECT_EQ(data[i], 2);
    }
}

// ---------------------------------------------------------
// 2. Failsafe Test: Extremely small element count (less than 4)
// ---------------------------------------------------------
TEST(SystemTest, ParallelForSmallElements)
{
    Coordinator coordinator;
    coordinator.Init();

    // Register via Coordinator to automatically inject the ThreadPool
    auto system = coordinator.RegisterSystem<ParallelTestSystem>(Signature{}, static_cast<SystemPhase>(0), 0);

    // Since the element count is 3 (< 4), it should fall back to single-threaded 
    // execution to avoid the overhead of waking up threads.
    const int DATA_SIZE = 3;
    std::vector<int> data(DATA_SIZE, 1);

    system->ProcessDataParallel(data);

    for (int i = 0; i < DATA_SIZE; ++i) {
        EXPECT_EQ(data[i], 2);
    }
}

// ---------------------------------------------------------
// 3. Normal fully multi-threaded parallel execution test
// ---------------------------------------------------------
TEST(SystemTest, ParallelForWithThreadPool)
{
    Coordinator coordinator;
    coordinator.Init(); // Internally instantiates a ThreadPool scaled to the hardware

    auto system = coordinator.RegisterSystem<ParallelTestSystem>(Signature{}, static_cast<SystemPhase>(0), 0);

    // A massive dataset, significantly larger than the number of available threads
    const int DATA_SIZE = 10000;
    std::vector<int> data(DATA_SIZE, 1);

    // Process the array concurrently
    system->ProcessDataParallel(data);

    // Verify that all chunks were processed without any omissions
    for (int i = 0; i < DATA_SIZE; ++i) {
        EXPECT_EQ(data[i], 2);
    }
}

// ---------------------------------------------------------
// 4. Thread Safety (Data Race) test during parallel execution
// ---------------------------------------------------------
TEST(SystemTest, ParallelForThreadSafety)
{
    Coordinator coordinator;
    coordinator.Init();

    auto system = coordinator.RegisterSystem<ParallelTestSystem>(Signature{}, static_cast<SystemPhase>(0), 0);

    const int TOTAL_ELEMENTS = 50000;
    std::atomic<int> counter{ 0 };

    // Distribute 50,000 increment operations across the thread pool
    system->IncrementAtomicCounter(TOTAL_ELEMENTS, counter);

    // Verify that no loops were dropped (ensures future.wait() properly blocked until all threads completed)
    EXPECT_EQ(counter.load(), TOTAL_ELEMENTS);
}
