// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include "c8/stats/api/summary.capnp.h"
#include "c8/vector.h"

namespace c8 {

void extractPDF(DistributionStats::Reader r, Vector<uint64_t> *binStart, Vector<uint64_t> *count);

float hasLatency(EventSummary::Reader event);
float latencyMean(EventSummary::Reader event);
float latencyQuantile(float ratio, EventSummary::Reader event);

float hasCounter(EventSummary::Reader event);
float counterRootMean(EventSummary::Reader event, float scalingFactor);
float counterGetSingleValue(EventSummary::Reader event);
float counterMean(EventSummary::Reader event);
float counterQuantile(float ratio, EventSummary::Reader event);

float hasRatio(EventSummary::Reader event);
float ratioMean(EventSummary::Reader event);

}  // namespace c8
