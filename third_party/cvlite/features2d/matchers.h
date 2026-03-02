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
#include "third_party/cvlite/flann/miniflann.h"

namespace c8cv {

/****************************************************************************************\
*                                  DescriptorMatcher                                     *
\****************************************************************************************/

//! @addtogroup features2d_match
//! @{

/** @brief Abstract base class for matching keypoint descriptors.

It has two groups of match methods: for matching descriptors of an image with another image or with
an image set.
 */
class CV_EXPORTS_W DescriptorMatcher : public c8cv::Algorithm {
public:
  enum {
    FLANNBASED = 1,
    BRUTEFORCE = 2,
    BRUTEFORCE_L1 = 3,
    BRUTEFORCE_HAMMING = 4,
    BRUTEFORCE_HAMMINGLUT = 5,
    BRUTEFORCE_SL2 = 6
  };
  virtual ~DescriptorMatcher();

  /** @brief Adds descriptors to train a CPU(trainDescCollectionis) or GPU(utrainDescCollectionis)
  descriptor collection.

  If the collection is not empty, the new descriptors are added to existing train descriptors.

  @param descriptors Descriptors to add. Each descriptors[i] is a set of descriptors from the same
  train image.
   */
  CV_WRAP virtual void add(c8cv::InputArrayOfArrays descriptors);

  /** @brief Returns a constant link to the train descriptor collection trainDescCollection .
   */
  CV_WRAP const std::vector<c8cv::Mat> &getTrainDescriptors() const;

  /** @brief Clears the train descriptor collections.
   */
  CV_WRAP virtual void clear();

  /** @brief Returns true if there are no train descriptors in the both collections.
   */
  CV_WRAP virtual bool empty() const;

  /** @brief Returns true if the descriptor matcher supports masking permissible matches.
   */
  CV_WRAP virtual bool isMaskSupported() const = 0;

  /** @brief Trains a descriptor matcher

  Trains a descriptor matcher (for example, the flann index). In all methods to match, the method
  train() is run every time before matching. Some descriptor matchers (for example,
  BruteForceMatcher) have an empty implementation of this method. Other matchers really train their
  inner structures (for example, FlannBasedMatcher trains flann::Index ).
   */
  CV_WRAP virtual void train();

  /** @brief Finds the best match for each descriptor from a query set.

  @param queryDescriptors Query set of descriptors.
  @param trainDescriptors Train set of descriptors. This set is not added to the train descriptors
  collection stored in the class object.
  @param matches c8cv::Matches. If a query descriptor is masked out in mask , no match is added for
  this descriptor. So, matches size may be smaller than the query descriptors count.
  @param mask Mask specifying permissible matches between an input query and train matrices of
  descriptors.

  In the first variant of this method, the train descriptors are passed as an input argument. In the
  second variant of the method, train descriptors collection that was set by DescriptorMatcher::add
  is used. Optional mask (or masks) can be passed to specify which query and training descriptors
  can be matched. Namely, queryDescriptors[i] can be matched with trainDescriptors[j] only if
  mask.at\<uchar\>(i,j) is non-zero.
   */
  CV_WRAP void match(
    c8cv::InputArray queryDescriptors,
    c8cv::InputArray trainDescriptors,
    CV_OUT std::vector<c8cv::DMatch> &matches,
    c8cv::InputArray mask = c8cv::noArray()) const;

  /** @brief Finds the k best matches for each descriptor from a query set.

  @param queryDescriptors Query set of descriptors.
  @param trainDescriptors Train set of descriptors. This set is not added to the train descriptors
  collection stored in the class object.
  @param mask Mask specifying permissible matches between an input query and train matrices of
  descriptors.
  @param matches c8cv::Matches. Each matches[i] is k or less matches for the same query descriptor.
  @param k Count of best matches found per each query descriptor or less if a query descriptor has
  less than k possible matches in total.
  @param compactResult Parameter used when the mask (or masks) is not empty. If compactResult is
  false, the matches vector has the same size as queryDescriptors rows. If compactResult is true,
  the matches vector does not contain matches for fully masked-out query descriptors.

  These extended variants of DescriptorMatcher::match methods find several best matches for each
  query descriptor. The matches are returned in the distance increasing order. See
  DescriptorMatcher::match for the details about query and train descriptors.
   */
  CV_WRAP void knnMatch(
    c8cv::InputArray queryDescriptors,
    c8cv::InputArray trainDescriptors,
    CV_OUT std::vector<std::vector<c8cv::DMatch> > &matches,
    int k,
    c8cv::InputArray mask = c8cv::noArray(),
    bool compactResult = false) const;

