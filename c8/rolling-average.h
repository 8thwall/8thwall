// Copyright (c) 2024 Niantic, Inc.
// Original Author: Anvith Ekkati (anvith@nianticlabs.com)

#pragma once

#include <deque>
#include <type_traits>

namespace c8 {
template <typename T>
class RollingAverage {
  static_assert(std::is_arithmetic<T>::value, "Template parameter must be a numeric type");

public:
  RollingAverage(size_t windowSize);

  // Adds a new value to the rolling average.
  void add(T value);

  // Returns the rolling average of the last windowSize_ values.
  double average() const;

private:
  size_t windowSize_;
  std::deque<T> values_;
  T sum_;
};

template <typename T>
RollingAverage<T>::RollingAverage(size_t windowSize) : windowSize_(windowSize), sum_(0) {}

template <typename T>
void RollingAverage<T>::add(T value) {
  values_.push_back(value);
  sum_ += value;
  if (values_.size() > windowSize_) {
    sum_ -= values_.front();
    values_.pop_front();
  }
}

template <typename T>
double RollingAverage<T>::average() const {
  if (values_.empty()) {
    return 0.f;
  }
  return sum_ / static_cast<double>(values_.size());
}
}  // namespace c8
