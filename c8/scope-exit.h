// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// Run code when exiting scope.
//
// Example Usage:
// File* file = fopen("foo", "rb");
// SCOPE_EXIT([file] {
//   fclose(file);
// });

#pragma once

#include <functional>
#include <future>
#include <optional>

namespace c8 {

// Macro to use for starting scope clauses.
#define SCOPE_EXIT ::c8::ScopeExit _SCOPE_EXIT_NAME(scopeExit_, __LINE__)

using PackagedTask = std::packaged_task<void()>;

class ScopeExit {
public:
  // Default constructor.
  ScopeExit(std::function<void()> func) : func_(func) {}
  ScopeExit(PackagedTask &&task) : task_(std::move(task)) {}

  // Run function when this class goes out of scope.
  ~ScopeExit() {
    if (func_) {
      func_->operator()();
    }
    if (task_) {
      task_->operator()();
    }
  }

  // Default move constructors.
  ScopeExit(ScopeExit &&) = default;
  ScopeExit &operator=(ScopeExit &&) = default;

  // Disallow copying.
  ScopeExit(const ScopeExit &) = delete;
  ScopeExit &operator=(const ScopeExit &) = delete;

private:
  // std::function is CopyConstructable and not MoveConstructable.
  std::optional<std::function<void()>> func_;

  // std::packaged_task is MoveConstructable and not CopyConstructable.
  // This is used for lambdas that capture via move semantics.
  // PackagedTasks can only be called once, and this is okay given for our context.
  std::optional<PackagedTask> task_;
};

// Internal Macros for generating unique names.
#define _SCOPE_EXIT_NAME(name, line) _MAKE_SCOPE_EXIT_NAME(name, line)
#define _MAKE_SCOPE_EXIT_NAME(name, line) name##line

}  // namespace c8
