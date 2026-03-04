#include <cmath>
#include <cstring>

#include "bzl/inliner/rules2.h"

cc_library {
  name = "physics";
  deps = {
    "@flecs//:flecs",
    "@joltphysics",
    "//c8:map",
    "//c8:vector",
    "//c8/ecs:collision-handlers",
  };
  hdrs = {
    "physics.h",
  };
}
cc_end(0xf5b8a5de);

// clang-format off
// This must be included before any other Jolt headers
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyLockInterface.h>
#include <Jolt/Physics/Collision/ContactListener.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/JobSystemSingleThreaded.h>
#include <Jolt/Geometry/AABox.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Collision/Shape/MutableCompoundShape.h>
#include <Jolt/Physics/Collision/Shape/ScaledShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
// clang-format on

#include "c8/ecs/collision-handlers.h"
#include "c8/ecs/physics.h"
#include "c8/map.h"
#include "c8/vector.h"
#include "flecs.h"

using namespace c8;
using c8::Vector;

using namespace JPH;

constexpr float DEFAULT_GRAVITY = -9.81f;

constexpr uint MAX_BODY_COUNT = 65536;
constexpr uint MAX_BODY_PAIRS = 65536;
constexpr uint MAX_CONTACT_CONSTRAINTS = 10240;

constexpr uint CIRCULAR_SHAPE_SEGMENTS = 32;
constexpr uint ECS_DISABLED_INDEX = 3;

constexpr int MIN_VERTICES_FOR_COMPOUND_SHAPE = 12;  // 4 vertices * 3 components (x,y,z)
constexpr int MIN_INDICES_FOR_COMPOUND_SHAPE = 3;    // At least 1 triangle

#ifdef __EMSCRIPTEN__
// NOTE(christoph): This memory layout must match the definitions in physics.ts and components.ts
static_assert(offsetof(PhysicsCollider, bodyPtr) == 0);
static_assert(offsetof(PhysicsCollider, system) == 4);
static_assert(offsetof(PhysicsCollider, width) == 8);
static_assert(offsetof(PhysicsCollider, height) == 12);
static_assert(offsetof(PhysicsCollider, depth) == 16);
static_assert(offsetof(PhysicsCollider, radius) == 20);
static_assert(offsetof(PhysicsCollider, mass) == 24);
static_assert(offsetof(PhysicsCollider, linearDamping) == 28);
static_assert(offsetof(PhysicsCollider, angularDamping) == 32);
static_assert(offsetof(PhysicsCollider, friction) == 36);
static_assert(offsetof(PhysicsCollider, restitution) == 40);
static_assert(offsetof(PhysicsCollider, gravityFactor) == 44);
static_assert(offsetof(PhysicsCollider, offsetX) == 48);
static_assert(offsetof(PhysicsCollider, offsetY) == 52);
static_assert(offsetof(PhysicsCollider, offsetZ) == 56);
static_assert(offsetof(PhysicsCollider, offsetQuaternionX) == 60);
static_assert(offsetof(PhysicsCollider, offsetQuaternionY) == 64);
static_assert(offsetof(PhysicsCollider, offsetQuaternionZ) == 68);
static_assert(offsetof(PhysicsCollider, offsetQuaternionW) == 72);
static_assert(offsetof(PhysicsCollider, shape) == 76);
static_assert(offsetof(PhysicsCollider, simplificationMode) == 80);
static_assert(offsetof(PhysicsCollider, type) == 84);
static_assert(offsetof(PhysicsCollider, eventOnly) == 85);
static_assert(offsetof(PhysicsCollider, lockXAxis) == 86);
static_assert(offsetof(PhysicsCollider, lockYAxis) == 87);
static_assert(offsetof(PhysicsCollider, lockZAxis) == 88);
static_assert(offsetof(PhysicsCollider, lockXPosition) == 89);
static_assert(offsetof(PhysicsCollider, lockYPosition) == 90);
static_assert(offsetof(PhysicsCollider, lockZPosition) == 91);
static_assert(offsetof(PhysicsCollider, highPrecision) == 92);
static_assert(sizeof(PhysicsCollider) == 96);
#endif

static_assert(offsetof(Position, x) == 0);
static_assert(offsetof(Position, y) == 4);
static_assert(offsetof(Position, z) == 8);
static_assert(sizeof(Position) == 12);

static_assert(offsetof(Scale, x) == 0);
static_assert(offsetof(Scale, y) == 4);
static_assert(offsetof(Scale, z) == 8);
static_assert(sizeof(Scale) == 12);

static_assert(offsetof(Rotation, x) == 0);
static_assert(offsetof(Rotation, y) == 4);
static_assert(offsetof(Rotation, z) == 8);
static_assert(offsetof(Rotation, w) == 12);
static_assert(sizeof(Rotation) == 16);

ECS_CTOR(PhysicsCollider, data, { data->bodyPtr = nullptr; });

void wakeBodiesInVicinity(PhysicsSystem *system, BodyID bodyId) {
  auto &bodyInterface = system->GetBodyInterface();
  const auto *body = system->GetBodyLockInterfaceNoLock().TryGetBody(bodyId);
  if (body == nullptr) {
    return;
  }
  // Get the bounding box of the body, expanded by a factor of 2 in all directions
  const auto boundingBox =
    body->GetShape()->GetWorldSpaceBounds(body->GetWorldTransform(), Vec3::sReplicate(2.0f));
  auto inLayer = body->GetObjectLayer();
  auto layerFilter = system->GetDefaultLayerFilter(inLayer);
  bodyInterface.ActivateBodiesInAABox(
    boundingBox, system->GetDefaultBroadPhaseLayerFilter(inLayer), layerFilter);
}

void freeCollider(PhysicsCollider *collider) {
  auto system = collider->system;
  if (system != nullptr && collider->bodyPtr != nullptr) {
    BodyID bodyID = *collider->bodyPtr;
    if (system->GetBodyInterface().IsAdded(bodyID)) {
      wakeBodiesInVicinity(system, bodyID);
      system->GetBodyInterface().RemoveBody(bodyID);
    }

    system->GetBodyInterface().DestroyBody(bodyID);
    delete collider->bodyPtr;
    collider->bodyPtr = nullptr;
  }
}

