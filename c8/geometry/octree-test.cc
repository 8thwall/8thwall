// Copyright (c) 2022 Niantic Inc.
// Original Author: Dat Chu (datchu@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":octree",
    "//c8:c8-log",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x8ce38ab3);

#include "c8/c8-log.h"
#include "c8/geometry/octree.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace c8 {

class OctreeTest : public ::testing::Test {};

TEST_F(OctreeTest, CreateEmptyTree) {
  Octree tree({{-1.f, -1.f, -1.f}, {1.f, 1.f, 1.f}}, NULL);
  EXPECT_TRUE(tree.isLeafNode());
  EXPECT_FALSE(tree.hasData());
}

TEST_F(OctreeTest, InsertSingleDatum) {
  Vector<Box3> boundingBoxes{
    {{-0.5f, -0.5f, -0.5f}, {0.5f, 0.5f, 0.5f}},
    {{0.5f, -0.5f, -0.5f}, {1.f, 0.5f, 0.5f}},
  };
  Octree tree({{-1.f, -1.f, -1.f}, {1.f, 1.f, 1.f}}, &boundingBoxes);
  // two bounding boxes that do not overlap
  LeafData triangleBbs{
    {0, 1},
  };
  tree.insert(triangleBbs);
  EXPECT_TRUE(tree.isLeafNode());
  EXPECT_TRUE(tree.hasData());
}

TEST_F(OctreeTest, InsertMultipleDataWillSplit) {
  Vector<Box3> boundingBoxes = {
    {{-0.5f, -0.5f, -0.5f}, {0.5f, 0.5f, 0.5f}},
    {{0.5f, -0.5f, -0.5f}, {1.f, 0.5f, 0.5f}},
  };
  Octree tree({{-1.f, -1.f, -1.f}, {1.f, 1.f, 1.f}}, &boundingBoxes);
  // two bounding boxes that do not overlap
  LeafData triangleBbs1{{0}};
  tree.insert(triangleBbs1);
  EXPECT_TRUE(tree.isLeafNode());

  LeafData triangleBbs2{{1}};
  tree.insert(triangleBbs2);
  EXPECT_FALSE(tree.isLeafNode());
}

TEST_F(OctreeTest, MoveTreeDoesNotCrashMem) {
  Vector<Box3> boundingBoxes = {
    {{-0.5f, -0.5f, -0.5f}, {0.5f, 0.5f, 0.5f}},
    {{0.5f, -0.5f, -0.5f}, {1.f, 0.5f, 0.5f}},
  };
  Octree tree({{-1.f, -1.f, -1.f}, {1.f, 1.f, 1.f}}, &boundingBoxes);
  LeafData triangleBbs1{{0}};
  tree.insert(triangleBbs1);
  EXPECT_TRUE(tree.isLeafNode());

  LeafData triangleBbs2{{1}};
  tree.insert(triangleBbs2);
  EXPECT_FALSE(tree.isLeafNode());

  Octree tree2{std::move(tree)};
  EXPECT_FALSE(tree2.isLeafNode());
}

TEST_F(OctreeTest, BuildFromMesh) {
  MeshGeometry mesh;
  mesh.points.push_back({0.f, 0.f, 0.f});
  mesh.points.push_back({0.f, 1.f, 0.f});
  mesh.points.push_back({0.f, 0.f, 1.f});
  mesh.points.push_back({1.f, 0.f, 0.f});
  mesh.triangles.push_back({0, 1, 2});
  mesh.triangles.push_back({0, 2, 3});
  mesh.triangles.push_back({0, 1, 3});
  mesh.triangles.push_back({1, 2, 3});

  Vector<Box3> boundingBoxes;
  auto tree = Octree::from(mesh, &boundingBoxes);
  EXPECT_FALSE(tree.isLeafNode());
}

class SplatOctreeNodeTest : public ::testing::Test {};

