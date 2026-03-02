#include "third_party/cvlite/flann/miniflann.h"

#include <cstdarg>
#include <cstdio>
#include <sstream>

#include "third_party/cvlite/core/core.hpp"
#include "third_party/cvlite/core/utility.hpp"

#include "third_party/cvlite/flann/dist.h"
#include "third_party/cvlite/flann/dummy.h"
#include "third_party/cvlite/flann/general.h"
#include "third_party/cvlite/flann/index-testing.h"
#include "third_party/cvlite/flann/params.h"
#include "third_party/cvlite/flann/saving.h"

// index types
#include "third_party/cvlite/flann/all-indices.h"
#include "third_party/cvlite/flann/flann-base.h"
#include "third_party/cvlite/core/private.hpp"

#define MINIFLANN_SUPPORT_EXOTIC_DISTANCE_TYPES 0

static c8flann::IndexParams &get_params(const c8cv::flann::IndexParams &p) {
  return *(c8flann::IndexParams *)(p.params);
}

c8cv::flann::IndexParams::~IndexParams() { delete &get_params(*this); }

namespace c8cv {

namespace flann {

using namespace c8flann;

IndexParams::IndexParams() { params = new ::c8flann::IndexParams(); }

template <typename T>
T getParam(const IndexParams &_p, const c8cv::String &key, const T &defaultVal = T()) {
  ::c8flann::IndexParams &p = get_params(_p);
  ::c8flann::IndexParams::const_iterator it = p.find(key);
  if (it == p.end())
    return defaultVal;
  return it->second.cast<T>();
}

template <typename T>
void setParam(IndexParams &_p, const c8cv::String &key, const T &value) {
  ::c8flann::IndexParams &p = get_params(_p);
  p[key] = value;
}

c8cv::String IndexParams::getString(const c8cv::String &key, const c8cv::String &defaultVal) const {
  return getParam(*this, key, defaultVal);
}

int IndexParams::getInt(const c8cv::String &key, int defaultVal) const {
  return getParam(*this, key, defaultVal);
}

double IndexParams::getDouble(const c8cv::String &key, double defaultVal) const {
  return getParam(*this, key, defaultVal);
}

void IndexParams::setString(const c8cv::String &key, const c8cv::String &value) {
  setParam(*this, key, value);
}

void IndexParams::setInt(const c8cv::String &key, int value) { setParam(*this, key, value); }

void IndexParams::setDouble(const c8cv::String &key, double value) { setParam(*this, key, value); }

void IndexParams::setFloat(const c8cv::String &key, float value) { setParam(*this, key, value); }

void IndexParams::setBool(const c8cv::String &key, bool value) { setParam(*this, key, value); }

void IndexParams::setAlgorithm(int value) {
  setParam(*this, "algorithm", (c8flann::flann_algorithm_t)value);
}

void IndexParams::getAll(
  std::vector<c8cv::String> &names,
  std::vector<int> &types,
  std::vector<c8cv::String> &strValues,
  std::vector<double> &numValues) const {
  names.clear();
  types.clear();
  strValues.clear();
  numValues.clear();

  ::c8flann::IndexParams &p = get_params(*this);
  ::c8flann::IndexParams::const_iterator it = p.begin(), it_end = p.end();

  for (; it != it_end; ++it) {
    names.push_back(it->first);
    try {
      c8cv::String val = it->second.cast<c8cv::String>();
      types.push_back(CV_USRTYPE1);
      strValues.push_back(val);
      numValues.push_back(-1);
      continue;
    } catch (...) {
    }

    strValues.push_back(it->second.type().name());

    try {
      double val = it->second.cast<double>();
      types.push_back(CV_64F);
      numValues.push_back(val);
      continue;
    } catch (...) {
    }
    try {
      float val = it->second.cast<float>();
      types.push_back(CV_32F);
      numValues.push_back(val);
      continue;
    } catch (...) {
    }
    try {
      int val = it->second.cast<int>();
      types.push_back(CV_32S);
      numValues.push_back(val);
      continue;
    } catch (...) {
    }
    try {
      short val = it->second.cast<short>();
      types.push_back(CV_16S);
      numValues.push_back(val);
      continue;
    } catch (...) {
    }
    try {
      ushort val = it->second.cast<ushort>();
      types.push_back(CV_16U);
      numValues.push_back(val);
      continue;
    } catch (...) {
    }
    try {
      char val = it->second.cast<char>();
      types.push_back(CV_8S);
      numValues.push_back(val);
      continue;
    } catch (...) {
    }
    try {
      uchar val = it->second.cast<uchar>();
      types.push_back(CV_8U);
      numValues.push_back(val);
      continue;
    } catch (...) {
    }
    try {
      bool val = it->second.cast<bool>();
      types.push_back(CV_MAKETYPE(CV_USRTYPE1, 2));
      numValues.push_back(val);
      continue;
    } catch (...) {
    }
    try {
      c8flann::flann_algorithm_t val = it->second.cast<c8flann::flann_algorithm_t>();
      types.push_back(CV_MAKETYPE(CV_USRTYPE1, 3));
      numValues.push_back(val);
      continue;
    } catch (...) {
    }

    types.push_back(-1);  // unknown type
    numValues.push_back(-1);
  }
}

KDTreeIndexParams::KDTreeIndexParams(int trees) {
  ::c8flann::IndexParams &p = get_params(*this);
  p["algorithm"] = FLANN_INDEX_KDTREE;
  p["trees"] = trees;
}

LinearIndexParams::LinearIndexParams() {
  ::c8flann::IndexParams &p = get_params(*this);
  p["algorithm"] = FLANN_INDEX_LINEAR;
}

CompositeIndexParams::CompositeIndexParams(
  int trees, int branching, int iterations, flann_centers_init_t centers_init, float cb_index) {
  ::c8flann::IndexParams &p = get_params(*this);
  p["algorithm"] = FLANN_INDEX_KMEANS;
  // number of randomized trees to use (for kdtree)
  p["trees"] = trees;
  // branching factor
  p["branching"] = branching;
  // max iterations to perform in one kmeans clustering (kmeans tree)
  p["iterations"] = iterations;
  // algorithm used for picking the initial cluster centers for kmeans tree
  p["centers_init"] = centers_init;
  // cluster boundary index. Used when searching the kmeans tree
  p["cb_index"] = cb_index;
}

AutotunedIndexParams::AutotunedIndexParams(
  float target_precision, float build_weight, float memory_weight, float sample_fraction) {
  ::c8flann::IndexParams &p = get_params(*this);
  p["algorithm"] = FLANN_INDEX_AUTOTUNED;
  // precision desired (used for autotuning, -1 otherwise)
  p["target_precision"] = target_precision;
  // build tree time weighting factor
  p["build_weight"] = build_weight;
  // index memory weighting factor
  p["memory_weight"] = memory_weight;
  // what fraction of the dataset to use for autotuning
  p["sample_fraction"] = sample_fraction;
}

KMeansIndexParams::KMeansIndexParams(
  int branching, int iterations, flann_centers_init_t centers_init, float cb_index) {
  ::c8flann::IndexParams &p = get_params(*this);
  p["algorithm"] = FLANN_INDEX_KMEANS;
  // branching factor
  p["branching"] = branching;
  // max iterations to perform in one kmeans clustering (kmeans tree)
  p["iterations"] = iterations;
  // algorithm used for picking the initial cluster centers for kmeans tree
  p["centers_init"] = centers_init;
  // cluster boundary index. Used when searching the kmeans tree
  p["cb_index"] = cb_index;
}

HierarchicalClusteringIndexParams::HierarchicalClusteringIndexParams(
  int branching, flann_centers_init_t centers_init, int trees, int leaf_size) {
  ::c8flann::IndexParams &p = get_params(*this);
  p["algorithm"] = FLANN_INDEX_HIERARCHICAL;
  // The branching factor used in the hierarchical clustering
  p["branching"] = branching;
  // Algorithm used for picking the initial cluster centers
  p["centers_init"] = centers_init;
  // number of parallel trees to build
  p["trees"] = trees;
  // maximum leaf size
  p["leaf_size"] = leaf_size;
}

LshIndexParams::LshIndexParams(int table_number, int key_size, int multi_probe_level) {
  ::c8flann::IndexParams &p = get_params(*this);
  p["algorithm"] = FLANN_INDEX_LSH;
  // The number of hash tables to use
  p["table_number"] = table_number;
  // The length of the key in the hash tables
  p["key_size"] = key_size;
  // Number of levels to use in multi-probe (0 for standard LSH)
  p["multi_probe_level"] = multi_probe_level;
}

SavedIndexParams::SavedIndexParams(const c8cv::String &_filename) {
  c8cv::String filename = _filename;
  ::c8flann::IndexParams &p = get_params(*this);

  p["algorithm"] = FLANN_INDEX_SAVED;
  p["filename"] = filename;
}

SearchParams::SearchParams(int checks, float eps, bool sorted) {
  ::c8flann::IndexParams &p = get_params(*this);

  // how many leafs to visit when searching for neighbours (-1 for unlimited)
  p["checks"] = checks;
  // search for eps-approximate neighbours (default: 0)
  p["eps"] = eps;
  // only for radius search, require neighbours sorted by distance (default: true)
  p["sorted"] = sorted;
}

template <typename Distance, typename IndexType>
void buildIndex_(
  void *&index, const c8cv::Mat &data, const IndexParams &params, const Distance &dist = Distance()) {
  typedef typename Distance::ElementType ElementType;
  if (c8cv::DataType<ElementType>::type != data.type())
    C8CV_Error_(c8cv::Error::StsUnsupportedFormat, ("type=%d\n", data.type()));
  if (!data.isContinuous())
    C8CV_Error(c8cv::Error::StsBadArg, "Only continuous arrays are supported");

  ::c8flann::Matrix<ElementType> dataset((ElementType *)data.data, data.rows, data.cols);
  IndexType *_index = new IndexType(dataset, get_params(params), dist);

  try {
    _index->buildIndex();
  } catch (...) {
    delete _index;
    _index = NULL;

    throw;
  }

  index = _index;
}

template <typename Distance>
void buildIndex(
  void *&index, const c8cv::Mat &data, const IndexParams &params, const Distance &dist = Distance()) {
  buildIndex_<Distance, ::c8flann::Index<Distance> >(index, data, params, dist);
}

typedef ::c8flann::HammingPopcount HammingDistance;

Index::Index() {
  index = 0;
  featureType = CV_32F;
  algo = FLANN_INDEX_LINEAR;
  distType = FLANN_DIST_L2;
}

Index::Index(c8cv::InputArray _data, const IndexParams &params, flann_distance_t _distType) {
  index = 0;
  featureType = CV_32F;
  algo = FLANN_INDEX_LINEAR;
  distType = FLANN_DIST_L2;
  build(_data, params, _distType);
}

void Index::build(c8cv::InputArray _data, const IndexParams &params, flann_distance_t _distType) {
  CV_INSTRUMENT_REGION()

  release();
  algo = getParam<flann_algorithm_t>(params, "algorithm", FLANN_INDEX_LINEAR);
  if (algo == FLANN_INDEX_SAVED) {
    load(_data, getParam<c8cv::String>(params, "filename", c8cv::String()));
    return;
  }

  c8cv::Mat data = _data.getMat();
  index = 0;
  featureType = data.type();
  distType = _distType;

  if (algo == FLANN_INDEX_LSH) {
    distType = FLANN_DIST_HAMMING;
  }

  switch (distType) {
    case FLANN_DIST_HAMMING:
      buildIndex<HammingDistance>(index, data, params);
      break;
    case FLANN_DIST_L2:
      buildIndex< ::c8flann::L2<float> >(index, data, params);
      break;
    case FLANN_DIST_L1:
      buildIndex< ::c8flann::L1<float> >(index, data, params);
      break;
#if MINIFLANN_SUPPORT_EXOTIC_DISTANCE_TYPES
    case FLANN_DIST_MAX:
      buildIndex< ::c8flann::MaxDistance<float> >(index, data, params);
      break;
    case FLANN_DIST_HIST_INTERSECT:
      buildIndex< ::c8flann::HistIntersectionDistance<float> >(index, data, params);
      break;
    case FLANN_DIST_HELLINGER:
      buildIndex< ::c8flann::HellingerDistance<float> >(index, data, params);
      break;
    case FLANN_DIST_CHI_SQUARE:
      buildIndex< ::c8flann::ChiSquareDistance<float> >(index, data, params);
      break;
    case FLANN_DIST_KL:
      buildIndex< ::c8flann::KL_Divergence<float> >(index, data, params);
      break;
#endif
    default:
      C8CV_Error(c8cv::Error::StsBadArg, "Unknown/unsupported distance type");
  }
}

template <typename IndexType>
void deleteIndex_(void *index) {
  delete (IndexType *)index;
}

template <typename Distance>
void deleteIndex(void *index) {
  deleteIndex_< ::c8flann::Index<Distance> >(index);
}

Index::~Index() { release(); }

void Index::release() {
  CV_INSTRUMENT_REGION()

  if (!index)
    return;

  switch (distType) {
    case FLANN_DIST_HAMMING:
      deleteIndex<HammingDistance>(index);
      break;
    case FLANN_DIST_L2:
      deleteIndex< ::c8flann::L2<float> >(index);
      break;
    case FLANN_DIST_L1:
      deleteIndex< ::c8flann::L1<float> >(index);
      break;
#if MINIFLANN_SUPPORT_EXOTIC_DISTANCE_TYPES
    case FLANN_DIST_MAX:
      deleteIndex< ::c8flann::MaxDistance<float> >(index);
      break;
    case FLANN_DIST_HIST_INTERSECT:
      deleteIndex< ::c8flann::HistIntersectionDistance<float> >(index);
      break;
    case FLANN_DIST_HELLINGER:
      deleteIndex< ::c8flann::HellingerDistance<float> >(index);
      break;
    case FLANN_DIST_CHI_SQUARE:
      deleteIndex< ::c8flann::ChiSquareDistance<float> >(index);
      break;
    case FLANN_DIST_KL:
      deleteIndex< ::c8flann::KL_Divergence<float> >(index);
      break;
#endif
    default:
      C8CV_Error(c8cv::Error::StsBadArg, "Unknown/unsupported distance type");
  }
  index = 0;
}

template <typename Distance, typename IndexType>
void runKnnSearch_(
  void *index,
  const c8cv::Mat &query,
  c8cv::Mat &indices,
  c8cv::Mat &dists,
  int knn,
  const SearchParams &params) {
  typedef typename Distance::ElementType ElementType;
  typedef typename Distance::ResultType DistanceType;
  int type = c8cv::DataType<ElementType>::type;
  int dtype = c8cv::DataType<DistanceType>::type;
  C8CV_Assert(query.type() == type && indices.type() == CV_32S && dists.type() == dtype);
  C8CV_Assert(query.isContinuous() && indices.isContinuous() && dists.isContinuous());

  ::c8flann::Matrix<ElementType> _query((ElementType *)query.data, query.rows, query.cols);
  ::c8flann::Matrix<int> _indices(indices.ptr<int>(), indices.rows, indices.cols);
  ::c8flann::Matrix<DistanceType> _dists(dists.ptr<DistanceType>(), dists.rows, dists.cols);

  ((IndexType *)index)
    ->knnSearch(_query, _indices, _dists, knn, (const ::c8flann::SearchParams &)get_params(params));
}

template <typename Distance>
void runKnnSearch(
  void *index,
  const c8cv::Mat &query,
  c8cv::Mat &indices,
  c8cv::Mat &dists,
  int knn,
  const SearchParams &params) {
  runKnnSearch_<Distance, ::c8flann::Index<Distance> >(index, query, indices, dists, knn, params);
}

template <typename Distance, typename IndexType>
int runRadiusSearch_(
  void *index,
  const c8cv::Mat &query,
  c8cv::Mat &indices,
  c8cv::Mat &dists,
  double radius,
  const SearchParams &params) {
  typedef typename Distance::ElementType ElementType;
  typedef typename Distance::ResultType DistanceType;
  int type = c8cv::DataType<ElementType>::type;
  int dtype = c8cv::DataType<DistanceType>::type;
  C8CV_Assert(query.type() == type && indices.type() == CV_32S && dists.type() == dtype);
  C8CV_Assert(query.isContinuous() && indices.isContinuous() && dists.isContinuous());

  ::c8flann::Matrix<ElementType> _query((ElementType *)query.data, query.rows, query.cols);
  ::c8flann::Matrix<int> _indices(indices.ptr<int>(), indices.rows, indices.cols);
  ::c8flann::Matrix<DistanceType> _dists(dists.ptr<DistanceType>(), dists.rows, dists.cols);

  return ((IndexType *)index)
    ->radiusSearch(
      _query,
      _indices,
      _dists,
      c8cv::saturate_cast<float>(radius),
      (const ::c8flann::SearchParams &)get_params(params));
}

template <typename Distance>
int runRadiusSearch(
  void *index,
  const c8cv::Mat &query,
  c8cv::Mat &indices,
  c8cv::Mat &dists,
  double radius,
  const SearchParams &params) {
  return runRadiusSearch_<Distance, ::c8flann::Index<Distance> >(
    index, query, indices, dists, radius, params);
}

static void createIndicesDists(
  c8cv::OutputArray _indices,
  c8cv::OutputArray _dists,
  c8cv::Mat &indices,
  c8cv::Mat &dists,
  int rows,
  int minCols,
  int maxCols,
  int dtype) {
  if (_indices.needed()) {
    indices = _indices.getMat();
    if (
      !indices.isContinuous() || indices.type() != CV_32S || indices.rows != rows
      || indices.cols < minCols
      || indices.cols > maxCols) {
      if (!indices.isContinuous())
        _indices.release();
      _indices.create(rows, minCols, CV_32S);
      indices = _indices.getMat();
    }
  } else
    indices.create(rows, minCols, CV_32S);

  if (_dists.needed()) {
    dists = _dists.getMat();
    if (
      !dists.isContinuous() || dists.type() != dtype || dists.rows != rows || dists.cols < minCols
      || dists.cols > maxCols) {
      if (!indices.isContinuous())
        _dists.release();
      _dists.create(rows, minCols, dtype);
      dists = _dists.getMat();
    }
  } else
    dists.create(rows, minCols, dtype);
}

void Index::knnSearch(
  c8cv::InputArray _query,
  c8cv::OutputArray _indices,
  c8cv::OutputArray _dists,
  int knn,
  const SearchParams &params) {
  CV_INSTRUMENT_REGION()

  c8cv::Mat query = _query.getMat(), indices, dists;
  int dtype = distType == FLANN_DIST_HAMMING ? CV_32S : CV_32F;

  createIndicesDists(_indices, _dists, indices, dists, query.rows, knn, knn, dtype);

  switch (distType) {
    case FLANN_DIST_HAMMING:
      runKnnSearch<HammingDistance>(index, query, indices, dists, knn, params);
      break;
    case FLANN_DIST_L2:
      runKnnSearch< ::c8flann::L2<float> >(index, query, indices, dists, knn, params);
      break;
    case FLANN_DIST_L1:
      runKnnSearch< ::c8flann::L1<float> >(index, query, indices, dists, knn, params);
      break;
#if MINIFLANN_SUPPORT_EXOTIC_DISTANCE_TYPES
    case FLANN_DIST_MAX:
      runKnnSearch< ::c8flann::MaxDistance<float> >(index, query, indices, dists, knn, params);
      break;
    case FLANN_DIST_HIST_INTERSECT:
      runKnnSearch< ::c8flann::HistIntersectionDistance<float> >(
        index, query, indices, dists, knn, params);
      break;
    case FLANN_DIST_HELLINGER:
      runKnnSearch< ::c8flann::HellingerDistance<float> >(
        index, query, indices, dists, knn, params);
      break;
    case FLANN_DIST_CHI_SQUARE:
      runKnnSearch< ::c8flann::ChiSquareDistance<float> >(
        index, query, indices, dists, knn, params);
      break;
    case FLANN_DIST_KL:
      runKnnSearch< ::c8flann::KL_Divergence<float> >(index, query, indices, dists, knn, params);
      break;
#endif
    default:
      C8CV_Error(c8cv::Error::StsBadArg, "Unknown/unsupported distance type");
  }
}

int Index::radiusSearch(
  c8cv::InputArray _query,
  c8cv::OutputArray _indices,
  c8cv::OutputArray _dists,
  double radius,
  int maxResults,
  const SearchParams &params) {
  CV_INSTRUMENT_REGION()

  c8cv::Mat query = _query.getMat(), indices, dists;
  int dtype = distType == FLANN_DIST_HAMMING ? CV_32S : CV_32F;
  C8CV_Assert(maxResults > 0);
  createIndicesDists(_indices, _dists, indices, dists, query.rows, maxResults, INT_MAX, dtype);

  if (algo == FLANN_INDEX_LSH)
    C8CV_Error(c8cv::Error::StsNotImplemented, "LSH index does not support radiusSearch operation");

  switch (distType) {
    case FLANN_DIST_HAMMING:
      return runRadiusSearch<HammingDistance>(index, query, indices, dists, radius, params);

    case FLANN_DIST_L2:
      return runRadiusSearch< ::c8flann::L2<float> >(index, query, indices, dists, radius, params);
    case FLANN_DIST_L1:
      return runRadiusSearch< ::c8flann::L1<float> >(index, query, indices, dists, radius, params);
#if MINIFLANN_SUPPORT_EXOTIC_DISTANCE_TYPES
    case FLANN_DIST_MAX:
      return runRadiusSearch< ::c8flann::MaxDistance<float> >(
        index, query, indices, dists, radius, params);
    case FLANN_DIST_HIST_INTERSECT:
      return runRadiusSearch< ::c8flann::HistIntersectionDistance<float> >(
        index, query, indices, dists, radius, params);
    case FLANN_DIST_HELLINGER:
      return runRadiusSearch< ::c8flann::HellingerDistance<float> >(
        index, query, indices, dists, radius, params);
    case FLANN_DIST_CHI_SQUARE:
      return runRadiusSearch< ::c8flann::ChiSquareDistance<float> >(
        index, query, indices, dists, radius, params);
    case FLANN_DIST_KL:
      return runRadiusSearch< ::c8flann::KL_Divergence<float> >(
        index, query, indices, dists, radius, params);
#endif
    default:
      C8CV_Error(c8cv::Error::StsBadArg, "Unknown/unsupported distance type");
  }
  return -1;
}

flann_distance_t Index::getDistance() const { return distType; }

flann_algorithm_t Index::getAlgorithm() const { return algo; }

template <typename IndexType>
void saveIndex_(const Index *index0, const void *index, FILE *fout) {
  IndexType *_index = (IndexType *)index;
  ::c8flann::save_header(fout, *_index);
  // some compilers may store short enumerations as bytes,
  // so make sure we always write integers (which are 4-byte values in any modern C compiler)
  int idistType = (int)index0->getDistance();
  ::c8flann::save_value<int>(fout, idistType);
  _index->saveIndex(fout);
}

template <typename Distance>
void saveIndex(const Index *index0, const void *index, FILE *fout) {
  saveIndex_< ::c8flann::Index<Distance> >(index0, index, fout);
}

void Index::save(const c8cv::String &filename) const {
  CV_INSTRUMENT_REGION()

  FILE *fout = fopen(filename.c_str(), "wb");
  if (fout == NULL)
    C8CV_Error_(
      c8cv::Error::StsError, ("Can not open file %s for writing FLANN index\n", filename.c_str()));

  switch (distType) {
    case FLANN_DIST_HAMMING:
      saveIndex<HammingDistance>(this, index, fout);
      break;
    case FLANN_DIST_L2:
      saveIndex< ::c8flann::L2<float> >(this, index, fout);
      break;
    case FLANN_DIST_L1:
      saveIndex< ::c8flann::L1<float> >(this, index, fout);
      break;
#if MINIFLANN_SUPPORT_EXOTIC_DISTANCE_TYPES
    case FLANN_DIST_MAX:
      saveIndex< ::c8flann::MaxDistance<float> >(this, index, fout);
      break;
    case FLANN_DIST_HIST_INTERSECT:
      saveIndex< ::c8flann::HistIntersectionDistance<float> >(this, index, fout);
      break;
    case FLANN_DIST_HELLINGER:
      saveIndex< ::c8flann::HellingerDistance<float> >(this, index, fout);
      break;
    case FLANN_DIST_CHI_SQUARE:
      saveIndex< ::c8flann::ChiSquareDistance<float> >(this, index, fout);
      break;
    case FLANN_DIST_KL:
      saveIndex< ::c8flann::KL_Divergence<float> >(this, index, fout);
      break;
#endif
    default:
      fclose(fout);
      fout = 0;
      C8CV_Error(c8cv::Error::StsBadArg, "Unknown/unsupported distance type");
  }
  if (fout)
    fclose(fout);
}

template <typename Distance, typename IndexType>
bool loadIndex_(
  Index *index0, void *&index, const c8cv::Mat &data, FILE *fin, const Distance &dist = Distance()) {
  typedef typename Distance::ElementType ElementType;
  C8CV_Assert(c8cv::DataType<ElementType>::type == data.type() && data.isContinuous());

  ::c8flann::Matrix<ElementType> dataset((ElementType *)data.data, data.rows, data.cols);

  ::c8flann::IndexParams params;
  params["algorithm"] = index0->getAlgorithm();
  IndexType *_index = new IndexType(dataset, params, dist);
  _index->loadIndex(fin);
  index = _index;
  return true;
}

template <typename Distance>
bool loadIndex(
  Index *index0, void *&index, const c8cv::Mat &data, FILE *fin, const Distance &dist = Distance()) {
  return loadIndex_<Distance, ::c8flann::Index<Distance> >(index0, index, data, fin, dist);
}

bool Index::load(c8cv::InputArray _data, const c8cv::String &filename) {
  c8cv::Mat data = _data.getMat();
  bool ok = true;
  release();
  FILE *fin = fopen(filename.c_str(), "rb");
  if (fin == NULL)
    return false;

  ::c8flann::IndexHeader header = ::c8flann::load_header(fin);
  algo = header.index_type;
  featureType = header.data_type == FLANN_UINT8
    ? CV_8U
    : header.data_type == FLANN_INT8 ? CV_8S
                                     : header.data_type == FLANN_UINT16
        ? CV_16U
        : header.data_type == FLANN_INT16 ? CV_16S
                                          : header.data_type == FLANN_INT32
            ? CV_32S
            : header.data_type == FLANN_FLOAT32 ? CV_32F
                                                : header.data_type == FLANN_FLOAT64 ? CV_64F : -1;

  if (
    (int)header.rows != data.rows || (int)header.cols != data.cols || featureType != data.type()) {
    fprintf(
      stderr,
      "Reading FLANN index error: the saved data size (%d, %d) or type (%d) is different from the "
      "passed one (%d, %d), %d\n",
      (int)header.rows,
      (int)header.cols,
      featureType,
      data.rows,
      data.cols,
      data.type());
    fclose(fin);
    return false;
  }

  int idistType = 0;
  ::c8flann::load_value(fin, idistType);
  distType = (flann_distance_t)idistType;

  if (!((distType == FLANN_DIST_HAMMING && featureType == CV_8U)
        || (distType != FLANN_DIST_HAMMING && featureType == CV_32F))) {
    fprintf(
      stderr,
      "Reading FLANN index error: unsupported feature type %d for the index type %d\n",
      featureType,
      algo);
    fclose(fin);
    return false;
  }

  switch (distType) {
    case FLANN_DIST_HAMMING:
      loadIndex<HammingDistance>(this, index, data, fin);
      break;
    case FLANN_DIST_L2:
      loadIndex< ::c8flann::L2<float> >(this, index, data, fin);
      break;
    case FLANN_DIST_L1:
      loadIndex< ::c8flann::L1<float> >(this, index, data, fin);
      break;
#if MINIFLANN_SUPPORT_EXOTIC_DISTANCE_TYPES
    case FLANN_DIST_MAX:
      loadIndex< ::c8flann::MaxDistance<float> >(this, index, data, fin);
      break;
    case FLANN_DIST_HIST_INTERSECT:
      loadIndex< ::c8flann::HistIntersectionDistance<float> >(index, data, fin);
      break;
    case FLANN_DIST_HELLINGER:
      loadIndex< ::c8flann::HellingerDistance<float> >(this, index, data, fin);
      break;
    case FLANN_DIST_CHI_SQUARE:
      loadIndex< ::c8flann::ChiSquareDistance<float> >(this, index, data, fin);
      break;
    case FLANN_DIST_KL:
      loadIndex< ::c8flann::KL_Divergence<float> >(this, index, data, fin);
      break;
#endif
    default:
      fprintf(stderr, "Reading FLANN index error: unsupported distance type %d\n", distType);
      ok = false;
  }

  if (fin)
    fclose(fin);
  return ok;
}

}  // namespace flann

}  // namespace c8cv
