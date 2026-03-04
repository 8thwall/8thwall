#pragma once

// clang-format off
// This must be included before any other Jolt headers
#include <Jolt/Jolt.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/JobSystemSingleThreaded.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>
#include <Jolt/Physics/PhysicsSystem.h>
// clang-format on

#include "c8/ecs/physics-shared.h"
#include "c8/map.h"
#include "c8/vector.h"
#include "flecs.h"

struct PhysicsCollider {
  JPH::BodyID *bodyPtr = nullptr;
  JPH::PhysicsSystem *system = nullptr;
  float width;
  float height;
  float depth;
  float radius;
  float mass;
  float linearDamping;
  float angularDamping;
  float friction;
  float restitution;
  float gravityFactor;
  float offsetX;
  float offsetY;
  float offsetZ;
  float offsetQuaternionX;
  float offsetQuaternionY;
  float offsetQuaternionZ;
  float offsetQuaternionW;
  collider_shape_t shape;
  uint32_t simplificationMode;
  collider_type_t type;
  bool eventOnly;
  bool lockXAxis;
  bool lockYAxis;
  bool lockZAxis;
  bool lockXPosition;
  bool lockYPosition;
  bool lockZPosition;
  bool highPrecision;  // Continuous Collision Detection
};

struct Position {
  float x;
  float y;
  float z;
};

struct Scale {
  float x;
  float y;
  float z;
};

struct Rotation {
  float x;
  float y;
  float z;
  float w;
};

void cleanupPhysicsContext(PhysicsContext *ctx);

PhysicsContext *initPhysicsContext(
  ecs_world_t *world,
  ecs_entity_t positionComponent,
  ecs_entity_t scaleComponent,
  ecs_entity_t rotationComponent);

void stepPhysics(ecs_world_t *world, PhysicsContext *ctx, float deltaTime);

bool setWorldGravity(PhysicsContext *ctx, float gravity);

float getWorldGravity(PhysicsContext *ctx);

bool setLinearVelocity(
  ecs_world_t *world,
  PhysicsContext *ctx,
  ecs_entity_t entity,
  float velocityX,
  float velocityY,
  float velocityZ);

float *getLinearVelocity(ecs_world_t *world, PhysicsContext *ctx, ecs_entity_t entity);

bool setAngularVelocity(
  ecs_world_t *world,
  PhysicsContext *ctx,
  ecs_entity_t entity,
  float velocityX,
  float velocityY,
  float velocityZ);

float *getAngularVelocity(ecs_world_t *world, PhysicsContext *ctx, ecs_entity_t entity);

bool applyCentralForce(
  ecs_world_t *world,
  PhysicsContext *ctx,
  ecs_entity_t entity,
  float forceX,
  float forceY,
  float forceZ);

bool applyImpulse(
  ecs_world_t *world,
  PhysicsContext *ctx,
  ecs_entity_t entity,
  float impulseX,
  float impulseY,
  float impulseZ);

bool applyTorque(
  ecs_world_t *world,
  PhysicsContext *ctx,
  ecs_entity_t entity,
  float torqueX,
  float torqueY,
  float torqueZ);

collider_shape_t registerConvexShape(PhysicsContext *ctx, float *verticesPtr, int verticesLength);

collider_shape_t registerCompoundShape(
  PhysicsContext *ctx,
  float *verticesPtr,
  int verticesLength,
  uint32_t *indicesPtr,
  int indicesLength);

void unregisterConvexShape(PhysicsContext *ctx, collider_shape_t shapeId);
