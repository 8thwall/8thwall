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
cc_end(0x7820f5a0);

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

class EcsObserverTest : public ::testing::Test {};

const char *hasComponent(ecs_world_t *world, ecs_entity_t entity, ecs_entity_t component) {
  return ecs_has_id(world, entity, component) ? "true" : "false";
}

struct ComponentObserverContext {
  TreeSet<ecs_entity_t> pendingAddEntities;
  ecs_entity_t observer;
};

void cleanupObserverContext(void *ctx) {
  auto observer = static_cast<ComponentObserverContext *>(ctx);
  delete observer;
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

ecs_query_desc_t defineQuery(ecs_entity_t component) {
  return ecs_query_desc_t{.terms = {ecs_term_t{.id = component}}};
}

void positionObserverAddCallback(ecs_iter_t *it) {
  ecs_world_t *world = it->world;
  ecs_entity_t event = it->event;

  for (int i = 0; i < it->count; i++) {
    ecs_entity_t e = it->entities[i];
    printf("positionObserverCallback: %s: %llu\n", ecs_get_name(world, event), e);
    if (event == EcsOnAdd) {
      // During the on_add hook, the entity has the component
      printf(
        "positionObserverAddCallback, has postion: %s\n",
        ecs_has_id(world, e, ecs_id(Position)) ? "true" : "false");
      modifyPosition(world, e, 500, 500, 500);
      modifyScale(world, e, 150, 150, 150);
    }
  }
}

void scaleObserverAddCallback(ecs_iter_t *it) {
  ecs_world_t *world = it->world;
  ecs_entity_t event = it->event;

  for (int i = 0; i < it->count; i++) {
    ecs_entity_t e = it->entities[i];
    printf("scaleObserverAddCallback: %s: %llu\n", ecs_get_name(world, event), e);
    if (event == EcsOnAdd) {
      printf(
        "scaleObserverAddCallback, has scale: %s\n",
        ecs_has_id(world, e, ecs_id(Scale)) ? "true" : "false");
      modifyScale(world, e, 23, 23, 23);
      modifyPosition(world, e, 77, 77, 77);
    }
  }
}

TEST_F(EcsObserverTest, OnAddObserver) {
  printf("OnAddObserver");

  ecs_world_t *world = ecs_init();
  ECS_COMPONENT_DEFINE(world, Position);
  ECS_COMPONENT_DEFINE(world, Scale);

  ecs_entity_t eid = ecs_new(world);

  Position p = {10, 20, 30};

  ecs_query_desc_t positionQuery = defineQuery(ecs_id(Position));
  ecs_observer_desc_t postionDesc = {
    .query = positionQuery,
    .events = {EcsOnAdd},
    .callback = positionObserverAddCallback,
  };
  ecs_observer_init(world, &postionDesc);

  ecs_query_desc_t scaleQuery = defineQuery(ecs_id(Scale));
  ecs_observer_desc_t scaleDesc = {
    .query = scaleQuery,
    .events = {EcsOnAdd},
    .callback = scaleObserverAddCallback,
  };
  ecs_observer_init(world, &scaleDesc);

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

  // Modifying the ptr within the on_add Observers will affect the values
  EXPECT_FLOAT_EQ(positionPtr->x, 587);
  EXPECT_FLOAT_EQ(positionPtr->y, 597);
  EXPECT_FLOAT_EQ(positionPtr->z, 607);

  const Scale *scalePtr = ecs_get(world, eid, Scale);
  printf("scale after defer: %f %f %f\n", scalePtr->x, scalePtr->y, scalePtr->z);

  // Adding a component from a different component's on_add Observer works
  EXPECT_FLOAT_EQ(scalePtr->x, 173);
  EXPECT_FLOAT_EQ(scalePtr->y, 173);
  EXPECT_FLOAT_EQ(scalePtr->z, 173);
}

void positionObserverSetAddCallback(ecs_iter_t *it) {
  ecs_world_t *world = it->world;
  ecs_entity_t event = it->event;
  auto ctx = static_cast<ComponentObserverContext *>(it->ctx);

  for (int i = 0; i < it->count; i++) {
    ecs_entity_t e = it->entities[i];
    printf("positionObserverSetAddCallback: %s: %llu\n", ecs_get_name(world, event), e);

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

void scaleObserverSetAddCallback(ecs_iter_t *it) {
  ecs_world_t *world = it->world;
  ecs_entity_t event = it->event;
  auto ctx = static_cast<ComponentObserverContext *>(it->ctx);

  for (int i = 0; i < it->count; i++) {
    ecs_entity_t e = it->entities[i];
    printf("scaleObserverSetAddCallback: %s: %llu\n", ecs_get_name(world, event), e);
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

TEST_F(EcsObserverTest, OnAddAndSetObserver) {
  ecs_world_t *world = ecs_init();
  ECS_COMPONENT_DEFINE(world, Position);
  ECS_COMPONENT_DEFINE(world, Scale);

  ecs_entity_t eid = ecs_new(world);

  Position p = {10, 20, 30};

  auto positionCtx = new ComponentObserverContext;
  ecs_query_desc_t positionQuery = defineQuery(ecs_id(Position));
  ecs_observer_desc_t positionDesc{
    .query = positionQuery,
    .events = {EcsOnAdd, EcsOnSet},
    .callback = &positionObserverSetAddCallback,
    .ctx = (void *)positionCtx,
    .ctx_free = cleanupObserverContext,
  };
  auto positionObserver = ecs_observer_init(world, &positionDesc);
  positionCtx->observer = positionObserver;

  auto scaleCtx = new ComponentObserverContext;
  ecs_query_desc_t scaleQuery = defineQuery(ecs_id(Scale));
  ecs_observer_desc_t scaleDesc{
    .query = scaleQuery,
    .events = {EcsOnAdd, EcsOnSet},
    .callback = &scaleObserverSetAddCallback,
    .ctx = (void *)scaleCtx,
    .ctx_free = cleanupObserverContext,
  };
  auto scaleObserver = ecs_observer_init(world, &scaleDesc);
  scaleCtx->observer = scaleObserver;

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

  EXPECT_FLOAT_EQ(res->x, 10);
  EXPECT_FLOAT_EQ(res->y, 20);
  EXPECT_FLOAT_EQ(res->z, 30);

  const Scale *scalePtr = ecs_get(world, eid, Scale);
  printf("scale after defer: %f %f %f\n", scalePtr->x, scalePtr->y, scalePtr->z);
  // Can add a component from another component's on_set Observer
  // Only the latest operation on the component pointer will be applied
  // In this case, the position on_set Observer runs addScale and modifyScale
  // But only modifyScale is applied
  // (both addScale and modifyScale retrieve their own pointer, which isn't the same across
  // multiple calls to ecs_ensure_id in deferred mode)
  EXPECT_FLOAT_EQ(scalePtr->x, 77);
  EXPECT_FLOAT_EQ(scalePtr->y, 77);
  EXPECT_FLOAT_EQ(scalePtr->z, 77);
}

void positionObserverRemoveCallback(ecs_iter_t *it) {
  ecs_world_t *world = it->world;
  ecs_entity_t event = it->event;
  for (int i = 0; i < it->count; i++) {
    ecs_entity_t e = it->entities[i];
    printf("positionObserverRemoveCallback: %s: %llu\n", ecs_get_name(world, event), e);
    if (event == EcsOnRemove) {
      addScale(world, e, 100, 100, 100);
      modifyScale(world, e, 77, 77, 77);
    }
  }
}

void scaleObserverCallback(ecs_iter_t *it) {
  ecs_world_t *world = it->world;
  ecs_entity_t event = it->event;
  auto ctx = static_cast<ComponentObserverContext *>(it->ctx);

  for (int i = 0; i < it->count; i++) {
    ecs_entity_t e = it->entities[i];
    printf("scaleObserverCallback: %s: %llu\n", ecs_get_name(world, event), e);
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

TEST_F(EcsObserverTest, AddComponentInOnRemoveObserver) {
  ecs_world_t *world = ecs_init();
  ECS_COMPONENT_DEFINE(world, Position);
  ECS_COMPONENT_DEFINE(world, Scale);

  ecs_entity_t eid = ecs_new(world);

  Position p = {10, 20, 30};

  auto positionCtx = new ComponentObserverContext;
  ecs_query_desc_t positionQuery = defineQuery(ecs_id(Position));
  ecs_observer_desc_t positionDesc{
    .query = positionQuery,
    .events = {EcsOnRemove},
    .callback = &positionObserverRemoveCallback,
    .ctx = (void *)positionCtx,
    .ctx_free = cleanupObserverContext,
  };
  auto positionObserver = ecs_observer_init(world, &positionDesc);
  positionCtx->observer = positionObserver;

  auto scaleCtx = new ComponentObserverContext;
  ecs_query_desc_t scaleQuery = defineQuery(ecs_id(Scale));
  ecs_observer_desc_t scaleDesc{
    .query = scaleQuery,
    .events = {EcsOnAdd, EcsOnSet},
    .callback = &scaleObserverCallback,
    .ctx = (void *)scaleCtx,
    .ctx_free = cleanupObserverContext,
  };
  auto scaleObserver = ecs_observer_init(world, &scaleDesc);
  scaleCtx->observer = scaleObserver;

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
  // Can add a component from another component's on_remove Observer
  // Only the latest operation on the component pointer will be applied
  // In this case, the position on_set Observer runs addScale and modifyScale
  // But only modifyScale is applied
  // (both addScale and modifyScale retrieve their own pointer, which isn't the same across
  // multiple calls to ecs_ensure_id in deferred mode)
  EXPECT_FLOAT_EQ(scalePtr->x, 77);
  EXPECT_FLOAT_EQ(scalePtr->y, 77);
  EXPECT_FLOAT_EQ(scalePtr->z, 77);
}

void positionObserverSetRemove(ecs_iter_t *it) {
  ecs_world_t *world = it->world;
  ecs_entity_t event = it->event;
  auto ctx = static_cast<ComponentObserverContext *>(it->ctx);

  for (int i = 0; i < it->count; i++) {
    ecs_entity_t e = it->entities[i];
    printf("positionObserverSetRemove: %s: %llu\n", ecs_get_name(world, event), e);

    if (event == EcsOnAdd) {
      printf("\tEcsOnAdd has position: %s\n", hasComponent(world, e, ecs_id(Position)));
      ctx->pendingAddEntities.insert(e);
    } else if (event == EcsOnSet) {
      int removed = ctx->pendingAddEntities.erase(it->entities[i]);
      if (removed) {
        printf("\tEcsOnSet has position:  %s\n", hasComponent(world, e, ecs_id(Position)));
        addScale(world, e, 100, 100, 100);
        modifyScale(world, e, 77, 77, 77);
      }
    } else if (event == EcsOnRemove) {
      printf("\tEcsOnRemove has position: %s\n", hasComponent(world, e, ecs_id(Position)));
      // Still has access to the component within the on_remove Observer
      const Position *ptr = ecs_get(world, e, Position);
      printf("\tposition: %f %f %f\n", ptr->x, ptr->y, ptr->z);
    }
  }
}

void scaleObserverSetRemove(ecs_iter_t *it) {
  ecs_world_t *world = it->world;
  ecs_entity_t event = it->event;

  for (int i = 0; i < it->count; i++) {
    ecs_entity_t e = it->entities[i];
    printf("scaleObserverSetRemove: %s: %llu\n", ecs_get_name(world, event), e);
    if (event == EcsOnRemove) {
      // Still has access to the component within the on_remove Observer
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

TEST_F(EcsObserverTest, RemovingInObserver) {

  ecs_world_t *world = ecs_init();
  ECS_COMPONENT_DEFINE(world, Position);
  ECS_COMPONENT_DEFINE(world, Scale);

  ecs_entity_t eid = ecs_new(world);

  Position p = {10, 20, 30};

  auto positionCtx = new ComponentObserverContext;
  ecs_query_desc_t positionQuery = defineQuery(ecs_id(Position));
  ecs_observer_desc_t positionDesc{
    .query = positionQuery,
    .events = {EcsOnAdd, EcsOnSet, EcsOnRemove},
    .callback = &positionObserverSetRemove,
    .ctx = (void *)positionCtx,
    .ctx_free = cleanupObserverContext,
  };
  auto positionObserver = ecs_observer_init(world, &positionDesc);
  positionCtx->observer = positionObserver;

  auto scaleCtx = new ComponentObserverContext;
  ecs_query_desc_t scaleQuery = defineQuery(ecs_id(Scale));
  ecs_observer_desc_t scaleDesc{
    .query = scaleQuery,
    .events = {EcsOnRemove},
    .callback = &scaleObserverSetRemove,
    .ctx = (void *)scaleCtx,
    .ctx_free = cleanupObserverContext,
  };
  auto scaleObserver = ecs_observer_init(world, &scaleDesc);
  scaleCtx->observer = scaleObserver;

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
