// Copyright (c) 2022 Niantic, Inc.
// Original Author: Dat Chu (datchu@nianticlabs.com)
#include <optional>

#include "c8/geometry/box3.h"
#include "c8/geometry/mesh-types.h"
#include "c8/hpoint.h"
#include "c8/vector.h"

namespace c8 {
static constexpr int DEFAULT_NUM_LOD_LEVELS = 1;
static constexpr int DEFAULT_LOD_LEVEL = 0;

class SplatOctreeNode {
public:
  SplatOctreeNode(int numLodLevels = DEFAULT_NUM_LOD_LEVELS)
      : numLodLevels_(numLodLevels), currentDepth_(0) {
    std::fill_n(children_, 8, nullptr);
    pointCountByLod_.resize(numLodLevels_);
    pointIdxsByLod_.resize(numLodLevels_);
  };
  SplatOctreeNode(HPoint3 min, HPoint3 max, int depth, int numLodLevels = DEFAULT_NUM_LOD_LEVELS)
      : aabb_{min, max}, numLodLevels_(numLodLevels), currentDepth_(depth) {
    std::fill_n(children_, 8, nullptr);
    pointCountByLod_.resize(numLodLevels_);
    pointIdxsByLod_.resize(numLodLevels_);
  };
  SplatOctreeNode(Box3 bbox, int depth, int numLodLevels = DEFAULT_NUM_LOD_LEVELS)
      : aabb_(bbox), numLodLevels_(numLodLevels), currentDepth_(depth) {
    std::fill_n(children_, 8, nullptr);
    pointCountByLod_.resize(numLodLevels_);
    pointIdxsByLod_.resize(numLodLevels_);
  };
  SplatOctreeNode(
    const Vector<HPoint3> &positions, int depth, int numLodLevels = DEFAULT_NUM_LOD_LEVELS)
      : aabb_(Box3::from(positions)), numLodLevels_(numLodLevels), currentDepth_(depth) {
    std::fill_n(children_, 8, nullptr);
    pointCountByLod_.resize(numLodLevels_);
    pointIdxsByLod_.resize(numLodLevels_);
  };
  SplatOctreeNode(const SplatOctreeNode &) = delete;             // Disable copy constructor
  SplatOctreeNode &operator=(const SplatOctreeNode &) = delete;  // Disable copy assignment operator

  void subdivide(const Vector<HPoint3> &positions);
  void insert(const Vector<HPoint3> &positions, int pointIndex, int lodLevel = DEFAULT_LOD_LEVEL);
  void collectPointIdxs(Vector<int> *collect, int lodLevel = DEFAULT_LOD_LEVEL) const;
  void collectVisiblePointIdxs(
    const HMatrix &camera, int level, Vector<int> *collect, int lodLevel = DEFAULT_LOD_LEVEL) const;

  float density(int lodLevel = DEFAULT_LOD_LEVEL) const {
    return pointCountByLod_[lodLevel] / aabb_.dimensions().l2Norm();
  }
  long long numPoints(int lodLevel = DEFAULT_LOD_LEVEL) const { return pointCountByLod_[lodLevel]; }

  bool contains(HPoint3 pt) { return aabb_.contains(pt); }
  bool hasChildren() { return hasChildren_; }  // for testing
  int maxPointsTilSubdivide() { return maxPointsTilSubdivide_; }

private:
  Box3 aabb_;
  const int treeMaxDepth_ = 8;
  const int maxPointsTilSubdivide_ = 100;

  // numLodLevels_ is the number of LoD levels in the octree. This will accordingly resize
  // pointCountByLod_ and pointIdxsByLod_
  int numLodLevels_;
  int currentDepth_;  // Depth at which this node is at.

  // pointCountByLod_ is the total number of points at or below this node. This includes points in
  // children. This is also structured by LoD level
  Vector<long long> pointCountByLod_;

  Vector<Vector<int>> pointIdxsByLod_;  // pointIdxs present at this node, structured by LoD level
  std::unique_ptr<SplatOctreeNode> children_[8];  // spatially distributed children
  bool hasChildren_ = false;                      // determines whether node has children
};

// Index into boundingBoxes_ to be stored in the tree
struct LeafData {
  Vector<size_t> indices;
};

// Octree that store a list of size_t per leaf node. This size_t index into the boundingBoxes array.
// Children follow a predictable pattern to make accesses simple.
// Here, - means less than 'origin' in that dimension, + means greater than.
// child:  0 1 2 3 4 5 6 7
// x:      - - - - + + + +
// y:      - - + + - - + +
// z:      - + - + - + - +
// Children nodes are created on-demand and not preallocated.
class Octree {
public:
  // Construct octree from a mesh.
  // @param mesh mesh geometry having at least position and triangles.
  // @param boundingBoxes hold onto this buffer until you no longer use the Octree.
  static Octree from(const MeshGeometry &mesh, Vector<Box3> *boundingBoxes);

  Octree(const Box3 &aabb, const Vector<Box3> *boundingBoxes);
  Octree(Octree &&other);

  // Disallow copying.
  Octree(const Octree &) = delete;
  Octree &operator=(const Octree &) = delete;

  ~Octree();

  // Is this node a leaf node (might have data, no children)
  bool isLeafNode() const;

  // Does this node have data? Only applicable to leaf nodes.
  bool hasData() const;

  // insert new data into this node
  void insert(const LeafData &point);

private:
  Box3 boundingBox_;
  // pointer to where indices can look up a bounding box
  const Vector<Box3> *boundingBoxes_;

  // The tree has up to eight children. Only leaves will store data
  Octree *children_[8];
  std::optional<LeafData> data_;
};
}  // namespace c8
