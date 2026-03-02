// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// FloatVector type and common linear algebra methods upon it.

#pragma once

#include <initializer_list>

#include "c8/vector.h"

namespace c8 {

// A FloatVector has a mostly similar API to Vector<Float> along with numerous
// methods helpful with linear algebra.
class FloatVector {
public:
  // ---------------------------------------------------------------------------------------
  // Constructors delegate to Vector<Float> constructors and have similar semantics.
  // ---------------------------------------------------------------------------------------
  FloatVector() = default;

  // Default move constructors.
  FloatVector(FloatVector &&) = default;
  FloatVector &operator=(FloatVector &&) = default;

  // Disallow copying. See clone() and copyFrom(...) below.
  FloatVector(const FloatVector &) = delete;
  FloatVector &operator=(const FloatVector &) = delete;

  // Create a FloatVector of size 'count' with all components initialized to 'value'.
  FloatVector(Vector<float>::size_type count, float value = 0.0f) : vector_(count, value) {}

  // Initialize a Float vector to the specified elements.
  FloatVector(std::initializer_list<float> init) : vector_(init) {}

  // ---------------------------------------------------------------------------------------
  // Here are the FloatVector member functions used for Linear Algebra.
  // ---------------------------------------------------------------------------------------

  // Add a scalar to every component in the FloatVector.
  FloatVector &operator+=(float value);

  // Subtract a scalar from every component in the FloatVector.
  FloatVector &operator-=(float value);

  // Multiply every component in the FloatVector by a scalar.
  FloatVector &operator*=(float value);

  // Add a FloatVector to this FloatVector. Dimensions must match.
  FloatVector &operator+=(const FloatVector &vec);

  // Subtract a FloatVector from this FloatVector. Dimensions must match.
  FloatVector &operator-=(const FloatVector &vec);

  // Component-wise multiplication of a FloatVector with this FloatVector. Dimensions must match.
  FloatVector &operator*=(const FloatVector &vec);

  // Return a copy of this FloatVector.
  FloatVector clone() const;

  // Copy values from another FloatVector.
  FloatVector &copyFrom(const FloatVector &vec);

  // Set all components of the vector to the provided value.
  void fill(float value);

  // Set all components of the vector to zero.
  void zeroOut() { fill(0.0f); }

  // L1 normalize the FloatVector.
  FloatVector &l1Normalize();

  // L2 normalize the FloatVector.
  FloatVector &l2Normalize();

  // SSR normalize the FloatVector, aka perform in-place sign square root.
  FloatVector &signSqrt();

  // In-place square root on each of the float vector components.
  FloatVector &sqrt();

  // Invert each of the components in the vector (i.e. 1.0f / value).
  FloatVector &invert();

  // ---------------------------------------------------------------------------------------
  // Const and non-const access to the underlying vector.
  // ---------------------------------------------------------------------------------------
  const Vector<float> &vector() const { return vector_; }
  Vector<float> *mutableVector() { return &vector_; }

  // ---------------------------------------------------------------------------------------
  // Delegation to Vector<float> members to allow this class to behave like an STL vector.
  // All Vector<float> methods are here except for assign, get_allocator, insert, emplace,
  // emplace_back, erase, and pop_back which can reached via the mutableVector() mutator if
  // needed.
  // ---------------------------------------------------------------------------------------
  using value_type = Vector<float>::value_type;
  using allocator_type = Vector<float>::allocator_type;
  using size_type = Vector<float>::size_type;
  using difference_type = Vector<float>::difference_type;
  using reference = Vector<float>::reference;
  using const_reference = Vector<float>::const_reference;
  using pointer = Vector<float>::pointer;
  using const_pointer = Vector<float>::const_pointer;
  using iterator = Vector<float>::iterator;
  using const_iterator = Vector<float>::const_iterator;
  using reverse_iterator = Vector<float>::reverse_iterator;
  using const_reverse_iterator = Vector<float>::const_reverse_iterator;

  reference at(size_type pos) { return vector_.at(pos); }
  const_reference at(size_type pos) const { return vector_.at(pos); }

  reference operator[](size_type pos) { return vector_[pos]; }
  const_reference operator[](size_type pos) const { return vector_[pos]; }

