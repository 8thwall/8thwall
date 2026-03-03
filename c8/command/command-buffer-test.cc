// Copyright (c) 2025 Niantic, Inc.
// Original Author: Erik Murphy-Chutorian (mc@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":command-buffer",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x876cb9a5);

#include <atomic>
#include <thread>

#include "c8/command/command-buffer.h"
#include "c8/command/command.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {

// Function to add an int and a float.
int funcA(int a, float b) {
  // Do something
  return a + static_cast<int>(b);
}

// Another function to add a double and a uint32_t.
int funcB(double a, uint32_t b) { return static_cast<int>(a) + static_cast<int>(b); }

// Third function with same type signature as funcB.
int funcC(float a, int b) { return static_cast<int>(a) + b; }

// Adds up four numbers in an array.
int addFour(const int *arr) { return arr[0] + arr[1] + arr[2] + arr[3]; }

}  // namespace

namespace c8 {

using cmd::TransferWrap;

class CommandBufferTest : public ::testing::Test {};

TEST_F(CommandBufferTest, TestTryQueueCommand) {
  // Create a 256 byte command buffer
  CommandBuffer<256> buffer;

  // Create three commands.
  EXPECT_TRUE(buffer.tryQueueCommand(funcA, 1, 2.0f));
  EXPECT_TRUE(buffer.tryQueueCommand(funcB, 3.0, 4));
  EXPECT_TRUE(buffer.tryQueueCommand(funcC, 5.0f, 6));

  // Run the commands.
  EXPECT_TRUE(buffer.runNextCommand());
  EXPECT_TRUE(buffer.runNextCommand());
  EXPECT_TRUE(buffer.runNextCommand());
}

TEST_F(CommandBufferTest, TestTryQueueCommandWithWrapping) {
  // Create a small command buffer, which will wrap around frequently.
  CommandBuffer<73> buffer;

  for (int i = 0; i < 100; ++i) {
    if (i % 2 == 0) {
      // Add two commands.
      EXPECT_TRUE(buffer.tryQueueCommand(funcA, i, i + 1));
      EXPECT_TRUE(buffer.tryQueueCommand(funcB, i, i + 1));
      // Run two commands.
      EXPECT_TRUE(buffer.runNextCommand());
      EXPECT_TRUE(buffer.runNextCommand());
    } else {
      // Add one command.
      EXPECT_TRUE(buffer.tryQueueCommand(funcB, i, i + 1));
      // Run the command.
      EXPECT_TRUE(buffer.runNextCommand());
    }
  }
}

TEST_F(CommandBufferTest, TestFullBuffer) {
  constexpr int BUF_SIZE = 1024;
  int count = 0;

  CommandBuffer<BUF_SIZE> buffer;

  while (buffer.tryQueueCommand(funcA, count, count + 1)) {
    count++;
  }

  constexpr int sizePerCommand = sizeof(&funcA) + sizeof(Command<decltype(&funcA)>);

  EXPECT_EQ(count, BUF_SIZE / sizePerCommand);

  while (buffer.runNextCommand()) {
    count--;
  }

  EXPECT_EQ(count, 0);
}

TEST_F(CommandBufferTest, TestQueueCommand) {
  CommandBuffer<256> buffer;

  std::atomic<bool> done = false;

  std::thread runner([&buffer, &done]() {
    while (!done) {
      buffer.runNextCommand();
    }
  });

  for (int i = 0; i < 100; ++i) {
    buffer.queueCommand(funcA, i, i + 1);
  }

  done = true;
  runner.join();
}

TEST_F(CommandBufferTest, TestSyncPoint) {
  CommandBuffer<256> buffer;

  std::atomic<bool> done = false;

  std::thread runner([&buffer, &done]() {
    while (!done) {
      buffer.runNextCommand();
    }
  });

  static int count = 0;

  for (int i = 0; i < 100; ++i) {
    buffer.queueCommand(+[]() { count++; });
  }

  EXPECT_EQ(100, buffer.runSyncCommand(+[]() { return count; }));

  for (int i = 0; i < 50; ++i) {
    buffer.queueCommand(+[]() { count++; });
  }

  EXPECT_EQ(150, buffer.runSyncCommand(+[]() { return count; }));

  done = true;
  runner.join();
}

// Test the command buffer using the built-in consumer thread.
TEST_F(CommandBufferTest, TestQueueCommandThread) {
  static int count = 0;
  {
    CommandBuffer<256> buffer;
    buffer.startThread();

    for (int i = 0; i < 100; ++i) {
      buffer.queueCommand(+[]() { count++; });
    }
  }

  // Ensure thread joins when the CommandBuffer goes out of scope, and all commands are run.
  EXPECT_EQ(100, count);
}

// Test the command buffer sync point using the built-in consumer thread.
TEST_F(CommandBufferTest, TestSyncPointThread) {
  CommandBuffer<256> buffer;
  buffer.startThread();

  static int count = 0;

  for (int i = 0; i < 100; ++i) {
    buffer.queueCommand(+[]() { count++; });
  }

  EXPECT_EQ(100, buffer.runSyncCommand(+[]() { return count; }));

  for (int i = 0; i < 50; ++i) {
    buffer.queueCommand(+[]() { count++; });
  }

  EXPECT_EQ(150, buffer.runSyncCommand(+[]() { return count; }));
}

// Test the command buffer with a TransferWrap.
TEST_F(CommandBufferTest, TestTransferWrap) {
  auto cb = CommandBuffer<64>::Create(/* transferBufferByteSize */ 128);
  cb.startThread();

  std::array<int, 4> arr{1, 2, 3, 4};

  for (int i = 0; i < 300; ++i) {
    cb.queueCommand(addFour, TransferWrap{arr.data(), arr.size()});
  }
}

// Test the command buffer with multiple writers.
TEST_F(CommandBufferTest, TestMultiWriter) {
  // Create a 256 byte command buffer that supports multiple writers.
  CommandBuffer<256, CommandBufferType::MPSC> buffer;

  constexpr int numWriters = 20;

  std::atomic<int> done = 0;

  std::thread runner([&buffer, &done]() {
    while (done.load() < numWriters) {
      buffer.runNextCommand();
    }
  });

  static int count = 0;

  // Spawn numWriters threads each which will run queue and runSync commands.
  std::vector<std::thread> threads;
  for (int i = 0; i < numWriters; ++i) {
    threads.emplace_back([&buffer, &done]() {
      int localCount = 0;
      for (int i = 0; i < 100; ++i) {
        buffer.queueCommand(
          +[](int *local) {
            (*local)++;
            count++;
          },
          &localCount);
      }

      EXPECT_EQ(100, buffer.runSyncCommand(+[](int *local) -> int { return *local; }, &localCount));

      for (int i = 0; i < 50; ++i) {
        buffer.queueCommand(
          +[](int *local) {
            (*local)++;
            count++;
          },
          &localCount);
      }

      EXPECT_EQ(150, buffer.runSyncCommand(+[](int *local) -> int { return *local; }, &localCount));

      done.fetch_add(1);
    });
  }

  runner.join();

  EXPECT_EQ(numWriters * 150, count);

  // Join all of the writer threads.
  for (auto &t : threads) {
    t.join();
  }
}

TEST_F(CommandBufferTest, TestSyncPointDifferentTypes) {
  CommandBuffer<256> buffer;
  buffer.startThread();

  buffer.queueCommand(+[]() -> void {});

  buffer.runSyncCommand(+[]() -> void {});

  int result = buffer.runSyncCommand(+[]() -> int { return 42; });

  EXPECT_EQ(result, 42);
}

TEST_F(CommandBufferTest, TestTryQueueCommandExceedingCapacity) {
  CommandBuffer<64> buffer;

  // Fill the buffer.
  bool success = true;
  int count = 0;
  while (success) {
    success = buffer.tryQueueCommand(funcA, 123456, 654.321f);
    count++;
  }
  // Make sure we actually were able to queue a few commands.
  EXPECT_EQ(count, 3);

  // Run all commands.
  while (buffer.runNextCommand()) {
  }

  // Should be able to queue again now.
  EXPECT_TRUE(buffer.tryQueueCommand(funcA, 1, 2.0f));
}

TEST_F(CommandBufferTest, TestGracefulShutdown) {
  {
    CommandBuffer<128> buffer;
    buffer.startThread();

    buffer.queueCommand(+[]() { std::this_thread::sleep_for(std::chrono::milliseconds(2)); });

    buffer.queueCommand(+[]() { std::this_thread::sleep_for(std::chrono::milliseconds(2)); });

    // Let it run, and ensure destructor doesn't deadlock or crash.
  }
}

TEST_F(CommandBufferTest, TestRunNextCommandEmpty) {
  CommandBuffer<256> buffer;
  EXPECT_FALSE(buffer.runNextCommand());
}

TEST_F(CommandBufferTest, TestInterleavedTransferWrapCommands) {
  auto cb = CommandBuffer<128>::Create(128);
  cb.startThread();

  std::array<int, 4> arr{10, 20, 30, 40};

  cb.queueCommand(funcA, 5, 3.0f);
  cb.queueCommand(addFour, TransferWrap{arr.data(), arr.size()});
  cb.queueCommand(funcB, 7.5, 2);

  cb.runSyncCommand(+[] {});  // Wait until all commands run
}

TEST_F(CommandBufferTest, TestRunSyncCommandWithReturnValuesSPSC) {
  CommandBuffer<256> buffer;
  std::atomic<bool> done = false;
  std::atomic<int> sharedCount = 0;

  std::thread runner([&]() {
    while (!done.load()) {
      buffer.runNextCommand();
    }
  });

  for (int i = 0; i < 10; ++i) {
    buffer.queueCommand(+[](std::atomic<int> *count) { count->fetch_add(10); }, &sharedCount);

    int result = buffer.runSyncCommand(
      +[](std::atomic<int> *count) -> int { return count->load(); }, &sharedCount);

    EXPECT_EQ(result, (i + 1) * 10);
  }

  done = true;
  runner.join();
}

TEST_F(CommandBufferTest, TestRunSyncCommandWithReturnValuesMPSC) {
  CommandBuffer<256, CommandBufferType::MPSC> buffer;
  std::atomic<bool> done = false;
  std::atomic<int> globalSum = 0;

  std::thread runner([&]() {
    while (!done.load()) {
      buffer.runNextCommand();
    }
  });

  constexpr int numThreads = 4;
  constexpr int opsPerThread = 25;
  std::vector<std::thread> threads;

  for (int t = 0; t < numThreads; ++t) {
    threads.emplace_back([&buffer, &globalSum]() {
      for (int i = 0; i < opsPerThread; ++i) {
        buffer.queueCommand(
          +[](std::atomic<int> *sum, int value) { sum->fetch_add(value); }, &globalSum, 1);

        int intermediate = buffer.runSyncCommand(
          +[](std::atomic<int> *sum) -> int { return sum->load(); }, &globalSum);

        EXPECT_GE(intermediate, 1);
        EXPECT_LE(intermediate, numThreads * opsPerThread);
      }
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }

  // After all writers finish, verify final sum
  int finalSum =
    buffer.runSyncCommand(+[](std::atomic<int> *sum) -> int { return sum->load(); }, &globalSum);

  EXPECT_EQ(finalSum, numThreads * opsPerThread);

  done = true;
  runner.join();
}

TEST_F(CommandBufferTest, TestFixedArrayWrap) {
  CommandBuffer<256> buffer;
  buffer.startThread();

  std::array<int, 4> arr = {10, 20, 30, 40};
  cmd::FixedArrayWrap<int, 4> wrappedArr(arr);
  buffer.queueCommand(addFour, wrappedArr);
  EXPECT_TRUE(buffer.runNextCommand());
  EXPECT_FALSE(buffer.runNextCommand());

  EXPECT_EQ(buffer.runSyncCommand(+[]() { return 99; }), 99);
  EXPECT_FALSE(buffer.runNextCommand());
}

static std::atomic<int> sharedCount = 0;
int readSharedCount() { return sharedCount.load(); }
TEST_F(CommandBufferTest, TestSyncPointMultiThreadedWriters) {
  CommandBuffer<512, CommandBufferType::MPSC> buffer;
  buffer.startThread();

  auto increment = +[](std::atomic<int> *cnt) { (*cnt)++; };

  std::vector<std::thread> writers;
  constexpr int numWriters = 10;
  for (int i = 0; i < numWriters; ++i) {
    writers.emplace_back([&buffer, increment]() {
      for (int j = 0; j < 50; ++j) {
        buffer.queueCommand(increment, &sharedCount);
      }
    });
  }

  for (auto &t : writers) {
    t.join();
  }

  EXPECT_EQ(buffer.runSyncCommand(readSharedCount), numWriters * 50);
}

}  // namespace c8