bool didMove(
  const Vec3 &positionA, const Quat &rotationA, const Vec3 &positionB, const Quat &rotationB) {
  if (!positionA.IsClose(positionB) || !rotationA.IsClose(rotationB)) {
    return true;
  }
  return false;
}

void maybeWakeBodiesInVicinity(
  const PhysicsContext *ctx,
  const PhysicsCollider *collider,
  const Quat &worldRotation,
  const Vec3 &worldPosition,
  const Vec3 &worldScale,
  ecs_entity_t eid) {
  const auto needsWakeOnMove = collider->type == COLLIDER_STATIC
    || (collider->type == COLLIDER_DYNAMIC && !ctx->bodyInterface->IsActive(*collider->bodyPtr));

  if (!needsWakeOnMove) {
    return;
  }

  const auto currentTransform = ctx->bodyInterface->GetWorldTransform(*collider->bodyPtr);
  const auto lastScale = ctx->bodyMetadata.at(eid)->lastScale;

  // Decompose current transform to get position and rotation
  Vec3 currentPosition = currentTransform.GetTranslation();
  Quat currentRotation = currentTransform.GetQuaternion().Normalized();
  const bool scaleChanged = !worldScale.IsClose(lastScale);

  // checks if there are external changes to the body position, rotation or scale
  if (didMove(currentPosition, currentRotation, worldPosition, worldRotation) || scaleChanged) {
    wakeBodiesInVicinity(collider->system, *collider->bodyPtr);
  }
}

ECS_DTOR(PhysicsCollider, data, { freeCollider(data); });

ECS_MOVE(PhysicsCollider, dst, src, {
  freeCollider(dst);
  std::memcpy(dst, src, sizeof(PhysicsCollider));
  src->bodyPtr = nullptr;  // Destination now owns the body.
});

ECS_COPY(PhysicsCollider, dst, src, {
  freeCollider(dst);
  std::memcpy(dst, src, sizeof(PhysicsCollider));
  dst->bodyPtr = nullptr;  // Signal that the destination needs to initialize its own body.
});

PhysicsContext *initPhysicsContext(
  ecs_world_t *world,
  ecs_entity_t positionComponent,
  ecs_entity_t scaleComponent,
  ecs_entity_t rotationComponent) {

  PhysicsContext *ctx = new PhysicsContext;

  ctx->positionComponent = positionComponent;
  ctx->scaleComponent = scaleComponent;
  ctx->rotationComponent = rotationComponent;

  // Set up collider component
  {
    // TODO(Dale): Make physics collider component inheritable
    ECS_COMPONENT(world, PhysicsCollider);
    ecs_type_hooks_t colliderHooks = {
      .ctor = ecs_ctor(PhysicsCollider),
      .dtor = ecs_dtor(PhysicsCollider),
      .copy = ecs_copy(PhysicsCollider),
      .move = ecs_move(PhysicsCollider),
    };

    ecs_set_hooks_id(world, ecs_id(PhysicsCollider), &colliderHooks);

    ctx->colliderComponent = ecs_id(PhysicsCollider);
  }

  // Set up query
  {
    ecs_entity_t components[] = {
      ctx->colliderComponent,
      positionComponent,
      rotationComponent,
    };

    int component_count = sizeof(components) / sizeof(components[0]);

    ecs_query_desc_t desc = {};

    for (auto i = 0; i < component_count; ++i) {
      desc.terms[i] = ecs_term_t{.id = components[i]};
    }

    // Note(Dale): Add EcsDisabled to the query to allow for removal of colliders when disabled
    desc.terms[component_count] = ecs_term_t{.id = EcsDisabled, .oper = EcsOptional};

    ctx->colliderQuery = ecs_query_init(world, &desc);
  }

  // Jolt Physics initialization
  // this is necessary to set up the Jolt factory and register types
  RegisterDefaultAllocator();
  Factory::sInstance = new Factory();
  RegisterTypes();

  // Allocate Jolt objects on heap to handle lifetime management (10 MB)
  ctx->tempAllocator = new TempAllocatorImpl(10 * 1024 * 1024);
  ctx->jobSystem = new JobSystemSingleThreaded(cMaxPhysicsJobs);

  ctx->broadPhaseLayerInterface = new BPLayerInterfaceImpl();
  ctx->objectBroadPhaseLayerFilter = new ObjectVsBroadPhaseLayerFilterImpl();
  ctx->objectLayerPairFilter = new ObjectLayerPairFilterImpl();

  ctx->system = new PhysicsSystem();
  ctx->system->Init(
    MAX_BODY_COUNT,
    0,
    MAX_BODY_PAIRS,
    MAX_CONTACT_CONSTRAINTS,
    *ctx->broadPhaseLayerInterface,
    *ctx->objectBroadPhaseLayerFilter,
    *ctx->objectLayerPairFilter);

  ctx->system->SetCombineRestitution(
    [](const Body &body1, const SubShapeID &, const Body &body2, const SubShapeID &) -> float {
      float restitution1 = body1.GetRestitution();
      float restitution2 = body2.GetRestitution();

      if (restitution1 >= 0.0f && restitution2 >= 0.0f) {
        return JPH::max(restitution1, restitution2);
      }

      if (restitution1 < 0.0f && restitution2 < 0.0f) {
        return 0.0f;
      }

      if (restitution1 < 0.0f) {
        return restitution2 * (1.0f + restitution1);
      } else {
        return restitution1 * (1.0f + restitution2);
      }
    });

  // We don't need to lock the body interface for the physics system when running on a single thread
  ctx->bodyInterface = &ctx->system->GetBodyInterfaceNoLock();
  ContactListener *contactListener = new ContactListenerImpl(ctx);
  ctx->system->SetContactListener(contactListener);

  // Set the gravity to a default value -9.81f in the Y direction
  ctx->system->SetGravity(Vec3(0, DEFAULT_GRAVITY, 0));

  return ctx;
}

