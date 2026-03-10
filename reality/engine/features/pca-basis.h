// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// Class to compute a PCA basis from a set of FloatVectors and project samples
// into that space.

#pragma once

#include "c8/float-vector.h"
#include "reality/engine/features/api/descriptors.capnp.h"

namespace c8 {

class PcaBasis {
public:
  // Default move constructors.
  PcaBasis(PcaBasis &&) = default;
  PcaBasis &operator=(PcaBasis &&) = default;

  // Disallow copying.
  PcaBasis(const PcaBasis &) = delete;
  PcaBasis &operator=(const PcaBasis &) = delete;

  // Create a PcaBasis from a capnp message.
  static PcaBasis loadBasis(PcaBasisData::Reader pb);

  // Create a PCA basis from a sequence of FloatVectors or similar
  // container type with M floats per vector. Input vectors need not have
  // zero-mean or unit variance, but should be representative of the mean and
  // variance of the input.
  //
  // Parameters:
  //   first - (in) first FloatVector in sequence
  //   last - (in) one past the last FloatVector in sequence
  //   retainVariance - (in/out) desired variance to retain in dimensionality-reduced
  //                    basis. Will be set to the actual variance retained.
  // Returns:
  //   A new PcaBasis class containing the PCA basis
  //
  // Example Usage:
  //   Vector<FloatVector> dataSamples = ...;
  //   pcaBasis.generateBasis(dataSamples.begin(), dataSamples.end(), 0.99);
  template <class FloatVectorInputIt>
  static PcaBasis generateBasis(
    FloatVectorInputIt first, FloatVectorInputIt last, float *retainVariance);

  // Store the PcaBasis in a capnp message, optionally truncating dimensions.
  void storeBasis(PcaBasisData::Builder pb, int outputDimensions = 0) const;

  // Binarize and store the PcaBasis in a BinaryBasisData capnp message.
  void storeBinaryBasis(BinaryPcaData::Builder builder) const;

  // Project a data sample into the PCA space using the basis, mean, and
  // scale that is loaded in the generator. If outputDimensions is specified,
  // project only this many dimensions.
  FloatVector project(const FloatVector &input, int outputDimensions = 0) const;

  // Project and a data sample into the PCA space then whiten it (scale such
  // that the output has an identity covariance matrix).
  // If outputDimensions is specified, project only this many dimensions.
  FloatVector projectAndWhiten(const FloatVector &input, int outputDimensions = 0) const;

  // Return the dimensionality of the projection.
  size_t projectionSize() const { return numEvToRetain_; }

private:
  // Default constructor is private. Use generateBasis or loadBasis to construct this class.
  PcaBasis() = default;

  // Contruct PcaBasis with provided PcaBasisData.
  PcaBasis(PcaBasisData::Reader pb);

  // Generate a PCA basis from a MxN matrix of data samples stored in
  // column-major order. Input vectors must be pre-processed to have zero-mean
  // and unit variance. Returns the variance retained.
  float generateBasisInternal(int m, int n, const float *data, float varianceToRetain);

  // Number of eigenvalues retained, also equal to dimensions of projected data.
  size_t numEvToRetain_ = 0;

  // Translate by -1.0 * mean of the input samples.
  FloatVector sampleTranslation_;

  // Inverse of the standard deviation of the input samples.
  FloatVector sampleScale_;

  // MxN Matrix storing the generated basis in Column-major order.
  FloatVector basis_;

  // Inverse of the sqrt of the eigenvalues, used for whitening.
  FloatVector whitening_;
};

// -----------------------------
// Template method definitions.
// -----------------------------

// static
template <class FloatVectorInputIt>
PcaBasis PcaBasis::generateBasis(
  FloatVectorInputIt first, FloatVectorInputIt last, float *retainVariance) {
  PcaBasis basis;

  int m = first->size();
  int n = std::distance(first, last);

  // Resize and zero out the sampleTranslation and sampleScale.
  basis.sampleTranslation_.resize(first->size());
  basis.sampleTranslation_.zeroOut();

  // Add small regularization parameter for sampleScale.
  basis.sampleScale_.resize(first->size());
  basis.sampleScale_.fill(0.00001);

  // Compute the sample translation.
  for (auto iter = first; iter != last; ++iter) {
    basis.sampleTranslation_ -= *iter;
  }
  basis.sampleTranslation_ *= 1.0f / n;

  // Compute the scale needed to convert input to unit variance.
  for (auto iter = first; iter != last; ++iter) {
    FloatVector unbiased = (*iter) + basis.sampleTranslation_;
    basis.sampleScale_ += unbiased * unbiased;
  }
  // Convert variance into 1.0/stddev.
  basis.sampleScale_.sqrt().invert();

  std::unique_ptr<float[]> inputData(new float[m * n]);
  float *start = inputData.get();
  for (auto iter = first; iter != last; ++iter) {
    FloatVector normalizedSample = basis.sampleScale_ * ((*iter) + basis.sampleTranslation_);
    std::copy(normalizedSample.begin(), normalizedSample.end(), start);
    start += m;
  }
  *retainVariance = basis.generateBasisInternal(m, n, inputData.get(), *retainVariance);
  return basis;
}

}  // namespace c8
