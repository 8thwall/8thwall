// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include <capnp/serialize.h>

#include <cstring>
#include <memory>
#include <utility>

namespace c8 {

// A MutableRootMessage is capnp reader backed by a variable-sized MallocMessageBuilder. It is
// suitable for modification, but not directly useable for serialization.
template <typename T>
class MutableRootMessage {
public:
  MutableRootMessage() : message_(new capnp::MallocMessageBuilder()) { message_->initRoot<T>(); }

  // Initialize a mutable root message as a copy of an existing reader.
  MutableRootMessage(typename T::Reader r) : message_(new capnp::MallocMessageBuilder) {
    message_->setRoot(r);
  }

  capnp::MessageBuilder *message() { return message_.get(); }

  // Rebuild this message with a new root. All previous references to builders and readers are
  // invalidated.
  void setRoot(typename T::Reader r) {
    message_.reset(new capnp::MallocMessageBuilder());
    message_->setRoot(r);
  }

  // Default move constructors.
  MutableRootMessage(MutableRootMessage &&) = default;
  MutableRootMessage &operator=(MutableRootMessage &&) = default;

  // Disallow copying.
  MutableRootMessage(const MutableRootMessage &) = delete;
  MutableRootMessage &operator=(const MutableRootMessage &) = delete;

  typename T::Builder builder() { return message_->getRoot<T>(); }
  typename T::Reader reader() const { return message_->getRoot<T>().asReader(); }

private:
  // MallocMessageBuilder is not moveable, so we wrap in a unique_ptr for move semantics.
  std::unique_ptr<capnp::MallocMessageBuilder> message_;
};

constexpr capnp::ReaderOptions NO_TRAVERSAL_LIMIT_READER_OPTIONS = {0xFFFFFFFFFFFFFFFF, 64};

// A ConstRootMessage is capnp reader backed by a fixed, contiguous array. As such, it should not
// be modified.
template <typename T>
class ConstRootMessage {
public:
  // Construct a ConstRootMessage by copying the data pointed by bytes into memory owned by this
  // root message.
  ConstRootMessage(const void *bytes, size_t size)
      : bytes_(copy(bytes, size)), message_(bytes_, NO_TRAVERSAL_LIMIT_READER_OPTIONS) {}

  // Construct a ConstRootMessage by copying the data pointed by bytes into memory owned by this
  // root message.
  ConstRootMessage(kj::Array<capnp::word> &&arrayToMove)
      : bytes_(std::forward<kj::Array<capnp::word>>(arrayToMove)),
        message_(bytes_, NO_TRAVERSAL_LIMIT_READER_OPTIONS) {}

  // Construct a ConstRootMessage by copying the data pointed by bytes into memory owned by this
  // root message.
  ConstRootMessage(capnp::Data::Reader data)
      : bytes_(copy(static_cast<const void *>(data.begin()), data.size())),
        message_(bytes_, NO_TRAVERSAL_LIMIT_READER_OPTIONS) {}

  // Construct a ConstRootMessage as a flat copy of a mutable message.
  ConstRootMessage(MutableRootMessage<T> &mutableMessage)
      : bytes_(capnp::messageToFlatArray(*(mutableMessage.message()))),
        message_(bytes_.asPtr(), NO_TRAVERSAL_LIMIT_READER_OPTIONS) {}

  // Construct a ConstRootMessage as a flat copy of a reader.
  ConstRootMessage(typename T::Reader reader)
      : bytes_(freezeReader(reader)), message_(bytes_.asPtr(), NO_TRAVERSAL_LIMIT_READER_OPTIONS) {}

  // Construct a ConstRootMessage with a default message.
  ConstRootMessage()
      : bytes_(empty()), message_(bytes_.asPtr(), NO_TRAVERSAL_LIMIT_READER_OPTIONS) {}

  // Default move constructors.
  ConstRootMessage(ConstRootMessage &&a)
      : bytes_(std::move(a.bytes_)), message_(bytes_.asPtr(), NO_TRAVERSAL_LIMIT_READER_OPTIONS) {}

  ConstRootMessage &operator=(ConstRootMessage &&a) {
    bytes_ = std::move(a.bytes_);
    message_ = capnp::FlatArrayMessageReader(bytes_.asPtr(), NO_TRAVERSAL_LIMIT_READER_OPTIONS);
    return *this;
  };

  // Explicitly create a copy.
  ConstRootMessage clone() const {
    return ConstRootMessage<T>(static_cast<const void *>(bytes().begin()), bytes().size());
  }

  // Disallow copying.
  ConstRootMessage(const ConstRootMessage &) = delete;
  ConstRootMessage &operator=(const ConstRootMessage &) = delete;

  typename T::Reader reader() const {
    // The returned reader is logically const as it can't mutate memory.
    return const_cast<capnp::FlatArrayMessageReader &>(message_).getRoot<T>();
  }

  kj::ArrayPtr<const uint8_t> bytes() const {
    return kj::ArrayPtr<const uint8_t>(
      reinterpret_cast<const uint8_t *>(bytes_.begin()), bytes_.size() * sizeof(capnp::word));
  }

private:
  kj::Array<capnp::word> bytes_;
  capnp::FlatArrayMessageReader message_;

  static kj::Array<capnp::word> copy(const void *bytes, size_t size) {
    auto arr = kj::heapArray<capnp::word>(size / sizeof(capnp::word));
    std::memcpy(arr.begin(), bytes, size);
    return arr;
  }

  static kj::Array<capnp::word> freezeReader(typename T::Reader reader) {
    MutableRootMessage<T> m(reader);
    return capnp::messageToFlatArray(*(m.message()));
  }

  static kj::Array<capnp::word> empty() {
    MutableRootMessage<T> src;
    return capnp::messageToFlatArray(*(src.message()));
  }
};

// A ConstRootMessageView is capnp reader backed by a fixed, contiguous array
// that is not owned by this class.
template <typename T>
class ConstRootMessageView {
public:
  // Construct a ConstRootMessageView by copying the data pointed by bytes into memory owned by this
  // root message.
  ConstRootMessageView(const void *bytes, size_t size)
      : message_(new capnp::FlatArrayMessageReader(
          kj::ArrayPtr<const capnp::word>(reinterpret_cast<const capnp::word *>(bytes), size),
          NO_TRAVERSAL_LIMIT_READER_OPTIONS)) {}

  // Default move constructors.
  ConstRootMessageView(ConstRootMessageView &&a) = default;
  ConstRootMessageView &operator=(ConstRootMessageView &&) = default;

  // Disallow copying.
  ConstRootMessageView(const ConstRootMessageView &) = delete;
  ConstRootMessageView &operator=(const ConstRootMessageView &) = delete;

  typename T::Reader reader() const {
    // The returned reader is logically const as it can't mutate memory.
    return const_cast<capnp::FlatArrayMessageReader *>(message_.get())->getRoot<T>();
  }

private:
  std::unique_ptr<capnp::FlatArrayMessageReader> message_;
};

}  // namespace c8
