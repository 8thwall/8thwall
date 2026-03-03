// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Paris Morgan (paris@8thwall.com)
//
// Holds a json container of parameters. Currently supports String, int, bool, double, and floats.
// NOTE - when using mergeJson(), if there is already a value set the incoming data will not
// overwrite it.

#pragma once

#include <memory>
#include <stack>
#include <variant>

#include "c8/string.h"

namespace c8 {

// NOTE: Currently doesn't support object or array parameters, could be added as needed.
class ParameterData {
public:
  ParameterData();
  ~ParameterData();

  template <typename T>
  T get(const char *key) const;

  // Gets the value for the key or, if not present, sets it.
  template <typename T>
  T getOrSet(const char *key, T fallback);

  // Sets the value for the key and updates the version.
  template <typename T>
  void set(const char *key, T val);

  bool contains(const char *key) const;

  void deleteKey(const char *key);

  // If there are duplicate keys in the base and the incoming, we use the incoming value.
  void mergeJsonString(const String &key);

  int version() const { return version_; }

  String toJsonString() const;

private:
  struct Parameters;
  Parameters *parameters_ = nullptr;
  // Version number used to help callers keep track of whether data has changed during runtime.
  int version_ = 0;
};

// Global static used to hold parameter data.
ParameterData &globalParams();

/**
 * Used to make a scoped update to a parameter. This will update the parameter value and revert it
 * back when out of scope.
 *
 * Usage
 *   {
 *     // The previous value was 4 is saved.
 *     ScopedParameterUpdate update("someKey.someItem", 5);
 *     globalParams().get<int>("someKey.someItem"); // Returns 5.
 *   }
 *   globalParams().get<int>("someKey.someItem"); // Returns 4.
 */

class ScopedParameterUpdateGeneric {
public:
  virtual ~ScopedParameterUpdateGeneric() = default;
};

template <typename T>
class ScopedParameterUpdate : public ScopedParameterUpdateGeneric {
public:
  // This verion will call globalParams()
  ScopedParameterUpdate(const char *key, T val);

  // Modify pd
  ScopedParameterUpdate(ParameterData *pd, const char *key, T val);

  ~ScopedParameterUpdate();

private:
  ParameterData *pd_;
  bool hasPreviousVal_ = false;
  T previousVal_;
  String key_;
};

// Implementation
template <typename T>
ScopedParameterUpdate<T>::ScopedParameterUpdate(const char *key, T val)
    : ScopedParameterUpdate<T>{&globalParams(), key, val} {};

template <typename T>
ScopedParameterUpdate<T>::ScopedParameterUpdate(ParameterData *pd, const char *key, T val) {
  if (pd->contains(key)) {
    // If the key is already set, save the previous value.
    previousVal_ = pd->get<T>(key);
    hasPreviousVal_ = true;
  }
  pd->set<T>(key, val);
  pd_ = pd;
  key_ = String(key);
}

template <typename T>
ScopedParameterUpdate<T>::~ScopedParameterUpdate() {
  if (hasPreviousVal_) {
    pd_->set<T>(key_.c_str(), previousVal_);
  } else {
    pd_->deleteKey(key_.c_str());
  }
}

class ScopedParameterUpdates {
public:
  // Struct enables using the {{KEY, VALUE}, {KEY, VALUE}} syntax to set multiple parameters.
  struct ParameterChange {
    const char *key;
    const std::variant<bool, int, int64_t, uint32_t, uint64_t, float, double, String> value;
  };

  // Same as ScopeTimer except with default constructor
  ScopedParameterUpdates() = default;
  ScopedParameterUpdates(ScopedParameterUpdates &&) = default;
  ScopedParameterUpdates &operator=(ScopedParameterUpdates &&) = default;
  ScopedParameterUpdates(const ScopedParameterUpdates &) = delete;
  ScopedParameterUpdates &operator=(const ScopedParameterUpdates &) = delete;

  // Constructor which enables {{KEY, VALUE}, {KEY, VALUE}} syntax
  ScopedParameterUpdates(std::initializer_list<ParameterChange> changes) { set(changes); }

  // Directly sets a parameter value.
  template <typename T>
  void set(const char *key, const T val) {
    updates_.push(std::make_unique<ScopedParameterUpdate<T>>(key, val));
  }

  // Enables setting with {{KEY, VALUE}, {KEY, VALUE}} syntax.
  void set(std::initializer_list<ParameterChange> changes) {
    for (auto &change : changes) {
      set(change);
    }
  }

  void mergeJsonString(const String &jsonString);

  // Destructor destroys updates_ in LIFO order, to ensure reverted values are correct
  ~ScopedParameterUpdates() {
    while (!updates_.empty()) {
      updates_.pop();
    }
  }

private:
  void set(const ParameterChange &change) {
    std::visit(
      [this, &change](auto &&arg) {
        using T = std::decay_t<decltype(arg)>;
        set<T>(change.key, arg);
      },
      change.value);
  }

  // Unique pointers ensure that no ScopedParameterUpdate is leaked, and it's destroyed when popped
  std::stack<std::unique_ptr<ScopedParameterUpdateGeneric>> updates_;
};

}  // namespace c8
