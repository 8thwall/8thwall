// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// RefVector and CRefVector, iterable vectors of object references.

#pragma once

#include <functional>
#include <iterator>
#include <memory>
#include <vector>

namespace c8 {

template <typename T>
class RefVector;

// Const version of RefVector.
template <typename T>
using CRefVector = RefVector<const T>;

template <typename T>
class RefVector {
private:
  using SubType = std::vector<std::reference_wrapper<T>>;
  using SubTypeTraits = std::iterator_traits<typename SubType::iterator>;

public:
  // Container traits.
  using size_type = typename SubType::size_type;
  using value_type = typename SubType::value_type;
  using allocator_type = typename SubType::allocator_type;
  using difference_type = typename SubType::difference_type;
  using reference = T &;
  using const_reference = const T &;
  using pointer = typename std::allocator_traits<allocator_type>::pointer;
  using const_pointer = typename std::allocator_traits<allocator_type>::const_pointer;

  class const_iterator {
  public:
    // iterator traits.
    using difference_type = typename SubTypeTraits::difference_type;
    using value_type = typename SubTypeTraits::value_type;
    using pointer = const T *;
    using reference = const T &;
    using iterator_category = typename SubTypeTraits::iterator_category;

    const_iterator(typename SubType::const_iterator iterIn) : iter(iterIn) {}
    const T &operator*() const { return *iter; }
    const T *operator->() const { return iter; }
    // Implement ForwardIterator.
    const_iterator &operator++() {
      ++iter;
      return *this;
    }
    const_iterator operator++(int) {
      const_iterator retval = *this;
      ++(*this);
      return retval;
    }
    // Implement BidirectionalIterator.
    const_iterator &operator--() {
      --iter;
      return *this;
    }
    const_iterator operator--(int) {
      const_iterator retval = *this;
      --(*this);
      return retval;
    }
    // Implement RandomAccessIterator.
    const T &operator[](difference_type rhs) const { return iter[rhs]; }
    const_iterator &operator+=(difference_type rhs) {
      iter += rhs;
      return *this;
    }
    const_iterator &operator-=(difference_type rhs) {
      iter -= rhs;
      return *this;
    }
    difference_type operator-(const const_iterator &rhs) const {
      return const_iterator(iter - rhs.iter);
    }
    const_iterator operator+(difference_type rhs) const { return const_iterator(iter + rhs); }
    const_iterator operator-(difference_type rhs) const { return const_iterator(iter - rhs); }
    friend const_iterator operator+(difference_type lhs, const const_iterator &rhs) {
      return const_iterator(lhs + rhs.iter);
    }

    bool operator==(const_iterator rhs) const { return iter == rhs.iter; }
    bool operator!=(const_iterator rhs) const { return iter != rhs.iter; }
    bool operator>(const_iterator rhs) const { return iter > rhs.iter; }
    bool operator<(const_iterator rhs) const { return iter < rhs.iter; }
    bool operator>=(const_iterator rhs) const { return iter >= rhs.iter; }
    bool operator<=(const_iterator rhs) const { return iter <= rhs.iter; }

  private:
    typename SubType::const_iterator iter;
  };

  class iterator {
  public:
    // iterator traits.
    using difference_type = typename SubTypeTraits::difference_type;
    using value_type = typename SubTypeTraits::value_type;
    using pointer = T *;
    using reference = T &;
    using iterator_category = typename SubTypeTraits::iterator_category;

    iterator(typename SubType::iterator iterIn) : iter(iterIn) {}
    T &operator*() const { return *iter; }
    T *operator->() const { return iter; }
    // Implement ForwardIterator.
    iterator &operator++() {
      ++iter;
      return *this;
    }
    iterator operator++(int) {
      iterator retval = *this;
      ++(*this);
      return retval;
    }
    // Implement BidirectionalIterator.
    iterator &operator--() {
      --iter;
      return *this;
    }
    iterator operator--(int) {
      iterator retval = *this;
      --(*this);
      return retval;
    }
    // Implement RandomAccessIterator.
    T &operator[](difference_type rhs) const { return iter[rhs]; }
    iterator &operator+=(difference_type rhs) {
      iter += rhs;
      return *this;
    }
    iterator &operator-=(difference_type rhs) {
      iter -= rhs;
      return *this;
    }
    difference_type operator-(const iterator &rhs) const { return iterator(iter - rhs.iter); }
    iterator operator+(difference_type rhs) const { return iterator(iter + rhs); }
    iterator operator-(difference_type rhs) const { return iterator(iter - rhs); }
    friend iterator operator+(difference_type lhs, const iterator &rhs) {
      return iterator(lhs + rhs.iter);
    }

    bool operator==(iterator rhs) const { return iter == rhs.iter; }
    bool operator!=(iterator rhs) const { return iter != rhs.iter; }
    bool operator>(iterator rhs) const { return iter > rhs.iter; }
    bool operator<(iterator rhs) const { return iter < rhs.iter; }
    bool operator>=(iterator rhs) const { return iter >= rhs.iter; }
    bool operator<=(iterator rhs) const { return iter <= rhs.iter; }

  private:
    typename SubType::iterator iter;
  };

  // Default constructor.
  RefVector() = default;

  // Fake constructor for debugging.
  RefVector(T &x, T &y, T &z) : vec{{x, y, z}} {}

  // Default move constructors.
  RefVector(RefVector &&) = default;
  RefVector &operator=(RefVector &&) = default;

  // Delete copy constructors.
  RefVector(const RefVector &) = delete;
  RefVector &operator=(const RefVector &) = delete;

  T &operator[](size_type rhs) { return vec[rhs]; }
  const T &operator[](size_type rhs) const { return vec[rhs]; }

  iterator begin() { return vec.begin(); }
  const_iterator begin() const { return vec.cbegin(); }
  const_iterator cbegin() const { return vec.cbegin(); }

  iterator end() { return vec.end(); }
  const_iterator end() const { return vec.cend(); }
  const_iterator cend() const { return vec.cend(); }

  bool empty() const noexcept { return vec.empty(); }
  size_type size() const noexcept { return vec.size(); }
  size_type max_size() const noexcept { return vec.max_size(); }
  void reserve(size_type size) { vec.reserve(size); }
  size_type capacity() const noexcept { return vec.capacity(); }
  void shrink_to_fit() { return vec.shrink_to_fit(); }
  void clear() noexcept { vec.clear(); };

  void push_back(const std::reference_wrapper<T> &ref) { vec.push_back(ref); }
  void pop_back() { vec.pop_back(); }

  // TODO(mc): Add other insert, erase methods if desired.

private:
  SubType vec;
};

}  // namespace c8
