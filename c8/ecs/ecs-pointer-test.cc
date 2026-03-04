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
cc_end(0x08c5bfb1);

#include "c8/ecs/physics.h"
#include "c8/set.h"
#include "c8/symbol-visibility.h"
#include "c8/vector.h"
#include "flecs.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace c8;

ECS_COMPONENT_DECLARE(Position);

namespace c8 {

class EcsPointerTest : public ::testing::Test {};

TEST_F(EcsPointerTest, NotDeferred) {

  ecs_world_t *world = ecs_init();
  ECS_COMPONENT_DEFINE(world, Position);

  ecs_entity_t eid1 = ecs_new(world);

  Position p = {10, 20, 30};
  ecs_set_ptr(world, eid1, Position, &p);

  const Position *position = ecs_get(world, eid1, Position);

  EXPECT_FLOAT_EQ(position->x, p.x);
  EXPECT_FLOAT_EQ(position->y, p.y);
  EXPECT_FLOAT_EQ(position->z, p.z);
}

TEST_F(EcsPointerTest, Deferred) {

  ecs_world_t *world = ecs_init();
  ECS_COMPONENT_DEFINE(world, Position);

  ecs_entity_t eid = ecs_new(world);

  Position p = {10, 20, 30};

  ecs_defer_begin(world);

  ecs_set_ptr(world, eid, Position, &p);

  const Position *ptr = ecs_get(world, eid, Position);

  EXPECT_EQ(ptr, nullptr);  // The ptr is null since the set is deferred
  EXPECT_EQ(ecs_has_id(world, eid, ecs_id(Position)), false);  // Deferred, add has not occurred yet

  ecs_defer_end(world);

  // The component has been added
  EXPECT_EQ(ecs_has_id(world, eid, ecs_id(Position)), true);

  const Position *positionPtr = ecs_get(world, eid, Position);

  EXPECT_FLOAT_EQ(positionPtr->x, p.x);
  EXPECT_FLOAT_EQ(positionPtr->y, p.y);
  EXPECT_FLOAT_EQ(positionPtr->z, p.z);
}

TEST_F(EcsPointerTest, SetComponentThenDefer) {

  ecs_world_t *world = ecs_init();
  ECS_COMPONENT_DEFINE(world, Position);

  ecs_entity_t eid = ecs_new(world);

  Position p = {10, 20, 30};

  ecs_set_ptr(world, eid, Position, &p);

  ecs_defer_begin(world);

  const Position *ptr = ecs_get(world, eid, Position);

  EXPECT_FLOAT_EQ(ptr->x, p.x);
  EXPECT_FLOAT_EQ(ptr->y, p.y);
  EXPECT_FLOAT_EQ(ptr->z, p.z);

  EXPECT_EQ(ecs_has_id(world, eid, ecs_id(Position)), true);

  Position *mutPtr = static_cast<Position *>(ecs_ensure_id(world, eid, ecs_id(Position)));

  EXPECT_FLOAT_EQ(mutPtr->x, p.x);
  EXPECT_FLOAT_EQ(mutPtr->y, p.y);
  EXPECT_FLOAT_EQ(mutPtr->z, p.z);

  mutPtr->x += 2;
  mutPtr->y += 2;
  mutPtr->z += 2;

  Position *mutPtr2 = static_cast<Position *>(ecs_ensure_id(world, eid, ecs_id(Position)));

  // Since the component exists, the pointer is the same as the previous and the changes are saved
  EXPECT_FLOAT_EQ(mutPtr2->x, 12);
  EXPECT_FLOAT_EQ(mutPtr2->y, 22);
  EXPECT_FLOAT_EQ(mutPtr2->z, 32);

  mutPtr->x += 7;
  mutPtr->y += 7;
  mutPtr->z += 7;

  ecs_defer_end(world);

  // The component has been added
  EXPECT_EQ(ecs_has_id(world, eid, ecs_id(Position)), true);

  const Position *positionPtr = ecs_get(world, eid, Position);

  EXPECT_FLOAT_EQ(positionPtr->x, 19);
  EXPECT_FLOAT_EQ(positionPtr->y, 29);
  EXPECT_FLOAT_EQ(positionPtr->z, 39);
}

TEST_F(EcsPointerTest, MultipleOperationsWhileDeferred) {

  ecs_world_t *world = ecs_init();
  ECS_COMPONENT_DEFINE(world, Position);

  ecs_entity_t eid = ecs_new(world);

  Position p = {10, 20, 30};

  ecs_defer_begin(world);

  ecs_set_ptr(world, eid, Position, &p);

  const Position *ptr = ecs_get(world, eid, Position);

  EXPECT_EQ(ptr, nullptr);  // The ptr is null since the set is deferred
  EXPECT_EQ(ecs_has_id(world, eid, ecs_id(Position)), false);  // Deferred, add has not occurred yet

  Position *mutPtr = static_cast<Position *>(ecs_ensure_id(world, eid, ecs_id(Position)));

  EXPECT_EQ(ecs_has_id(world, eid, ecs_id(Position)), false);  // Deferred, add has not occurred yet

  mutPtr->x = 28;
  mutPtr->y = 77;
  mutPtr->z = 60;

  ecs_defer_end(world);

  EXPECT_EQ(ecs_has_id(world, eid, ecs_id(Position)), true);

  const Position *positionPtr = ecs_get(world, eid, Position);

  // The latest operation on the component pointer while deferred is applied
  EXPECT_FLOAT_EQ(positionPtr->x, 28);
  EXPECT_FLOAT_EQ(positionPtr->y, 77);
  EXPECT_FLOAT_EQ(positionPtr->z, 60);
}

TEST_F(EcsPointerTest, MultipleEnsureWhileDeferred) {

  ecs_world_t *world = ecs_init();
  ECS_COMPONENT_DEFINE(world, Position);

  ecs_entity_t eid = ecs_new(world);

  Position p = {10, 20, 30};
  Position p2 = {100, 200, 300};

  ecs_defer_begin(world);

  ecs_set_ptr(world, eid, Position, &p);

  const Position *ptr = ecs_get(world, eid, Position);

  EXPECT_EQ(ptr, nullptr);  // The ptr is null since the set is deferred
  EXPECT_EQ(ecs_has_id(world, eid, ecs_id(Position)), false);  // Deferred, add has not occurred yet

  Position *mutPtr = static_cast<Position *>(ecs_ensure_id(world, eid, ecs_id(Position)));

  // Retrieving a pointer to a component that has not been added (deferred mode) will not
  // have the previous data
  EXPECT_EQ(mutPtr->x == p.x, false);
  EXPECT_EQ(mutPtr->y == p.y, false);
  EXPECT_EQ(mutPtr->z == p.z, false);

  EXPECT_EQ(ecs_has_id(world, eid, ecs_id(Position)), false);

  mutPtr->x = 28;
  mutPtr->y = 77;
  mutPtr->z = 60;

  Position *mutPtr2 = static_cast<Position *>(ecs_ensure_id(world, eid, ecs_id(Position)));
  // Also doesn't have the previous data set previously on the eid+component
  EXPECT_EQ(mutPtr2->x == 28, false);
  EXPECT_EQ(mutPtr2->y == 77, false);
  EXPECT_EQ(mutPtr2->z == 60, false);

  ecs_set_ptr(world, eid, Position, &p2);

  ecs_defer_end(world);

  EXPECT_EQ(ecs_has_id(world, eid, ecs_id(Position)), true);

  const Position *positionPtr = ecs_get(world, eid, Position);
  // The latest operation on the component pointer while deferred is applied
  EXPECT_FLOAT_EQ(positionPtr->x, p2.x);
  EXPECT_FLOAT_EQ(positionPtr->y, p2.y);
  EXPECT_FLOAT_EQ(positionPtr->z, p2.z);
}

TEST_F(EcsPointerTest, EnsureNestedDefer) {

  ecs_world_t *world = ecs_init();
  ECS_COMPONENT_DEFINE(world, Position);

  ecs_entity_t eid = ecs_new(world);

  Position p = {10, 20, 30};
  Position p2 = {100, 200, 300};

  // 1st defer begin
  ecs_defer_begin(world);

  ecs_set_ptr(world, eid, Position, &p);

  const Position *ptr = ecs_get(world, eid, Position);

  EXPECT_EQ(ptr, nullptr);  // The ptr is null since the set is deferred
  EXPECT_EQ(ecs_has_id(world, eid, ecs_id(Position)), false);  // Deferred, add has not occurred yet

  // 2nd defer begin
  ecs_defer_begin(world);

  Position *mutPtr = static_cast<Position *>(ecs_ensure_id(world, eid, ecs_id(Position)));

  // Retrieving a pointer to a component that has not been added (deferred mode) will not
  // have the previous data
  EXPECT_EQ(mutPtr->x == p.x, false);
  EXPECT_EQ(mutPtr->y == p.y, false);
  EXPECT_EQ(mutPtr->z == p.z, false);

  EXPECT_EQ(ecs_has_id(world, eid, ecs_id(Position)), false);

  mutPtr->x = 28;
  mutPtr->y = 77;
  mutPtr->z = 60;

  ecs_defer_end(world);
  // 2nd defer end

  EXPECT_EQ(ecs_has_id(world, eid, ecs_id(Position)), false);

  // 3rd defer begin
  ecs_defer_begin(world);
  Position *mutPtr2 = static_cast<Position *>(ecs_ensure_id(world, eid, ecs_id(Position)));
  // Also doesn't have the previous data set previously on the eid+component
  EXPECT_EQ(mutPtr2->x == 28, false);
  EXPECT_EQ(mutPtr2->y == 77, false);
  EXPECT_EQ(mutPtr2->z == 60, false);

  ecs_set_ptr(world, eid, Position, &p2);

  // 3rd defer end
  ecs_defer_end(world);

  EXPECT_EQ(ecs_has_id(world, eid, ecs_id(Position)), false);

  // 1st defer end
  ecs_defer_end(world);

  EXPECT_EQ(ecs_has_id(world, eid, ecs_id(Position)), true);

  const Position *positionPtr = ecs_get(world, eid, Position);
  // The latest operation on the component pointer while deferred is applied
  EXPECT_FLOAT_EQ(positionPtr->x, p2.x);
  EXPECT_FLOAT_EQ(positionPtr->y, p2.y);
  EXPECT_FLOAT_EQ(positionPtr->z, p2.z);
}

}  // namespace c8
