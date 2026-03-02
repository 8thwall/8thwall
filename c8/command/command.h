// Copyright (c) 2025 Niantic, Inc.
// Original Author: Erik Murphy-Chutorian (mc@nianticlabs.com)
//
// Data object (Command) and wrappers (cmd::TransferWrap and cmd::FixedArrayWrap) for encapsulating
// a command that can be run. Intended for use with the CommandBuffer class.

#pragma once

#include <any>
#include <array>
#include <concepts>
#include <cstring>
#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

#include "c8/command/transfer-buffer.h"

namespace c8 {

namespace internal {

// Trait to extract the argument and return types of a function pointer.
template <typename Func>
struct FuncTraits;

// Specialization for function pointers.
template <typename R, typename... Args>
struct FuncTraits<R (*)(Args...)> {
  using ArgsType = std::tuple<Args...>;
  using ResultType = R;
};

// Concept to check if a type is a function pointer.
template <typename T>
concept FunctionPointer = std::is_pointer_v<T> && std::is_function_v<std::remove_pointer_t<T>>;

// Function to unpack a tuple and check the condition
template <typename... Tuple>
struct allTupleElementsTriviallyCopyable;

template <typename... Args>
struct allTupleElementsTriviallyCopyable<std::tuple<Args...>> {
  static constexpr bool value = (std::is_trivially_copyable_v<Args> && ...);
};

}  // namespace internal

namespace cmd {

// FixedArrayWrap is a class to wrap a fixed-size array of trivially copyable data for use with
// Command.
template <typename T, size_t N>
requires std::is_trivially_copyable_v<T>
class FixedArrayWrap {
public:
  FixedArrayWrap() = default;
  FixedArrayWrap(const std::array<T, N> &data) : data_(data) {}
  FixedArrayWrap(const T *data) { std::memcpy(data_.data(), data, sizeof(T) * N); }
  FixedArrayWrap(const T (&data)[N]) { std::memcpy(data_.data(), data, sizeof(T) * N); }

  // Implicit conversion to pointer, required for passing FixedArrayWrap's to a Command's function.
  operator const T *() const { return data_.data(); }

private:
  std::array<T, N> data_;
};

// TransferWrap is a class to wrap a dynamic-size array of trivially copyable data for use with
// Command. The data is owned by a TransferBuffer.
template <typename T>
requires(std::is_trivially_copyable_v<T> || std::is_void_v<T>) class TransferWrap {
public:
  using Type = T;
  TransferWrap() = default;
  TransferWrap(const T *src, std::size_t count, std::ptrdiff_t offset = 0)
      : ptr_(reinterpret_cast<const PointerType *>(src)), count_(count), offset_(offset) {}

  bool store(TransferBuffer &buffer) {
    if (ptr_ == nullptr) {
      return true;
    }
    const PointerType *newPtr = buffer.store(ptr_ + offset_, count_);
    if (newPtr == nullptr) {
      return false;
    } else {
      // Update the ptr_ and return true if the data was successfully stored in the buffer.
      ptr_ = newPtr;
      return true;
    }
  }

  // Implicit conversion to pointer, required for passing TransferWrap's to a Command's function.
  operator const T *() const { return ptr_; }

  void release(TransferBuffer &buffer) const {
    if (ptr_ == nullptr) {
      return;
    }
    buffer.release(ptr_ + offset_ + count_);
  }

private:
  // Use char for void types.
  using PointerType = std::conditional_t<std::is_void_v<T>, char, T>;

  const PointerType *ptr_;
  std::size_t count_;
  std::ptrdiff_t offset_;
};

}  // namespace cmd

namespace internal {

template <typename T>
struct IsTransferWrap : std::false_type {};

template <typename T>
struct IsTransferWrap<cmd::TransferWrap<T>> : std::true_type {};

template <typename T>
void releaseTransferWrap(T &&arg, TransferBuffer &buffer) {
  if constexpr (IsTransferWrap<std::decay_t<T>>::value) {
    arg.release(buffer);
  }
}

template <typename T>
bool storeTransferWrap(T &&arg, TransferBuffer &buffer) {
  if constexpr (IsTransferWrap<std::decay_t<T>>::value) {
    return arg.store(buffer);
  } else {
    return true;
  }
}

}  // namespace internal

// Create a command object that can be run.
//
// Example Usage:
//  Command<glClearColor> clearColorCommand { 0.0f, 0.0f, 0.0f, 1.0f };
//  Command<glClear> clearCommand { GL_COLOR_BUFFER_BIT };
template <typename Func, typename... Args>
requires internal::FunctionPointer<Func>
class Command {
public:
  using ArgsType = std::tuple<Args...>;
  // Ensure all arguments of Func are trivially copyable.
  static_assert(
    internal::allTupleElementsTriviallyCopyable<ArgsType>::value,
    "All arguments must be trivially copyable.");

private:
  // Default constructor is private.
  Command() noexcept = default;

public:
  using CommandTrait = void;
  using ResultType = typename internal::FuncTraits<Func>::ResultType;
  using FuncArgsType = typename internal::FuncTraits<Func>::ArgsType;
  using FuncType = ResultType(FuncArgsType...);