  /** @brief For each query descriptor, finds the training descriptors not farther than the
  specified distance.

  @param queryDescriptors Query set of descriptors.
  @param trainDescriptors Train set of descriptors. This set is not added to the train descriptors
  collection stored in the class object.
  @param matches Found matches.
  @param compactResult Parameter used when the mask (or masks) is not empty. If compactResult is
  false, the matches vector has the same size as queryDescriptors rows. If compactResult is true,
  the matches vector does not contain matches for fully masked-out query descriptors.
  @param maxDistance Threshold for the distance between matched descriptors. Distance means here
  metric distance (e.g. Hamming distance), not the distance between coordinates (which is measured
  in Pixels)!
  @param mask Mask specifying permissible matches between an input query and train matrices of
  descriptors.

  For each query descriptor, the methods find such training descriptors that the distance between
  the query descriptor and the training descriptor is equal or smaller than maxDistance. Found
  matches are returned in the distance increasing order.
   */
  CV_WRAP void radiusMatch(
    c8cv::InputArray queryDescriptors,
    c8cv::InputArray trainDescriptors,
    CV_OUT std::vector<std::vector<c8cv::DMatch> > &matches,
    float maxDistance,
    c8cv::InputArray mask = c8cv::noArray(),
    bool compactResult = false) const;

  /** @overload
  @param queryDescriptors Query set of descriptors.
  @param matches c8cv::Matches. If a query descriptor is masked out in mask , no match is added for
  this descriptor. So, matches size may be smaller than the query descriptors count.
  @param masks Set of masks. Each masks[i] specifies permissible matches between the input query
  descriptors and stored train descriptors from the i-th image trainDescCollection[i].
  */
  CV_WRAP void match(
    c8cv::InputArray queryDescriptors,
    CV_OUT std::vector<c8cv::DMatch> &matches,
    c8cv::InputArrayOfArrays masks = c8cv::noArray());
  /** @overload
  @param queryDescriptors Query set of descriptors.
  @param matches c8cv::Matches. Each matches[i] is k or less matches for the same query descriptor.
  @param k Count of best matches found per each query descriptor or less if a query descriptor has
  less than k possible matches in total.
  @param masks Set of masks. Each masks[i] specifies permissible matches between the input query
  descriptors and stored train descriptors from the i-th image trainDescCollection[i].
  @param compactResult Parameter used when the mask (or masks) is not empty. If compactResult is
  false, the matches vector has the same size as queryDescriptors rows. If compactResult is true,
  the matches vector does not contain matches for fully masked-out query descriptors.
  */
  CV_WRAP void knnMatch(
    c8cv::InputArray queryDescriptors,
    CV_OUT std::vector<std::vector<c8cv::DMatch> > &matches,
    int k,
    c8cv::InputArrayOfArrays masks = c8cv::noArray(),
    bool compactResult = false);

  /** @brief Perform knnMatch assuming a pretranied matcher, see knnMatch */
  CV_WRAP void knnMatchPretrained(
    c8cv::InputArray queryDescriptors,
    CV_OUT std::vector<std::vector<c8cv::DMatch> > &matches,
    int k,
    c8cv::InputArray mask = c8cv::noArray(),
    bool compactResult = false);

  /** @overload
  @param queryDescriptors Query set of descriptors.
  @param matches Found matches.
  @param maxDistance Threshold for the distance between matched descriptors. Distance means here
  metric distance (e.g. Hamming distance), not the distance between coordinates (which is measured
  in Pixels)!
  @param masks Set of masks. Each masks[i] specifies permissible matches between the input query
  descriptors and stored train descriptors from the i-th image trainDescCollection[i].
  @param compactResult Parameter used when the mask (or masks) is not empty. If compactResult is
  false, the matches vector has the same size as queryDescriptors rows. If compactResult is true,
  the matches vector does not contain matches for fully masked-out query descriptors.
  */
  CV_WRAP void radiusMatch(
    c8cv::InputArray queryDescriptors,
    CV_OUT std::vector<std::vector<c8cv::DMatch> > &matches,
    float maxDistance,
    c8cv::InputArrayOfArrays masks = c8cv::noArray(),
    bool compactResult = false);

