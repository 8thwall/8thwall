// Copyright (c) 2025 Niantic, Inc.
// Original Author: Erik Murphy-Chutorian (mc@nianticlabs.com)
//
// Implementation of a performance sensitive Single-Producer-Single-Consumer (SPSC) command buffer.

#pragma once

#include <any>
#include <atomic>
#include <cstddef>
#include <mutex>
#include <thread>
#include <type_traits>

#include "c8/command/circular-memcpy.h"
#include "c8/command/command.h"
#include "c8/command/transfer-buffer.h"

namespace c8 {

enum class CommandBufferType {
  // Single-Producer-Single-Consumer (SPSC) type is lock-free for writes.
  SPSC,
  // Multi-Producer-Single-Consumer (MPSC) type has client-side locks for writes.
  MPSC,
};

template <std::size_t BufferSize, CommandBufferType UseType = CommandBufferType::SPSC>
class CommandBuffer {
  using DeserializeAndRunFunc =
    std::any (*)(const char *&, const char *, const char *, TransferBuffer &);

public:
  static CommandBuffer<BufferSize> Create(std::size_t transferBufferByteSize) noexcept {
    return CommandBuffer<BufferSize>(TransferBuffer(transferBufferByteSize));
  }

  CommandBuffer(TransferBuffer &&transferBuffer = TransferBuffer(0)) noexcept
      : transferBuffer_(std::move(transferBuffer)),
        writeMarkerAtomic_(commandBuffer_),
        readMarkerAtomic_(commandBuffer_) {}

  ~CommandBuffer() noexcept {
    // Stop the consumer thread if it is running.
    stopThread();
  }

  void startThread(
    std::function<void(CommandBuffer *)> func = &CommandBuffer::startThreadFunc) noexcept {
    // Stop the consumer thread if it is running.
    stopThread();
    consumerThread_ = std::thread(func, this);
  }

  void stopThread() noexcept {
    if (!consumerThread_.joinable()) {
      // The consumer thread is not running.
      return;
    }
    // Wait for the consumer thread to finish.
    const char *writeMarker = writeMarkerAtomic_.load(std::memory_order_relaxed);
    const char *readMarker = readMarkerAtomic_.load(std::memory_order_acquire);
    while (readMarker != writeMarker) {
      readMarkerAtomic_.wait(readMarker, std::memory_order_acquire);
      readMarker = readMarkerAtomic_.load(std::memory_order_acquire);
    }

    // Set the write marker to nullptr to signal the consumer thread to stop.
    writeMarkerAtomic_.store(nullptr, std::memory_order_release);
    writeMarkerAtomic_.notify_one();
    consumerThread_.join();
  }

  // Try to add a command to the buffer, returning false if there isn't enough space. In the case of
  // a client/server model, this should be called on the client side.
  template <typename Func, typename... Args>
  bool tryQueueCommand(Func &&func, Args &&...args) noexcept {
    return nullptr
      == tryQueueCommandImpl(Command{std::forward<Func>(func), std::forward<Args>(args)...});
  }

  // Add the command to the buffer, blocking until there is enough space. Ensure that another thread
  // is draining the buffer, or this will deadlock. In the case of a client/server model, this
  // should be called on the client side.
  template <typename Func, typename... Args>
  void queueCommand(Func &&func, Args &&...args) noexcept {
    queueCommandImpl(Command{std::forward<Func>(func), std::forward<Args>(args)...});
  }

  // Add a sync point, blocking until all commands before this have been run and return the result
  // of this command. Ensure that another thread is draining the buffer, or this will deadlock.
  // In the case of a client/server model, this should be called on the client side.
  template <typename Func, typename... Args>
    requires internal::FunctionPointer<Func>
  typename Command<Func>::ResultType runSyncCommand(Func func, Args... args) noexcept {
    return runSyncCommandImpl(Command{std::forward<Func>(func), std::forward<Args>(args)...});
  }

  // Run the next command in the buffer, returning false if there are no more commands to run. In
  // the case of a client/server model, this should be called on the server side.
  bool runNextCommand() noexcept { return runNextCommandImpl(); }

  static void startThreadFunc(CommandBuffer *self) noexcept {
    char *writeMarker = self->writeMarkerAtomic_.load(std::memory_order_acquire);
    while (true) {
      if (self->runNextCommandImpl()) {
        writeMarker = self->writeMarkerAtomic_.load(std::memory_order_acquire);
      } else {
        // If there are no more commands to run, wait for the next command to be added.
        self->writeMarkerAtomic_.wait(writeMarker, std::memory_order_acquire);
        writeMarker = self->writeMarkerAtomic_.load(std::memory_order_acquire);
      }
      if (writeMarker == nullptr) {
        // Time to cleanup, the class has been destroyed.
        break;
      }
    }
  }

private:
  // Private no-op lock class for single-writer support.
  struct [[maybe_unused]] NoOpLock {
    NoOpLock(std::recursive_mutex &) {}
  };
  // Conditional type for the lock based on the CommandBufferType.
  using MultiWriteLock = std::conditional_t<
    UseType == CommandBufferType::MPSC,
    std::lock_guard<std::recursive_mutex>,
    NoOpLock>;