  // Copy constructor is deleted.
  Command(const Command &) noexcept = delete;

  // Move constructor is default.
  Command(Command &&) noexcept = default;

  // Constructor takes the same parameters as the templated function.
  Command(Func func, Args... args) noexcept : func_(func), args_(std::forward<Args>(args)...) {}

  // Executes and stores the result of the command.
  std::conditional_t<std::is_void_v<ResultType>, void, ResultType> run() noexcept {
    return std::apply(func_, args_);
  }

  // Clone the Command.
  Command clone() const noexcept {
    Command clone;
    std::memcpy(&clone, this, sizeof(Command));
    return clone;
  }

  // Read the command from a buffer, and run it. Modifies and advances the src pointerby the size of
  // the command, accounting for a possible buffer wrap-around. Releases any transfer wraps.
  static ResultType deserializeRun(
    const char *&src,
    const char *bufferStart,
    const char *bufferEnd,
    TransferBuffer &transferBuffer) {
    // Create a command object on the stack and memcpy the serialized data into it.
    Command command;

    std::size_t bytesUntilEnd = bufferEnd - src;
    if (bytesUntilEnd < sizeof(Command)) [[unlikely]] {
      std::memcpy(reinterpret_cast<char *>(&command), src, bytesUntilEnd);
      std::memcpy(
        reinterpret_cast<char *>(&command) + bytesUntilEnd,
        bufferStart,
        sizeof(Command) - bytesUntilEnd);
      src = bufferStart + sizeof(Command) - bytesUntilEnd;
    } else [[likely]] {
      std::memcpy(&command, src, sizeof(Command));
      src += sizeof(Command);
    }

    if constexpr (std::is_void_v<ResultType>) {
      command.run();
      // Release any transfer wraps.
      command.releaseTransferWraps(transferBuffer);
      return;
    } else {
      ResultType result = command.run();
      command.releaseTransferWraps(transferBuffer);
      return result;
    }
  }

  // Same as the above, but returns the result as an std::any.
  static std::any deserializeRunAny(
    const char *&src,
    const char *bufferStart,
    const char *bufferEnd,
    TransferBuffer &transferBuffer) {
    std::any result;
    // If ResultType is void, return an any empty value otherwise return the result.
    if constexpr (std::is_void_v<ResultType>) {
      deserializeRun(src, bufferStart, bufferEnd, transferBuffer);
      return std::any();
    } else {
      return std::make_any<ResultType>(deserializeRun(src, bufferStart, bufferEnd, transferBuffer));
    }
  }

  bool storeTransferWraps(TransferBuffer &tb) {
    // Do a compile-time check to ensure that there are at most one TransferWrap in the arguments.
    // This is to prevent the need for complicated unwinding if one Transfer buffer store succeeds
    // but another fails in the same command.
    static_assert(
      (0 + ... + internal::IsTransferWrap<std::decay_t<Args>>::value) <= 1,
      "Only one TransferWrap is allowed per command.");

    return std::apply(
      [&tb](auto &&...args) {
        return ((internal::storeTransferWrap(std::forward<decltype(args)>(args), tb)) && ...);
      },
      args_);
  }

  void releaseTransferWraps(TransferBuffer &tb) {
    std::apply(
      [&tb](auto &&...args) {
        (internal::releaseTransferWrap(std::forward<decltype(args)>(args), tb), ...);
      },
      args_);
  }

private:
  Func func_;
  ArgsType args_;
};

// Concept to check if a type is a Command type.
template <typename T>
concept CommandConcept = requires {
  typename T::CommandTrait;  // Check if `CommandTrait` exists in T
};

}  // namespace c8