void cleanupPhysicsContext(PhysicsContext *ctx) {
  if (ctx == nullptr) {
    return;
  }

  // Clean up custom shapes
  ctx->customShapes.clear();

  // Clean up body metadata
  for (auto &pair : ctx->bodyMetadata) {
    delete pair.second;
  }
  ctx->bodyMetadata.clear();

  // Get and clean up contact listener before destroying the system
  ContactListener *contactListener = ctx->system->GetContactListener();
  ctx->system->SetContactListener(nullptr);
  delete contactListener;

  // Clean up physics system and related objects
  delete ctx->system;
  delete ctx->objectLayerPairFilter;
  delete ctx->objectBroadPhaseLayerFilter;
  delete ctx->broadPhaseLayerInterface;
  delete ctx->jobSystem;
  delete ctx->tempAllocator;

  // Clean up Jolt factory
  delete Factory::sInstance;
  Factory::sInstance = nullptr;

  delete ctx;
}

EMotionType getMotionTypeFromCollider(collider_type_t colliderType) {
  // note(alan): As of now, all bodies are candidates to be static, dynamic or kinematic.
  if (colliderType == COLLIDER_STATIC) {
    return EMotionType::Static;
  } else if (colliderType == COLLIDER_DYNAMIC) {
    return EMotionType::Dynamic;
  } else if (colliderType == COLLIDER_KINEMATIC) {
    return EMotionType::Kinematic;
  }
  return EMotionType::Static;
}

ObjectLayer getObjectLayerFromCollider(collider_type_t colliderType) {
  // Kinematic and dynamic bodies are both in MOVING layer to enable collision events
  return (colliderType == COLLIDER_DYNAMIC || colliderType == COLLIDER_KINEMATIC)
    ? Layers::MOVING
    : Layers::NON_MOVING;
}

// Handles locking rotation and position of a collider.
// should be very cheap to run this, we can think of it as obj->allowedDOFs
EAllowedDOFs calculateAllowedDoFs(PhysicsCollider *collider) {
  auto allowedDofs = EAllowedDOFs::None;

  if (!collider->lockXPosition) {
    allowedDofs |= EAllowedDOFs::TranslationX;
  }
  if (!collider->lockYPosition) {
    allowedDofs |= EAllowedDOFs::TranslationY;
  }
  if (!collider->lockZPosition) {
    allowedDofs |= EAllowedDOFs::TranslationZ;
  }
  if (!collider->lockXAxis) {
    allowedDofs |= EAllowedDOFs::RotationX;
  }
  if (!collider->lockYAxis) {
    allowedDofs |= EAllowedDOFs::RotationY;
  }
  if (!collider->lockZAxis) {
    allowedDofs |= EAllowedDOFs::RotationZ;
  }

  return allowedDofs;
}

collider_type_t getEffectiveColliderType(collider_type_t colliderType, EAllowedDOFs allowedDofs) {
  if (colliderType == COLLIDER_DYNAMIC && allowedDofs != EAllowedDOFs::None) {
    return COLLIDER_DYNAMIC;
  } else if (
    (colliderType == COLLIDER_DYNAMIC && allowedDofs == EAllowedDOFs::None)
    || colliderType == COLLIDER_KINEMATIC) {
    return COLLIDER_KINEMATIC;
  }
  return COLLIDER_STATIC;
}

bool bodyNeedsUpdate(
  PhysicsContext *ctx, PhysicsCollider *collider, Vec3 worldScale, ecs_entity_t eid) {
  if (ctx->bodyMetadata.find(eid) == ctx->bodyMetadata.end()) {
    return false;
  }
  BodyMetadata *meta = ctx->bodyMetadata[eid];
  Vec3 scaleDifference = meta->lastScale - worldScale;
  bool scaleChanged = !scaleDifference.IsNearZero();
  const auto allowedDofs = calculateAllowedDoFs(collider);
  const auto effectiveColliderType = getEffectiveColliderType(collider->type, allowedDofs);

  return (
    meta->lastWidth != collider->width || meta->lastHeight != collider->height
    || meta->lastDepth != collider->depth || meta->lastRadius != collider->radius
    || meta->lastMass != collider->mass || meta->lastType != effectiveColliderType
    || meta->lastShape != collider->shape || meta->lastGravityFactor != collider->gravityFactor
    || meta->lastLinearDamping != collider->linearDamping
    || meta->lastFriction != collider->friction
    || meta->lastAngularDamping != collider->angularDamping || meta->lastDofs != allowedDofs
    || meta->lastEventOnly != collider->eventOnly || scaleChanged);
}

// returns true if the gravity was set or is the same value,
// false if the context is null thus no action was taken
bool setWorldGravity(PhysicsContext *ctx, float gravity) {
  if (ctx == nullptr) {
    return false;
  }
  // Update world gravity in the physics system
  ctx->system->SetGravity(Vec3(0, -gravity, 0));
  return true;
}

float getWorldGravity(PhysicsContext *ctx) {
  if (ctx == nullptr) {
    return 0.0f;
  }

  return ctx->system->GetGravity().GetY();
}

Vec3 transformColliderOffset(const Vec3 &offset, const Quat &rotation, const Vec3 &scale) {
  // Transform the offset by the entity's scale and rotation to get the world-space offset
  Vec3 scaledOffset = offset * scale;
  // Apply rotation to the scaled offset
  return rotation * scaledOffset;
}

Mat44 getEcsLocalTransform(ecs_world_t *world, PhysicsContext *ctx, ecs_entity_t entity) {
  auto position = static_cast<const Position *>(ecs_get_id(world, entity, ctx->positionComponent));
  auto scale = static_cast<const Scale *>(ecs_get_id(world, entity, ctx->scaleComponent));
  auto quaternion =
    static_cast<const Rotation *>(ecs_get_id(world, entity, ctx->rotationComponent));

  Quat quat(quaternion->x, quaternion->y, quaternion->z, quaternion->w);
  Vec3 pos(position->x, position->y, position->z);
  Vec3 scaleVec(scale->x, scale->y, scale->z);

  Mat44 rotationTranslation = Mat44::sRotationTranslation(quat, pos);
  return rotationTranslation * Mat44::sScale(scaleVec);
}

