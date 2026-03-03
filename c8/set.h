// Copyright (c) 2016 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// Vector alias for std::vector, allowing 8th Wall
// to more easily swap out vector implementations in the future.

#pragma once

#include <set>
#include <unordered_set>

namespace c8 {

namespace _ {

// Use typedef classes so we can tune the specification in the future.
template <typename T>
struct TreeSet {
  typedef std::set<T> type;
};

template <typename T>
struct HashSet {
  typedef std::unordered_set<T> type;
};

}  // namespace _

// Use alias declaration for creating the actual types.
template <typename T>
using TreeSet = typename _::TreeSet<T>::type;

template <typename T>
using HashSet = typename _::HashSet<T>::type;

}  // namespace c8
