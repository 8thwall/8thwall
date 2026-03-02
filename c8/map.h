// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Pooja Bansal (pooja@8thwall.com)
//
// This is a 8th Wall wrapper for the STL containers for map and multimap

#pragma once

#include <map>
#include <unordered_map>

namespace c8 {

namespace _ {

// Use typedef classes so we can tune the specification in the future
template <typename Key, typename T>
struct HashMap {
  using type = std::unordered_map<Key, T>;
};

template <typename Key, typename T>
struct TreeMap {
  using type = std::map<Key, T>;
};

template <typename Key, typename T>
struct HashMultiMap {
  using type = std::unordered_multimap<Key, T>;
};

template <typename Key, typename T>
struct TreeMultiMap {
  using type = std::multimap<Key, T>;
};

}  // namespace _

// Use alias declaration for creating the actual types.
template <typename Key, typename T>
using HashMap = typename _::HashMap<Key, T>::type;

template <typename Key, typename T>
using TreeMap = typename _::TreeMap<Key, T>::type;

template <typename Key, typename T>
using HashMultiMap = typename _::HashMultiMap<Key, T>::type;

template <typename Key, typename T>
using TreeMultiMap = typename _::TreeMultiMap<Key, T>::type;

}  // namespace c8