  CV_WRAP void write(const c8cv::String &fileName) const {
    c8cv::FileStorage fs(fileName, c8cv::FileStorage::WRITE);
    write(fs);
  }

  CV_WRAP void read(const c8cv::String &fileName) {
    c8cv::FileStorage fs(fileName, c8cv::FileStorage::READ);
    read(fs.root());
  }
  // Reads matcher object from a file node
  virtual void read(const c8cv::FileNode &);
  // Writes matcher object to a file storage
  virtual void write(c8cv::FileStorage &) const;

  /** @brief Clones the matcher.

  @param emptyTrainData If emptyTrainData is false, the method creates a deep copy of the object,
  that is, copies both parameters and train data. If emptyTrainData is true, the method creates an
  object copy with the current parameters but with empty train data.
   */
  CV_WRAP virtual c8cv::Ptr<DescriptorMatcher> clone(bool emptyTrainData = false) const = 0;

  /** @brief Creates a descriptor matcher of a given type with the default parameters (using default
  constructor).

  @param descriptorMatcherType Descriptor matcher type. Now the following matcher types are
  supported:
  -   `BruteForce` (it uses L2 )
  -   `BruteForce-L1`
  -   `BruteForce-Hamming`
  -   `BruteForce-Hamming(2)`
  -   `FlannBased`
   */
  CV_WRAP static c8cv::Ptr<DescriptorMatcher> create(const c8cv::String &descriptorMatcherType);

  CV_WRAP static c8cv::Ptr<DescriptorMatcher> create(int matcherType);

protected:
  /**
   * Class to work with descriptors from several images as with one merged matrix.
   * It is used e.g. in FlannBasedMatcher.
   */
  class CV_EXPORTS DescriptorCollection {
  public:
    DescriptorCollection();
    DescriptorCollection(const DescriptorCollection &collection);
    virtual ~DescriptorCollection();

    // Vector of matrices "descriptors" will be merged to one matrix "mergedDescriptors" here.
    void set(const std::vector<c8cv::Mat> &descriptors);
    virtual void clear();

    const c8cv::Mat &getDescriptors() const;
    const c8cv::Mat getDescriptor(int imgIdx, int localDescIdx) const;
    const c8cv::Mat getDescriptor(int globalDescIdx) const;
    void getLocalIdx(int globalDescIdx, int &imgIdx, int &localDescIdx) const;

    int size() const;

  protected:
    c8cv::Mat mergedDescriptors;
    std::vector<int> startIdxs;
  };

  //! In fact the matching is implemented only by the following two methods. These methods suppose
  //! that the class object has been trained already. Public match methods call these methods
  //! after calling train().
  virtual void knnMatchImpl(
    c8cv::InputArray queryDescriptors,
    std::vector<std::vector<c8cv::DMatch> > &matches,
    int k,
    c8cv::InputArrayOfArrays masks = c8cv::noArray(),
    bool compactResult = false) = 0;
  virtual void radiusMatchImpl(
    c8cv::InputArray queryDescriptors,
    std::vector<std::vector<c8cv::DMatch> > &matches,
    float maxDistance,
    c8cv::InputArrayOfArrays masks = c8cv::noArray(),
    bool compactResult = false) = 0;

  static bool isPossibleMatch(c8cv::InputArray mask, int queryIdx, int trainIdx);
  static bool isMaskedOut(c8cv::InputArrayOfArrays masks, int queryIdx);

  static c8cv::Mat clone_op(c8cv::Mat m) { return m.clone(); }
  void checkMasks(c8cv::InputArrayOfArrays masks, int queryDescriptorsCount) const;

  //! Collection of descriptors from train images.
  std::vector<c8cv::Mat> trainDescCollection;
  std::vector<c8cv::UMat> utrainDescCollection;
};

/** @brief Brute-force descriptor matcher.

For each descriptor in the first set, this matcher finds the closest descriptor in the second set
by trying each one. This descriptor matcher supports masking permissible matches of descriptor
sets.
 */
class CV_EXPORTS_W BFMatcher : public DescriptorMatcher {
public:
  /** @brief Brute-force matcher constructor (obsolete). Please use BFMatcher.create()
   *
   *
   */
  CV_WRAP BFMatcher(int normType = c8cv::NORM_L2, bool crossCheck = false);

