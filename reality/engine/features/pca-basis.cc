// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "pca-basis.h",
  };
  deps = {
    "//c8:c8-log",
    "//c8:exceptions",
    "//c8:float-vector",
    "//c8:map",
    "//c8/string:format",
    "//reality/engine/features/api:descriptors.capnp-cc",
    "@eigen3",
  };
}
cc_end(0xd48a0060);

#include <Eigen/Dense>
#include <numeric>

#include "c8/c8-log.h"
#include "c8/exceptions.h"
#include "c8/map.h"
#include "c8/string/format.h"
#include "reality/engine/features/pca-basis.h"

namespace c8 {

PcaBasis::PcaBasis(PcaBasisData::Reader pb) { loadBasis(pb); }

// static
PcaBasis PcaBasis::loadBasis(PcaBasisData::Reader pb) {
  PcaBasis obj;
  auto basis = pb.getBasis();
  auto basisVectorSize = pb.getBasisVectorSize();
  obj.numEvToRetain_ = basis.size() / basisVectorSize;

  obj.basis_.resize(basis.size());
  std::copy(basis.begin(), basis.end(), obj.basis_.begin());

  obj.sampleTranslation_.resize(basisVectorSize);
  auto trans = pb.getTranslation();
  if (trans.size() == 0) {
    obj.sampleTranslation_.zeroOut();
  } else if (trans.size() == basisVectorSize) {
    std::copy(trans.begin(), trans.end(), obj.sampleTranslation_.begin());
  } else {
    C8_THROW("Basis translation has invalid size");
  }

  obj.sampleScale_.resize(basisVectorSize);
  auto scale = pb.getScale();
  if (scale.size() == 0) {
    obj.sampleScale_.fill(1.0f);
  } else if (scale.size() == basisVectorSize) {
    std::copy(scale.begin(), scale.end(), obj.sampleScale_.begin());
  } else {
    C8_THROW("Basis scale has invalid size");
  }

  obj.whitening_.resize(obj.numEvToRetain_);
  auto whitening = pb.getWhitening();
  if (whitening.size() == 0) {
    obj.whitening_.fill(1.0f);
  } else if (whitening.size() == obj.numEvToRetain_) {
    std::copy(whitening.begin(), whitening.end(), obj.whitening_.begin());
  } else {
    C8_THROW("Basis whitening has invalid size");
  }

  return obj;
}

void PcaBasis::storeBasis(PcaBasisData::Builder pb, int outputDimensions) const {
  outputDimensions = outputDimensions > 0
    ? std::min(outputDimensions, static_cast<int>(numEvToRetain_))
    : numEvToRetain_;

  auto basisVectorSize = sampleTranslation_.size();
  pb.setBasisVectorSize(basisVectorSize);

  size_t fullBasisSize = basisVectorSize * outputDimensions;

  // Store the basis vectors in the message.
  auto outBasis = pb.initBasis(fullBasisSize);
  for (int i = 0; i < basisVectorSize; ++i) {
    int inStart = i * numEvToRetain_;
    int outStart = i * outputDimensions;
    for (int j = 0; j < outputDimensions; ++j) {
      outBasis.set(outStart + j, basis_[inStart + j]);
    }
  }

  // Store the sample translation, scale.
  auto outTrans = pb.initTranslation(basisVectorSize);
  auto outScale = pb.initScale(basisVectorSize);
  for (int i = 0; i < basisVectorSize; ++i) {
    outTrans.set(i, sampleTranslation_[i]);
    outScale.set(i, sampleScale_[i]);
  }

  // Store the whitening.
  auto outWhitening = pb.initWhitening(outputDimensions);
  for (int i = 0; i < outputDimensions; ++i) {
    outWhitening.set(i, whitening_[i]);
  }
}

// Binarize and store the PcaBasis in a BinaryPcaData capnp message,
// optionally truncating dimensions.
void PcaBasis::storeBinaryBasis(BinaryPcaData::Builder builder) const {
  const uint32_t basisVectorSize = sampleTranslation_.size();

  if (basisVectorSize % 64 != 0) {
    C8_THROW(
      "BinaryBasisData requires basis vector size divisible by 64. %lu %% 64 != 0",
      basisVectorSize);
  }

  // Copy the translation vector.
  FloatVector translations = sampleTranslation_.clone();

  // Sort the translation components.
  std::sort(translations.begin(), translations.end());

  // Compute a quantitzation bin width based on the inter-quantile distance.
  float p25 = translations[translations.size() / 4];
  float p75 = translations[3 * translations.size() / 4];
  float iqd = p75 - p25;
  float binWidth = 2.0f * iqd / std::cbrt(static_cast<float>(translations.size()));

  // Determine which bins are present in the data.
  Vector<float> bins;
  TreeMap<int, int> binOffsetToIdx;
  auto getBin = [binWidth](float val) -> int {
    return static_cast<int>(std::floor(val / binWidth + 0.5f));
  };
  int lastBin = std::numeric_limits<int>::min();
  {
    int b = 0;
    for (auto val : translations) {
      int bin = getBin(val);
      if (lastBin == bin) {
        continue;
      }
      bins.push_back(bin * binWidth);
      lastBin = bin;
      binOffsetToIdx[bin] = b;
      ++b;
    }
  }

  if (bins.size() >= std::numeric_limits<uint8_t>::max()) {
    C8_THROW("%lu bins is more than can fit in a uint8_t", bins.size());
  }

  // Write the translation LUT into the message.
  builder.setTranslationLut({bins.data(), bins.size()});

  // Write the translation indices into the message.
  auto translationIdx = builder.initTranslationIdx(sampleTranslation_.size());
  for (int i = 0; i < sampleTranslation_.size(); ++i) {
    translationIdx.set(i, binOffsetToIdx[getBin(sampleTranslation_[i])]);
  }

  builder.setBasisVectorSize(basisVectorSize);
  builder.setWhitening({whitening_.data(), whitening_.size()});

  constexpr int bitsPerBlock = sizeof(uint64_t) << 3;

  auto basis = builder.initBasis(basis_.size() / bitsPerBlock);

  const int blocks = basisVectorSize / bitsPerBlock;
  const int dimension = numEvToRetain_;

  // We need to transpose and binarize the data.
  for (int d = 0; d < dimension; ++d) {
    for (int p = 0; p < blocks; ++p) {
      uint64_t block = 0;
      for (int b = bitsPerBlock - 1; b >= 0; --b) {
        block = (block << 1)
          | static_cast<uint64_t>(0.0f < basis_[(p * bitsPerBlock + b) * dimension + d]);
      }
      basis.set(d * blocks + p, block);
    }
  }
}

float PcaBasis::generateBasisInternal(
  int m, int n, const float *inputData, float varianceToRetain) {
  Eigen::Map<const Eigen::MatrixXf> x(inputData, m, n);

  float nInv = 1.0f / n;

  Eigen::MatrixXf sigma = nInv * (x * x.transpose());

  Eigen::BDCSVD<Eigen::MatrixXf> svd(sigma, Eigen::ComputeThinU);

  // Regularization parameter.
  float epsilon = 0.00001;

  const float evTotalSum = svd.singularValues().sum();

  float evSum = 0.0f;
  numEvToRetain_ = m;
  float varianceRetained = 1.0f;

  if (varianceToRetain < 1.0f) {
    for (int i = 0; i < m; ++i) {
      evSum += svd.singularValues()(i);
      if (evSum / evTotalSum >= varianceToRetain) {
        numEvToRetain_ = i + 1;
        break;
      }
    }
    varianceRetained = evSum / evTotalSum;
  }

  whitening_.resize(numEvToRetain_);
  for (int i = 0; i < numEvToRetain_; ++i) {
    whitening_[i] = 1.0f / std::sqrt((svd.singularValues()(i) + epsilon));
  }

  Eigen::MatrixXf Uprime = svd.matrixU().leftCols(numEvToRetain_).transpose();
  basis_.resize(numEvToRetain_ * m);
  Eigen::Map<Eigen::MatrixXf>(basis_.data(), numEvToRetain_, m) = Uprime;
  return varianceRetained;
}

FloatVector PcaBasis::project(const FloatVector &input, int outputDimensions) const {
  FloatVector sample = sampleScale_ * (input + sampleTranslation_);

  outputDimensions = outputDimensions > 0
    ? std::min(outputDimensions, static_cast<int>(numEvToRetain_))
    : numEvToRetain_;

  FloatVector result(outputDimensions);

  // Compute the matrix multiplication Y = AX. This could be done with BLAS
  // instead of Eigen, and a future benchmark could focus on improving the speed
  // of this.
  Eigen::Map<Eigen::MatrixXf>(result.data(), outputDimensions, 1) =
    Eigen::Map<const Eigen::MatrixXf>(basis_.data(), numEvToRetain_, sample.size())
      .topRows(outputDimensions)
    * Eigen::Map<const Eigen::MatrixXf>(sample.data(), sample.size(), 1);

  return result;
}

FloatVector PcaBasis::projectAndWhiten(const FloatVector &input, int outputDimensions) const {
  FloatVector result = project(input, outputDimensions);

  for (int i = 0; i < result.size(); ++i) {
    result[i] *= whitening_[i];
  }

  return result;
}

}  // namespace c8
