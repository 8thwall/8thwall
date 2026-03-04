#include "bzl/inliner/rules2.h"

cc_binary {
  name = "ecs-js";
  deps = {
    "@flecs//:flecs",
    "//c8:string",
    "//c8:symbol-visibility",
    "//c8/stats:scope-timer",
    "//c8:vector",
    "//c8:set",
    ":physics",
  };
  target_compatible_with = {
    "@platforms//cpu:wasm32",
  };
}
cc_end(0x76c0f01a);

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "c8/ecs/physics.h"
#include "c8/set.h"
#include "c8/symbol-visibility.h"
#include "c8/vector.h"
#include "flecs.h"

using namespace c8;

extern "C" {

// WORLD

struct WorldContext {
  PhysicsContext *physicsContext = nullptr;
  ecs_entity_t SelfDisabled;
  ecs_entity_t objectComponent;
  ecs_entity_t RootInstance;
  int uniqueNameCounter;
};

void cleanupWorldContext(void *voidCtx) {
  auto ctx = reinterpret_cast<WorldContext *>(voidCtx);
  if (ctx->physicsContext != nullptr) {
    cleanupPhysicsContext(ctx->physicsContext);
  }
  delete ctx;
}

WorldContext *getWorldContext(ecs_world_t *world) {
  return reinterpret_cast<WorldContext *>(ecs_get_ctx(world));
}

PhysicsContext *getPhysicsContext(ecs_world_t *world) {
  return getWorldContext(world)->physicsContext;
}

ecs_entity_t getSelfDisabled(ecs_world_t *world) { return getWorldContext(world)->SelfDisabled; }

ecs_entity_t getRootInstance(ecs_world_t *world) { return getWorldContext(world)->RootInstance; }

void setUniqueName(ecs_world_t *world, ecs_entity_t entity) {
  auto ctx = getWorldContext(world);
  int count = ctx->uniqueNameCounter;
  ecs_set_name(world, entity, std::to_string(count).c_str());
  ctx->uniqueNameCounter += 1;
}

void entityDeleteCallback(ecs_iter_t *it) {
  for (auto i = 0; i < it->count; ++i) {
#ifdef __EMSCRIPTEN__
    EM_ASM_({ globalThis._ecsDeleteEntityCallback($0); }, it->entities[i]);
#endif
  }
}

void setupEntityDeleteObserver(ecs_world_t *world, ecs_entity_t objectComponent) {
  ecs_query_desc_t queryDesc = {
    .terms = {{.id = objectComponent}, {.id = EcsDisabled, .oper = EcsOptional}}};
  ecs_observer_desc_t desc{
    .query = queryDesc,
    .events = {EcsOnRemove},
    .callback = &entityDeleteCallback,
  };
  ecs_observer_init(world, &desc);
}

C8_PUBLIC
ecs_world_t *c8EmAsm_createWorld() {
  auto world = ecs_init();
  auto ctx = new WorldContext;
  ctx->uniqueNameCounter = 1;
  ctx->SelfDisabled = ecs_new(world);
  ctx->RootInstance = ecs_new(world);
  ecs_set_ctx(world, (void *)ctx, cleanupWorldContext);
  return world;
}

C8_PUBLIC
void c8EmAsm_initWithComponents(
  ecs_world_t *world,
  int positionComponent,
  int scaleComponent,
  int rotationComponent,
  int objectComponent) {

  WorldContext *ctx = getWorldContext(world);
  ctx->physicsContext =
    initPhysicsContext(world, positionComponent, scaleComponent, rotationComponent);
  ctx->objectComponent = objectComponent;
  setupEntityDeleteObserver(world, objectComponent);
}

C8_PUBLIC
void c8EmAsm_deleteWorld(ecs_world_t *world) { ecs_fini(world); }

C8_PUBLIC
bool c8EmAsm_progressWorld(ecs_world_t *world, float deltaTime) {
  ecs_progress(world, deltaTime);

  PhysicsContext *ctx = getPhysicsContext(world);
  if (ctx != nullptr && ctx->enabled) {
    stepPhysics(world, ctx, deltaTime);
    return 1;
  } else {
    return 0;
  }
}

// COMPONENT

C8_PUBLIC
int c8EmAsm_createComponent(ecs_world_t *world, int size, int alignment) {
  ecs_type_info_t info = {
    .size = size,
    .alignment = alignment,
  };

  ecs_component_desc_t desc = {
    .type = info,
  };

  int component = ecs_component_init(world, &desc);

  ecs_add_pair(world, component, EcsOnInstantiate, EcsInherit);

  return component;
}

// ENTITY

C8_PUBLIC
ecs_entity_t c8EmAsm_createEntity(ecs_world_t *world) { return ecs_new(world); }

C8_PUBLIC
ecs_entity_t c8EmAsm_createPrefab(ecs_world_t *world) {
  ecs_entity_t prefab = ecs_new_w_id(world, EcsPrefab);
  return prefab;
}

C8_PUBLIC
ecs_entity_t c8EmAsm_createPrefabChild(ecs_world_t *world, ecs_entity_t prefab) {
  // Note(dale): this is a dummy entity that stores all the components that are added to the
  // prefab child. When an instance of the prefab is created, the
  // instance child entity will then inherit the components from the base entity.
  ecs_entity_t baseChild = ecs_new_w_id(world, EcsPrefab);
  ecs_entity_t prefabChild = ecs_new_w_id(world, EcsPrefab);

  ecs_add_pair(world, prefabChild, EcsIsA, baseChild);
  ecs_add_pair(world, prefabChild, EcsSlotOf, prefab);

  // Note(dale): in order for EcsSlotOf to work, the prefab child must be named
  setUniqueName(world, prefabChild);

  return prefabChild;
}

C8_PUBLIC
ecs_entity_t c8EmAsm_getBaseEntity(ecs_world_t *world, ecs_entity_t prefabChild) {
  return ecs_get_target(world, prefabChild, EcsIsA, 0);
}

C8_PUBLIC
ecs_entity_t c8EmAsm_spawnPrefab(ecs_world_t *world, ecs_entity_t prefab) {
  ecs_entity_t RootInstance = getRootInstance(world);
  ecs_entity_t instance = ecs_new_w_pair(world, EcsIsA, prefab);
  ecs_add_id(world, instance, RootInstance);
  return instance;
}

ecs_entity_t getParentPrefabEid(ecs_world_t *world, ecs_entity_t prefabChild) {
  ecs_entity_t current = prefabChild;
  ecs_entity_t lastValidPrefab = current;

  while (current) {
    ecs_entity_t parent = ecs_get_parent(world, current);
    if (!parent) {
      return lastValidPrefab;
    }

    if (!ecs_has_id(world, parent, EcsPrefab)) {
      return lastValidPrefab;
    }

    lastValidPrefab = parent;
    current = parent;
  }
  return lastValidPrefab;
}

C8_PUBLIC
ecs_entity_t c8EmAsm_getInstanceChild(
  ecs_world_t *world, ecs_entity_t instance, ecs_entity_t prefabChild) {
  // if the prefab-root is given this function will return 0n
  ecs_entity_t result = ecs_get_target(world, instance, prefabChild, 0);
  if (ecs_has_id(world, prefabChild, EcsPrefab) && !result) {
    auto prefabRoot = getParentPrefabEid(world, prefabChild);
    if (prefabRoot && prefabChild && prefabRoot == prefabChild) {
      return instance;
    }
  }

  return result;
}

ecs_entity_t getEntityOrBaseEntity(ecs_world_t *world, ecs_entity_t entity, int component) {
  // Note(dale): if the entity is a collider component, we want to return the entity itself
  // because the collider component is not inheritable.
  auto ctx = getWorldContext(world);
  if (ctx->physicsContext && component == ctx->physicsContext->colliderComponent) {
    return entity;
  }

  // If entity is a prefab and has a prefab parent, we can assume it is a prefab child
  auto isPrefab = ecs_has_id(world, entity, EcsPrefab);
  ecs_entity_t parent = ecs_get_parent(world, entity);
  auto hasPrefabParent = parent && ecs_has_id(world, parent, EcsPrefab);

  if (isPrefab && hasPrefabParent) {
    ecs_entity_t baseChild = ecs_get_target(world, entity, EcsIsA, 0);
    if (baseChild) {
      return baseChild;
    }
  }

  return entity;
}

C8_PUBLIC
void c8EmAsm_deleteEntity(ecs_world_t *world, ecs_entity_t entity) { ecs_delete(world, entity); }

C8_PUBLIC
void c8EmAsm_entityAddComponent(ecs_world_t *world, ecs_entity_t entity, int component) {
  ecs_entity_t entityToUse = getEntityOrBaseEntity(world, entity, component);
  ecs_add_id(world, entityToUse, component);
}

C8_PUBLIC
bool c8EmAsm_entityHasComponent(ecs_world_t *world, ecs_entity_t entity, int component) {
  ecs_entity_t entityToUse = getEntityOrBaseEntity(world, entity, component);
  return ecs_has_id(world, entityToUse, component);
}

C8_PUBLIC
const void *c8EmAsm_entityGetComponent(ecs_world_t *world, ecs_entity_t entity, int component) {
  ecs_entity_t entityToUse = getEntityOrBaseEntity(world, entity, component);
  return ecs_get_id(world, entityToUse, component);
}

C8_PUBLIC
void *c8EmAsm_entityGetMutComponent(ecs_world_t *world, ecs_entity_t entity, int component) {
  ecs_entity_t entityToUse = getEntityOrBaseEntity(world, entity, component);
  if (!ecs_has_id(world, entityToUse, component)) {
    return nullptr;
  }
  auto res = ecs_ensure_id(world, entityToUse, component);
  ecs_modified_id(world, entityToUse, component);
  return res;
}

C8_PUBLIC
void *c8EmAsm_entityStartMutComponent(ecs_world_t *world, ecs_entity_t entity, int component) {
  ecs_entity_t entityToUse = getEntityOrBaseEntity(world, entity, component);
  ecs_defer_begin(world);
  auto res = ecs_ensure_id(world, entityToUse, component);
  return res;
}

C8_PUBLIC
void *c8EmAsm_entityStartMutExistingComponent(
  ecs_world_t *world, ecs_entity_t entity, int component) {
  ecs_entity_t entityToUse = getEntityOrBaseEntity(world, entity, component);
  if (!ecs_has_id(world, entityToUse, component)) {
    return nullptr;
  }
  return c8EmAsm_entityStartMutComponent(world, entityToUse, component);
}

C8_PUBLIC
void c8EmAsm_entityEndMutComponent(
  ecs_world_t *world, ecs_entity_t entity, int component, bool modified) {
  ecs_entity_t entityToUse = getEntityOrBaseEntity(world, entity, component);
  if (modified) {
    ecs_modified_id(world, entityToUse, component);
  }
  ecs_defer_end(world);
}

C8_PUBLIC
void c8EmAsm_entityComponentModified(ecs_world_t *world, ecs_entity_t entity, int component) {
  ecs_entity_t entityToUse = getEntityOrBaseEntity(world, entity, component);
  ecs_modified_id(world, entityToUse, component);
}

C8_PUBLIC
void c8EmAsm_entityRemoveComponent(ecs_world_t *world, ecs_entity_t entity, int component) {
  ecs_entity_t entityToUse = getEntityOrBaseEntity(world, entity, component);
  ecs_remove_id(world, entityToUse, component);
}

ecs_entity_t getParentInstance(ecs_world_t *world, ecs_entity_t entity) {
  ecs_entity_t RootInstance = getRootInstance(world);
  if (!entity) {
    return 0;
  }
  if (ecs_has_id(world, entity, RootInstance)) {
    return entity;
  }
  ecs_entity_t parent = ecs_get_parent(world, entity);
  if (!parent) {
    return 0;
  }
  return getParentInstance(world, parent);
}

C8_PUBLIC
ecs_entity_t c8EmAsm_getEidFromComponentState(
  ecs_world_t *world, ecs_entity_t entity, const ecs_entity_t *ptr) {
  if (!ptr || !*ptr) {
    return 0;
  }

  if (!ecs_has_id(world, *ptr, EcsPrefab)) {
    return *ptr;
  }

  ecs_entity_t instance = getParentInstance(world, entity);
  ecs_entity_t prefab = ecs_get_target(world, instance, EcsIsA, 0);
  ecs_entity_t instanceChild = c8EmAsm_getInstanceChild(world, instance, *ptr);

  const bool isPrefabReference =
    ecs_has_id(world, *ptr, EcsPrefab) && getParentPrefabEid(world, *ptr) == prefab;

  if (prefab && isPrefabReference && instanceChild) {
    return instanceChild;
  }

  return *ptr;
}

void entitySetEnabledHelper(ecs_world_t *world, ecs_entity_t entity, bool isParentEnabled);

C8_PUBLIC
void c8EmAsm_entitySetParent(ecs_world_t *world, ecs_entity_t entity, ecs_entity_t parent) {
  if (parent == 0) {
    ecs_remove_pair(world, entity, EcsChildOf, EcsWildcard);
  } else {
    ecs_add_pair(world, entity, EcsChildOf, parent);
  }
  bool isParentEnabled = parent == 0 || !ecs_has_id(world, parent, EcsDisabled);
  entitySetEnabledHelper(world, entity, isParentEnabled);
}

C8_PUBLIC
ecs_entity_t c8EmAsm_entityGetParent(ecs_world_t *world, ecs_entity_t entity) {
  return ecs_get_parent(world, entity);
}

void fillWorldChildren(ecs_world_t *world, std::vector<ecs_entity_t> &children) {
  ecs_iter_t it = ecs_children(world, 0);
  auto ctx = getWorldContext(world);

  while (ecs_children_next(&it)) {
    for (int i = 0; i < it.count; i++) {
      if (
        ecs_has_id(world, it.entities[i], ctx->objectComponent)
        && !ecs_has_id(world, it.entities[i], EcsPrefab)) {
        children.push_back(it.entities[i]);
      }
    }
  }
}

void fillEntityChildren(
  ecs_world_t *world, ecs_entity_t parent, std::vector<ecs_entity_t> &children) {
  ecs_iter_t it = ecs_children(world, parent);
  while (ecs_children_next(&it)) {
    for (int i = 0; i < it.count; i++) {
      children.push_back(it.entities[i]);
    }
  }
}

C8_PUBLIC
int c8EmAsm_entityGetChildren(ecs_world_t *world, ecs_entity_t parent, ecs_entity_t **listPtr) {
  std::vector<ecs_entity_t> children;

  if (parent == 0) {
    fillWorldChildren(world, children);
  } else {
    fillEntityChildren(world, parent, children);
  }
  // NOTE(christoph): We pass ownership of the array pointer out to Javascript so that it can
  // consume it, then free it after.
  ecs_entity_t *array = new ecs_entity_t[children.size()];
  std::copy(children.begin(), children.end(), array);
  *listPtr = array;
  return children.size();
}

// NOTE(cindyhu): we have two flags to manage disabled states:
// 1. SelfDisabled: the entity is directly disabled
// 2. EcsDisabled: the true disabled state of the entity to be used in matching queries
void entitySetEnabledHelper(ecs_world_t *world, ecs_entity_t entity, bool isParentEnabled) {
  bool isEnabled = isParentEnabled && !ecs_has_id(world, entity, getSelfDisabled(world));

  // early out if the entity is already in the correct disabled state
  if (isEnabled == !ecs_has_id(world, entity, EcsDisabled)) {
    return;
  }

  ecs_enable(world, entity, isEnabled);
  // Create an iterator for all children of this entity
  ecs_iter_t it = ecs_children(world, entity);

  // Traverse through each child
  while (ecs_children_next(&it)) {
    for (int i = 0; i < it.count; i++) {
      ecs_entity_t child = it.entities[i];

      // Recursively enable/disable each child entity
      entitySetEnabledHelper(world, child, isEnabled);
    }
  }
}

C8_PUBLIC
void c8EmAsm_entitySetEnabled(ecs_world_t *world, ecs_entity_t entity, bool enabled) {
  ecs_entity_t SelfDisabled = getSelfDisabled(world);
  if (enabled) {
    ecs_remove_id(world, entity, SelfDisabled);
  } else {
    ecs_add_id(world, entity, SelfDisabled);
  }
  ecs_entity_t parent = ecs_get_parent(world, entity);
  bool isParentEnabled = parent == 0 || !ecs_has_id(world, parent, EcsDisabled);
  entitySetEnabledHelper(world, entity, isParentEnabled);
}

C8_PUBLIC
bool c8EmAsm_entityIsEnabled(ecs_world_t *world, ecs_entity_t entity) {
  return !ecs_has_id(world, entity, EcsDisabled);
}

C8_PUBLIC
bool c8EmAsm_entityIsSelfDisabled(ecs_world_t *world, ecs_entity_t entity) {
  ecs_entity_t SelfDisabled = getSelfDisabled(world);
  return ecs_has_id(world, entity, SelfDisabled);
}

C8_PUBLIC
int c8EmAsm_getDisabledComponentId(ecs_world_t *world) { return getSelfDisabled(world); }

// Physics

C8_PUBLIC
bool c8EmAsm_togglePhysics(ecs_world_t *world, bool enabled) {
  auto ctx = getPhysicsContext(world);
  if (ctx == nullptr) {
    return false;
  }
  ctx->enabled = enabled;
  return true;
}

C8_PUBLIC
bool c8EmAsm_setWorldGravity(ecs_world_t *world, float gravity) {
  auto ctx = getPhysicsContext(world);
  return setWorldGravity(ctx, gravity);
}

C8_PUBLIC
float c8EmAsm_getWorldGravity(ecs_world_t *world) {
  auto ctx = getPhysicsContext(world);
  return getWorldGravity(ctx);
}

C8_PUBLIC
bool c8EmAsm_setLinearVelocityToEntity(
  ecs_world_t *world, ecs_entity_t entity, float x, float y, float z) {
  auto ctx = getPhysicsContext(world);
  return setLinearVelocity(world, ctx, entity, x, y, z);
}

C8_PUBLIC
float *c8EmAsm_getLinearVelocityFromEntity(ecs_world_t *world, ecs_entity_t entity) {
  auto ctx = getPhysicsContext(world);
  return getLinearVelocity(world, ctx, entity);
}

C8_PUBLIC
bool c8EmAsm_setAngularVelocityToEntity(
  ecs_world_t *world, ecs_entity_t entity, float x, float y, float z) {
  auto ctx = getPhysicsContext(world);
  return setAngularVelocity(world, ctx, entity, x, y, z);
}

C8_PUBLIC
float *c8EmAsm_getAngularVelocityFromEntity(ecs_world_t *world, ecs_entity_t entity) {
  auto ctx = getPhysicsContext(world);
  return getAngularVelocity(world, ctx, entity);
}

C8_PUBLIC
bool c8EmAsm_applyForceToEntity(
  ecs_world_t *world, ecs_entity_t entity, float x, float y, float z) {
  auto ctx = getPhysicsContext(world);
  return applyCentralForce(world, ctx, entity, x, y, z);
}

C8_PUBLIC
bool c8EmAsm_applyImpulseToEntity(
  ecs_world_t *world, ecs_entity_t entity, float x, float y, float z) {
  auto ctx = getPhysicsContext(world);
  return applyImpulse(world, ctx, entity, x, y, z);
}

C8_PUBLIC
bool c8EmAsm_applyTorqueToEntity(
  ecs_world_t *world, ecs_entity_t entity, float x, float y, float z) {
  auto ctx = getPhysicsContext(world);
  return applyTorque(world, ctx, entity, x, y, z);
}

C8_PUBLIC
uint32_t c8EmAsm_registerConvexShape(ecs_world_t *world, float *verticesPtr, int verticesLength) {
  auto ctx = getPhysicsContext(world);
  return registerConvexShape(ctx, verticesPtr, verticesLength);
}

C8_PUBLIC
uint32_t c8EmAsm_registerCompoundShape(
  ecs_world_t *world,
  float *verticesPtr,
  int verticesLength,
  uint32_t *indicesPtr,
  int indicesLength) {
  auto ctx = getPhysicsContext(world);
  return registerCompoundShape(ctx, verticesPtr, verticesLength, indicesPtr, indicesLength);
}

C8_PUBLIC
void c8EmAsm_unregisterConvexShape(ecs_world_t *world, uint32_t shapeId) {
  auto ctx = getPhysicsContext(world);
  unregisterConvexShape(ctx, shapeId);
}

// QUERY

ecs_query_desc_t createQueryDesc(
  const uint32_t *components, int component_count, bool forceCaching = false) {
  ecs_query_desc_t desc = {.cache_kind = forceCaching ? EcsQueryCacheAll : EcsQueryCacheAuto};
  for (auto i = 0; i < component_count && i < FLECS_TERM_COUNT_MAX; ++i) {
    desc.terms[i] = ecs_term_t{.id = components[i]};
  }

  desc.terms[component_count] = ecs_term_t{.id = EcsDisabled, .oper = EcsNot};
  return desc;
}

ecs_observer_desc_t createObserverDesc(
  const uint32_t *components, int component_count, const ecs_entity_t *events, int event_count) {

  ecs_query_desc_t queryDesc = createQueryDesc(components, component_count);

  ecs_observer_desc_t desc{};
  desc.query = queryDesc;

  for (int i = 0; i < event_count && i < FLECS_EVENT_DESC_MAX; i++) {
    desc.events[i] = events[i];
  }

  return desc;
}

C8_PUBLIC
ecs_query_t *c8EmAsm_createQuery(
  ecs_world_t *world, const uint32_t *components, int component_count, bool forceCaching = false) {
  ecs_query_desc_t desc = createQueryDesc(components, component_count);
  auto query = ecs_query_init(world, &desc);
  return query;
}

C8_PUBLIC
void c8EmAsm_deleteQuery(ecs_query_t *query) { ecs_query_fini(query); }

C8_PUBLIC
int c8EmAsm_executeQuery(
  ecs_world_t *world,
  ecs_query_t *query,
  ecs_iter_t **iterPtr,
  const ecs_entity_t **listPtr,
  void **dataPtrs = nullptr,
  int fieldCount = 0) {
  ecs_iter_t *iter;
  if (*iterPtr == nullptr) {
    iter = new ecs_iter_t;
    *iter = ecs_query_iter(world, query);
    *iterPtr = iter;
  } else {
    iter = *iterPtr;
  }

  auto matched = ecs_query_next(iter);
  if (!matched) {
    free(iter);
    return -1;
  }

  *listPtr = iter->entities;

  if (dataPtrs != nullptr) {
    for (auto i = 0; i < fieldCount; ++i) {
      dataPtrs[i] = ecs_field_w_size(iter, ecs_field_size(iter, i), i);
    }
  }

  return iter->count;
}

C8_PUBLIC
int c8EmAsm_countQuery(ecs_world_t *world, ecs_query_t *query) {
  int count = 0;

  ecs_iter_t it = ecs_query_iter(world, query);

  while (ecs_query_next(&it)) {
    count += it.count;
  }

  return count;
}

struct ObserverContext {
  TreeSet<ecs_entity_t> newEntities;
  std::vector<ecs_entity_t> seenEntities;
  ecs_entity_t observer;
};

void handleObserveEvent(ecs_iter_t *it) {
  auto ctx = static_cast<ObserverContext *>(it->ctx);
  for (auto i = 0; i < it->count; ++i) {
    ctx->newEntities.insert(it->entities[i]);
  }
}

void cleanupObserverContext(void *ctx) {
  auto observer = static_cast<ObserverContext *>(ctx);
  delete observer;
}

ObserverContext *createObserver(
  ecs_world_t *world, ecs_entity_t event, const uint32_t *components, int component_count) {
  auto ctx = new ObserverContext;
  ecs_observer_desc_t desc = createObserverDesc(components, component_count, &event, 1);
  desc.callback = &handleObserveEvent;
  desc.ctx = (void *)ctx;
  desc.ctx_free = cleanupObserverContext;

  auto observer = ecs_observer_init(world, &desc);
  ctx->observer = observer;

  return ctx;
}

C8_PUBLIC
ObserverContext *c8EmAsm_createAddObserver(
  ecs_world_t *world, const uint32_t *components, int component_count) {
  return createObserver(world, EcsOnAdd, components, component_count);
}

C8_PUBLIC
ObserverContext *c8EmAsm_createRemoveObserver(
  ecs_world_t *world, const uint32_t *components, int component_count) {
  return createObserver(world, EcsOnRemove, components, component_count);
}

C8_PUBLIC
ObserverContext *c8EmAsm_createChangeObserver(
  ecs_world_t *world, const uint32_t *components, int component_count) {
  return createObserver(world, EcsOnSet, components, component_count);
}

C8_PUBLIC
int c8EmAsm_flushObserver(ObserverContext *ctx, ecs_entity_t **listPtr) {
  ctx->seenEntities.clear();
  ctx->seenEntities.reserve(ctx->newEntities.size());
  for (auto entity : ctx->newEntities) {
    ctx->seenEntities.push_back(entity);
  }
  ctx->newEntities.clear();
  *listPtr = ctx->seenEntities.data();
  return ctx->seenEntities.size();
}

struct LifecycleContext {
  TreeSet<ecs_entity_t> newAddEntities;
  std::vector<ecs_entity_t> seenAddEntities;
  TreeSet<ecs_entity_t> newChangeEntities;
  std::vector<ecs_entity_t> seenChangeEntities;
  TreeSet<ecs_entity_t> newRemoveEntities;
  std::vector<ecs_entity_t> seenRemoveEntities;
  // Note(Dale): Only add to newAddEntities if we haven't seen this entity added before
  // This prevents duplicate enter callbacks when entity transitions from
  // inheriting to owning a component
  TreeSet<ecs_entity_t> everAddedEntities;
  ecs_entity_t observer;
  ecs_entity_t flushOn = 0;
};

void handleLifeCycleEvent(ecs_iter_t *it) {
  auto ctx = static_cast<LifecycleContext *>(it->ctx);
  ecs_entity_t event = it->event;
  if (event == EcsOnAdd) {
    for (auto i = 0; i < it->count; ++i) {
      ecs_entity_t entity = it->entities[i];
      if (!ctx->everAddedEntities.contains(entity)) {
        ctx->newAddEntities.insert(entity);
        ctx->everAddedEntities.insert(entity);
      }
    }
  } else if (event == EcsOnSet) {
    for (auto i = 0; i < it->count; ++i) {
      if (!ctx->newAddEntities.contains(it->entities[i])) {
        ctx->newChangeEntities.insert(it->entities[i]);
      }
    }
  } else if (event == EcsOnRemove) {
    for (auto i = 0; i < it->count; ++i) {
      ecs_entity_t entity = it->entities[i];
      int removed = ctx->newAddEntities.erase(entity);
      ctx->newChangeEntities.erase(entity);
      if (!removed) {
        ctx->newRemoveEntities.insert(entity);
      }
      ctx->everAddedEntities.erase(entity);
    }
  }
}

void cleanupLifecycleContext(void *ctx) {
  auto lifecycle = static_cast<LifecycleContext *>(ctx);
  delete lifecycle;
}

C8_PUBLIC
LifecycleContext *c8EmAsm_createLifecycleObserver(
  ecs_world_t *world, const uint32_t *components, int component_count) {
  auto ctx = new LifecycleContext;

  ecs_entity_t events[3] = {EcsOnAdd, EcsOnSet, EcsOnRemove};
  ecs_observer_desc_t desc = createObserverDesc(components, component_count, events, 3);
  desc.callback = &handleLifeCycleEvent;
  desc.ctx = (void *)ctx;
  desc.ctx_free = cleanupLifecycleContext;

  auto observer = ecs_observer_init(world, &desc);
  ctx->observer = observer;

  return ctx;
}

void flushEntities(TreeSet<ecs_entity_t> &newEntities, std::vector<ecs_entity_t> &seenEntities) {
  seenEntities.clear();
  seenEntities.reserve(newEntities.size());

  for (auto entity : newEntities) {
    seenEntities.push_back(entity);
  }

  newEntities.clear();
}

void maybeFlushLifecycleObserver(LifecycleContext *ctx, ecs_entity_t event) {
  if (!ctx->flushOn) {
    ctx->flushOn = event;
  }

  if (ctx->flushOn != event) {
    return;
  }

  flushEntities(ctx->newAddEntities, ctx->seenAddEntities);
  flushEntities(ctx->newChangeEntities, ctx->seenChangeEntities);
  flushEntities(ctx->newRemoveEntities, ctx->seenRemoveEntities);
}

C8_PUBLIC
int c8EmAsm_accessLifecycleAdded(LifecycleContext *ctx, ecs_entity_t **addPtr) {
  maybeFlushLifecycleObserver(ctx, EcsOnAdd);
  *addPtr = ctx->seenAddEntities.data();
  return ctx->seenAddEntities.size();
}

C8_PUBLIC
int c8EmAsm_accessLifecycleChanged(LifecycleContext *ctx, ecs_entity_t **changedPtr) {
  maybeFlushLifecycleObserver(ctx, EcsOnSet);
  *changedPtr = ctx->seenChangeEntities.data();
  return ctx->seenChangeEntities.size();
}

C8_PUBLIC
int c8EmAsm_accessLifecycleRemoved(LifecycleContext *ctx, ecs_entity_t **removePtr) {
  maybeFlushLifecycleObserver(ctx, EcsOnRemove);
  *removePtr = ctx->seenRemoveEntities.data();
  return ctx->seenRemoveEntities.size();
}

C8_PUBLIC
int c8EmAsm_echo(int input) { return input; }

C8_PUBLIC
int c8EmAsm_getColliderComponentId(ecs_world_t *world) {
  return getPhysicsContext(world)->colliderComponent;
}

C8_PUBLIC
int c8EmAsm_getCollisionStartEvents(ecs_world_t *world, ecs_entity_t **listPtr) {
  *listPtr = getPhysicsContext(world)->collisionStarts.data();
  return getPhysicsContext(world)->collisionStarts.size();
}

C8_PUBLIC
int c8EmAsm_getCollisionEndEvents(ecs_world_t *world, ecs_entity_t **listPtr) {
  *listPtr = getPhysicsContext(world)->collisionEnds.data();
  return getPhysicsContext(world)->collisionEnds.size();
}

}  // extern "C"