Mat44 getEcsWorldTransform(ecs_world_t *world, PhysicsContext *ctx, ecs_entity_t entity) {
  ecs_entity_t currentEntity = entity;
  Mat44 worldMatrix = Mat44::sIdentity();

  while (currentEntity != 0) {
    worldMatrix = getEcsLocalTransform(world, ctx, currentEntity) * worldMatrix;
    currentEntity = ecs_get_parent(world, currentEntity);
  }
  return worldMatrix;
}

bool isCoreShapeOutdated(PhysicsCollider *collider, BodyMetadata *meta) {
  return meta->lastHeight != collider->height || meta->lastWidth != collider->width
    || meta->lastDepth != collider->depth || meta->lastRadius != collider->radius
    || meta->lastShape != collider->shape;
}

void setBodyMetadata(
  PhysicsContext *ctx, PhysicsCollider *collider, ecs_entity_t eid, Vec3 worldScale) {

  if (ctx->bodyMetadata.find(eid) == ctx->bodyMetadata.end()) {
    ctx->bodyMetadata[eid] = new BodyMetadata();
  }

  BodyMetadata *meta = ctx->bodyMetadata.at(eid);
  const auto allowedDofs = calculateAllowedDoFs(collider);
  const auto effectiveColliderType = getEffectiveColliderType(collider->type, allowedDofs);

  meta->lastWidth = collider->width;
  meta->lastHeight = collider->height;
  meta->lastDepth = collider->depth;
  meta->lastRadius = collider->radius;
  meta->lastMass = collider->mass;
  meta->lastType = effectiveColliderType;
  meta->lastShape = collider->shape;
  meta->lastGravityFactor = collider->gravityFactor;
  meta->lastScale = worldScale;
  meta->lastFriction = collider->friction;
  meta->lastLinearDamping = collider->linearDamping;
  meta->lastAngularDamping = collider->angularDamping;
  meta->lastDofs = allowedDofs;
  meta->lastEventOnly = collider->eventOnly;
}

void clearBodyMetadata(PhysicsContext *ctx, ecs_entity_t eid) {
  if (ctx->bodyMetadata.find(eid) != ctx->bodyMetadata.end()) {
    delete ctx->bodyMetadata[eid];
    ctx->bodyMetadata.erase(eid);
  }
}

Shape *createShapeFromCollider(PhysicsContext *ctx, PhysicsCollider *collider, Vec3 worldScale) {

  JPH::ShapeSettings::ShapeResult shapeResult;
  const Shape *shape = nullptr;
  switch (collider->shape) {
    case BOX_SHAPE: {
      BoxShapeSettings boxShape(Vec3(collider->width, collider->height, collider->depth) / 2);
      shape = new BoxShape(boxShape, shapeResult);
      break;
    }
    case SPHERE_SHAPE: {
      SphereShapeSettings sphereShape(collider->radius);
      shape = new SphereShape(sphereShape, shapeResult);
      break;
    }
    case PLANE_SHAPE: {
      // Create a flat quad in the X-Y plane to represent a plane shape
      Vec3 internalScale(collider->width, collider->height, 1.0f);
      Array<Vec3> points;
      points.push_back(Vec3(-0.5f, -0.5f, 0.0f) * internalScale);
      points.push_back(Vec3(-0.5f, 0.5f, 0.0f) * internalScale);
      points.push_back(Vec3(0.5f, 0.5f, 0.0f) * internalScale);
      points.push_back(Vec3(0.5f, -0.5f, 0.0f) * internalScale);
      ConvexHullShapeSettings planeShape(points);
      shape = new ConvexHullShape(planeShape, shapeResult);
      break;
    }
    case CONE_SHAPE: {
      Array<Vec3> points;
      const float radius = collider->radius;
      const float halfHeight = collider->height / 2.0f;

      points.reserve(CIRCULAR_SHAPE_SEGMENTS + 2);

      // Apex
      points.push_back(Vec3(0.0f, halfHeight, 0.0f));

      // Base center
      points.push_back(Vec3(0.0f, -halfHeight, 0.0f));

      // Points around the base circle in XZ plane
      for (int i = 0; i < CIRCULAR_SHAPE_SEGMENTS; i++) {
        float angle = (float)i / CIRCULAR_SHAPE_SEGMENTS * 2.0f * M_PI;
        float x = cos(angle) * radius;
        float z = sin(angle) * radius;
        points.push_back(Vec3(x, -halfHeight, z));
      }

      ConvexHullShapeSettings coneShape(points);
      shape = new ConvexHullShape(coneShape, shapeResult);
      break;
    }
    case CYLINDER_SHAPE: {
      CylinderShapeSettings cylinderShape(collider->height / 2, collider->radius);
      shape = new CylinderShape(cylinderShape, shapeResult);
      break;
    }
    case CAPSULE_SHAPE: {
      CapsuleShapeSettings capsuleShape(collider->height / 2, collider->radius);
      shape = new CapsuleShape(capsuleShape, shapeResult);
      break;
    }
    case CIRCLE_SHAPE: {
      Array<Vec3> points;
      // NOTE(coco): 32 is the default radial segments in a circle shape
      for (int i = 0; i < CIRCULAR_SHAPE_SEGMENTS; i++) {
        float angle = (float)i / CIRCULAR_SHAPE_SEGMENTS * M_PI * 2.0f;
        points.push_back(Vec3(cos(angle) * collider->radius, sin(angle) * collider->radius, 0.0f));
      }
      ConvexHullShapeSettings circleShape(points);
      shape = new ConvexHullShape(circleShape, shapeResult);
      break;
    }
    default: {
      // Custom convex hull shape
      collider_shape_t id = collider->shape;
      if (ctx->customShapes.find(id) != ctx->customShapes.end()) {
        shape = ctx->customShapes[id];
      }
    }
  }
  if (shape == nullptr) {
    return nullptr;
  }

  Vec3 validScale = shape->MakeScaleValid(worldScale);
  ScaledShape *scaledShape = new ScaledShape(shape, validScale);

  if (scaledShape == nullptr) {
    return nullptr;
  }

  return scaledShape;
}

