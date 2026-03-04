#pragma once

#include <cstddef>
#include <cstdint>

extern "C" {

struct ecs_world_t;
struct ecs_query_t;
struct ecs_iter_t;
struct ObserverContext;
struct LifecycleContext;
struct PhysicsContext;
typedef uint64_t ecs_entity_t;

// World
ecs_world_t *c8EmAsm_createWorld();
void c8EmAsm_initWithComponents(
  ecs_world_t *world,
  int positionComponent,
  int scaleComponent,
  int rotationComponent,
  int objectComponent);
void c8EmAsm_deleteWorld(ecs_world_t *world);
bool c8EmAsm_progressWorld(ecs_world_t *world, float deltaTime);

// Component
int c8EmAsm_createComponent(ecs_world_t *world, int size, int alignment);

// Entity
ecs_entity_t c8EmAsm_createEntity(ecs_world_t *world);
ecs_entity_t c8EmAsm_createPrefab(ecs_world_t *world);
ecs_entity_t c8EmAsm_createPrefabChild(ecs_world_t *world, ecs_entity_t prefab);
ecs_entity_t c8EmAsm_getBaseEntity(ecs_world_t *world, ecs_entity_t prefabChild);
ecs_entity_t c8EmAsm_spawnPrefab(ecs_world_t *world, ecs_entity_t prefab);
ecs_entity_t c8EmAsm_getInstanceChild(
  ecs_world_t *world, ecs_entity_t instance, ecs_entity_t prefabChild);
void c8EmAsm_deleteEntity(ecs_world_t *world, ecs_entity_t entity);

// Entity
void c8EmAsm_entityAddComponent(ecs_world_t *world, ecs_entity_t entity, int component);
bool c8EmAsm_entityHasComponent(ecs_world_t *world, ecs_entity_t entity, int component);
const void *c8EmAsm_entityGetComponent(ecs_world_t *world, ecs_entity_t entity, int component);
void *c8EmAsm_entityGetMutComponent(ecs_world_t *world, ecs_entity_t entity, int component);
void *c8EmAsm_entityStartMutComponent(ecs_world_t *world, ecs_entity_t entity, int component);
void *c8EmAsm_entityStartMutExistingComponent(
  ecs_world_t *world, ecs_entity_t entity, int component);
void c8EmAsm_entityEndMutComponent(
  ecs_world_t *world, ecs_entity_t entity, int component, bool modified);
void c8EmAsm_entityComponentModified(ecs_world_t *world, ecs_entity_t entity, int component);
void c8EmAsm_entityRemoveComponent(ecs_world_t *world, ecs_entity_t entity, int component);

// Entity hierarchy
void c8EmAsm_entitySetParent(ecs_world_t *world, ecs_entity_t entity, ecs_entity_t parent);
ecs_entity_t c8EmAsm_entityGetParent(ecs_world_t *world, ecs_entity_t entity);
int c8EmAsm_entityGetChildren(ecs_world_t *world, ecs_entity_t parent, ecs_entity_t **listPtr);

// Entity state
void c8EmAsm_entitySetEnabled(ecs_world_t *world, ecs_entity_t entity, bool enabled);
bool c8EmAsm_entityIsEnabled(ecs_world_t *world, ecs_entity_t entity);
bool c8EmAsm_entityIsSelfDisabled(ecs_world_t *world, ecs_entity_t entity);
int c8EmAsm_getDisabledComponentId(ecs_world_t *world);

ecs_entity_t c8EmAsm_getEidFromComponentState(
  ecs_world_t *world, ecs_entity_t entity, const ecs_entity_t *ptr);

// Physics
bool c8EmAsm_togglePhysics(ecs_world_t *world, bool enabled);
bool c8EmAsm_setWorldGravity(ecs_world_t *world, float gravity);
float c8EmAsm_getWorldGravity(ecs_world_t *world);
bool c8EmAsm_setLinearVelocityToEntity(
  ecs_world_t *world, ecs_entity_t entity, float x, float y, float z);
float *c8EmAsm_getLinearVelocityFromEntity(ecs_world_t *world, ecs_entity_t entity);
bool c8EmAsm_setAngularVelocityToEntity(
  ecs_world_t *world, ecs_entity_t entity, float x, float y, float z);
float *c8EmAsm_getAngularVelocityFromEntity(ecs_world_t *world, ecs_entity_t entity);
bool c8EmAsm_applyForceToEntity(ecs_world_t *world, ecs_entity_t entity, float x, float y, float z);
bool c8EmAsm_applyImpulseToEntity(
  ecs_world_t *world, ecs_entity_t entity, float x, float y, float z);
bool c8EmAsm_applyTorqueToEntity(
  ecs_world_t *world, ecs_entity_t entity, float x, float y, float z);
uint32_t c8EmAsm_registerConvexShape(ecs_world_t *world, float *verticesPtr, int verticesLength);
void c8EmAsm_unregisterConvexShape(ecs_world_t *world, uint32_t shapeId);
int c8EmAsm_getColliderComponentId(ecs_world_t *world);
int c8EmAsm_getCollisionStartEvents(ecs_world_t *world, ecs_entity_t **listPtr);
int c8EmAsm_getCollisionEndEvents(ecs_world_t *world, ecs_entity_t **listPtr);

// Query system
ecs_query_t *c8EmAsm_createQuery(
  ecs_world_t *world, const uint32_t *components, int component_count, bool forceCaching);
void c8EmAsm_deleteQuery(ecs_query_t *query);
int c8EmAsm_executeQuery(
  ecs_world_t *world,
  ecs_query_t *query,
  ecs_iter_t **iterPtr,
  const ecs_entity_t **listPtr,
  void **dataPtrs,
  int fieldCount);
int c8EmAsm_countQuery(ecs_world_t *world, ecs_query_t *query);

// Observer system
ObserverContext *c8EmAsm_createAddObserver(
  ecs_world_t *world, const uint32_t *components, int component_count);
ObserverContext *c8EmAsm_createRemoveObserver(
  ecs_world_t *world, const uint32_t *components, int component_count);
ObserverContext *c8EmAsm_createChangeObserver(
  ecs_world_t *world, const uint32_t *components, int component_count);
int c8EmAsm_flushObserver(ObserverContext *ctx, ecs_entity_t **listPtr);

// Lifecycle observer system
LifecycleContext *c8EmAsm_createLifecycleObserver(
  ecs_world_t *world, const uint32_t *components, int component_count);
int c8EmAsm_accessLifecycleAdded(LifecycleContext *ctx, ecs_entity_t **addPtr);
int c8EmAsm_accessLifecycleChanged(LifecycleContext *ctx, ecs_entity_t **changedPtr);
int c8EmAsm_accessLifecycleRemoved(LifecycleContext *ctx, ecs_entity_t **removePtr);

// Testing / Debugging
PhysicsContext *getPhysicsContext(ecs_world_t *world);

// Utility
int c8EmAsm_echo(int input);

}  // extern "C"