  virtual ~BFMatcher() {}

  virtual bool isMaskSupported() const { return true; }

  /* @brief Brute-force matcher create method.
  @param normType One of NORM_L1, NORM_L2, NORM_HAMMING, NORM_HAMMING2. L1 and L2 norms are
  preferable choices for SIFT and SURF descriptors, NORM_HAMMING should be used with ORB, BRISK and
  BRIEF, NORM_HAMMING2 should be used with ORB when WTA_K==3 or 4 (see ORB::ORB constructor
  description).
  @param crossCheck If it is false, this is will be default BFMatcher behaviour when it finds the k
  nearest neighbors for each query descriptor. If crossCheck==true, then the knnMatch() method with
  k=1 will only return pairs (i,j) such that for i-th query descriptor the j-th descriptor in the
  matcher's collection is the nearest and vice versa, i.e. the BFMatcher will only return consistent
  pairs. Such technique usually produces best results with minimal number of outliers when there are
  enough matches. This is alternative to the ratio test, used by D. Lowe in SIFT paper.
   */
  CV_WRAP static c8cv::Ptr<BFMatcher> create(int normType = c8cv::NORM_L2, bool crossCheck = false);

  virtual c8cv::Ptr<DescriptorMatcher> clone(bool emptyTrainData = false) const;

protected:
  virtual void knnMatchImpl(
    c8cv::InputArray queryDescriptors,
    std::vector<std::vector<c8cv::DMatch> > &matches,
    int k,
    c8cv::InputArrayOfArrays masks = c8cv::noArray(),
    bool compactResult = false);
  virtual void radiusMatchImpl(
    c8cv::InputArray queryDescriptors,
    std::vector<std::vector<c8cv::DMatch> > &matches,
    float maxDistance,
    c8cv::InputArrayOfArrays masks = c8cv::noArray(),
    bool compactResult = false);

  int normType;
  bool crossCheck;
};

/** @brief Flann-based descriptor matcher.

This matcher trains c8cv::flann::Index on a train descriptor collection and calls its nearest search
methods to find the best matches. So, this matcher may be faster when matching a large train
collection than the brute force matcher. FlannBasedMatcher does not support masking permissible
matches of descriptor sets because flann::Index does not support this. :
 */
class CV_EXPORTS_W FlannBasedMatcher : public DescriptorMatcher {
public:
  CV_WRAP FlannBasedMatcher(
    const c8cv::Ptr<flann::IndexParams> &indexParams = c8cv::makePtr<flann::KDTreeIndexParams>(),
    const c8cv::Ptr<flann::SearchParams> &searchParams = c8cv::makePtr<flann::SearchParams>());

  virtual void add(c8cv::InputArrayOfArrays descriptors);
  virtual void clear();

  // Reads matcher object from a file node
  virtual void read(const c8cv::FileNode &);
  // Writes matcher object to a file storage
  virtual void write(c8cv::FileStorage &) const;

  virtual void train();
  virtual bool isMaskSupported() const;

  CV_WRAP static c8cv::Ptr<FlannBasedMatcher> create();

  virtual c8cv::Ptr<DescriptorMatcher> clone(bool emptyTrainData = false) const;

protected:
  static void convertToDMatches(
    const DescriptorCollection &descriptors,
    const c8cv::Mat &indices,
    const c8cv::Mat &distances,
    std::vector<std::vector<c8cv::DMatch> > &matches);

  virtual void knnMatchImpl(
    c8cv::InputArray queryDescriptors,
    std::vector<std::vector<c8cv::DMatch> > &matches,
    int k,
    c8cv::InputArrayOfArrays masks = c8cv::noArray(),
    bool compactResult = false);
  virtual void radiusMatchImpl(
    c8cv::InputArray queryDescriptors,
    std::vector<std::vector<c8cv::DMatch> > &matches,
    float maxDistance,
    c8cv::InputArrayOfArrays masks = c8cv::noArray(),
    bool compactResult = false);

  c8cv::Ptr<flann::IndexParams> indexParams;
  c8cv::Ptr<flann::SearchParams> searchParams;
  c8cv::Ptr<flann::Index> flannIndex;

  DescriptorCollection mergedDescriptors;
  int addedDescCount;
};

//! @} features2d_match

}  // namespace c8cv
