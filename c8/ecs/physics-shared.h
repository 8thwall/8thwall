#pragma once

// clang-format off
// This must be included before any other Jolt headers
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Collision/ContactListener.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/JobSystemSingleThreaded.h>
#include <Jolt/Physics/PhysicsSystem.h>
// clang-format on
#include "c8/map.h"
#include "c8/vector.h"
#include "flecs.h"

typedef uint8_t collider_type_t;
typedef uint32_t collider_shape_t;

constexpr collider_type_t COLLIDER_STATIC = 0;
constexpr collider_type_t COLLIDER_DYNAMIC = 1;
constexpr collider_type_t COLLIDER_KINEMATIC = 2;

constexpr collider_shape_t BOX_SHAPE = 0;
constexpr collider_shape_t SPHERE_SHAPE = 1;
constexpr collider_shape_t PLANE_SHAPE = 2;
constexpr collider_shape_t CAPSULE_SHAPE = 3;
constexpr collider_shape_t CONE_SHAPE = 4;
constexpr collider_shape_t CYLINDER_SHAPE = 5;
constexpr collider_shape_t CIRCLE_SHAPE = 6;
constexpr collider_shape_t CUSTOM_SHAPE_START_ID = 1000;

namespace Layers {
static constexpr JPH::ObjectLayer NON_MOVING = 0;
static constexpr JPH::ObjectLayer MOVING = 1;
static constexpr JPH::uint NUM_LAYERS = 2;
};  // namespace Layers

namespace BroadPhaseLayers {
static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
static constexpr JPH::BroadPhaseLayer MOVING(1);
static constexpr JPH::uint NUM_LAYERS(2);
};  // namespace BroadPhaseLayers

struct BodyMetadata {
  // NOTE(alancastillo): This is necessary to trigger shape refresh if values change
  float lastWidth;
  float lastHeight;
  float lastDepth;
  float lastRadius;
  float lastMass;
  float lastFriction;
  float lastLinearDamping;
  float lastAngularDamping;
  collider_type_t lastType;
  collider_shape_t lastShape;
  float lastGravityFactor;
  JPH::Vec3 lastScale;
  JPH::EAllowedDOFs lastDofs;
  bool lastEventOnly;
};

struct SortedEntityPair {
  ecs_entity_t lowerId;
  ecs_entity_t higherId;

  SortedEntityPair(ecs_entity_t id1, ecs_entity_t id2) {
    lowerId = std::min(id1, id2);
    higherId = std::max(id1, id2);
  }

  bool operator<(const SortedEntityPair &other) const {
    if (lowerId < other.lowerId) {
      return true;
    } else if (lowerId > other.lowerId) {
      return false;
    } else {
      return higherId < other.higherId;
    }
  }
};

struct PhysicsContext {
  bool enabled = true;
  ecs_entity_t positionComponent;
  ecs_entity_t scaleComponent;
  ecs_entity_t rotationComponent;
  JPH::PhysicsSystem *system = nullptr;
  JPH::BodyInterface *bodyInterface = nullptr;
  JPH::BroadPhaseLayerInterface *broadPhaseLayerInterface = nullptr;
  JPH::ObjectVsBroadPhaseLayerFilter *objectBroadPhaseLayerFilter = nullptr;
  JPH::ObjectLayerPairFilter *objectLayerPairFilter = nullptr;
  JPH::TempAllocatorImpl *tempAllocator = nullptr;
  JPH::JobSystemSingleThreaded *jobSystem = nullptr;
  ecs_query_t *colliderQuery;
  ecs_entity_t colliderComponent;
  // Maps a collision pair to the last "collision frame" it was active
  c8::TreeMap<SortedEntityPair, uint8_t> activeCollisions = {};
  c8::TreeMap<SortedEntityPair, uint8_t> previousFrameCollisions = {};
  c8::TreeMap<collider_shape_t, JPH::Ref<JPH::Shape>> customShapes = {};
  collider_shape_t customShapesCounter = CUSTOM_SHAPE_START_ID;
  c8::TreeMap<ecs_entity_t, BodyMetadata *> bodyMetadata = {};
  // New/old collisions that occurred in the last frame.
  // Each collision is represented by two elements, which will be read out by JavaScript
  // using direct memory access.
  c8::Vector<ecs_entity_t> collisionStarts = {};
  c8::Vector<ecs_entity_t> collisionEnds = {};
};
