/***********************************************************************
 * Software License Agreement (BSD License)
 *
 * Copyright 2008-2009  Marius Muja (mariusm@cs.ubc.ca). All rights reserved.
 * Copyright 2008-2009  David G. Lowe (lowe@cs.ubc.ca). All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *************************************************************************/

#pragma once

#include "c8/exceptions.h"
#include "third_party/cvlite/flann/general.h"

#include "third_party/cvlite/flann/autotuned-index.h"
#include "third_party/cvlite/flann/composite-index.h"
#include "third_party/cvlite/flann/hierarchical-clustering-index.h"
#include "third_party/cvlite/flann/kdtree-index.h"
#include "third_party/cvlite/flann/kdtree-single-index.h"
#include "third_party/cvlite/flann/kmeans-index.h"
#include "third_party/cvlite/flann/linear-index.h"
#include "third_party/cvlite/flann/lsh-index.h"
#include "third_party/cvlite/flann/nn-index.h"

namespace c8flann {

template <typename KDTreeCapability, typename VectorSpace, typename Distance>
struct index_creator {
  static NNIndex<Distance> *create(
    const Matrix<typename Distance::ElementType> &dataset,
    const IndexParams &params,
    const Distance &distance) {
    flann_algorithm_t index_type = get_param<flann_algorithm_t>(params, "algorithm");

    NNIndex<Distance> *nnIndex;
    switch (index_type) {
      case FLANN_INDEX_LINEAR:
        nnIndex = new LinearIndex<Distance>(dataset, params, distance);
        break;
      case FLANN_INDEX_KDTREE_SINGLE:
        nnIndex = new KDTreeSingleIndex<Distance>(dataset, params, distance);
        break;
      case FLANN_INDEX_KDTREE:
        nnIndex = new KDTreeIndex<Distance>(dataset, params, distance);
        break;
      case FLANN_INDEX_KMEANS:
        nnIndex = new KMeansIndex<Distance>(dataset, params, distance);
        break;
      case FLANN_INDEX_COMPOSITE:
        nnIndex = new CompositeIndex<Distance>(dataset, params, distance);
        break;
      case FLANN_INDEX_AUTOTUNED:
        nnIndex = new AutotunedIndex<Distance>(dataset, params, distance);
        break;
      case FLANN_INDEX_HIERARCHICAL:
        nnIndex = new HierarchicalClusteringIndex<Distance>(dataset, params, distance);
        break;
      case FLANN_INDEX_LSH:
        nnIndex = new LshIndex<Distance>(dataset, params, distance);
        break;
      default:
        C8_THROW("Unknown index type");
    }

    return nnIndex;
  }
};

template <typename VectorSpace, typename Distance>
struct index_creator<False, VectorSpace, Distance> {
  static NNIndex<Distance> *create(
    const Matrix<typename Distance::ElementType> &dataset,
    const IndexParams &params,
    const Distance &distance) {
    flann_algorithm_t index_type = get_param<flann_algorithm_t>(params, "algorithm");

    NNIndex<Distance> *nnIndex;
    switch (index_type) {
      case FLANN_INDEX_LINEAR:
        nnIndex = new LinearIndex<Distance>(dataset, params, distance);
        break;
      case FLANN_INDEX_KMEANS:
        nnIndex = new KMeansIndex<Distance>(dataset, params, distance);
        break;
      case FLANN_INDEX_HIERARCHICAL:
        nnIndex = new HierarchicalClusteringIndex<Distance>(dataset, params, distance);
        break;
      case FLANN_INDEX_LSH:
        nnIndex = new LshIndex<Distance>(dataset, params, distance);
        break;
      default:
        C8_THROW("Unknown index type");
    }

    return nnIndex;
  }
};

template <typename Distance>
struct index_creator<False, False, Distance> {
  static NNIndex<Distance> *create(
    const Matrix<typename Distance::ElementType> &dataset,
    const IndexParams &params,
    const Distance &distance) {
    flann_algorithm_t index_type = get_param<flann_algorithm_t>(params, "algorithm");

    NNIndex<Distance> *nnIndex;
    switch (index_type) {
      case FLANN_INDEX_LINEAR:
        nnIndex = new LinearIndex<Distance>(dataset, params, distance);
        break;
      case FLANN_INDEX_HIERARCHICAL:
        nnIndex = new HierarchicalClusteringIndex<Distance>(dataset, params, distance);
        break;
      case FLANN_INDEX_LSH:
        nnIndex = new LshIndex<Distance>(dataset, params, distance);
        break;
      default:
        C8_THROW("Unknown index type");
    }

    return nnIndex;
  }
};

template <typename Distance>
NNIndex<Distance> *create_index_by_type(
  const Matrix<typename Distance::ElementType> &dataset,
  const IndexParams &params,
  const Distance &distance) {
  return index_creator<
    typename Distance::is_kdtree_distance,
    typename Distance::is_vector_space_distance,
    Distance>::create(dataset, params, distance);
}

}  // namespace c8flann
