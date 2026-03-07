#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":ecs-js-test",
    "//c8:quaternion",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x6379b169);

#include "c8/ecs/ecs-js.h"
#include "c8/ecs/physics.h"
#include "c8/quaternion.h"
#include "flecs.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace c8;

// Physics timestep in milliseconds (60 FPS)
constexpr float kDeltaTime = 17.0f;

struct Object {
  uint32_t order;
};

ECS_COMPONENT_DECLARE(Position);
ECS_COMPONENT_DECLARE(Scale);
ECS_COMPONENT_DECLARE(Rotation);
ECS_COMPONENT_DECLARE(Object);
ECS_COMPONENT_DECLARE(PhysicsCollider);

class PhysicsTest : public ::testing::Test {
protected:
  void SetUp() override {
    world = c8EmAsm_createWorld();
    ASSERT_NE(world, nullptr);

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_COMPONENT_DEFINE(world, Scale);
    ECS_COMPONENT_DEFINE(world, Rotation);
    ECS_COMPONENT_DEFINE(world, Object);
  }
  void TearDown() override {
    if (world) {
      // process all the deletions before deleting the world
      c8EmAsm_progressWorld(world, kDeltaTime);
      c8EmAsm_deleteWorld(world);
      world = nullptr;
    }
  }

  void setDefaultCollider(PhysicsCollider *collider) {
    collider->width = 1.0f;
    collider->height = 1.0f;
    collider->depth = 1.0f;
    collider->radius = 0.5f;
    collider->mass = 1.0f;
    collider->linearDamping = 0.1f;
    collider->angularDamping = 0.1f;
    collider->friction = 0.5f;
    collider->restitution = 0.3f;
    collider->gravityFactor = 1.0f;
    collider->offsetX = 0.0f;
    collider->offsetY = 0.0f;
    collider->offsetZ = 0.0f;
    collider->offsetQuaternionX = 0.0f;
    collider->offsetQuaternionY = 0.0f;
    collider->offsetQuaternionZ = 0.0f;
    collider->offsetQuaternionW = 1.0f;
    collider->type = COLLIDER_STATIC;
    collider->shape = BOX_SHAPE;
    collider->eventOnly = false;
    collider->lockXAxis = false;
    collider->lockYAxis = false;
    collider->lockZAxis = false;
    collider->lockXPosition = false;
    collider->lockYPosition = false;
    collider->lockZPosition = false;
    collider->highPrecision = false;
  }

  ecs_entity_t createTestEntity(const Position &position, const Scale &scale) {
    int positionId = ecs_id(Position);
    int scaleId = ecs_id(Scale);
    int rotationId = ecs_id(Rotation);

    ecs_entity_t entity = c8EmAsm_createEntity(world);

    auto *pos = static_cast<Position *>(c8EmAsm_entityStartMutComponent(world, entity, positionId));
    *pos = position;
    c8EmAsm_entityEndMutComponent(world, entity, positionId, true);

    auto *entityScale =
      static_cast<Scale *>(c8EmAsm_entityStartMutComponent(world, entity, scaleId));
    *entityScale = scale;
    c8EmAsm_entityEndMutComponent(world, entity, scaleId, true);

    auto *rot = static_cast<Rotation *>(c8EmAsm_entityStartMutComponent(world, entity, rotationId));
    rot->x = 0.0f;
    rot->y = 0.0f;
    rot->z = 0.0f;
    rot->w = 1.0f;
    c8EmAsm_entityEndMutComponent(world, entity, rotationId, true);

    return entity;
  }

  void verifyScale(
    ecs_entity_t entity,
    float expectedX,
    float expectedY,
    float expectedZ,
    float tolerance = 0.001f) {
    int scaleId = ecs_id(Scale);
    const auto *scale =
      static_cast<const Scale *>(c8EmAsm_entityGetComponent(world, entity, scaleId));
    ASSERT_NE(scale, nullptr);
    EXPECT_FLOAT_EQ(scale->x, expectedX);
    EXPECT_FLOAT_EQ(scale->y, expectedY);
    EXPECT_FLOAT_EQ(scale->z, expectedZ);
  }

  void verifyPosition(
    ecs_entity_t entity,
    float expectedX,
    float expectedY,
    float expectedZ,
    float tolerance = 0.001f) {
    int positionId = ecs_id(Position);
    const auto *position =
      static_cast<const Position *>(c8EmAsm_entityGetComponent(world, entity, positionId));
    ASSERT_NE(position, nullptr);
    EXPECT_FLOAT_EQ(position->x, expectedX);
    EXPECT_FLOAT_EQ(position->y, expectedY);
    EXPECT_FLOAT_EQ(position->z, expectedZ);
  }

  void verifyRotation(
    ecs_entity_t entity,
    float expectedX,
    float expectedY,
    float expectedZ,
    float expectedW,
    float tolerance = 0.00001f) {
    int rotationId = ecs_id(Rotation);
    const auto *rotation =
      static_cast<const Rotation *>(c8EmAsm_entityGetComponent(world, entity, rotationId));
    ASSERT_NE(rotation, nullptr);
    EXPECT_NEAR(rotation->x, expectedX, tolerance);
    EXPECT_NEAR(rotation->y, expectedY, tolerance);
    EXPECT_NEAR(rotation->z, expectedZ, tolerance);
    EXPECT_NEAR(rotation->w, expectedW, tolerance);
  }

  ecs_entity_t createStaticFloor(
    const Position &position = {0.0f, 0.0f, 0.0f}, const Scale &scale = {10.0f, 1.0f, 10.0f}) {
    int colliderId = c8EmAsm_getColliderComponentId(world);
    ecs_entity_t floorEntity = createTestEntity(position, scale);
    EXPECT_NE(floorEntity, 0);

    auto *floorCollider = static_cast<PhysicsCollider *>(
      c8EmAsm_entityStartMutComponent(world, floorEntity, colliderId));
    setDefaultCollider(floorCollider);
    floorCollider->mass = 0.0f;
    floorCollider->type = COLLIDER_STATIC;
    floorCollider->shape = BOX_SHAPE;
    c8EmAsm_entityEndMutComponent(world, floorEntity, colliderId, true);

    EXPECT_TRUE(c8EmAsm_entityHasComponent(world, floorEntity, colliderId));
    return floorEntity;
  }

  ecs_entity_t createDynamicBall(
    const Position &position = {0.0f, 5.0f, 0.0f},
    const Scale &scale = {1.0f, 1.0f, 1.0f},
    float radius = 0.5f) {
    int colliderId = c8EmAsm_getColliderComponentId(world);
    ecs_entity_t ballEntity = createTestEntity(position, scale);
    EXPECT_NE(ballEntity, 0);

    auto *ballCollider = static_cast<PhysicsCollider *>(
      c8EmAsm_entityStartMutComponent(world, ballEntity, colliderId));
    setDefaultCollider(ballCollider);
    ballCollider->mass = 1.0f;
    ballCollider->type = COLLIDER_DYNAMIC;
    ballCollider->shape = SPHERE_SHAPE;
    ballCollider->radius = radius;
    c8EmAsm_entityEndMutComponent(world, ballEntity, colliderId, true);

    EXPECT_TRUE(c8EmAsm_entityHasComponent(world, ballEntity, colliderId));
    return ballEntity;
  }

  void simulatePhysics(int iterations) {
    for (int i = 0; i < iterations; i++) {
      c8EmAsm_progressWorld(world, kDeltaTime);
    }
  }

  void initializeWorldWithStandardComponents() {
    int positionId = ecs_id(Position);
    int scaleId = ecs_id(Scale);
    int rotationId = ecs_id(Rotation);
    int objectId = ecs_id(Object);
    c8EmAsm_initWithComponents(world, positionId, scaleId, rotationId, objectId);
  }