  // Try to add a command to the buffer, returning nullptr on success and the current location of
  // the readMarker on failure so the caller can try again when the pointer has moved.
  template <typename Command>
    requires CommandConcept<Command>
  const char *tryQueueCommandImpl(Command &&command) noexcept {
    // Lock the buffer if UseType allows for multi-writer support.
    MultiWriteLock lock(multiWriteRecursiveMutex_);

    // Calculate the space needed for the command and the static run function pointer.
    constexpr int spaceNeeded = sizeof(DeserializeAndRunFunc) + sizeof(Command);

    // Figure out how much space is left until the read pointer.
    char *writeMarker = writeMarkerAtomic_.load(std::memory_order_relaxed);
    const char *readMarker = readMarkerAtomic_.load(std::memory_order_acquire);

    // Figure out how much space is left until the read pointer, and wrap around if necessary.
    const int spaceRemaining = (readMarker == writeMarker)
      ? BufferSize - 1  // Subtract one to ensure we don't end up with readMarker == writeMarker.
      : (readMarker - writeMarker + BufferSize) % BufferSize - 1;  // Same here.

    if (spaceRemaining < spaceNeeded) {
      // There isn't enough space in the buffer.
      return readMarker;
    }

    // Store any transfer wraps in the transfer buffer.
    if (!command.storeTransferWraps(transferBuffer_)) {
      // There isn't enough space in the transfer buffer.
      return readMarker;
    }

    char *bufferEnd = commandBuffer_ + BufferSize;

    // Write the static run function pointer to the commandBuffer_.
    DeserializeAndRunFunc runFunction = &Command::deserializeRunAny;
    writeMarker = circularMemcpyStore(
      writeMarker,
      reinterpret_cast<const char *>(&runFunction),
      sizeof(DeserializeAndRunFunc),
      commandBuffer_,
      bufferEnd);

    // Write the Command to the commandBuffer_.
    writeMarker = circularMemcpyStore(
      writeMarker,
      reinterpret_cast<const char *>(&command),
      sizeof(Command),
      commandBuffer_,
      bufferEnd);

    // Move the atomic write pointer, ensuring the memcpy is synchronized with a release fence.
    writeMarkerAtomic_.store(writeMarker, std::memory_order_release);
    writeMarkerAtomic_.notify_one();
    return nullptr;
  }

  // Add the command to the buffer, blocking until there is enough space. Ensure that another thread
  // is draining the buffer, or this will deadlock.
  template <typename Command>
    requires CommandConcept<Command>
  void queueCommandImpl(Command &&command) noexcept {
    // Lock the buffer if UseType allows for multi-writer support.
    MultiWriteLock lock(multiWriteRecursiveMutex_);

    while (auto readMarker = tryQueueCommandImpl(command.clone())) {
      // If the readbuffer is full, wait until the readMarker moves and try again.
      readMarkerAtomic_.wait(readMarker, std::memory_order_acquire);
    }
  }

  // Add a sync point, blocking until all commands before this have been run and return the result
  // of this command. Ensure that another thread is draining the buffer, or this will deadlock.
  template <typename Command>
    requires CommandConcept<Command>
  typename Command::ResultType runSyncCommandImpl(Command &&command) noexcept {
    // Lock the buffer if UseType allows for multi-writer support.
    MultiWriteLock lock(multiWriteRecursiveMutex_);

    queueCommandImpl(command.clone());

    // Wait for the sync point to be reached.
    char *writeMarker = writeMarkerAtomic_.load(std::memory_order_relaxed);
    const char *readMarker = readMarkerAtomic_.load(std::memory_order_acquire);
    while (readMarker != writeMarker) {
      readMarkerAtomic_.wait(readMarker, std::memory_order_acquire);
      readMarker = readMarkerAtomic_.load(std::memory_order_acquire);
    }

    if constexpr (std::is_void_v<typename Command::ResultType>) {
      return;
    } else {
      return std::any_cast<typename Command::ResultType>(syncPointResult_);
    }
  }

  // Run the next command in the buffer, returning false if there are no more commands to run. In
  // the case of a client/server model, this should be called on the server side.
  bool runNextCommandImpl() noexcept {
    // Read the read and write pointer locations.
    const char *readMarker = readMarkerAtomic_.load(std::memory_order_relaxed);
    const char *writeMarker = writeMarkerAtomic_.load(std::memory_order_acquire);
    if (writeMarker == nullptr) {
      // The buffer is being destroyed.
      return false;
    }

    // Check if the read pointer has caught up to the write pointer.
    if (readMarker == writeMarker) {
      return false;
    }

    const char *bufferEnd = commandBuffer_ + BufferSize;

    // Read the static run function pointer from the commandBuffer_.
    DeserializeAndRunFunc runFunction;
    readMarker = circularMemcpyLoad(
      reinterpret_cast<char *>(&runFunction),
      readMarker,
      sizeof(DeserializeAndRunFunc),
      commandBuffer_,
      bufferEnd);

    // Read marker is advanced in runFunction.
    syncPointResult_ = runFunction(readMarker, commandBuffer_, bufferEnd, transferBuffer_);

    // Update the read pointer atomically.
    readMarkerAtomic_.store(readMarker, std::memory_order_release);
    readMarkerAtomic_.notify_one();
    return true;
  }

private:
  TransferBuffer transferBuffer_;
  char commandBuffer_[BufferSize];
  std::atomic<char *> writeMarkerAtomic_;
  std::atomic<const char *> readMarkerAtomic_;
  std::any syncPointResult_;
  std::thread consumerThread_;
  std::recursive_mutex multiWriteRecursiveMutex_;
};

}  // namespace c8