TEST(SplatOctreeNodeTest, Subdivide) {
  Box3 boundingBox{HPoint3(0.0f, 0.0f, 0.0f), HPoint3(10.0f, 10.0f, 10.0f)};
  SplatOctreeNode node1(boundingBox, 0);
  Vector<HPoint3> positions = {HPoint3(1.0f, 1.0f, 1.0f), HPoint3(2.0f, 2.0f, 2.0f)};
  node1.insert(positions, 0);
  node1.insert(positions, 1);
  EXPECT_FALSE(node1.hasChildren());
  node1.subdivide(positions);
  EXPECT_TRUE(node1.hasChildren());

  SplatOctreeNode node2(boundingBox, 0);
  Vector<HPoint3> positions2;
  for (int i = 0; i < node2.maxPointsTilSubdivide(); i++) {
    positions2.push_back(HPoint3(1.0f, 1.0f, 1.0f));
    node2.insert(positions2, i);
    EXPECT_FALSE(node2.hasChildren());
  }
  positions2.push_back(HPoint3(1.0f, 1.0f, 1.0f));
  node2.insert(positions2, node2.maxPointsTilSubdivide());
  EXPECT_TRUE(node2.hasChildren());
}

TEST(SplatOctreeNodeTest, InsertOne) {
  Box3 boundingBox{HPoint3(0.0f, 0.0f, 0.0f), HPoint3(10.0f, 10.0f, 10.0f)};
  SplatOctreeNode node(boundingBox, 0);
  Vector<HPoint3> positions = {HPoint3(1.0f, 1.0f, 1.0f), HPoint3(2.0f, 2.0f, 2.0f)};
  node.insert(positions, 0);
  EXPECT_EQ(node.numPoints(), 1);
  Vector<int> collect;
  node.collectPointIdxs(&collect);
  EXPECT_EQ(collect.size(), 1);
  EXPECT_EQ(collect[0], 0);
}

TEST(SplatOctreeNodeTest, CollectVisibleSplatIdxs) {
  Box3 boundingBox{HPoint3(-10.0f, -10.0f, -10.0f), HPoint3(10.0f, 10.0f, 10.0f)};
  SplatOctreeNode node(boundingBox, 0);
  Vector<HPoint3> positions = {HPoint3(-8.0f, -8.0f, -8.0f), HPoint3(8.0f, 8.0f, 8.0f)};
  node.insert(positions, 0);
  node.subdivide(positions);
  node.insert(positions, 1);
  HMatrix camera = HMatrixGen::i();
  Vector<int> collected;
  node.collectVisiblePointIdxs(camera, 6, &collected);
  EXPECT_EQ(collected.size(), 1);
  EXPECT_EQ(collected[0], 0);
}

TEST(SplatOctreeNodeTest, LodSubdivide) {
  Box3 boundingBox{HPoint3(0.0f, 0.0f, 0.0f), HPoint3(10.0f, 10.0f, 10.0f)};
  SplatOctreeNode node1(boundingBox, 0, 2);
  Vector<HPoint3> positions = {HPoint3(1.0f, 1.0f, 1.0f), HPoint3(2.0f, 2.0f, 2.0f)};
  node1.insert(positions, 0, 0);
  node1.insert(positions, 1, 1);
  EXPECT_FALSE(node1.hasChildren());
  node1.subdivide(positions);
  EXPECT_TRUE(node1.hasChildren());

  SplatOctreeNode node2(boundingBox, 0, 2);
  Vector<HPoint3> positions2;
  for (int l = 0; l < 2; l++) {
    for (int i = 0; i < node2.maxPointsTilSubdivide(); i++) {
      positions2.push_back(HPoint3(1.0f, 1.0f, 1.0f));
      node2.insert(positions2, i, l);
      EXPECT_FALSE(node2.hasChildren());
    }
  }
  positions2.push_back(HPoint3(1.0f, 1.0f, 1.0f));
  node2.insert(positions2, node2.maxPointsTilSubdivide(), 0);
  EXPECT_TRUE(node2.hasChildren());
}

TEST(SplatOctreeNodeTest, LodInsertOne) {
  Box3 boundingBox{HPoint3(0.0f, 0.0f, 0.0f), HPoint3(10.0f, 10.0f, 10.0f)};
  SplatOctreeNode node(boundingBox, 0, 2);
  Vector<HPoint3> positions = {HPoint3(1.0f, 1.0f, 1.0f), HPoint3(2.0f, 2.0f, 2.0f)};
  node.insert(positions, 0);
  EXPECT_EQ(node.numPoints(0), 1);
  EXPECT_EQ(node.numPoints(1), 0);
  Vector<int> collect;
  node.collectPointIdxs(&collect, 0);
  EXPECT_EQ(collect.size(), 1);
  EXPECT_EQ(collect[0], 0);
  Vector<int> collect2;
  node.collectPointIdxs(&collect2, 1);
  EXPECT_EQ(collect2.size(), 0);
}

}  // namespace c8