void updateMotionProperties(
  const PhysicsCollider *collider, Body *body, EAllowedDOFs allowedDofs, bool massPropsChanged) {
  auto *motionProps = body->GetMotionProperties();
  motionProps->SetLinearDamping(collider->linearDamping);
  motionProps->SetAngularDamping(collider->angularDamping);
  motionProps->SetGravityFactor(collider->gravityFactor);

  // Update allowed DOFs and mass properties if they changed
  const bool updateAllowedDofs = allowedDofs != motionProps->GetAllowedDOFs();

  if (updateAllowedDofs || massPropsChanged) {
    MassProperties massProps;
    massProps.mMass = collider->mass;
    motionProps->SetMassProperties(allowedDofs, massProps);
  }
}

void updateRigidBody(
  PhysicsContext *ctx, PhysicsCollider *collider, ecs_entity_t eid, Vec3 worldScale) {
  BodyMetadata *meta = ctx->bodyMetadata.at(eid);
  Body *body = ctx->system->GetBodyLockInterfaceNoLock().TryGetBody(*collider->bodyPtr);
  if (meta == nullptr || body == nullptr) {
    return;
  }

  const auto allowedDofs = calculateAllowedDoFs(collider);
  const auto effectiveColliderType = getEffectiveColliderType(collider->type, allowedDofs);
  // Update body type first
  if (meta->lastType != effectiveColliderType) {
    ctx->bodyInterface->SetMotionType(
      *collider->bodyPtr, getMotionTypeFromCollider(effectiveColliderType), EActivation::Activate);
    ctx->bodyInterface->SetObjectLayer(
      *collider->bodyPtr, getObjectLayerFromCollider(collider->type));
  }

  const bool scaleChanged = !Vec3(meta->lastScale - worldScale).IsNearZero();
  const bool isCoreShapeOutdatedFlag = isCoreShapeOutdated(collider, meta);
  if (isCoreShapeOutdatedFlag) {
    ctx->bodyInterface->RemoveBody(*collider->bodyPtr);
    const auto *newScaledShape = createShapeFromCollider(ctx, collider, worldScale);

    // Shape updates are handled by the body interface since they affect broad-phase
    ctx->bodyInterface->SetShape(*collider->bodyPtr, newScaledShape, false, EActivation::Activate);
    ctx->bodyInterface->AddBody(*collider->bodyPtr, EActivation::Activate);
  } else if (scaleChanged) {
    // Get the current shape and update its scale
    auto currentShape = ctx->bodyInterface->GetShape(*collider->bodyPtr);

    // All shapes are ScaledShapes, we use the original shape to avoid nesting ScaledShapes classes
    const auto *scaledShape = static_cast<const ScaledShape *>(currentShape.GetPtr());
    const auto *innerShape = scaledShape->GetInnerShape();
    Vec3 validScale = innerShape->MakeScaleValid(worldScale);
    ctx->bodyInterface->SetShape(
      *collider->bodyPtr, new ScaledShape(innerShape, validScale), false, EActivation::Activate);
  }

  // Friction and restitution
  ctx->bodyInterface->SetFriction(*collider->bodyPtr, collider->friction);
  ctx->bodyInterface->SetRestitution(*collider->bodyPtr, collider->restitution);

  // Event only colliders are sensors
  body->SetIsSensor(collider->eventOnly);

  if (effectiveColliderType == COLLIDER_DYNAMIC) {
    const bool massPropsChanged =
      isCoreShapeOutdatedFlag || scaleChanged || meta->lastMass != collider->mass;
    updateMotionProperties(collider, body, allowedDofs, massPropsChanged);
  }

  // Note: when one of these properties change, we need to wake up bodies in vicinity
  // because they might be affected by this
  const bool environmentalChange =
    collider->friction < meta->lastFriction || collider->eventOnly != meta->lastEventOnly;

  if (
    (environmentalChange || collider->gravityFactor != meta->lastGravityFactor)
    && effectiveColliderType == COLLIDER_DYNAMIC) {
    // activating the current body will handle activating bodies in vicinity
    ctx->bodyInterface->ActivateBody(*collider->bodyPtr);
  } else if (environmentalChange) {
    wakeBodiesInVicinity(collider->system, *collider->bodyPtr);
  }

  setBodyMetadata(ctx, collider, eid, worldScale);
}

void initPhysicsCollider(
  ecs_world_t *world, PhysicsContext *ctx, ecs_entity_t eid, PhysicsCollider *collider) {

  // Initial Transforms
  Mat44 worldTransform = getEcsWorldTransform(world, ctx, eid);
  Vec3 worldScale;
  Mat44 rotationMatrix = worldTransform.Decompose(worldScale);
  Vec3 worldPosition = worldTransform.GetTranslation();
  Quat worldRotation = rotationMatrix.GetQuaternion();

  const auto *scaledShape = createShapeFromCollider(ctx, collider, worldScale);
  if (scaledShape == nullptr) {
    return;
  }

  const auto allowedDofs = calculateAllowedDoFs(collider);
  const auto effectiveColliderType = getEffectiveColliderType(collider->type, allowedDofs);
  // Use decomposed transform for initial position and rotation
  BodyCreationSettings bodySettings(
    scaledShape,
    worldPosition,
    worldRotation,
    getMotionTypeFromCollider(effectiveColliderType),
    getObjectLayerFromCollider(collider->type));
  bodySettings.mAllowSleeping = true;
  bodySettings.mLinearDamping = collider->linearDamping;
  bodySettings.mAngularDamping = collider->angularDamping;
  bodySettings.mFriction = collider->friction;
  bodySettings.mRestitution = collider->restitution;
  bodySettings.mGravityFactor = collider->gravityFactor;
  bodySettings.mAllowDynamicOrKinematic = true;
  bodySettings.mIsSensor = collider->eventOnly;
  bodySettings.mCollideKinematicVsNonDynamic = true;
  bodySettings.mUserData = eid;

  // Set ccd for high precision colliders
  // sensor can't have ccd
  bodySettings.mMotionQuality =
    (effectiveColliderType == COLLIDER_DYNAMIC && collider->highPrecision && !collider->eventOnly)
    ? EMotionQuality::LinearCast
    : EMotionQuality::Discrete;

  // note: (Alan) IMPORTANT Set mass properties, unused if the body is static or kinematic
  // but needs to be set regardless otherwise we will face undefined behavior when we update the
  // body to dynamic
  bodySettings.mMassPropertiesOverride.mMass = collider->mass;

  if (effectiveColliderType == COLLIDER_DYNAMIC) {
    bodySettings.mOverrideMassProperties = EOverrideMassProperties::CalculateInertia;
    // Set degrees of freedom based on lock properties
    bodySettings.mAllowedDOFs = allowedDofs;
  }
  BodyID bodyId =
    ctx->system->GetBodyInterface().CreateAndAddBody(bodySettings, EActivation::Activate);
  collider->bodyPtr = new BodyID(bodyId);

  // set the current system
  collider->system = ctx->system;

  // Set the body metadata for the collider
  setBodyMetadata(ctx, collider, eid, worldScale);
}

