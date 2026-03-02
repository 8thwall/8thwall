// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// Code8 exception classes and utility functions.

#pragma once

#include <stdexcept>
#include <system_error>

#include "c8/c8-log.h"
#include "c8/string/format.h"

namespace c8 {

// Logic Errors:
//
// The following are exception classes to use in case of logic errors, e.g.,
// programming mistakes or invalid inputs. These exceptions are typically
// uncaught in normal program execution and indicate fatal behavior. Use the
// logic error most specific to your use case, for the purpose of making errors
// more immediately understandable.
//
// Example Usage:
//
// void pickANumberBetweenOneAndFive(int number) {
//   if (number < 1 || number > 5) {
//     C8_THROW_INVALID_ARGUMENT("number must be one of {1, 2, 3, 4, 5}");
//   }
//   ...
// }
//
// Exception to throw when receiving an argument doesn't satisfy preconditions.
using InvalidArgument = std::invalid_argument;

// Exception to throw when encountering when attempting to access elements out
// of a defined range.
using OutOfRange = std::out_of_range;

// Base class for logic errors that aren't InvalidArgument or OutOfRange. If you
// need to create additional logic errors, you can inherit from LogicError, but
// preferably you would add that new class to this file.
using LogicError = std::logic_error;

// Runtime Errors:
//
// Exception classes to use for runtime errors that are outside the control of
// the program, such as network unavailable or file permissions error. Use the
// RuntimeError class directly for most errors, and inherit from it for any
// specific exceptions that need to be caught and handled independently from
// other RuntimeErrors. New subclasses of RuntimeError should be defined in the
// codebase close to where they are thrown. For example, a
// CameraUnavailableError can be defined a hypothetical
// sensors/camera/exceptions.h file.
using RuntimeError = std::runtime_error;

// System error is a RuntimeError thrown by various standard library methods.
// Its error messages are derived from predefined platform-dependent and
// platform-independent error codes.
using SystemError = std::system_error;

template <class ExceptionType, class... Args>
[[noreturn]] void throwOrDie(const char *fmt) {
#ifdef C8_NO_EXCEPT
  C8Log(fmt);
  std::abort();
#else
  throw ExceptionType(fmt);
#endif
}

template <class ExceptionType, class... Args>
[[noreturn]] void throwOrDie(const String &fmt) {
#ifdef C8_NO_EXCEPT
  C8Log(fmt.c_str());
  std::abort();
#else
  throw ExceptionType(fmt);
#endif
}

template <class ExceptionType, class... Args>
[[noreturn]] void throwOrDie(const char *fmt, Args &&...args) {
#ifdef C8_NO_EXCEPT
  C8Log(fmt, std::forward<Args>(args)...);
  std::abort();
#else
  throw ExceptionType(format(fmt, std::forward<Args>(args)...));
#endif
}

template <class ExceptionType, class... Args>
[[noreturn]] void throwOrDie(const String &fmt, Args &&...args) {
#ifdef C8_NO_EXCEPT
  C8Log(fmt.c_str(), std::forward<Args>(args)...);
  std::abort();
#else
  throw ExceptionType(format(fmt, std::forward<Args>(args)...));
#endif
}

}  // namespace c8

#define C8_THROW(...) (c8::throwOrDie<c8::RuntimeError>(__VA_ARGS__))
#define C8_THROW_INVALID_ARGUMENT(...) (c8::throwOrDie<c8::InvalidArgument>(__VA_ARGS__))
#define C8_THROW_OUT_OF_RANGE(...) (c8::throwOrDie<c8::OutOfRange>(__VA_ARGS__))
#define C8_THROW_LOGIC_ERROR(...) (c8::throwOrDie<c8::LogicError>(__VA_ARGS__))
#define C8_THROW_SYSTEM_ERROR(...) (c8::throwOrDie<c8::SystemError>(__VA_ARGS__))