  reference front() { return vector_.front(); }
  const_reference front() const { return vector_.front(); }

  reference back() { return vector_.back(); }
  const_reference back() const { return vector_.back(); }

  float *data() noexcept { return vector_.data(); }
  const float *data() const noexcept { return vector_.data(); }

  iterator begin() noexcept { return vector_.begin(); }
  const_iterator begin() const noexcept { return vector_.begin(); }
  const_iterator cbegin() const noexcept { return vector_.cbegin(); }

  iterator end() noexcept { return vector_.end(); }
  const_iterator end() const noexcept { return vector_.end(); }
  const_iterator cend() const noexcept { return vector_.cend(); }

  reverse_iterator rbegin() noexcept { return vector_.rbegin(); }
  const_reverse_iterator rbegin() const noexcept { return vector_.rbegin(); }
  const_reverse_iterator crbegin() const noexcept { return vector_.crbegin(); }

  reverse_iterator rend() noexcept { return vector_.rend(); }
  const_reverse_iterator rend() const noexcept { return vector_.rend(); }
  const_reverse_iterator crend() const noexcept { return vector_.crend(); }

  bool empty() const noexcept { return vector_.empty(); }
  size_type size() const noexcept { return vector_.size(); }
  size_type max_size() const noexcept { return vector_.max_size(); }
  void reserve(size_type new_cap) { vector_.reserve(new_cap); }
  size_type capacity() const noexcept { return vector_.capacity(); }
  void shrink_to_fit() { vector_.shrink_to_fit(); }

  void push_back(const float &value) { vector_.push_back(value); }
  void push_back(float &&value) { vector_.push_back(std::forward<float>(value)); }

  void resize(size_type count) { vector_.resize(count); }
  void resize(size_type count, const value_type &value) { vector_.resize(count, value); }

  void clear() noexcept { vector_.clear(); }
  void swap(FloatVector &other) noexcept { vector_.swap(other.vector_); }

private:
  Vector<float> vector_;
};

// ---------------------------------------------------------------------------------------
// Non-member methods for Linear Algebra on Float Vectors.
// ---------------------------------------------------------------------------------------

// Compute the L1 distance between two FloatVectors. Both are expected to have
// the same dimension.
float l1Distance(const FloatVector &a, const FloatVector &b);

// Compute the L2 distance between two FloatVectors. Both are expected to have
// the same dimension.
float l2Distance(const FloatVector &a, const FloatVector &b);

// Compute the square of the L2 distance between two  Both are expected to have
// the same dimension.
float l2SquaredDistance(const FloatVector &a, const FloatVector &b);

// Compute the L1 norm of a FloatVector.
float l1Norm(const FloatVector &vec);

// Compute the L2 norm of a FloatVector.
float l2Norm(const FloatVector &vec);

// Compute the square of the L2 norm of a FloatVector.
float l2SquaredNorm(const FloatVector &vec);

// Compute the mean value of the FloatVector components.
float mean(const FloatVector &vec);

// Compute the inner product of two FloatVectors.
float innerProduct(const FloatVector &a, const FloatVector &b);

// Add a scalar to every component in the FloatVector.
FloatVector operator+(const FloatVector &vec, float value);
inline FloatVector operator+(float value, const FloatVector &vec) { return vec + value; }

// Subtract a scalar from every component in the FloatVector.
FloatVector operator-(const FloatVector &vec, float value);
FloatVector operator-(float value, const FloatVector &vec) = delete;

// Multiply every component in the FloatVector by a scalar.
FloatVector operator*(const FloatVector &vec, float value);
inline FloatVector operator*(float value, const FloatVector &vec) { return vec * value; }

// Add a FloatVector to this FloatVector. Dimensions must match.
FloatVector operator+(const FloatVector &a, const FloatVector &b);

// Subtract a FloatVector from this FloatVector. Dimensions must match.
FloatVector operator-(const FloatVector &a, const FloatVector &b);

// Component-wise multiplication of two FloatVectors. Dimensions must match.
FloatVector operator*(const FloatVector &a, const FloatVector &b);

}  // namespace c8