void syncPhysicsToWorld(ecs_world_t *world, PhysicsContext *ctx) {
  ecs_iter_t iter = ecs_query_iter(world, ctx->colliderQuery);

  while (ecs_query_next(&iter)) {
    auto *colliderPtr = ecs_field(&iter, PhysicsCollider, 0);
    auto *positionPtr = ecs_field(&iter, Position, 1);
    auto *rotationPtr = ecs_field(&iter, Rotation, 2);
    for (int i = 0; i < iter.count; i++) {
      PhysicsCollider *collider = colliderPtr + i;
      flecs::entity_t eid = iter.entities[i];
      const bool isReady =
        collider->bodyPtr != nullptr && ctx->bodyInterface->IsAdded(*collider->bodyPtr);

      if (isReady && collider->type == COLLIDER_DYNAMIC) {
        // Get world transform from physics body
        auto *body = ctx->system->GetBodyLockInterfaceNoLock().TryGetBody(*collider->bodyPtr);
        Vec3 physicsPosition = body->GetPosition();
        Quat physicsRotation = body->GetRotation();
        Vec3 worldScale = ctx->bodyMetadata.at(eid)->lastScale;

        Vec3 offsets = Vec3(collider->offsetX, collider->offsetY, collider->offsetZ);
        Quat rotationOffset = Quat(
          collider->offsetQuaternionX,
          collider->offsetQuaternionY,
          collider->offsetQuaternionZ,
          collider->offsetQuaternionW);
        Vec3 entityWorldPosition = physicsPosition;
        Quat entityWorldRotation = physicsRotation;

        if (!rotationOffset.IsClose(Quat::sIdentity())) {
          entityWorldRotation = physicsRotation * rotationOffset.Conjugated();
        }

        if (!offsets.IsNearZero()) {
          Vec3 transformedOffsets =
            transformColliderOffset(offsets, entityWorldRotation, worldScale);
          entityWorldPosition = physicsPosition - transformedOffsets;
        }

        // create entity world transform matrix based on the shape rotation and scaled offset
        Mat44 worldTransform =
          Mat44::sRotationTranslation(entityWorldRotation, entityWorldPosition);

        ecs_entity_t parent = ecs_get_parent(world, eid);
        if (parent != 0) {
          Mat44 parentTransform = getEcsWorldTransform(world, ctx, parent);
          Mat44 inverseParent = parentTransform.Inversed();
          // NOTE(christoph): If the object has a parent, we multiply by the inverse of the parent
          // transform, giving us the local transform of the current object. This is necessary
          // because the position/scale/quaternion components are all in local space.
          worldTransform = inverseParent * worldTransform;
        }

        Vec3 position(worldTransform(0, 3), worldTransform(1, 3), worldTransform(2, 3));

        positionPtr[i].x = position.GetX();
        positionPtr[i].y = position.GetY();
        positionPtr[i].z = position.GetZ();

        Quat q = worldTransform.GetQuaternion();
        rotationPtr[i].x = q.GetX();
        rotationPtr[i].y = q.GetY();
        rotationPtr[i].z = q.GetZ();
        rotationPtr[i].w = q.GetW();

        if (ctx->bodyInterface->IsActive(*collider->bodyPtr)) {
          // NOTE(christoph): Mark the position/rotation as modified to notify observers
          ecs_modified_id(world, eid, ctx->positionComponent);
          ecs_modified_id(world, eid, ctx->rotationComponent);
        }
      }
    }
  }
}

