/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000-2008, Intel Corporation, all rights reserved.
// Copyright (C) 2009, Willow Garage Inc., all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

#pragma once

#include "third_party/cvlite/core/core.hpp"
#include "third_party/cvlite/flann/defines.h"

namespace c8cv {

namespace flann {

struct CV_EXPORTS IndexParams {
  IndexParams();
  ~IndexParams();

  c8cv::String getString(const c8cv::String &key, const c8cv::String &defaultVal = c8cv::String()) const;
  int getInt(const c8cv::String &key, int defaultVal = -1) const;
  double getDouble(const c8cv::String &key, double defaultVal = -1) const;

  void setString(const c8cv::String &key, const c8cv::String &value);
  void setInt(const c8cv::String &key, int value);
  void setDouble(const c8cv::String &key, double value);
  void setFloat(const c8cv::String &key, float value);
  void setBool(const c8cv::String &key, bool value);
  void setAlgorithm(int value);

  void getAll(
    std::vector<c8cv::String> &names,
    std::vector<int> &types,
    std::vector<c8cv::String> &strValues,
    std::vector<double> &numValues) const;

  void *params;
};

struct CV_EXPORTS KDTreeIndexParams : public IndexParams {
  KDTreeIndexParams(int trees = 4);
};

struct CV_EXPORTS LinearIndexParams : public IndexParams {
  LinearIndexParams();
};

struct CV_EXPORTS CompositeIndexParams : public IndexParams {
  CompositeIndexParams(
    int trees = 4,
    int branching = 32,
    int iterations = 11,
    c8flann::flann_centers_init_t centers_init = c8flann::FLANN_CENTERS_RANDOM,
    float cb_index = 0.2f);
};

struct CV_EXPORTS AutotunedIndexParams : public IndexParams {
  AutotunedIndexParams(
    float target_precision = 0.8f,
    float build_weight = 0.01f,
    float memory_weight = 0,
    float sample_fraction = 0.1f);
};

struct CV_EXPORTS HierarchicalClusteringIndexParams : public IndexParams {
  HierarchicalClusteringIndexParams(
    int branching = 32,
    c8flann::flann_centers_init_t centers_init = c8flann::FLANN_CENTERS_RANDOM,
    int trees = 4,
    int leaf_size = 100);
};

struct CV_EXPORTS KMeansIndexParams : public IndexParams {
  KMeansIndexParams(
    int branching = 32,
    int iterations = 11,
    c8flann::flann_centers_init_t centers_init = c8flann::FLANN_CENTERS_RANDOM,
    float cb_index = 0.2f);
};

struct CV_EXPORTS LshIndexParams : public IndexParams {
  LshIndexParams(int table_number, int key_size, int multi_probe_level);
};

struct CV_EXPORTS SavedIndexParams : public IndexParams {
  SavedIndexParams(const c8cv::String &filename);
};

struct CV_EXPORTS SearchParams : public IndexParams {
  SearchParams(int checks = 32, float eps = 0, bool sorted = true);
};

class CV_EXPORTS_W Index {
public:
  CV_WRAP Index();
  CV_WRAP Index(
    c8cv::InputArray features,
    const IndexParams &params,
    c8flann::flann_distance_t distType = c8flann::FLANN_DIST_L2);
  virtual ~Index();

  CV_WRAP virtual void build(
    c8cv::InputArray features,
    const IndexParams &params,
    c8flann::flann_distance_t distType = c8flann::FLANN_DIST_L2);
  CV_WRAP virtual void knnSearch(
    c8cv::InputArray query,
    c8cv::OutputArray indices,
    c8cv::OutputArray dists,
    int knn,
    const SearchParams &params = SearchParams());

  CV_WRAP virtual int radiusSearch(
    c8cv::InputArray query,
    c8cv::OutputArray indices,
    c8cv::OutputArray dists,
    double radius,
    int maxResults,
    const SearchParams &params = SearchParams());

  CV_WRAP virtual void save(const c8cv::String &filename) const;
  CV_WRAP virtual bool load(c8cv::InputArray features, const c8cv::String &filename);
  CV_WRAP virtual void release();
  CV_WRAP c8flann::flann_distance_t getDistance() const;
  CV_WRAP c8flann::flann_algorithm_t getAlgorithm() const;

protected:
  c8flann::flann_distance_t distType;
  c8flann::flann_algorithm_t algo;
  int featureType;
  void *index;
};

}  // namespace flann
}  // namespace c8cv
