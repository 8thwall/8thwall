#include "bzl/inliner/rules2.h"

cc_library {
  name = "collision-handlers";
  deps = {
    "@flecs//:flecs",
    "@joltphysics",
    "//c8:map",
    "//c8:vector",
    "//c8:quaternion",
    "//c8/ecs:physics-shared",
  };
  hdrs = {
    "collision-handlers.h",
  };
}
cc_end(0x88e6132a);

// clang-format off
// This must be included before any other Jolt headers
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/Collision/CollideShape.h>
#include <Jolt/Physics/Collision/ContactListener.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
// clang-format on

#include "c8/ecs/collision-handlers.h"
#include "c8/ecs/physics-shared.h"

using namespace JPH;

using namespace c8;

uint BPLayerInterfaceImpl::GetNumBroadPhaseLayers() const { return BroadPhaseLayers::NUM_LAYERS; }

BroadPhaseLayer BPLayerInterfaceImpl::GetBroadPhaseLayer(ObjectLayer inLayer) const {
  return inLayer == Layers::NON_MOVING ? BroadPhaseLayers::NON_MOVING : BroadPhaseLayers::MOVING;
}

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
const char *BPLayerInterfaceImpl::GetBroadPhaseLayerName(BroadPhaseLayer inLayer) const {
  switch ((BroadPhaseLayer::Type)inLayer) {
    case (BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:
      return "NON_MOVING";
    case (BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:
      return "MOVING";
    default:
      return "INVALID";
  }
}
#endif

bool ObjectVsBroadPhaseLayerFilterImpl::ShouldCollide(
  ObjectLayer inLayer1, BroadPhaseLayer inLayer2) const {
  // TODO: add more layer mappings to allow for more complex interactions. Example:
  // Bullet penetration on some objects, etc...
  switch (inLayer1) {
    case Layers::NON_MOVING:
      return inLayer2 == BroadPhaseLayers::MOVING;
    case Layers::MOVING:
      return true;
    default:
      return false;
  }
}

bool ObjectLayerPairFilterImpl::ShouldCollide(ObjectLayer inObject1, ObjectLayer inObject2) const {
  switch (inObject1) {
    case Layers::NON_MOVING:
      return inObject2 == Layers::MOVING;  // Non moving only collides with moving
    case Layers::MOVING:
      return true;  // Moving collides with everything
    default:
      return false;
  }
}

ContactListenerImpl::ContactListenerImpl(PhysicsContext *ctx) : ctx_(ctx) {}

ValidateResult ContactListenerImpl::OnContactValidate(
  const Body &inBody1,
  const Body &inBody2,
  RVec3Arg inBaseOffset,
  const CollideShapeResult &inCollisionResult) {
  return ValidateResult::AcceptAllContactsForThisBodyPair;
}

void ContactListenerImpl::OnContactAdded(
  const Body &inBody1,
  const Body &inBody2,
  const ContactManifold &inManifold,
  ContactSettings &ioSettings) {
  ecs_entity_t entity1 = inBody1.GetUserData();
  ecs_entity_t entity2 = inBody2.GetUserData();

  // Handle the collision contact
  if (ctx_) {
    // TODO: expose collision data
    auto key = SortedEntityPair{entity1, entity2};
    if (ctx_->activeCollisions.count(key) == 0) {
      ctx_->collisionStarts.push_back(entity1);
      ctx_->collisionStarts.push_back(entity2);
    }
    ctx_->activeCollisions[key] = 1;
  }
}

void ContactListenerImpl::OnContactPersisted(
  const Body &inBody1,
  const Body &inBody2,
  const ContactManifold &inManifold,
  ContactSettings &ioSettings) {
  ecs_entity_t entity1 = inBody1.GetUserData();
  ecs_entity_t entity2 = inBody2.GetUserData();

  // Mark this collision pair as still active this frame
  if (ctx_) {
    auto key = SortedEntityPair{entity1, entity2};
    ctx_->activeCollisions[key] = 1;
  }
}