void syncWorldToPhysics(ecs_world_t *world, PhysicsContext *ctx, float deltaTime) {
  ecs_iter_t iter = ecs_query_iter(world, ctx->colliderQuery);

  while (ecs_query_next(&iter)) {
    auto *colliderPtr = ecs_field(&iter, PhysicsCollider, 0);
    for (int i = 0; i < iter.count; i++) {
      PhysicsCollider *collider = colliderPtr + i;
      ecs_entity_t eid = iter.entities[i];
      // Note(Dale): Checking if EcsDisabled is set
      if (ecs_field_is_set(&iter, ECS_DISABLED_INDEX)) {
        freeCollider(collider);
        clearBodyMetadata(ctx, eid);
        continue;
      }

      // Sync Transform
      Vec3 worldScale;
      const Mat44 transform = getEcsWorldTransform(world, ctx, eid).Decompose(worldScale);
      Quat worldRotation = transform.GetQuaternion();
      Vec3 worldPosition = transform.GetTranslation();

      Vec3 offsets(collider->offsetX, collider->offsetY, collider->offsetZ);
      Quat rotationOffset(
        collider->offsetQuaternionX,
        collider->offsetQuaternionY,
        collider->offsetQuaternionZ,
        collider->offsetQuaternionW);

      if (!offsets.IsNearZero()) {
        Vec3 localOffsets = transformColliderOffset(offsets, worldRotation, worldScale);
        worldPosition = worldPosition + localOffsets;
      }

      if (!rotationOffset.IsClose(Quat::sIdentity())) {
        worldRotation = worldRotation * rotationOffset;
      }

      if (collider->bodyPtr == nullptr) {
        initPhysicsCollider(world, ctx, eid, collider);
      } else {
        maybeWakeBodiesInVicinity(ctx, collider, worldRotation, worldPosition, worldScale, eid);
        if (bodyNeedsUpdate(ctx, collider, worldScale, eid)) {
          updateRigidBody(ctx, collider, eid, worldScale);
        }
      }

      if (collider->type == COLLIDER_KINEMATIC) {
        // Kinematic bodies should not be moved directly, they should be updated through
        // MoveKinematic to ensure proper physics interactions.
        ctx->system->GetBodyInterface().MoveKinematic(
          *collider->bodyPtr, worldPosition, worldRotation, deltaTime);
      } else {
        ctx->system->GetBodyInterface().SetPositionAndRotationWhenChanged(
          *collider->bodyPtr, worldPosition, worldRotation, EActivation::Activate);
      }

      if (collider->type == COLLIDER_DYNAMIC) {
        // Note(Dale): If the entity is inheriting the Position/Rotation components, calling
        // ecs_add_id will remove the inheritance relationship.
        ecs_add_id(world, iter.entities[i], ctx->positionComponent);
        ecs_add_id(world, iter.entities[i], ctx->rotationComponent);
      }
    }
  }
}

void detectCollisionEnds(ecs_world_t *world, PhysicsContext *ctx) {
  // Note(Dale): Detect collision ends by finding pairs that were active last frame but not
  // this frame
  for (const auto &pair : ctx->previousFrameCollisions) {
    if (ctx->activeCollisions.count(pair.first) == 0) {
      ctx->collisionEnds.push_back(pair.first.lowerId);
      ctx->collisionEnds.push_back(pair.first.higherId);
    }
  }

  std::swap(ctx->previousFrameCollisions, ctx->activeCollisions);
  ctx->activeCollisions.clear();
}

void stepPhysics(ecs_world_t *world, PhysicsContext *ctx, float deltaTime) {
  // NOTE(Christoph): deltaTime is in milliseconds, if we're running at > 1000 fps something is
  // horribly wrong.
  if (deltaTime < 1) {
    return;
  }
  float deltaTimeInSeconds = deltaTime / 1000;

  // Clear previous frame's collision events before processing new ones
  ctx->collisionStarts.clear();
  ctx->collisionEnds.clear();

  // Note: We should explore the idea of a prePhysicsUpdate where we apply all the rigid body
  // updates before we step the physics system.
  syncWorldToPhysics(world, ctx, deltaTimeInSeconds);
  ctx->system->Update(deltaTimeInSeconds, 1, ctx->tempAllocator, ctx->jobSystem);
  syncPhysicsToWorld(world, ctx);
  detectCollisionEnds(world, ctx);
}

// returns the valid body pointer if the collider is present and initialized, nullptr otherwise
BodyID *ensureBodyInitialized(ecs_world_t *world, PhysicsContext *ctx, ecs_entity_t entity) {
  auto collider =
    static_cast<const PhysicsCollider *>(ecs_get_id(world, entity, ctx->colliderComponent));

  if (collider == nullptr) {
    return nullptr;
  }

  if (collider->bodyPtr == nullptr) {
    initPhysicsCollider(world, ctx, entity, const_cast<PhysicsCollider *>(collider));
  }

  return collider->bodyPtr;
}

// returns true if velocity was set, false if the collider is not present thus no action was taken
bool setLinearVelocity(
  ecs_world_t *world,
  PhysicsContext *ctx,
  ecs_entity_t entity,
  float velocityX,
  float velocityY,
  float velocityZ) {
  auto bodyId = ensureBodyInitialized(world, ctx, entity);
  if (bodyId != nullptr) {
    ctx->bodyInterface->SetLinearVelocity(*bodyId, Vec3(velocityX, velocityY, velocityZ));
    return true;
  }
  return false;
}

float *getLinearVelocity(ecs_world_t *world, PhysicsContext *ctx, ecs_entity_t entity) {
  auto bodyId = ensureBodyInitialized(world, ctx, entity);
  if (bodyId != nullptr) {
    // NOTE(christoph): This pointer is freed on the Javascript side
    float *velocityPtr = new float[3];
    Vec3 velocity = ctx->bodyInterface->GetLinearVelocity(*bodyId);
    velocityPtr[0] = velocity.GetX();
    velocityPtr[1] = velocity.GetY();
    velocityPtr[2] = velocity.GetZ();
    return velocityPtr;
  }
  return nullptr;
}

// returns true if velocity was set, false if the collider is not present thus no action was taken
bool setAngularVelocity(
  ecs_world_t *world,
  PhysicsContext *ctx,
  ecs_entity_t entity,
  float velocityX,
  float velocityY,
  float velocityZ) {
  auto bodyId = ensureBodyInitialized(world, ctx, entity);
  if (bodyId != nullptr) {
    ctx->bodyInterface->SetAngularVelocity(*bodyId, Vec3(velocityX, velocityY, velocityZ));
    return true;
  }
  return false;
}

float *getAngularVelocity(ecs_world_t *world, PhysicsContext *ctx, ecs_entity_t entity) {
  auto bodyId = ensureBodyInitialized(world, ctx, entity);
  if (bodyId != nullptr) {
    // NOTE(christoph): This pointer is freed on the Javascript side
    float *velocityPtr = new float[3];
    Vec3 velocity = ctx->bodyInterface->GetAngularVelocity(*bodyId);
    velocityPtr[0] = velocity.GetX();
    velocityPtr[1] = velocity.GetY();
    velocityPtr[2] = velocity.GetZ();
    return velocityPtr;
  }
  return nullptr;
}

