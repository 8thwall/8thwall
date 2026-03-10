// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include <cstdint>
#include <functional>

#include "c8/string.h"
#include "c8/string/format.h"
namespace c8 {

// Base class for XR ids.
class XrId {
public:
  XrId() = default;
  XrId(const XrId &) = default;

  constexpr XrId(size_t index, uint32_t id) : index_(index), id_(id) {}
  static constexpr XrId nullId() { return XrId{0ul, 0ul}; }

  bool operator==(XrId rhs) const { return index_ == rhs.index_ && id_ == rhs.id_; }
  bool operator!=(XrId rhs) const { return index_ != rhs.index_ || id_ != rhs.id_; }
  bool operator<(XrId rhs) const { return id_ == rhs.id_ ? index_ < rhs.index_ : id_ < rhs.id_; }

  size_t index() const { return index_; }
  uint32_t id() const { return id_; }

  String toString() const noexcept { return c8::format("(id: %zu, index: %zu)", id_, index_); }

private:
  size_t index_ = 0ul;
  uint32_t id_ = 0ul;
};

// Identifier for world points.
class WorldMapPointId : public XrId {
public:
  WorldMapPointId() = default;
  constexpr WorldMapPointId(size_t index, uint32_t id) : XrId(index, id) {}
  static constexpr WorldMapPointId nullId() { return WorldMapPointId{0ul, 0ul}; }
};

// Identifier for world map's keyframes. To be used with WorldMap::getKeyframe.
// @param index: auto-incremented index of this keyframe on creation. Since we re-use keyframes, the
//        keyframes_[index] might no longer be valid for this id.
// @param id: unique id of this keyframe made on creation
class WorldMapKeyframeId : public XrId {
public:
  WorldMapKeyframeId() = default;
  constexpr WorldMapKeyframeId(size_t index, uint32_t id) : XrId(index, id) {}
  static constexpr WorldMapKeyframeId nullId() { return WorldMapKeyframeId{0ul, 0ul}; }
};

}  // namespace c8

namespace std {
// Implement std::hash for XRIds.
template <>
struct hash<c8::WorldMapPointId> {
  size_t operator()(const c8::WorldMapPointId &id) const {
    return hash<size_t>()(id.index()) ^ (hash<uint32_t>()(id.id()) << 1);
  }
};

template <>
struct hash<c8::WorldMapKeyframeId> {
  size_t operator()(const c8::WorldMapKeyframeId &id) const {
    return hash<size_t>()(id.index()) ^ (hash<uint32_t>()(id.id()) << 1);
  }
};

}  // namespace std
