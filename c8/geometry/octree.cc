// Copyright (c) 2022 Niantic, Inc.
// Original Author: Dat Chu (datchu@nianticlabs.com)

#include "bzl/inliner/rules2.h"
#include "c8/hpoint.h"

cc_library {
  hdrs = {"octree.h"};
  deps = {
    "//c8/stats:scope-timer",
    "//c8:c8-log",
    "//c8:vector",
    ":box3",
    ":mesh-types",
  };
  copts = {};
  visibility = {
    "//visibility:public",
  };
}
cc_end(0xfab0e555);

#include "c8/c8-log.h"
#include "c8/geometry/octree.h"
#include "c8/stats/scope-timer.h"

namespace c8 {
namespace {
// Whichever box+idx intersects with the boundary is pushed into childData
void collectOverlapBoxes(
  const Box3 &boundary,
  const LeafData &data,
  const Vector<Box3> *boundingBoxes,
  LeafData *childData) {
  for (auto idx : data.indices) {
    if (boundary.intersects((*boundingBoxes)[idx])) {
      childData->indices.push_back(idx);
    }
  }
}
}  // namespace

void SplatOctreeNode::subdivide(const Vector<HPoint3> &positions) {
  if (!hasChildren_) {
    int i = 0;
    for (auto bbox : aabb_.splitOctants()) {
      if (children_[i] == nullptr) {
        children_[i++] = std::make_unique<SplatOctreeNode>(bbox, currentDepth_ + 1, numLodLevels_);
      } else {
        C8Log("[octree] Error: children_ is not empty when subdividing");
      }
    }

    for (int lodLevel = 0; lodLevel < numLodLevels_; lodLevel++) {
      for (auto idx : pointIdxsByLod_[lodLevel]) {
        for (auto &child : children_) {
          if (child->contains(positions[idx])) {
            child->insert(positions, idx, lodLevel);
            break;
          }
        }
      }
      pointIdxsByLod_[lodLevel].clear();
    }
  }
  hasChildren_ = true;
}

void SplatOctreeNode::insert(const Vector<HPoint3> &positions, int pointIndex, int lodLevel) {
  pointCountByLod_[lodLevel]++;
  // Since the octree is depth limited, if we're at max depth just insert
  if (currentDepth_ >= treeMaxDepth_) {
    pointIdxsByLod_[lodLevel].push_back(pointIndex);
    return;
  }

  // If we have space at the current node, then insert here.
  if (!hasChildren_ && pointIdxsByLod_[lodLevel].size() < maxPointsTilSubdivide_) {
    pointIdxsByLod_[lodLevel].push_back(pointIndex);
    return;
  }

  // If we're at capacity at this node, we subdivide and move all points to children. Points only
  // reside at leaves. This will only happen once per node
  if (!hasChildren_ && pointIdxsByLod_[lodLevel].size() >= maxPointsTilSubdivide_) {
    subdivide(positions);
  }
  // if this node has already been divided, insert into correct node
  if (hasChildren_) {
    for (auto &child : children_) {
      if (child->contains(positions[pointIndex])) {
        child->insert(positions, pointIndex, lodLevel);
        break;
      }
    }
  }
}

void SplatOctreeNode::collectPointIdxs(Vector<int> *collect, int lodLevel) const {
  if (hasChildren_) {
    for (auto &child : children_) {
      child->collectPointIdxs(collect, lodLevel);
    }
  } else {
    for (auto idx : pointIdxsByLod_[lodLevel]) {
      collect->push_back(idx);
    }
  }
}

void SplatOctreeNode::collectVisiblePointIdxs(
  const HMatrix &camera, int level, Vector<int> *collect, int lodLevel) const {
  // We want to collect all the point idxs that are visible, with our visibility granularity being
  // `level` (ie if the bbox at `level` is visible, we collect all points in that bbox).
  // By checking visbiility at lower levels of the octree, we can avoid unnecessary checks at a
  // higher level

  // We can improve this visiblity check to do frustum intersection with bbox
  bool visible = false;
  auto pointsToCheck = aabb_.corners();
  pointsToCheck.push_back(aabb_.center());
  for (auto vertex : pointsToCheck) {
    auto pt = camera.inv() * vertex;
    if (pt.z() < 0) {
      visible = true;
      break;
    }
  }
  if (!visible) {
    return;
  }
  if (currentDepth_ < level && hasChildren_) {
    for (auto &child : children_) {
      child->collectVisiblePointIdxs(camera, level, collect, lodLevel);
    }
  }
  if ((currentDepth_ < level && !hasChildren_) || (currentDepth_ == level)) {
    collectPointIdxs(collect, lodLevel);
  }
}

// Below are previous Octree Functions

Octree Octree::from(const MeshGeometry &mesh, Vector<Box3> *boundingBoxes) {
  ScopeTimer t("octree-from-mesh");
  Octree tree{Box3::from(mesh.points), boundingBoxes};
  // TODO(dat): Remove printing before octree is ready for usage. Right now things are super slow.
  C8Log("[octree] Processing %d points and %d tri", mesh.points.size(), mesh.triangles.size());
  boundingBoxes->clear();
  boundingBoxes->reserve(mesh.triangles.size());
  for (const auto &triIndices : mesh.triangles) {
    boundingBoxes->push_back(Box3::from(
      {mesh.points[triIndices.a], mesh.points[triIndices.b], mesh.points[triIndices.c]}));
  }

  for (size_t i = 0; i < mesh.triangles.size(); i++) {
    if (i % 1000 == 0) {
      C8Log("[octree] Processing %d-th tri %.2f", i, 1.f * i / mesh.triangles.size());
    }
    tree.insert({{i}});
  }
  return tree;
}

Octree::Octree(const Box3 &aabb, const Vector<Box3> *boundingBoxes)
    : boundingBox_(aabb), boundingBoxes_(boundingBoxes) {
  // Initially, there is no child in children_
  for (int i = 0; i < 8; ++i) {
    children_[i] = NULL;
  }
}

Octree::Octree(Octree &&other) {
  boundingBox_ = other.boundingBox_;
  data_ = std::move(other.data_);
  for (int i = 0; i < 8; i++) {
    children_[i] = other.children_[i];
    other.children_[i] = NULL;
  }
}

Octree::~Octree() {
  // Recursively destroy octants
  for (int i = 0; i < 8; ++i) {
    delete children_[i];
    children_[i] = NULL;
  }
}

bool Octree::isLeafNode() const {
  // We are a leaf iff we have no children_. Since we either have none, or all eight, it is
  // sufficient to just check the first.
  return children_[0] == NULL;
}

bool Octree::hasData() const { return data_.has_value(); }

void Octree::insert(const LeafData &point) {
  if (!isLeafNode()) {
    // We are not at a leaf, insert recursively into whichever child octants that our bb can fit.
    for (int i = 0; i < 8; i++) {
      LeafData childData;
      collectOverlapBoxes(children_[i]->boundingBox_, point, boundingBoxes_, &childData);
      // If we overlap with this child, insert data into it
      if (childData.indices.size() > 0) {
        children_[i]->insert(childData);
      }
    }
    return;
  }

  // We are dealing with a leaf node from here on.
  if (!data_) {
    // There is still space to put our data
    data_ = point;
    return;
  }

  // We're at a leaf, but there's already something here. We will split this node so that it has
  // 8 child octants and then insert the old data that was here, along with this new data point.

  // Save this data point that was here for a later re-insert
  LeafData oldPoint = *data_;
  data_.reset();

  // Split the current node and create new empty trees for each child octant.
  auto childOctants = boundingBox_.splitOctants();
  for (int i = 0; i < 8; ++i) {
    children_[i] = new Octree(childOctants[i], boundingBoxes_);
  }

  // Re-insert the old point, then insert the new point
  // We don't need to insert from the root, because we already know it's not in this part of the
  // tree
  for (int i = 0; i < 8; i++) {
    LeafData childData;
    collectOverlapBoxes(children_[i]->boundingBox_, point, boundingBoxes_, &childData);
    collectOverlapBoxes(children_[i]->boundingBox_, oldPoint, boundingBoxes_, &childData);
    // If we overlap with this child, insert data into it
    if (childData.indices.size() > 0) {
      children_[i]->insert(childData);
    }
  }
}

}  // namespace c8