// returns true if torque was applied, false if the collider is not present thus no action was taken
bool applyCentralForce(
  ecs_world_t *world,
  PhysicsContext *ctx,
  ecs_entity_t entity,
  float forceX,
  float forceY,
  float forceZ) {
  auto bodyId = ensureBodyInitialized(world, ctx, entity);
  if (bodyId != nullptr) {
    ctx->bodyInterface->AddForce(*bodyId, Vec3(forceX, forceY, forceZ));
    return true;
  }
  return false;
}

// returns true if torque was applied, false if the collider is not present thus no action was taken
bool applyTorque(
  ecs_world_t *world,
  PhysicsContext *ctx,
  ecs_entity_t entity,
  float torqueX,
  float torqueY,
  float torqueZ) {
  auto bodyId = ensureBodyInitialized(world, ctx, entity);
  if (bodyId != nullptr) {
    ctx->bodyInterface->AddTorque(*bodyId, Vec3(torqueX, torqueY, torqueZ));
    return true;
  }
  return false;
}

// returns true if impulse was applied, false if the collider is not present thus no action was
// taken
bool applyImpulse(
  ecs_world_t *world,
  PhysicsContext *ctx,
  ecs_entity_t entity,
  float impulseX,
  float impulseY,
  float impulseZ) {
  auto bodyId = ensureBodyInitialized(world, ctx, entity);
  if (bodyId != nullptr) {
    ctx->bodyInterface->AddImpulse(*bodyId, Vec3(impulseX, impulseY, impulseZ));
    return true;
  }
  return false;
}

collider_shape_t registerConvexShape(PhysicsContext *ctx, float *verticesPtr, int verticesLength) {
  if (verticesLength < 3) {
    return -1;
  }

  // set the id to be the next available id
  collider_shape_t id = ctx->customShapesCounter++;
  Array<Vec3> points;
  for (int i = 0; i < verticesLength; i += 3) {
    // do not calculate aabb on every add point
    points.push_back(Vec3(verticesPtr[i], verticesPtr[i + 1], verticesPtr[i + 2]));
  }

  ConvexHullShapeSettings convexHullShape(points);

  JPH::ShapeSettings::ShapeResult shapeResult;
  auto shape = new ConvexHullShape(convexHullShape, shapeResult);

  if (shapeResult.HasError()) {
    delete shape;
    return 0;
  }

  ctx->customShapes[id] = shape;

  return id;
}

Array<Array<Vec3>> decomposeTrianglesIntoConvexHulls(
  float *verticesPtr,
  int verticesLength,
  uint32_t *indicesPtr,
  int indicesLength,
  int maxTrianglesPerHull = 16) {
  Array<Array<Vec3>> hulls;

  if (
    verticesLength < MIN_VERTICES_FOR_COMPOUND_SHAPE
    || indicesLength < MIN_INDICES_FOR_COMPOUND_SHAPE || verticesLength % 3 != 0
    || indicesLength % 3 != 0) {
    return hulls;
  }

  Array<Vec3> allVertices(verticesLength / 3);
  for (int i = 0; i < verticesLength; i += 3) {
    allVertices[i / 3] = Vec3(verticesPtr[i], verticesPtr[i + 1], verticesPtr[i + 2]);
  }

  // Note/TODO (Dale): Triangle-based decomposition: group triangles into convex hulls.
  // Use a more sophisticated convex decomposition algorithm like V-HACD.
  int totalTriangles = indicesLength / 3;
  int numHulls =
    (totalTriangles + maxTrianglesPerHull - 1) / maxTrianglesPerHull;  // Ceiling division

  for (int hullIndex = 0; hullIndex < numHulls; hullIndex++) {
    Array<Vec3> hullVertices;

    int startTriangle = hullIndex * maxTrianglesPerHull;
    int endTriangle = std::min(startTriangle + maxTrianglesPerHull, totalTriangles);

    for (int triIdx = startTriangle; triIdx < endTriangle; triIdx++) {
      int baseIdx = triIdx * 3;
      for (int vertIdx = 0; vertIdx < 3; vertIdx++) {
        uint32_t index = indicesPtr[baseIdx + vertIdx];
        if (index >= allVertices.size()) {
          return hulls;
        }
        hullVertices.push_back(allVertices[index]);
      }
    }

    // Note(Dale): Only add if we have enough vertices for a valid convex hull
    if (hullVertices.size() >= 4) {
      hulls.push_back(hullVertices);
    }
  }

  return hulls;
}

collider_shape_t registerCompoundShape(
  PhysicsContext *ctx,
  float *verticesPtr,
  int verticesLength,
  uint32_t *indicesPtr,
  int indicesLength) {
  if (
    verticesLength < MIN_VERTICES_FOR_COMPOUND_SHAPE
    || indicesLength < MIN_INDICES_FOR_COMPOUND_SHAPE) {
    return -1;
  }

  Array<Array<Vec3>> convexHulls =
    decomposeTrianglesIntoConvexHulls(verticesPtr, verticesLength, indicesPtr, indicesLength);

  if (convexHulls.empty()) {
    return -1;
  }

  collider_shape_t id = ctx->customShapesCounter++;
  MutableCompoundShapeSettings compoundSettings;

  for (const auto &hullVertices : convexHulls) {
    ConvexHullShapeSettings convexHullSettings(hullVertices);

    const auto shapeResult = convexHullSettings.Create();

    if (!shapeResult.HasError() && shapeResult.Get() != nullptr) {
      compoundSettings.AddShape(Vec3::sZero(), Quat::sIdentity(), shapeResult.Get());
    }
  }

  const auto finalResult = compoundSettings.Create();

  if (finalResult.HasError() || finalResult.Get() == nullptr) {
    return -1;
  }

  ctx->customShapes[id] = finalResult.Get();
  return id;
}

void unregisterConvexShape(PhysicsContext *ctx, collider_shape_t id) {
  if (ctx->customShapes.find(id) != ctx->customShapes.end()) {
    // No need to delete the shape, as the Ref<> will handle that
    ctx->customShapes.erase(id);
  }
}