  float letBallSettleAndGetY(ecs_entity_t ballEntity, int iterations = 180) {
    simulatePhysics(iterations);

    int positionId = ecs_id(Position);
    const auto *settledPos =
      static_cast<const Position *>(c8EmAsm_entityGetComponent(world, ballEntity, positionId));
    EXPECT_NE(settledPos, nullptr);
    EXPECT_LE(settledPos->y, 1.5f);
    EXPECT_GE(settledPos->y, 0.5f);

    return settledPos->y;
  }

  void verifyBallDropped(ecs_entity_t ballEntity, float settledY, float minDrop = 0.0f) {
    int positionId = ecs_id(Position);
    const auto *finalPos =
      static_cast<const Position *>(c8EmAsm_entityGetComponent(world, ballEntity, positionId));
    EXPECT_NE(finalPos, nullptr);
    EXPECT_LT(finalPos->y, settledY - minDrop);
  }

  void verifyBodyIsAwake(const ecs_entity_t entity, const bool expected) {
    PhysicsContext *ctx = getPhysicsContext(world);
    ASSERT_NE(ctx, nullptr);

    auto colliderPtr =
      static_cast<const PhysicsCollider *>(ecs_get_id(world, entity, ctx->colliderComponent));
    const auto bodyPtr = *colliderPtr->bodyPtr;
    ASSERT_EQ(ctx->bodyInterface->IsActive(bodyPtr), expected);
  }

  ecs_world_t *world;
};

