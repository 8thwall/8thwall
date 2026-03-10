// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)
//

#pragma once

#include <functional>

#include "c8/vector.h"

namespace c8 {

template <typename I, typename O>
Vector<O> map(const Vector<I> &i, std::function<O(const I &)> t) {
  Vector<O> o;
  o.reserve(i.size());
  std::transform(i.begin(), i.end(), std::back_inserter(o), t);
  return o;
}

template <typename T>
Vector<T> flatten(const std::initializer_list<Vector<T>> &items) {
  Vector<T> result;
  for (auto &item : items) {
    result.insert(result.end(), item.begin(), item.end());
  }
  return result;
}

template <typename T>
Vector<T> flattenVec(const Vector<Vector<T>> &items) {
  Vector<T> result;
  for (auto &item : items) {
    result.insert(result.end(), item.begin(), item.end());
  }
  return result;
}

}  // namespace c8
