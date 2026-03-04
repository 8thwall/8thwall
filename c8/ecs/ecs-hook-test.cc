#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":physics",
    "//c8:set",
    "//c8:string",
    "//c8:symbol-visibility",
    "//c8:vector",
    "//c8/stats:scope-timer",
    "@com_google_googletest//:gtest_main",
    "@flecs",
  };
}
cc_end(0xc2f851fc);

#include "c8/ecs/physics.h"
#include "c8/set.h"
#include "c8/symbol-visibility.h"
#include "c8/vector.h"
#include "flecs.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace c8;

ECS_COMPONENT_DECLARE(Position);
ECS_COMPONENT_DECLARE(Scale);

namespace c8 {

class EcsHookTest : public ::testing::Test {};

struct ComponentHookContext {
  TreeSet<ecs_entity_t> pendingAddEntities;
};

const char *hasComponent(ecs_world_t *world, ecs_entity_t entity, ecs_entity_t component) {
  return ecs_has_id(world, entity, component) ? "true" : "false";
}

void addPosition(ecs_world_t *world, ecs_entity_t entity, float x, float y, float z) {
  ecs_defer_begin(world);
  Position *ptr = static_cast<Position *>(ecs_ensure_id(world, entity, ecs_id(Position)));
  ptr->x = x;
  ptr->y = y;
  ptr->z = z;
  printf("\taddPosition: %f %f %f\n", ptr->x, ptr->y, ptr->z);
  ecs_modified_id(world, entity, ecs_id(Position));
  ecs_defer_end(world);
}

void addScale(ecs_world_t *world, ecs_entity_t entity, float x, float y, float z) {
  ecs_defer_begin(world);
  Scale *ptr = static_cast<Scale *>(ecs_ensure_id(world, entity, ecs_id(Scale)));
  ptr->x = x;
  ptr->y = y;
  ptr->z = z;
  printf("\taddScale: %f %f %f\n", ptr->x, ptr->y, ptr->z);
  ecs_modified_id(world, entity, ecs_id(Scale));
  ecs_defer_end(world);
}

void modifyPosition(ecs_world_t *world, ecs_entity_t entity, float x, float y, float z) {
  ecs_defer_begin(world);
  Position *ptr = static_cast<Position *>(ecs_ensure_id(world, entity, ecs_id(Position)));
  printf("\tmodifyPosition, initial ptr values: %f %f %f\n", ptr->x, ptr->y, ptr->z);
  if (!ecs_has_id(world, entity, ecs_id(Position))) {
    printf("\tmodifyPosition: Position does not exist, initializing values to 0\n");
    ptr->x = 0;
    ptr->y = 0;
    ptr->z = 0;
  }
  ptr->x += x;
  ptr->y += y;
  ptr->z += z;
  printf("\tmodifyPosition: after change: %f %f %f\n", ptr->x, ptr->y, ptr->z);
  ecs_modified_id(world, entity, ecs_id(Position));
  ecs_defer_end(world);
}

void modifyScale(ecs_world_t *world, ecs_entity_t entity, float x, float y, float z) {
  ecs_defer_begin(world);
  Scale *ptr = static_cast<Scale *>(ecs_ensure_id(world, entity, ecs_id(Scale)));
  printf("\tmodifyScale, initial ptr values: %f %f %f\n", ptr->x, ptr->y, ptr->z);
  if (!ecs_has_id(world, entity, ecs_id(Scale))) {
    printf("\tmodifyScale: Scale does not exist, initializing values to 0\n");
    ptr->x = 0;
    ptr->y = 0;
    ptr->z = 0;
  }
  ptr->x += x;
  ptr->y += y;
  ptr->z += z;
  printf("\tmodifyScale: after change: %f %f %f\n", ptr->x, ptr->y, ptr->z);
  ecs_modified_id(world, entity, ecs_id(Scale));
  ecs_defer_end(world);
}

void positionHookAddCallback(ecs_iter_t *it) {
  ecs_world_t *world = it->world;
  ecs_entity_t event = it->event;

  for (int i = 0; i < it->count; i++) {
    ecs_entity_t e = it->entities[i];
    printf("positionHookCallback: %s: %llu\n", ecs_get_name(world, event), e);
    if (event == EcsOnAdd) {
      // During the on_add hook, the entity does not yet have the component
      printf(
        "\tpositionHookAddCallback, has postion: %s\n", hasComponent(world, e, ecs_id(Position)));
      modifyScale(world, e, 150, 150, 150);
    }
  }
}

void scaleHookAddCallback(ecs_iter_t *it) {
  ecs_world_t *world = it->world;
  ecs_entity_t event = it->event;

  for (int i = 0; i < it->count; i++) {
    ecs_entity_t e = it->entities[i];
    printf("scaleHookAddCallback: %s: %llu\n", ecs_get_name(world, event), e);
    if (event == EcsOnAdd) {
      modifyPosition(world, e, 77, 77, 77);
    }
  }
}

TEST_F(EcsHookTest, OnAddHook) {

  ecs_world_t *world = ecs_init();
  ECS_COMPONENT_DEFINE(world, Position);
  ECS_COMPONENT_DEFINE(world, Scale);

  ecs_entity_t eid = ecs_new(world);

  Position p = {10, 20, 30};

  static const ecs_type_hooks_t postionHooks = {
    .on_add = positionHookAddCallback,
  };
  ecs_set_hooks_id(world, ecs_id(Position), &postionHooks);

  static const ecs_type_hooks_t scaleHooks = {
    .on_add = scaleHookAddCallback,
  };
  ecs_set_hooks_id(world, ecs_id(Scale), &scaleHooks);

  ecs_defer_begin(world);

  Position *res = static_cast<Position *>(ecs_ensure_id(world, eid, ecs_id(Position)));

  res->x = p.x;
  res->y = p.y;
  res->z = p.z;

  EXPECT_FLOAT_EQ(res->x, 10);
  EXPECT_FLOAT_EQ(res->y, 20);
  EXPECT_FLOAT_EQ(res->z, 30);

  ecs_modified_id(world, eid, ecs_id(Position));

  // Still in deferred mode, component has not been added yet
  EXPECT_EQ(ecs_has_id(world, eid, ecs_id(Position)), false);

  ecs_defer_end(world);

  const Position *positionPtr = ecs_get(world, eid, Position);
  printf("position after defer: %f %f %f\n", positionPtr->x, positionPtr->y, positionPtr->z);

  // Modifying the ptr within the on_add hooks will not affect the values
  EXPECT_FLOAT_EQ(res->x, 10);
  EXPECT_FLOAT_EQ(res->y, 20);
  EXPECT_FLOAT_EQ(res->z, 30);

  const Scale *scalePtr = ecs_get(world, eid, Scale);
  printf("scale after defer: %f %f %f\n", scalePtr->x, scalePtr->y, scalePtr->z);

  // Adding a component from a different component's on_add hook works
  EXPECT_FLOAT_EQ(scalePtr->x, 150);
  EXPECT_FLOAT_EQ(scalePtr->y, 150);
  EXPECT_FLOAT_EQ(scalePtr->z, 150);
}

void positionHookSetAddCallback(ecs_iter_t *it) {
  ecs_world_t *world = it->world;
  ecs_entity_t event = it->event;
  auto ctx = static_cast<ComponentHookContext *>(it->ctx);

  for (int i = 0; i < it->count; i++) {
    ecs_entity_t e = it->entities[i];
    printf("positionHookSetAddCallback: %s: %llu\n", ecs_get_name(world, event), e);

    if (event == EcsOnAdd) {
      ctx->pendingAddEntities.insert(e);
    } else if (event == EcsOnSet) {
      int removed = ctx->pendingAddEntities.erase(it->entities[i]);
      if (removed) {
        addScale(world, e, 100, 100, 100);
        modifyScale(world, e, 77, 77, 77);
      }
    }
  }
}

void scaleHookSetAddCallback(ecs_iter_t *it) {
  ecs_world_t *world = it->world;
  ecs_entity_t event = it->event;
  auto ctx = static_cast<ComponentHookContext *>(it->ctx);

  for (int i = 0; i < it->count; i++) {
    ecs_entity_t e = it->entities[i];
    printf("scaleHookSetAddCallback: %s: %llu\n", ecs_get_name(world, event), e);
    if (event == EcsOnAdd) {
      ctx->pendingAddEntities.insert(e);
    } else if (event == EcsOnSet) {
      int removed = ctx->pendingAddEntities.erase(it->entities[i]);
      if (removed) {
        Scale *ptr = static_cast<Scale *>(ecs_ensure_id(world, e, ecs_id(Scale)));
        printf("\tOnSet scale ptr: %f %f %f\n", ptr->x, ptr->y, ptr->z);
      }
    }
  }
}

TEST_F(EcsHookTest, OnAddAndSetHook) {
  ecs_world_t *world = ecs_init();
  ECS_COMPONENT_DEFINE(world, Position);
  ECS_COMPONENT_DEFINE(world, Scale);

  ecs_entity_t eid = ecs_new(world);

  Position p = {10, 20, 30};

  auto positionCtx = new ComponentHookContext;
  static const ecs_type_hooks_t postionHooks = {
    .on_add = positionHookSetAddCallback,
    .on_set = positionHookSetAddCallback,
    .ctx = (void *)positionCtx,
  };
  ecs_set_hooks_id(world, ecs_id(Position), &postionHooks);

  auto scaleCtx = new ComponentHookContext;
  static const ecs_type_hooks_t scaleHooks = {
    .on_add = scaleHookSetAddCallback,
    .on_set = scaleHookSetAddCallback,
    .ctx = (void *)scaleCtx,
  };
  ecs_set_hooks_id(world, ecs_id(Scale), &scaleHooks);

  ecs_defer_begin(world);

  Position *res = static_cast<Position *>(ecs_ensure_id(world, eid, ecs_id(Position)));

  res->x = p.x;
  res->y = p.y;
  res->z = p.z;

  EXPECT_FLOAT_EQ(res->x, 10);
  EXPECT_FLOAT_EQ(res->y, 20);
  EXPECT_FLOAT_EQ(res->z, 30);

  ecs_modified_id(world, eid, ecs_id(Position));

  // Still in deferred mode, component has not been added yet
  EXPECT_EQ(ecs_has_id(world, eid, ecs_id(Position)), false);

  ecs_defer_end(world);

  const Position *positionPtr = ecs_get(world, eid, Position);
  printf("position after defer: %f %f %f\n", positionPtr->x, positionPtr->y, positionPtr->z);

  EXPECT_FLOAT_EQ(positionPtr->x, 10);
  EXPECT_FLOAT_EQ(positionPtr->y, 20);
  EXPECT_FLOAT_EQ(positionPtr->z, 30);

  const Scale *scalePtr = ecs_get(world, eid, Scale);
  printf("scale after defer: %f %f %f\n", scalePtr->x, scalePtr->y, scalePtr->z);
  // Can add a component from another component's on_set hook
  // Only the latest operation on the component pointer will be applied
  // In this case, the position on_set hook runs addScale and modifyScale
  // But only modifyScale is applied
  // (both addScale and modifyScale retrieve their own pointer, which isn't the same across multiple
  // calls to ecs_ensure_id in deferred mode)
  EXPECT_FLOAT_EQ(scalePtr->x, 77);
  EXPECT_FLOAT_EQ(scalePtr->y, 77);
  EXPECT_FLOAT_EQ(scalePtr->z, 77);
}

void positionHookRemoveCallback(ecs_iter_t *it) {
  ecs_world_t *world = it->world;
  ecs_entity_t event = it->event;

  for (int i = 0; i < it->count; i++) {
    ecs_entity_t e = it->entities[i];
    printf("positionHookRemoveCallback: %s: %llu\n", ecs_get_name(world, event), e);
    if (event == EcsOnRemove) {
      addScale(world, e, 100, 100, 100);
      modifyScale(world, e, 77, 77, 77);
    }
  }
}

void scaleHookCallback(ecs_iter_t *it) {
  ecs_world_t *world = it->world;
  ecs_entity_t event = it->event;
  auto ctx = static_cast<ComponentHookContext *>(it->ctx);

  for (int i = 0; i < it->count; i++) {
    ecs_entity_t e = it->entities[i];
    printf("scaleHookCallback: %s: %llu\n", ecs_get_name(world, event), e);
    if (event == EcsOnAdd) {
      ctx->pendingAddEntities.insert(e);
    }
    if (event == EcsOnSet) {
      int removed = ctx->pendingAddEntities.erase(it->entities[i]);
      if (removed) {
        Scale *ptr = static_cast<Scale *>(ecs_ensure_id(world, e, ecs_id(Scale)));
        printf("\tOnSet scale: %f %f %f\n", ptr->x, ptr->y, ptr->z);
      }
    }
  }
}

TEST_F(EcsHookTest, AddComponentInOnRemoveHook) {
  ecs_world_t *world = ecs_init();
  ECS_COMPONENT_DEFINE(world, Position);
  ECS_COMPONENT_DEFINE(world, Scale);

  ecs_entity_t eid = ecs_new(world);

  Position p = {10, 20, 30};

  auto positionCtx = new ComponentHookContext;
  static const ecs_type_hooks_t postionHooks = {
    .on_remove = positionHookRemoveCallback,
    .ctx = (void *)positionCtx,
  };
  ecs_set_hooks_id(world, ecs_id(Position), &postionHooks);

  auto scaleCtx = new ComponentHookContext;
  static const ecs_type_hooks_t scaleHooks = {
    .on_add = scaleHookCallback,
    .on_set = scaleHookCallback,
    .ctx = (void *)scaleCtx,
  };
  ecs_set_hooks_id(world, ecs_id(Scale), &scaleHooks);

  ecs_set_ptr(world, eid, Position, &p);

  const Position *positionPtr = ecs_get(world, eid, Position);

  EXPECT_FLOAT_EQ(positionPtr->x, 10);
  EXPECT_FLOAT_EQ(positionPtr->y, 20);
  EXPECT_FLOAT_EQ(positionPtr->z, 30);

  EXPECT_EQ(ecs_has_id(world, eid, ecs_id(Scale)), false);

  ecs_remove_id(world, eid, ecs_id(Position));

  EXPECT_EQ(ecs_has_id(world, eid, ecs_id(Position)), false);

  const Scale *scalePtr = ecs_get(world, eid, Scale);
  printf("scale after remove: %f %f %f\n", scalePtr->x, scalePtr->y, scalePtr->z);
  // Can add a component from another component's on_remove hook
  // Only the latest operation on the component pointer will be applied
  // In this case, the position on_set hook runs addScale and modifyScale
  // But only modifyScale is applied
  // (both addScale and modifyScale retrieve their own pointer, which isn't the same across
  // multiple calls to ecs_ensure_id in deferred mode)
  EXPECT_FLOAT_EQ(scalePtr->x, 77);
  EXPECT_FLOAT_EQ(scalePtr->y, 77);
  EXPECT_FLOAT_EQ(scalePtr->z, 77);
}

void positionHookSetRemove(ecs_iter_t *it) {
  ecs_world_t *world = it->world;
  ecs_entity_t event = it->event;
  auto ctx = static_cast<ComponentHookContext *>(it->ctx);

  for (int i = 0; i < it->count; i++) {
    ecs_entity_t e = it->entities[i];
    printf("positionHookSetRemove: %s: %llu\n", ecs_get_name(world, event), e);

    if (event == EcsOnAdd) {
      printf("\tEcsOnAdd has position: %s\n", hasComponent(world, e, ecs_id(Position)));
      ctx->pendingAddEntities.insert(e);
    } else if (event == EcsOnSet) {
      int removed = ctx->pendingAddEntities.erase(it->entities[i]);
      if (removed) {
        printf("\tEcsOnSet has position: %s\n", hasComponent(world, e, ecs_id(Position)));
        addScale(world, e, 100, 100, 100);
        modifyScale(world, e, 77, 77, 77);
      }
    } else if (event == EcsOnRemove) {
      printf("\ttEcsOnRemove has position: %s\n", hasComponent(world, e, ecs_id(Position)));
      // Still has access to the component within the on_remove hook
      const Position *ptr = ecs_get(world, e, Position);
      printf("\tposition: %f %f %f\n", ptr->x, ptr->y, ptr->z);
    }
  }
}

void scaleHookSetRemove(ecs_iter_t *it) {
  ecs_world_t *world = it->world;
  ecs_entity_t event = it->event;

  for (int i = 0; i < it->count; i++) {
    ecs_entity_t e = it->entities[i];
    printf("scaleHookSetRemove: %s: %llu\n", ecs_get_name(world, event), e);
    if (event == EcsOnRemove) {
      // Still has access to the component within the on_remove hook
      printf("\thas scale: %s\n", hasComponent(world, e, ecs_id(Scale)));
      printf("\thas position: %s\n", hasComponent(world, e, ecs_id(Position)));

      const Position *ptr = ecs_get(world, e, Position);
      printf("\tposition: %f %f %f\n", ptr->x, ptr->y, ptr->z);

      const Scale *sPtr = ecs_get(world, e, Scale);
      printf("\tscale: %f %f %f\n", sPtr->x, sPtr->y, sPtr->z);

      ecs_remove_id(world, e, ecs_id(Position));
      printf("\thas position after remove: %s\n", hasComponent(world, e, ecs_id(Position)));
    }
  }
}

TEST_F(EcsHookTest, RemovingInHook) {

  ecs_world_t *world = ecs_init();
  ECS_COMPONENT_DEFINE(world, Position);
  ECS_COMPONENT_DEFINE(world, Scale);

  ecs_entity_t eid = ecs_new(world);

  Position p = {10, 20, 30};

  auto positionCtx = new ComponentHookContext;
  static const ecs_type_hooks_t postionHooks = {
    .on_add = positionHookSetRemove,
    .on_set = positionHookSetRemove,
    .on_remove = positionHookSetRemove,
    .ctx = (void *)positionCtx,
  };
  ecs_set_hooks_id(world, ecs_id(Position), &postionHooks);

  auto scaleCtx = new ComponentHookContext;
  static const ecs_type_hooks_t scaleHooks = {
    .on_remove = scaleHookSetRemove,
    .ctx = (void *)scaleCtx,
  };
  ecs_set_hooks_id(world, ecs_id(Scale), &scaleHooks);

  ecs_defer_begin(world);
  ecs_set_ptr(world, eid, Position, &p);
  ecs_defer_end(world);

  const Position *positionPtr = ecs_get(world, eid, Position);

  EXPECT_FLOAT_EQ(positionPtr->x, 10);
  EXPECT_FLOAT_EQ(positionPtr->y, 20);
  EXPECT_FLOAT_EQ(positionPtr->z, 30);

  EXPECT_EQ(ecs_has_id(world, eid, ecs_id(Scale)), true);

  ecs_defer_begin(world);
  ecs_remove_id(world, eid, ecs_id(Scale));
  ecs_defer_end(world);

  printf(
    "After defer: position - %s, scale - %s\n",
    ecs_has_id(world, eid, ecs_id(Position)) ? "exists" : "doesn't exist",
    ecs_has_id(world, eid, ecs_id(Scale)) ? "exists" : "doesn't exist");
  EXPECT_EQ(ecs_has_id(world, eid, ecs_id(Scale)), false);
  EXPECT_EQ(ecs_has_id(world, eid, ecs_id(Position)), false);
}

}  // namespace c8
