// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"logging-summaries.h"};
  deps = {
    "//c8:vector",
    "//c8:exceptions",
    "//c8/stats/api:summary.capnp-cc",
  };
}
cc_end(0x36a0df0d);

#include <cmath>

#include "c8/exceptions.h"
#include "c8/stats/logging-summaries.h"

using namespace c8;

namespace c8 {

namespace {

float quantile(float ratio, const Vector<uint64_t> &binStart, const Vector<uint64_t> &count) {
  if (ratio < 0.f) {
    return 0.f;
  }
  if (ratio > 1.0f) {
    return binStart.back() * 2.f;
  }
  uint64_t total = 0;
  for (auto c : count) {
    total += c;
  }
  if (total == 0) {
    return 0.f;
  }
  uint64_t target = ratio * total;
  uint64_t acc = 0;
  int bin = 0;
  for (; bin < count.size(); ++bin) {
    if (acc + count[bin] >= target) {
      break;
    }
    acc += count[bin];
  }
  if (bin == count.size()) {
    return 2.f * binStart[bin - 1];
  }
  uint64_t nextAcc = acc + count[bin];
  float r = 1.f - static_cast<double>(nextAcc - target) / static_cast<double>(nextAcc - acc);
  uint64_t nextBinStart = bin == (count.size() - 1) ? 2 * binStart[bin] : binStart[bin + 1];
  return binStart[bin] + r * (nextBinStart - binStart[bin]);
}

float quantile(float ratio, DistributionStats::Reader r) {
  Vector<uint64_t> binStart;
  Vector<uint64_t> count;

  extractPDF(r, &binStart, &count);
  return quantile(ratio, binStart, count);
}

}  // namespace

void extractPDF(DistributionStats::Reader r, Vector<uint64_t> *binStart, Vector<uint64_t> *count) {
  auto h = r.getPositiveHistogram();

  *binStart = Vector<uint64_t>{
    0,       1,        2,        4,        8,         16,        32,        64,
    128,     256,      512,      1024,     2048,      4096,      8192,      16384,
    32768,   65536,    131072,   262144,   524288,    1048576,   2097152,   4194304,
    8388608, 16777216, 33554432, 67108864, 134217728, 268435456, 536870912, 1073741824,
  };

  *count = Vector<uint64_t>{
    h.getBucket0(),
    h.getBucket1(),
    h.getBucket2To3(),
    h.getBucket4To7(),
    h.getBucket8To15(),
    h.getBucket16To31(),
    h.getBucket32To63(),
    h.getBucket64To127(),
    h.getBucket128To255(),
    h.getBucket256To511(),
    h.getBucket512To1023(),
    h.getBucket1024To2047(),
    h.getBucket2048To4095(),
    h.getBucket4096To8191(),
    h.getBucket8192To16383(),
    h.getBucket16384To32767(),
    h.getBucket32768To65535(),
    h.getBucket65536To131071(),
    h.getBucket131072To262143(),
    h.getBucket262144To524287(),
    h.getBucket524288To1048575(),
    h.getBucket1048576To2097151(),
    h.getBucket2097152To4194303(),
    h.getBucket4194304To8388607(),
    h.getBucket8388608To16777215(),
    h.getBucket16777216To33554431(),
    h.getBucket33554432To67108863(),
    h.getBucket67108864To134217727(),
    h.getBucket134217728To268435455(),
    h.getBucket268435456To536870911(),
    h.getBucket536870912To1073741823(),
    h.getBucket1073741824ToInf(),
  };
}

float hasLatency(EventSummary::Reader event) {
  return event.getLatencyMicros().getMeanStats().getSum() > 0;
}

float latencyQuantile(float ratio, EventSummary::Reader event) {
  return quantile(ratio, event.getLatencyMicros());
}

float latencyMean(EventSummary::Reader event) {
  return static_cast<float>(
    static_cast<double>(event.getLatencyMicros().getMeanStats().getSum())
    / static_cast<double>(event.getLatencyMicros().getMeanStats().getNumDataPoints()));
}

float hasCounter(EventSummary::Reader event) {
  return event.getPositiveCount().getMeanStats().getSum() > 0;
}

float counterGetSingleValue(EventSummary::Reader event) {
  if (event.getPositiveCount().getMeanStats().getNumDataPoints() != 1) {
    C8_THROW(
      "[logging-summaries@counterGetSingleValue] Event does not have only one element (n=%d)",
      event.getPositiveCount().getMeanStats().getNumDataPoints());
  }
  return static_cast<float>(event.getPositiveCount().getMeanStats().getSum());
}

float counterMean(EventSummary::Reader event) {
  return static_cast<float>(
    static_cast<double>(event.getPositiveCount().getMeanStats().getSum())
    / static_cast<double>(event.getPositiveCount().getMeanStats().getNumDataPoints()));
}

float counterRootMean(EventSummary::Reader event, float scalingFactor) {
  // This makes the assumption that we multiplied by a scaling factor for the input data.
  double averageScaled = static_cast<double>(event.getPositiveCount().getMeanStats().getSum())
    / static_cast<double>(event.getPositiveCount().getMeanStats().getNumDataPoints());
  return std::sqrt(averageScaled * scalingFactor);
}

float counterQuantile(float ratio, EventSummary::Reader event) {
  return quantile(ratio, event.getPositiveCount());
}

float hasRatio(EventSummary::Reader event) {
  return event.getOfTotalPositiveCount().getMeanStats().getSum() > 0;
}

float ratioMean(EventSummary::Reader event) {
  return static_cast<float>(
    static_cast<double>(event.getPositiveCount().getMeanStats().getSum())
    / static_cast<double>(event.getOfTotalPositiveCount().getMeanStats().getSum()));
}

}  // namespace c8