// Creates a dynamic entity and checks if its position changes after physics simulation
TEST_F(PhysicsTest, CreateDynamicEntity) {
  initializeWorldWithStandardComponents();

  int positionId = ecs_id(Position);

  int colliderId = c8EmAsm_getColliderComponentId(world);

  ecs_entity_t entity = createTestEntity({0.0f, 5.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
  ASSERT_NE(entity, 0);

  auto *collider =
    static_cast<PhysicsCollider *>(c8EmAsm_entityStartMutComponent(world, entity, colliderId));
  setDefaultCollider(collider);
  collider->mass = 1.0f;
  collider->type = COLLIDER_DYNAMIC;
  collider->shape = BOX_SHAPE;
  collider->gravityFactor = 1.0f;
  c8EmAsm_entityEndMutComponent(world, entity, colliderId, true);

  ASSERT_TRUE(c8EmAsm_entityHasComponent(world, entity, colliderId));

  const auto *initialPos =
    static_cast<const Position *>(c8EmAsm_entityGetComponent(world, entity, positionId));
  float initialY = initialPos->y;

  simulatePhysics(60);

  const auto *finalPos =
    static_cast<const Position *>(c8EmAsm_entityGetComponent(world, entity, positionId));
  ASSERT_NE(finalPos, nullptr);
  EXPECT_NE(finalPos->y, initialY);

  c8EmAsm_deleteEntity(world, entity);
}

// Verifies that a dynamic entity has a non-zero velocity after physics simulation
TEST_F(PhysicsTest, TestVelocity) {
  initializeWorldWithStandardComponents();

  int colliderId = c8EmAsm_getColliderComponentId(world);

  ecs_entity_t entity = createTestEntity({0.0f, 5.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
  ASSERT_NE(entity, 0);

  auto *collider =
    static_cast<PhysicsCollider *>(c8EmAsm_entityStartMutComponent(world, entity, colliderId));
  setDefaultCollider(collider);

  // make collider dynamic by setting mass (bullet)
  collider->mass = 1.0f;
  collider->type = COLLIDER_DYNAMIC;

  c8EmAsm_entityEndMutComponent(world, entity, colliderId, true);

  ASSERT_TRUE(c8EmAsm_entityHasComponent(world, entity, colliderId));

  simulatePhysics(60);

  float *velocity = c8EmAsm_getLinearVelocityFromEntity(world, entity);
  ASSERT_NE(velocity, nullptr);

  // velocity should be -y since object is falling down due to gravity
  EXPECT_LE(velocity[1], 0.0f);

  c8EmAsm_progressWorld(world, kDeltaTime);

  ASSERT_TRUE(c8EmAsm_setLinearVelocityToEntity(world, entity, 2.0f, 1.0f, -1.0f));
  velocity = c8EmAsm_getLinearVelocityFromEntity(world, entity);
  ASSERT_NE(velocity, nullptr);
  EXPECT_NEAR(velocity[0], 2.0f, 0.01f);
  EXPECT_NEAR(velocity[1], 1.0f, 0.01f);
  EXPECT_NEAR(velocity[2], -1.0f, 0.01f);

  c8EmAsm_deleteEntity(world, entity);
}

// Tests physics collision by creating a dynamic entity and checking if it falls onto a static floor
TEST_F(PhysicsTest, TestPhysicsCollision) {
  int positionId = ecs_id(Position);
  int scaleId = ecs_id(Scale);
  int rotationId = ecs_id(Rotation);
  int objectId = ecs_id(Object);

  c8EmAsm_initWithComponents(world, positionId, scaleId, rotationId, objectId);

  int colliderId = c8EmAsm_getColliderComponentId(world);
  ASSERT_NE(colliderId, 0);

  // Create static floor entity
  ecs_entity_t floorEntity = createTestEntity({0.0f, 0.0f, 0.0f}, {10.0f, 1.0f, 10.0f});
  ASSERT_NE(floorEntity, 0);

  auto *floorCollider =
    static_cast<PhysicsCollider *>(c8EmAsm_entityStartMutComponent(world, floorEntity, colliderId));
  setDefaultCollider(floorCollider);
  floorCollider->mass = 0.0f;
  floorCollider->type = COLLIDER_STATIC;
  floorCollider->shape = 0;
  c8EmAsm_entityEndMutComponent(world, floorEntity, colliderId, true);

  ASSERT_TRUE(c8EmAsm_entityHasComponent(world, floorEntity, colliderId));

  ecs_entity_t entity = createTestEntity({0.0f, 5.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
  ASSERT_NE(entity, 0);

  auto *collider =
    static_cast<PhysicsCollider *>(c8EmAsm_entityStartMutComponent(world, entity, colliderId));
  setDefaultCollider(collider);
  collider->mass = 1.0f;
  collider->type = COLLIDER_DYNAMIC;
  collider->shape = BOX_SHAPE;
  c8EmAsm_entityEndMutComponent(world, entity, colliderId, true);

  ASSERT_TRUE(c8EmAsm_entityHasComponent(world, entity, colliderId));

  // Run simulation to test collision with floor
  simulatePhysics(180);

  const auto *finalPos =
    static_cast<const Position *>(c8EmAsm_entityGetComponent(world, entity, positionId));
  ASSERT_NE(finalPos, nullptr);
  EXPECT_LE(finalPos->y, 1.5f);
  EXPECT_GE(finalPos->y, 0.5f);

  c8EmAsm_deleteEntity(world, floorEntity);
  c8EmAsm_deleteEntity(world, entity);
}

// Test physics collision of a dynamic entity falling into a static floor
// no collision should happen if the floor entity is disabled
TEST_F(PhysicsTest, TestPhysicsCollisionDisabledEntity) {
  int positionId = ecs_id(Position);
  int scaleId = ecs_id(Scale);
  int rotationId = ecs_id(Rotation);
  int objectId = ecs_id(Object);

  c8EmAsm_initWithComponents(world, positionId, scaleId, rotationId, objectId);

  int colliderId = c8EmAsm_getColliderComponentId(world);

  // Create static floor entity
  ecs_entity_t floorEntity = createTestEntity({0.0f, 0.0f, 0.0f}, {10.0f, 1.0f, 10.0f});
  ASSERT_NE(floorEntity, 0);

  auto *floorCollider =
    static_cast<PhysicsCollider *>(c8EmAsm_entityStartMutComponent(world, floorEntity, colliderId));
  setDefaultCollider(floorCollider);
  floorCollider->mass = 0.0f;
  floorCollider->type = COLLIDER_STATIC;
  floorCollider->type = BOX_SHAPE;
  c8EmAsm_entityEndMutComponent(world, floorEntity, colliderId, true);

  ASSERT_TRUE(c8EmAsm_entityHasComponent(world, floorEntity, colliderId));

  // Disable the floor entity
  c8EmAsm_entitySetEnabled(world, floorEntity, false);

  ecs_entity_t entity = createTestEntity({0.0f, 5.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
  ASSERT_NE(entity, 0);

  auto *collider =
    static_cast<PhysicsCollider *>(c8EmAsm_entityStartMutComponent(world, entity, colliderId));
  setDefaultCollider(collider);
  collider->mass = 1.0f;
  collider->type = COLLIDER_DYNAMIC;
  collider->shape = BOX_SHAPE;
  c8EmAsm_entityEndMutComponent(world, entity, colliderId, true);

  ASSERT_TRUE(c8EmAsm_entityHasComponent(world, entity, colliderId));

  // Run simulation to test collision with floor
  simulatePhysics(180);

  const auto *finalPos =
    static_cast<const Position *>(c8EmAsm_entityGetComponent(world, entity, positionId));
  ASSERT_NE(finalPos, nullptr);
  EXPECT_LE(finalPos->y, 0.f);

  c8EmAsm_deleteEntity(world, floorEntity);
  c8EmAsm_deleteEntity(world, entity);
}

// Test creating 10 entities with colliders and then deleting them
TEST_F(PhysicsTest, CreateAndDeleteMultipleEntitiesWithColliders) {
  initializeWorldWithStandardComponents();

  int positionId = ecs_id(Position);
  int colliderId = c8EmAsm_getColliderComponentId(world);
  constexpr int kNumEntities = 10;
  constexpr float heightAboveGround = 5.0f;
  // Create kNumEntities entities with colliders
  ecs_entity_t entities[kNumEntities];
  for (int i = 0; i < kNumEntities; i++) {
    auto spacing = static_cast<float>(i) * 2.0f;
    entities[i] = createTestEntity({spacing, heightAboveGround, 0.0f}, {1.0f, 1.0f, 1.0f});
    ASSERT_NE(entities[i], 0);

    auto *collider = static_cast<PhysicsCollider *>(
      c8EmAsm_entityStartMutComponent(world, entities[i], colliderId));
    setDefaultCollider(collider);
    collider->mass = 1.0f;
    collider->type = COLLIDER_DYNAMIC;
    collider->shape = BOX_SHAPE;
    c8EmAsm_entityEndMutComponent(world, entities[i], colliderId, true);

    ASSERT_TRUE(c8EmAsm_entityHasComponent(world, entities[i], colliderId));
  }

  // Run physics simulation for a second
  for (int step = 0; step < 60; step++) {
    c8EmAsm_progressWorld(world, kDeltaTime);
  }

  // Verify all entities still exist and have moved
  for (int i = 0; i < kNumEntities; i++) {
    const auto *pos =
      static_cast<const Position *>(c8EmAsm_entityGetComponent(world, entities[i], positionId));
    ASSERT_NE(pos, nullptr);
    EXPECT_LT(pos->y, heightAboveGround - 1.f);  // Should have fallen due to gravity
  }

  // Delete all entities
  for (int i = 0; i < kNumEntities; i++) {
    c8EmAsm_deleteEntity(world, entities[i]);
  }
}

// Test updating ECS state on static, dynamic and kinematic colliders
TEST_F(PhysicsTest, UpdateEcsStateOnAllColliderTypes) {
  int positionId = ecs_id(Position);
  int scaleId = ecs_id(Scale);
  int rotationId = ecs_id(Rotation);
  int objectId = ecs_id(Object);

  c8EmAsm_initWithComponents(world, positionId, scaleId, rotationId, objectId);

  int colliderId = c8EmAsm_getColliderComponentId(world);

  // Create static entity
  ecs_entity_t staticEntity = createTestEntity({0.0f, 2.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
  auto *staticCollider = static_cast<PhysicsCollider *>(
    c8EmAsm_entityStartMutComponent(world, staticEntity, colliderId));
  setDefaultCollider(staticCollider);
  c8EmAsm_entityEndMutComponent(world, staticEntity, colliderId, true);

  // Create dynamic entity
  ecs_entity_t dynamicEntity = createTestEntity({3.0f, 2.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
  auto *dynamicCollider = static_cast<PhysicsCollider *>(
    c8EmAsm_entityStartMutComponent(world, dynamicEntity, colliderId));
  setDefaultCollider(dynamicCollider);
  dynamicCollider->type = COLLIDER_DYNAMIC;
  dynamicCollider->mass = 1.0f;
  dynamicCollider->gravityFactor = 0.0f;
  dynamicCollider->linearDamping = 1.0f;
  dynamicCollider->angularDamping = 1.0f;
  c8EmAsm_entityEndMutComponent(world, dynamicEntity, colliderId, true);

  // Create kinematic entity
  ecs_entity_t kinematicEntity = createTestEntity({6.0f, 2.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
  auto *kinematicCollider = static_cast<PhysicsCollider *>(
    c8EmAsm_entityStartMutComponent(world, kinematicEntity, colliderId));
  setDefaultCollider(kinematicCollider);
  kinematicCollider->type = COLLIDER_KINEMATIC;
  kinematicCollider->mass = 1.0f;
  c8EmAsm_entityEndMutComponent(world, kinematicEntity, colliderId, true);

  // Progress world to apply changes
  simulatePhysics(30);

  // Update scale to 2.0f for all entities
  constexpr float expectedScale = 2.0f;

  auto *staticScale =
    static_cast<Scale *>(c8EmAsm_entityStartMutComponent(world, staticEntity, scaleId));
  staticScale->x = expectedScale;
  staticScale->y = expectedScale;
  staticScale->z = expectedScale;
  c8EmAsm_entityEndMutComponent(world, staticEntity, scaleId, true);

  auto *dynamicScale =
    static_cast<Scale *>(c8EmAsm_entityStartMutComponent(world, dynamicEntity, scaleId));
  dynamicScale->x = expectedScale;
  dynamicScale->y = expectedScale;
  dynamicScale->z = expectedScale;
  c8EmAsm_entityEndMutComponent(world, dynamicEntity, scaleId, true);

  auto *kinematicScale =
    static_cast<Scale *>(c8EmAsm_entityStartMutComponent(world, kinematicEntity, scaleId));
  kinematicScale->x = expectedScale;
  kinematicScale->y = expectedScale;
  kinematicScale->z = expectedScale;
  c8EmAsm_entityEndMutComponent(world, kinematicEntity, scaleId, true);

  // Progress world to apply changes
  simulatePhysics(30);

  // Update position
  // Expected position values
  constexpr float expectedStaticPos = 1.0f;
  constexpr float expectedDynamicPos = 20.0f;
  constexpr float expectedKinematicPos = 3.0f;

  // Update rotation (45 degrees around Y axis)
  constexpr float rotationX = 0.0f;
  constexpr float rotationY = 0.38268f;
  constexpr float rotationZ = 0.0f;
  constexpr float rotationW = 0.92388f;

  auto *staticPos =
    static_cast<Position *>(c8EmAsm_entityStartMutComponent(world, staticEntity, positionId));
  staticPos->x = expectedStaticPos;
  staticPos->y = expectedStaticPos;
  staticPos->z = expectedStaticPos;
  c8EmAsm_entityEndMutComponent(world, staticEntity, positionId, true);

  auto *dynamicPos =
    static_cast<Position *>(c8EmAsm_entityStartMutComponent(world, dynamicEntity, positionId));
  dynamicPos->x = expectedDynamicPos;
  dynamicPos->y = expectedDynamicPos;
  dynamicPos->z = expectedDynamicPos;
  c8EmAsm_entityEndMutComponent(world, dynamicEntity, positionId, true);

  auto *kinematicPos =
    static_cast<Position *>(c8EmAsm_entityStartMutComponent(world, kinematicEntity, positionId));
  kinematicPos->x = expectedKinematicPos;
  kinematicPos->y = expectedKinematicPos;
  kinematicPos->z = expectedKinematicPos;

  c8EmAsm_entityEndMutComponent(world, kinematicEntity, positionId, true);

  // Progress world to apply changes
  simulatePhysics(30);

  auto *staticRot =
    static_cast<Rotation *>(c8EmAsm_entityStartMutComponent(world, staticEntity, rotationId));
  staticRot->x = rotationX;
  staticRot->y = rotationY;
  staticRot->z = rotationZ;
  staticRot->w = rotationW;
  c8EmAsm_entityEndMutComponent(world, staticEntity, rotationId, true);

  auto *dynamicRot =
    static_cast<Rotation *>(c8EmAsm_entityStartMutComponent(world, dynamicEntity, rotationId));
  dynamicRot->x = rotationX;
  dynamicRot->y = rotationY;
  dynamicRot->z = rotationZ;
  dynamicRot->w = rotationW;
  c8EmAsm_entityEndMutComponent(world, dynamicEntity, rotationId, true);

  auto *kinematicRot =
    static_cast<Rotation *>(c8EmAsm_entityStartMutComponent(world, kinematicEntity, rotationId));
  kinematicRot->x = rotationX;
  kinematicRot->y = rotationY;
  kinematicRot->z = rotationZ;
  kinematicRot->w = rotationW;
  c8EmAsm_entityEndMutComponent(world, kinematicEntity, rotationId, true);

  // Progress world to apply changes
  simulatePhysics(30);

  // Verify static entity values
  verifyScale(staticEntity, expectedScale, expectedScale, expectedScale);
  verifyPosition(staticEntity, expectedStaticPos, expectedStaticPos, expectedStaticPos);
  verifyRotation(staticEntity, rotationX, rotationY, rotationZ, rotationW);

  // Verify dynamic entity values
  verifyScale(dynamicEntity, expectedScale, expectedScale, expectedScale);
  // Skip position check since dynamic entity position can be affected by physics simulation
  verifyRotation(dynamicEntity, rotationX, rotationY, rotationZ, rotationW);

  // Verify kinematic entity values
  verifyScale(kinematicEntity, expectedScale, expectedScale, expectedScale);
  verifyPosition(kinematicEntity, expectedKinematicPos, expectedKinematicPos, expectedKinematicPos);
  verifyRotation(kinematicEntity, rotationX, rotationY, rotationZ, rotationW);

  c8EmAsm_deleteEntity(world, staticEntity);
  c8EmAsm_deleteEntity(world, dynamicEntity);
  c8EmAsm_deleteEntity(world, kinematicEntity);
}

// Test unlocking and locking Allowed degrees of Freedom (DoFs) on a dynamic entity
TEST_F(PhysicsTest, TestAllowedDofsDynamicEntity) {
  int positionId = ecs_id(Position);
  int scaleId = ecs_id(Scale);
  int rotationId = ecs_id(Rotation);
  int objectId = ecs_id(Object);
  c8EmAsm_initWithComponents(world, positionId, scaleId, rotationId, objectId);

  int colliderId = c8EmAsm_getColliderComponentId(world);

  ecs_entity_t entity = createTestEntity({2.0f, 5.0f, 3.0f}, {1.0f, 1.0f, 1.0f});
  ASSERT_NE(entity, 0);

  auto *collider =
    static_cast<PhysicsCollider *>(c8EmAsm_entityStartMutComponent(world, entity, colliderId));
  setDefaultCollider(collider);
  collider->mass = 1.0f;
  collider->type = COLLIDER_DYNAMIC;
  collider->shape = BOX_SHAPE;

  // Lock all DoFs
  collider->lockXPosition = true;
  collider->lockYPosition = true;
  collider->lockZPosition = true;
  collider->lockXAxis = true;
  collider->lockYAxis = true;
  collider->lockZAxis = true;
  c8EmAsm_entityEndMutComponent(world, entity, colliderId, true);

  ASSERT_TRUE(c8EmAsm_entityHasComponent(world, entity, colliderId));

  // Run simulation to test that object does not move
  simulatePhysics(60);

  const auto *lockedPosition =
    static_cast<const Position *>(c8EmAsm_entityGetComponent(world, entity, positionId));

  ASSERT_NE(lockedPosition, nullptr);
  EXPECT_FLOAT_EQ(lockedPosition->x, 2.0f);
  EXPECT_FLOAT_EQ(lockedPosition->y, 5.0f);
  EXPECT_FLOAT_EQ(lockedPosition->z, 3.0f);

  collider =
    static_cast<PhysicsCollider *>(c8EmAsm_entityStartMutComponent(world, entity, colliderId));
  setDefaultCollider(collider);
  collider->mass = 1.0f;
  collider->type = COLLIDER_DYNAMIC;
  collider->shape = BOX_SHAPE;

  // Unlock all DoFs
  collider->lockXPosition = false;
  collider->lockYPosition = false;
  collider->lockZPosition = false;
  collider->lockXAxis = false;
  collider->lockYAxis = false;
  collider->lockZAxis = false;
  c8EmAsm_entityEndMutComponent(world, entity, colliderId, true);

  // Run simulation to test that object now drops, i.e., y should decrease
  simulatePhysics(60);

  const auto *finalPos =
    static_cast<const Position *>(c8EmAsm_entityGetComponent(world, entity, positionId));

  // Since the only force acting on the object is gravity,
  // it should only have moved downwards in Y
  ASSERT_NE(finalPos, nullptr);
  EXPECT_FLOAT_EQ(finalPos->x, 2.0f);
  EXPECT_LT(finalPos->y, 5.0f);
  EXPECT_FLOAT_EQ(finalPos->z, 3.0f);

  c8EmAsm_deleteEntity(world, entity);
}

// Test registering and unregistering a custom convex shape
TEST_F(PhysicsTest, TestRegisterConvexShape) {
  initializeWorldWithStandardComponents();

  int colliderId = c8EmAsm_getColliderComponentId(world);

  float vertices[] = {
    -0.5f,
    -0.5f,
    0.0f,  // Point 0
    -0.5f,
    0.5f,
    0.0f,  // Point 1
    0.5f,
    0.5f,
    0.0f,  // Point 2
    0.5f,
    -0.5f,
    0.0f  // Point 3
  };
  constexpr int verticesLength = 12;  // 4 points * 3 components each

  // Register the shape
  uint32_t shapeId = c8EmAsm_registerConvexShape(world, vertices, verticesLength);
  EXPECT_NE(shapeId, 0);
  EXPECT_GE(shapeId, 1000);  // Should be >= CUSTOM_SHAPE_START_ID

  // Create an entity using the custom shape
  ecs_entity_t entity = createTestEntity({0.0f, 5.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
  ASSERT_NE(entity, 0);

  auto *collider =
    static_cast<PhysicsCollider *>(c8EmAsm_entityStartMutComponent(world, entity, colliderId));
  setDefaultCollider(collider);
  collider->mass = 1.0f;
  collider->type = COLLIDER_DYNAMIC;
  collider->shape = shapeId;  // Use the custom shape ID
  c8EmAsm_entityEndMutComponent(world, entity, colliderId, true);

  ASSERT_TRUE(c8EmAsm_entityHasComponent(world, entity, colliderId));

  // Progress the world to see physics simulation
  simulatePhysics(60);

  // Unregister the shape while other entities are using it
  c8EmAsm_unregisterConvexShape(world, shapeId);

  // Progress the world to see physics simulation
  simulatePhysics(60);

  c8EmAsm_deleteEntity(world, entity);
}

// Test sleeping bodies
TEST_F(PhysicsTest, TestSleepingBodies) {
  int positionId = ecs_id(Position);
  int scaleId = ecs_id(Scale);
  int rotationId = ecs_id(Rotation);
  int objectId = ecs_id(Object);
  c8EmAsm_initWithComponents(world, positionId, scaleId, rotationId, objectId);

  int colliderId = c8EmAsm_getColliderComponentId(world);

  // Create a dynamic entity that should eventually sleep
  ecs_entity_t entity = createTestEntity({0.0f, 5.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
  ASSERT_NE(entity, 0);

  auto *collider =
    static_cast<PhysicsCollider *>(c8EmAsm_entityStartMutComponent(world, entity, colliderId));
  setDefaultCollider(collider);
  collider->mass = 1.0f;
  collider->type = COLLIDER_DYNAMIC;
  collider->shape = BOX_SHAPE;
  c8EmAsm_entityEndMutComponent(world, entity, colliderId, true);

  // Create floor for object to land on
  ecs_entity_t floorEntity = createTestEntity({0.0f, 0.0f, 0.0f}, {10.0f, 1.0f, 10.0f});
  auto *floorCollider =
    static_cast<PhysicsCollider *>(c8EmAsm_entityStartMutComponent(world, floorEntity, colliderId));
  setDefaultCollider(floorCollider);
  floorCollider->mass = 0.0f;
  floorCollider->type = COLLIDER_STATIC;
  floorCollider->shape = BOX_SHAPE;
  c8EmAsm_entityEndMutComponent(world, floorEntity, colliderId, true);

  c8EmAsm_progressWorld(world, kDeltaTime);

  // Verify the body is active initially
  verifyBodyIsAwake(entity, true);

  // Run physics for a long time to allow object to settle and then sleep
  simulatePhysics(180);

  // Verify the body is inactive (asleep)
  verifyBodyIsAwake(entity, false);

  const auto *finalPos =
    static_cast<const Position *>(c8EmAsm_entityGetComponent(world, entity, positionId));
  ASSERT_NE(finalPos, nullptr);
  // Object should have settled above the floor
  EXPECT_GE(finalPos->y, 0.0f);
}

// Test dropping a ball onto a static plane, letting it settle, then removing the floor
TEST_F(PhysicsTest, TestBallDropAndFloorRemoval) {
  initializeWorldWithStandardComponents();

  ecs_entity_t floorEntity = createStaticFloor();
  ecs_entity_t ballEntity = createDynamicBall();

  float settledY = letBallSettleAndGetY(ballEntity);

  // Delete the floor
  c8EmAsm_deleteEntity(world, floorEntity);

  simulatePhysics(60);
  verifyBallDropped(ballEntity, settledY);

  c8EmAsm_deleteEntity(world, ballEntity);
}

// Test dropping a ball onto a static plane, letting it settle, then setting collider to event-only
TEST_F(PhysicsTest, TestBallDropAndEventOnlyCollider) {
  initializeWorldWithStandardComponents();

  ecs_entity_t floorEntity = createStaticFloor();
  ecs_entity_t ballEntity = createDynamicBall();

  float settledY = letBallSettleAndGetY(ballEntity);

  // Set the floor collider to event-only (should make it non-colliding)
  int colliderId = c8EmAsm_getColliderComponentId(world);
  auto *floorCollider = static_cast<PhysicsCollider *>(

    c8EmAsm_entityStartMutComponent(world, floorEntity, colliderId));
  floorCollider->eventOnly = true;
  c8EmAsm_entityEndMutComponent(world, floorEntity, colliderId, true);

  simulatePhysics(60);
  verifyBallDropped(ballEntity, settledY);

  c8EmAsm_deleteEntity(world, floorEntity);
  c8EmAsm_deleteEntity(world, ballEntity);
}

// Test dropping a ball onto a static plane, letting it settle, then disabling the floor
TEST_F(PhysicsTest, TestBallDropAndFloorDisable) {
  initializeWorldWithStandardComponents();

  ecs_entity_t floorEntity = createStaticFloor();
  ecs_entity_t ballEntity = createDynamicBall();

  float settledY = letBallSettleAndGetY(ballEntity);

  // Disable the floor
  c8EmAsm_entitySetEnabled(world, floorEntity, false);

  simulatePhysics(60);
  verifyBallDropped(ballEntity, settledY, 1.0f);
}

// Test dropping a ball onto a static plane, letting it settle, then positioning the floor far up
TEST_F(PhysicsTest, TestBallDropAndFloorTranslate) {
  initializeWorldWithStandardComponents();

  ecs_entity_t floorEntity = createStaticFloor();
  ecs_entity_t ballEntity = createDynamicBall();

  float settledY = letBallSettleAndGetY(ballEntity);

  // Move floor far up so the ball can drop again
  int positionId = ecs_id(Position);
  auto *floorPos =
    static_cast<Position *>(c8EmAsm_entityStartMutComponent(world, floorEntity, positionId));
  ASSERT_NE(floorPos, nullptr);
  floorPos->y = 100.0f;
  c8EmAsm_entityEndMutComponent(world, floorEntity, positionId, true);

  simulatePhysics(60);
  verifyBallDropped(ballEntity, settledY);
}

// Test dropping a ball onto a static plane, letting it settle
// then changing the floor collider offset
TEST_F(PhysicsTest, TestBallDropAndColliderOffset) {
  initializeWorldWithStandardComponents();

  ecs_entity_t floorEntity = createStaticFloor();
  ecs_entity_t ballEntity = createDynamicBall();

  float settledY = letBallSettleAndGetY(ballEntity);

  // Change floor collider offset
  int colliderId = c8EmAsm_getColliderComponentId(world);
  auto *floorCollider =
    static_cast<PhysicsCollider *>(c8EmAsm_entityStartMutComponent(world, floorEntity, colliderId));
  floorCollider->offsetY = -5.0f;
  c8EmAsm_entityEndMutComponent(world, floorEntity, colliderId, true);

  simulatePhysics(60);
  verifyBallDropped(ballEntity, settledY);
}

// Test dropping a ball onto a static plane, letting it settle
// then changing the ball collider offset while moving it in the opposite direction
TEST_F(PhysicsTest, TestBallDropAndColliderOffsetWithInversePosition) {
  initializeWorldWithStandardComponents();

  ecs_entity_t floorEntity = createStaticFloor();
  ecs_entity_t ballEntity = createDynamicBall();

  float settledY = letBallSettleAndGetY(ballEntity);

  // Change ball collider offset and move position in opposite direction to cancel out
  int colliderId = c8EmAsm_getColliderComponentId(world);
  int positionId = ecs_id(Position);

  verifyBodyIsAwake(ballEntity, false);

  auto *ballCollider =
    static_cast<PhysicsCollider *>(c8EmAsm_entityStartMutComponent(world, ballEntity, colliderId));
  ballCollider->offsetY = -5.0f;
  auto *ballPosition =
    static_cast<Position *>(c8EmAsm_entityStartMutComponent(world, ballEntity, positionId));
  ASSERT_NE(ballPosition, nullptr);
  ballPosition->y += 5.0f;

  c8EmAsm_entityEndMutComponent(world, ballEntity, colliderId, true);
  c8EmAsm_entityEndMutComponent(world, ballEntity, positionId, true);

  c8EmAsm_progressWorld(world, kDeltaTime);

  // Verify body is still inactive after the change
  verifyBodyIsAwake(ballEntity, false);

  simulatePhysics(60);

  // Verify ball stayed at the new position (changes cancelled out)
  const auto *finalPos =
    static_cast<const Position *>(c8EmAsm_entityGetComponent(world, ballEntity, positionId));
  ASSERT_NE(finalPos, nullptr);
  EXPECT_EQ(finalPos->y, settledY + 5.0f);

  c8EmAsm_deleteEntity(world, floorEntity);
  c8EmAsm_deleteEntity(world, ballEntity);
}

// Test dropping a ball onto a static plane, letting it settle, then rotating the floor
TEST_F(PhysicsTest, TestBallDropAndFloorRotate) {
  initializeWorldWithStandardComponents();

  ecs_entity_t floorEntity = createStaticFloor();
  ecs_entity_t ballEntity = createDynamicBall();

  float settledY = letBallSettleAndGetY(ballEntity);

  // Rotate floor so the ball can drop again
  int rotationId = ecs_id(Rotation);
  auto *floorRotation =
    static_cast<Rotation *>(c8EmAsm_entityStartMutComponent(world, floorEntity, rotationId));
  ASSERT_NE(floorRotation, nullptr);
  auto q = Quaternion::fromEulerAngleDegrees(0.0f, 0.0f, 4.0f);
  q = q.normalize();
  floorRotation->x = q.x();
  floorRotation->y = q.y();
  floorRotation->z = q.z();
  floorRotation->w = q.w();
  c8EmAsm_entityEndMutComponent(world, floorEntity, rotationId, true);

  simulatePhysics(60);
  verifyBallDropped(ballEntity, settledY);
}

// Test dropping a ball onto a static plane, letting it settle, then scaling the floor
TEST_F(PhysicsTest, TestBallAndFloorScale) {
  int positionId = ecs_id(Position);
  int scaleId = ecs_id(Scale);
  int rotationId = ecs_id(Rotation);
  int objectId = ecs_id(Object);

  c8EmAsm_initWithComponents(world, positionId, scaleId, rotationId, objectId);

  int colliderId = c8EmAsm_getColliderComponentId(world);
  ASSERT_NE(colliderId, 0);

  // Create static floor entity
  ecs_entity_t floorEntity = createTestEntity({0.0f, 0.0f, 0.0f}, {10.0f, 1.0f, 10.0f});
  ASSERT_NE(floorEntity, 0);

  auto *floorCollider =
    static_cast<PhysicsCollider *>(c8EmAsm_entityStartMutComponent(world, floorEntity, colliderId));
  setDefaultCollider(floorCollider);
  floorCollider->mass = 0.0f;
  floorCollider->type = COLLIDER_STATIC;
  floorCollider->shape = BOX_SHAPE;
  c8EmAsm_entityEndMutComponent(world, floorEntity, colliderId, true);

  ASSERT_TRUE(c8EmAsm_entityHasComponent(world, floorEntity, colliderId));

  // Create dynamic ball entity, setting it off to the side to let it drop when we scale the floor
  ecs_entity_t ballEntity = createTestEntity({-4.9f, 5.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
  ASSERT_NE(ballEntity, 0);

  auto *ballCollider =
    static_cast<PhysicsCollider *>(c8EmAsm_entityStartMutComponent(world, ballEntity, colliderId));
  setDefaultCollider(ballCollider);
  ballCollider->mass = 1.0f;
  ballCollider->type = COLLIDER_DYNAMIC;
  ballCollider->shape = SPHERE_SHAPE;
  ballCollider->radius = 0.5f;
  c8EmAsm_entityEndMutComponent(world, ballEntity, colliderId, true);

  ASSERT_TRUE(c8EmAsm_entityHasComponent(world, ballEntity, colliderId));

  // Let ball settle for 180 iterations
  simulatePhysics(180);

  // Verify ball has settled on the floor
  const auto *settledPos =
    static_cast<const Position *>(c8EmAsm_entityGetComponent(world, ballEntity, positionId));
  ASSERT_NE(settledPos, nullptr);
  EXPECT_LE(settledPos->y, 1.5f);
  EXPECT_GE(settledPos->y, 0.5f);

  float settledY = settledPos->y;

  // scale the floor down in x so the ball can drop
  auto *floorScale =
    static_cast<Scale *>(c8EmAsm_entityStartMutComponent(world, floorEntity, scaleId));
  ASSERT_NE(floorScale, nullptr);
  floorScale->x = 0.9f;
  floorScale->y = 1.0f;
  floorScale->z = 1.0f;
  c8EmAsm_entityEndMutComponent(world, floorEntity, scaleId, true);

  // Let physics run for a few more iterations to see the ball drop
  simulatePhysics(60);

  // Verify ball has dropped below its settled position
  const auto *finalPos =
    static_cast<const Position *>(c8EmAsm_entityGetComponent(world, ballEntity, positionId));
  ASSERT_NE(finalPos, nullptr);
  EXPECT_LT(finalPos->y, settledY);

  c8EmAsm_deleteEntity(world, ballEntity);
  c8EmAsm_deleteEntity(world, floorEntity);
}

// Test scaling a shape
TEST_F(PhysicsTest, TestShapeScaling) {
  int positionId = ecs_id(Position);
  int scaleId = ecs_id(Scale);
  int rotationId = ecs_id(Rotation);
  int objectId = ecs_id(Object);
  c8EmAsm_initWithComponents(world, positionId, scaleId, rotationId, objectId);

  int colliderId = c8EmAsm_getColliderComponentId(world);
  ASSERT_NE(colliderId, 0);

  // Create dynamic entity with box shape
  ecs_entity_t entity = createTestEntity({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
  ASSERT_NE(entity, 0);

  auto *collider =
    static_cast<PhysicsCollider *>(c8EmAsm_entityStartMutComponent(world, entity, colliderId));
  setDefaultCollider(collider);
  collider->mass = 1.0f;
  collider->type = COLLIDER_DYNAMIC;
  collider->gravityFactor = 0.0f;
  c8EmAsm_entityEndMutComponent(world, entity, colliderId, true);

  ASSERT_TRUE(c8EmAsm_entityHasComponent(world, entity, colliderId));

  // Scale the shape 100 times with different scale values
  const float defaultScale = 1.0f;
  float scaleValue = defaultScale;
  for (int i = 0; i < 100; i++) {
    // Update the scale component
    auto *entityScale =
      static_cast<Scale *>(c8EmAsm_entityStartMutComponent(world, entity, scaleId));
    entityScale->x = scaleValue;
    entityScale->y = scaleValue;
    entityScale->z = scaleValue;
    c8EmAsm_entityEndMutComponent(world, entity, scaleId, true);

    // Progress the world to apply the scale change
    c8EmAsm_progressWorld(world, kDeltaTime);

    // Verify the entity still exists and has the scale component
    ASSERT_TRUE(c8EmAsm_entityHasComponent(world, entity, scaleId));

    const auto *currentScale =
      static_cast<const Scale *>(c8EmAsm_entityGetComponent(world, entity, scaleId));
    ASSERT_NE(currentScale, nullptr);
    EXPECT_NEAR(currentScale->x, scaleValue, 0.001f);
    EXPECT_NEAR(currentScale->y, scaleValue, 0.001f);
    EXPECT_NEAR(currentScale->z, scaleValue, 0.001f);

    scaleValue += 1.f;  // Increment scale for next iteration
  }

  // Verify the entity still exists and is functional after all the scaling operations
  ASSERT_TRUE(c8EmAsm_entityHasComponent(world, entity, colliderId));
  ASSERT_TRUE(c8EmAsm_entityHasComponent(world, entity, positionId));

  // Run a few more physics steps to ensure stability
  simulatePhysics(10);

  // revert back to original scale
  auto *entityScale = static_cast<Scale *>(c8EmAsm_entityStartMutComponent(world, entity, scaleId));
  entityScale->x = defaultScale;
  entityScale->y = defaultScale;
  entityScale->z = defaultScale;

  c8EmAsm_entityEndMutComponent(world, entity, scaleId, true);

  c8EmAsm_progressWorld(world, kDeltaTime);

  const auto *currentScale =
    static_cast<const Scale *>(c8EmAsm_entityGetComponent(world, entity, scaleId));
  ASSERT_NE(currentScale, nullptr);
  EXPECT_NEAR(currentScale->x, defaultScale, 0.001f);
  EXPECT_NEAR(currentScale->y, defaultScale, 0.001f);
  EXPECT_NEAR(currentScale->z, defaultScale, 0.001f);

  const auto *finalPos =
    static_cast<const Position *>(c8EmAsm_entityGetComponent(world, entity, positionId));
  ASSERT_NE(finalPos, nullptr);
  EXPECT_FLOAT_EQ(finalPos->x, 0.0f);
  EXPECT_FLOAT_EQ(finalPos->y, 0.0f);
  EXPECT_FLOAT_EQ(finalPos->z, 0.0f);

  c8EmAsm_deleteEntity(world, entity);
}

// Test a single ball falling with gravity - start with no gravity then add gravity
TEST_F(PhysicsTest, TestSingleBallGravityFactorChange) {
  initializeWorldWithStandardComponents();

  ecs_entity_t ballEntity = createDynamicBall({0.0f, 10.0f, 0.0f});

  // Set gravity factor to 0 initially
  int colliderId = c8EmAsm_getColliderComponentId(world);
  auto *ballCollider =
    static_cast<PhysicsCollider *>(c8EmAsm_entityStartMutComponent(world, ballEntity, colliderId));
  ballCollider->gravityFactor = 0.0f;
  c8EmAsm_entityEndMutComponent(world, ballEntity, colliderId, true);

  int positionId = ecs_id(Position);
  const auto *initialPos =
    static_cast<const Position *>(c8EmAsm_entityGetComponent(world, ballEntity, positionId));
  ASSERT_NE(initialPos, nullptr);
  float initialY = initialPos->y;
  EXPECT_FLOAT_EQ(initialY, 10.0f);

  simulatePhysics(60);

  // Verify ball has NOT fallen (no gravity)
  const auto *noGravityPos =
    static_cast<const Position *>(c8EmAsm_entityGetComponent(world, ballEntity, positionId));
  ASSERT_NE(noGravityPos, nullptr);
  EXPECT_FLOAT_EQ(noGravityPos->y, initialY);

  // Enable gravity
  ballCollider =
    static_cast<PhysicsCollider *>(c8EmAsm_entityStartMutComponent(world, ballEntity, colliderId));
  ballCollider->gravityFactor = 1.0f;
  c8EmAsm_entityEndMutComponent(world, ballEntity, colliderId, true);

  simulatePhysics(60);

  // Verify ball has now fallen due to gravity
  const auto *afterGravityPos =
    static_cast<const Position *>(c8EmAsm_entityGetComponent(world, ballEntity, positionId));
  ASSERT_NE(afterGravityPos, nullptr);
  EXPECT_LT(afterGravityPos->y, initialY);

  c8EmAsm_deleteEntity(world, ballEntity);
}

// Test that changing friction reactivates a sleeping object on a slope
TEST_F(PhysicsTest, TestFrictionChange) {
  initializeWorldWithStandardComponents();

  int colliderId = c8EmAsm_getColliderComponentId(world);
  int rotationId = ecs_id(Rotation);
  int positionId = ecs_id(Position);

  // Create a 20-degree sloped floor
  ecs_entity_t slopeEntity = createStaticFloor();

  // Rotate the floor 20 degrees around Z axis
  auto *slopeRotation =
    static_cast<Rotation *>(c8EmAsm_entityStartMutComponent(world, slopeEntity, rotationId));
  ASSERT_NE(slopeRotation, nullptr);
  auto q = Quaternion::fromEulerAngleDegrees(0.0f, 0.0f, 20.0f);
  q = q.normalize();
  slopeRotation->x = q.x();
  slopeRotation->y = q.y();
  slopeRotation->z = q.z();
  slopeRotation->w = q.w();
  c8EmAsm_entityEndMutComponent(world, slopeEntity, rotationId, true);

  // Set slope friction
  auto *slopeCollider =
    static_cast<PhysicsCollider *>(c8EmAsm_entityStartMutComponent(world, slopeEntity, colliderId));
  slopeCollider->friction = 1.0f;
  c8EmAsm_entityEndMutComponent(world, slopeEntity, colliderId, true);

  // Create a dynamic box on the slope
  ecs_entity_t boxEntity = createTestEntity({0.0f, 3.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
  ASSERT_NE(boxEntity, 0);

  auto *boxCollider =
    static_cast<PhysicsCollider *>(c8EmAsm_entityStartMutComponent(world, boxEntity, colliderId));
  setDefaultCollider(boxCollider);
  boxCollider->mass = 1.0f;
  boxCollider->type = COLLIDER_DYNAMIC;
  boxCollider->shape = BOX_SHAPE;
  boxCollider->friction = 1.0f;
  boxCollider->linearDamping = 0.1f;
  boxCollider->angularDamping = 0.1f;
  c8EmAsm_entityEndMutComponent(world, boxEntity, colliderId, true);

  // Let the box settle on the slope (240 iterations = 4 seconds)
  simulatePhysics(240);

  verifyBodyIsAwake(boxEntity, false);

  // Get the settled position
  const auto *settledPos =
    static_cast<const Position *>(c8EmAsm_entityGetComponent(world, boxEntity, positionId));
  ASSERT_NE(settledPos, nullptr);
  float settledX = settledPos->x;
  float settledY = settledPos->y;

  // Reduce friction significantly
  boxCollider =
    static_cast<PhysicsCollider *>(c8EmAsm_entityStartMutComponent(world, boxEntity, colliderId));
  boxCollider->friction = 0.01f;
  c8EmAsm_entityEndMutComponent(world, boxEntity, colliderId, true);

  // Verify object is reactivated after friction change
  c8EmAsm_progressWorld(world, kDeltaTime);
  verifyBodyIsAwake(boxEntity, true);

  const auto *finalPos =
    static_cast<const Position *>(c8EmAsm_entityGetComponent(world, boxEntity, positionId));
  ASSERT_NE(finalPos, nullptr);

  simulatePhysics(120);

  // Verify that the box slides down the slope
  ASSERT_LT(finalPos->x, settledX - 0.1f);
  ASSERT_LT(finalPos->y, settledY - 0.1f);

  c8EmAsm_deleteEntity(world, boxEntity);
  c8EmAsm_deleteEntity(world, slopeEntity);
}

// Test that changing friction on a static object activates bodies dynamic bodies contact
TEST_F(PhysicsTest, TestFrictionChangeStatic) {
  initializeWorldWithStandardComponents();

  int colliderId = c8EmAsm_getColliderComponentId(world);
  int rotationId = ecs_id(Rotation);
  int positionId = ecs_id(Position);

  // Create a 20-degree sloped floor
  ecs_entity_t slopeEntity = createStaticFloor();

  // Rotate the floor 20 degrees around Z axis
  auto *slopeRotation =
    static_cast<Rotation *>(c8EmAsm_entityStartMutComponent(world, slopeEntity, rotationId));
  ASSERT_NE(slopeRotation, nullptr);
  auto q = Quaternion::fromEulerAngleDegrees(0.0f, 0.0f, 20.0f);
  q = q.normalize();
  slopeRotation->x = q.x();
  slopeRotation->y = q.y();
  slopeRotation->z = q.z();
  slopeRotation->w = q.w();
  c8EmAsm_entityEndMutComponent(world, slopeEntity, rotationId, true);

  // Set slope friction
  auto *slopeCollider =
    static_cast<PhysicsCollider *>(c8EmAsm_entityStartMutComponent(world, slopeEntity, colliderId));
  slopeCollider->friction = 1.0f;
  c8EmAsm_entityEndMutComponent(world, slopeEntity, colliderId, true);

  // Create a dynamic box on the slope
  ecs_entity_t boxEntity = createTestEntity({0.0f, 3.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
  ASSERT_NE(boxEntity, 0);

  auto *boxCollider =
    static_cast<PhysicsCollider *>(c8EmAsm_entityStartMutComponent(world, boxEntity, colliderId));
  setDefaultCollider(boxCollider);
  boxCollider->mass = 1.0f;
  boxCollider->type = COLLIDER_DYNAMIC;
  boxCollider->shape = BOX_SHAPE;
  boxCollider->friction = 1.0f;
  boxCollider->linearDamping = 0.1f;
  boxCollider->angularDamping = 0.1f;
  c8EmAsm_entityEndMutComponent(world, boxEntity, colliderId, true);

  // Let the box settle on the slope (240 iterations = 4 seconds)
  simulatePhysics(240);

  verifyBodyIsAwake(boxEntity, false);

  // Get the settled position
  const auto *settledPos =
    static_cast<const Position *>(c8EmAsm_entityGetComponent(world, boxEntity, positionId));
  ASSERT_NE(settledPos, nullptr);
  float settledX = settledPos->x;
  float settledY = settledPos->y;

  // Reduce friction significantly
  boxCollider =
    static_cast<PhysicsCollider *>(c8EmAsm_entityStartMutComponent(world, slopeEntity, colliderId));
  boxCollider->friction = 0.001f;
  c8EmAsm_entityEndMutComponent(world, slopeEntity, colliderId, true);

  // Verify object is reactivated after friction change
  c8EmAsm_progressWorld(world, kDeltaTime);
  verifyBodyIsAwake(boxEntity, true);

  const auto *finalPos =
    static_cast<const Position *>(c8EmAsm_entityGetComponent(world, boxEntity, positionId));
  ASSERT_NE(finalPos, nullptr);

  simulatePhysics(120);

  // Verify that the box slides down the slope
  ASSERT_LT(finalPos->x, settledX - 0.1f);
  ASSERT_LT(finalPos->y, settledY - 0.1f);

  c8EmAsm_deleteEntity(world, boxEntity);
  c8EmAsm_deleteEntity(world, slopeEntity);
}

// Test scaling an entity with locked Y position and verifying it stays locked with offsets
TEST_F(PhysicsTest, TestOffsetAndLockedYPositionWithScaling) {
  initializeWorldWithStandardComponents();

  int colliderId = c8EmAsm_getColliderComponentId(world);
  int positionId = ecs_id(Position);
  int scaleId = ecs_id(Scale);

  // Create a dynamic entity with locked Y position
  constexpr float initialY = 5.0f;
  ecs_entity_t entity = createTestEntity({0.0f, initialY, 0.0f}, {1.0f, 1.0f, 1.0f});
  ASSERT_NE(entity, 0);

  auto *collider =
    static_cast<PhysicsCollider *>(c8EmAsm_entityStartMutComponent(world, entity, colliderId));
  setDefaultCollider(collider);
  collider->mass = 1.0f;
  collider->type = COLLIDER_DYNAMIC;
  collider->shape = BOX_SHAPE;
  collider->lockYPosition = true;
  collider->offsetY = 1.0f;
  c8EmAsm_entityEndMutComponent(world, entity, colliderId, true);

  ASSERT_TRUE(c8EmAsm_entityHasComponent(world, entity, colliderId));

  // Ensure object is not moving before scaling
  simulatePhysics(1);

  const auto *initialPos =
    static_cast<const Position *>(c8EmAsm_entityGetComponent(world, entity, positionId));
  ASSERT_NE(initialPos, nullptr);
  EXPECT_FLOAT_EQ(initialPos->y, initialY);

  // Scale up and down repeatedly
  constexpr int numScaleIterations = 10;
  for (int i = 0; i < numScaleIterations; i++) {
    // Scale up
    auto *entityScale =
      static_cast<Scale *>(c8EmAsm_entityStartMutComponent(world, entity, scaleId));
    entityScale->x = 2.0f;
    entityScale->y = 2.0f;
    entityScale->z = 2.0f;
    c8EmAsm_entityEndMutComponent(world, entity, scaleId, true);

    simulatePhysics(1);

    // Verify Y position is still locked
    const auto *scaledUpPos =
      static_cast<const Position *>(c8EmAsm_entityGetComponent(world, entity, positionId));
    ASSERT_NE(scaledUpPos, nullptr);
    EXPECT_FLOAT_EQ(scaledUpPos->y, initialY);

    // Scale down
    entityScale = static_cast<Scale *>(c8EmAsm_entityStartMutComponent(world, entity, scaleId));
    entityScale->x = 1.0f;
    entityScale->y = 1.0f;
    entityScale->z = 1.0f;
    c8EmAsm_entityEndMutComponent(world, entity, scaleId, true);

    simulatePhysics(1);

    // Verify Y position is still locked
    const auto *scaledDownPos =
      static_cast<const Position *>(c8EmAsm_entityGetComponent(world, entity, positionId));
    ASSERT_NE(scaledDownPos, nullptr);
    EXPECT_FLOAT_EQ(scaledDownPos->y, initialY);
  }

  // Final verification after all scaling operations
  const auto *finalPos =
    static_cast<const Position *>(c8EmAsm_entityGetComponent(world, entity, positionId));
  ASSERT_NE(finalPos, nullptr);
  EXPECT_FLOAT_EQ(finalPos->y, initialY);

  c8EmAsm_deleteEntity(world, entity);
}

// Test that rotational offsets work correctly
TEST_F(PhysicsTest, TestRotationOffset) {
  initializeWorldWithStandardComponents();

  int rotationId = ecs_id(Rotation);
  int colliderId = c8EmAsm_getColliderComponentId(world);

  // Create a kinematic entity with a rotation offset
  ecs_entity_t entity = createTestEntity({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
  ASSERT_NE(entity, 0);

  auto *rotation =
    static_cast<Rotation *>(c8EmAsm_entityStartMutComponent(world, entity, rotationId));
  rotation->x = 0.0f;
  rotation->y = 0.0f;
  rotation->z = 0.0f;
  rotation->w = 1.0f;
  c8EmAsm_entityEndMutComponent(world, entity, rotationId, true);

  auto *collider =
    static_cast<PhysicsCollider *>(c8EmAsm_entityStartMutComponent(world, entity, colliderId));
  setDefaultCollider(collider);
  collider->type = COLLIDER_KINEMATIC;
  collider->shape = BOX_SHAPE;
  collider->width = 2.0f;
  collider->height = 1.0f;
  collider->depth = 1.0f;

  // Set rotation offset for 90 degrees around Y axis
  auto rotationOffset = Quaternion::yDegrees(90.0f);
  collider->offsetQuaternionX = rotationOffset.x();
  collider->offsetQuaternionY = rotationOffset.y();
  collider->offsetQuaternionZ = rotationOffset.z();
  collider->offsetQuaternionW = rotationOffset.w();

  c8EmAsm_entityEndMutComponent(world, entity, colliderId, true);

  ASSERT_TRUE(c8EmAsm_entityHasComponent(world, entity, colliderId));

  simulatePhysics(2);

  // The entity's rotation should remain identity, but the physics body should have the offset
  // applied
  const auto *entityRotation =
    static_cast<const Rotation *>(c8EmAsm_entityGetComponent(world, entity, rotationId));
  ASSERT_NE(entityRotation, nullptr);
  EXPECT_FLOAT_EQ(entityRotation->x, 0.0f);
  EXPECT_FLOAT_EQ(entityRotation->y, 0.0f);
  EXPECT_FLOAT_EQ(entityRotation->z, 0.0f);
  EXPECT_FLOAT_EQ(entityRotation->w, 1.0f);

  const auto *colliderData =
    static_cast<const PhysicsCollider *>(c8EmAsm_entityGetComponent(world, entity, colliderId));
  ASSERT_NE(colliderData, nullptr);
  ASSERT_NE(colliderData->bodyPtr, nullptr);

  PhysicsContext *ctx = getPhysicsContext(world);
  ASSERT_NE(ctx, nullptr);
  const auto bodyTransform = ctx->bodyInterface->GetWorldTransform(*colliderData->bodyPtr);
  const auto bodyRotation = bodyTransform.GetQuaternion();

  // The physics body should have the rotation offset applied (90 degrees around Y axis)
  auto expectedRotation = Quaternion::yDegrees(90.0f);
  EXPECT_FLOAT_EQ(bodyRotation.GetX(), expectedRotation.x());
  EXPECT_FLOAT_EQ(bodyRotation.GetY(), expectedRotation.y());
  EXPECT_FLOAT_EQ(bodyRotation.GetZ(), expectedRotation.z());
  EXPECT_FLOAT_EQ(bodyRotation.GetW(), expectedRotation.w());

  c8EmAsm_deleteEntity(world, entity);
}
