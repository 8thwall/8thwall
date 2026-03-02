// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "latency-summarizer.h",
  };
  deps = {
    ":logging-context",
    ":logging-summaries",
    "//c8:c8-log",
    "//c8:exceptions",
    "//c8:string",
    "//c8:map",
    "//c8:vector",
    "//c8/io:capnp-messages",
    "//c8/stats/api:detail.capnp-cc",
    "//c8/stats/api:summary.capnp-cc",
    "//c8/string:format",
  };
}
cc_end(0x39958dd9);

#include <capnp/message.h>
#include <capnp/pretty-print.h>
#include <math.h>

#include <algorithm>
#include <sstream>

#include "c8/c8-log.h"
#include "c8/exceptions.h"
#include "c8/io/capnp-messages.h"
#include "c8/stats/latency-summarizer.h"
#include "c8/stats/logging-summaries.h"
#include "c8/string/format.h"

using MutableEventSummary = c8::MutableRootMessage<c8::EventSummary>;
using MutableLoggingDetail = c8::MutableRootMessage<c8::LoggingDetail>;
using MutableLoggingSummary = c8::MutableRootMessage<c8::LoggingSummary>;

namespace c8 {

// Add a logging context directly.
void LatencySummarizer::summarize(const LoggingContext &rootLoggingContext) {
  if (!rootLoggingContext.wasMarkedComplete()) {
    C8_THROW(
      String("Calling summarize from logging context ") + rootLoggingContext.tag()
      + " before it was marked complete.");
  }

  // Logging context to collect stats about summarization.
  auto lc = LoggingContext::createRootLoggingTree("summarizer");
  {
    auto &context = lc.createChild(rootLoggingContext.tag().substr(1));
    // Summarize the passed in context, collecting stats about the summarization.

    MutableLoggingDetail message;
    auto detail = message.builder();
    rootLoggingContext.exportDetail(&detail);
    summarize(detail);
    context.addCounter("num-counters", detail.getEvents().size());
    context.markCompletionTimepoint();
  }

  // Summarize the summarization stats -- these are not monitored. The goal here is to minimize
  // the overhead that is not captured in the stats and to have a reasonable way to estimate that
  // un-logged overhead. There are 5 counters logged and summarized per summarization event. To
  // estimate the overhead here, we need to divide the number of summarization events by the
  // framerate, and then find a comparable ~5 event tree to use to estimate the timing for each of
  // those events. For example, if there are 8 summarization events per frame, and comparable-in-
  // size events take 10 micros, we can estimate that the overhead here is 80 micros per frame that
  // isn't accounted for in our logging.
  {
    lc.markCompletionTimepoint();
    MutableLoggingDetail message;
    auto detail = message.builder();
    lc.exportDetail(&detail);
    summarize(detail);
  }
}

// definitions
String LatencySummarizer::formatSummaryDetailed(const LoggingSummary::Reader &summary) {
  std::stringstream out;
  for (auto event : summary.getEvents()) {
    out << event.getEventName().cStr() << ":  " << event.getEventCount() << " events";
    if (hasLatency(event)) {
      auto q50 = latencyQuantile(.5f, event);
      auto q90 = latencyQuantile(.9f, event);
      auto q99 = latencyQuantile(.99f, event);
      out << "  (Latency: " << latencyMean(event) << " micros";
      out << " -- 50%/90%/99%: " << q50 << "/" << q90 << "/" << q99 << ")";
    }
    if (hasCounter(event)) {
      auto q50 = counterQuantile(.5f, event);
      auto q90 = counterQuantile(.9f, event);
      auto q99 = counterQuantile(.99f, event);
      out << "  (Count: " << counterMean(event);
      out << " -- 50%/90%/99%: " << q50 << "/" << q90 << "/" << q99 << ")";
    }
    if (hasRatio(event)) {
      out << "  (%: " << (100.0 * ratioMean(event)) << ")";
    }
    out << std::endl;
  }
  return out.str();
}

String LatencySummarizer::briefSummaryForEvent(const EventSummary::Reader &event) {
  std::stringstream out;
  String fullTag = event.getEventName().cStr();
  int depth = std::count(fullTag.begin(), fullTag.end(), '/') - 1;
  auto lastToken = fullTag.rfind("/") + 1;
  for (int i = 0; i < depth; ++i) {
    out << "| ";
  }
  out << fullTag.substr(lastToken, fullTag.size()) << ":  ";
  if (hasLatency(event)) {
    out << format("(%0.3f ms, %d x)", latencyMean(event) * 1e-3f, event.getEventCount());
  }
  if (hasCounter(event)) {
    out << format("(%0.3f #, %d x)", counterMean(event), event.getEventCount());
  }
  if (hasRatio(event)) {
    out << format("(%%: %0.3f, %d x)", 100.0 * ratioMean(event), event.getEventCount());
  }
  return out.str();
}

// definitions
String LatencySummarizer::formatSummaryBrief(const LoggingSummary::Reader &summary) {
  std::stringstream out;
  for (auto event : summary.getEvents()) {
    out << briefSummaryForEvent(event);
    out << std::endl;
  }
  return out.str();
}

String LatencySummarizer::formatSummaryFull(const LoggingSummary::Reader &summary) {
  String raw = capnp::prettyPrint(summary).flatten().cStr();
  std::istringstream stream(raw);
  std::stringstream out;
  for (String line; std::getline(stream, line);) {
    String endString = line.substr(line.size() - 5, line.size());
    if (endString == " = 0,") {
      continue;
    }
    out << line << std::endl;
  }
  return out.str();
}

void LatencySummarizer::reset() { statMap_.clear(); }

void LatencySummarizer::logFullSummary() const {
  MutableLoggingSummary summaryMessage;
  auto summary = summaryMessage.builder();
  this->exportSummary(&summary);
  C8LogLines(LatencySummarizer::formatSummaryFull(summary));
}

void LatencySummarizer::logBriefSummary() const {
  MutableLoggingSummary summaryMessage;
  auto summary = summaryMessage.builder();
  this->exportSummary(&summary);
  C8LogLines(LatencySummarizer::formatSummaryBrief(summary));
}

void LatencySummarizer::logDetailedSummary() const {
  MutableLoggingSummary summaryMessage;
  auto summary = summaryMessage.builder();
  this->exportSummary(&summary);
  C8LogLines(LatencySummarizer::formatSummaryDetailed(summary));
}

void LatencySummarizer::summarize(const LoggingDetail::Reader &detail) {
  for (auto event : detail.getEvents()) {
    summarizeOneEvent(event);
  }
}

void LatencySummarizer::exportSummary(LoggingSummary::Builder *summaryBuilder) const {
  summaryBuilder->initEvents(statMap_.size());
  int i = 0;
  for (auto it = statMap_.begin(); it != statMap_.end(); ++it, ++i) {
    summaryBuilder->getEvents().setWithCaveats(i, it->second->reader());
  }
}

ConstRootMessage<EventSummary> LatencySummarizer::summary(const String &tag) const {
  auto mapElem = statMap_.find(tag);
  if (mapElem == statMap_.end()) {
    return ConstRootMessage<EventSummary>();
  }
  return ConstRootMessage<EventSummary>(mapElem->second->reader());
}

void LatencySummarizer::summarizeOneEvent(const EventDetail::Reader &event) {
  // Get the relevant map element if it exists, or initialize a new one.
  String key = event.getEventName().cStr();
  auto mapElem = statMap_.find(key);
  if (mapElem == statMap_.end()) {
    // Insert a new message into the map, initializing its key.
    statMap_[key] = std::unique_ptr<MutableEventSummary>(new MutableEventSummary());
    mapElem = statMap_.find(key);
    mapElem->second->builder().setEventName(key);
  }

  auto summaryBuilder = mapElem->second->builder();
  summaryBuilder.setEventCount(summaryBuilder.getEventCount() + 1);

  updateDistribution(event, &summaryBuilder);
}

void LatencySummarizer::updateDistribution(
  const EventDetail::Reader &event, EventSummary::Builder *summaryBuilder) {
  // If end time is not set, don't increment any other latency distribution stats.
  if (event.getEndTimeMicros() > 0) {
    int64_t latencyRaw = event.getEndTimeMicros() - event.getStartTimeMicros();
    uint64_t latency = latencyRaw < 0 ? 0 : latencyRaw;
    auto latencySummary = summaryBuilder->getLatencyMicros();
    updatePositiveHistogram(latency, &latencySummary);
  }

  // If no other counters are set, there's nothing to increment.
  if (event.getPositiveCount() == 0 && event.getOfTotalPositiveCount() == 0) {
    return;
  }

  auto countRaw = event.getPositiveCount();
  uint64_t count = countRaw < 0 ? 0 : countRaw;
  auto positiveCountSummary = summaryBuilder->getPositiveCount();
  updatePositiveHistogram(count, &positiveCountSummary);

  auto ofTotal = event.getOfTotalPositiveCount();

  if (ofTotal <= 0) {
    return;
  }

  auto ofTotalSummary = summaryBuilder->getOfTotalPositiveCount();
  updatePositiveHistogram(ofTotal, &ofTotalSummary);

  auto percentSummary = summaryBuilder->getPercentPositiveCount();
  updatePercentHistogram(count, ofTotal, &percentSummary);
}

void LatencySummarizer::updatePositiveHistogram(
  uint64_t val, DistributionStats::Builder *summaryBuilder) {

  uint64_t valForLog = val < 1 ? 1 : val;

  auto meanStats = summaryBuilder->getMeanStats();

  meanStats.setNumDataPoints(meanStats.getNumDataPoints() + 1);

  // Increment mean values.
  meanStats.setSum(meanStats.getSum() + val);

  meanStats.setSumOfSquares(meanStats.getSumOfSquares() + val * val);

  meanStats.setSumOfLogs(meanStats.getSumOfLogs() + log(valForLog));

  auto h = summaryBuilder->getPositiveHistogram();

  // Increment histogram bucket.
  if (val < 1) {
    h.setBucket0(h.getBucket0() + 1);
  } else if (val < 2) {
    h.setBucket1(h.getBucket1() + 1);
  } else if (val < 4) {
    h.setBucket2To3(h.getBucket2To3() + 1);
  } else if (val < 8) {
    h.setBucket4To7(h.getBucket4To7() + 1);
  } else if (val < 16) {
    h.setBucket8To15(h.getBucket8To15() + 1);
  } else if (val < 32) {
    h.setBucket16To31(h.getBucket16To31() + 1);
  } else if (val < 64) {
    h.setBucket32To63(h.getBucket32To63() + 1);
  } else if (val < 128) {
    h.setBucket64To127(h.getBucket64To127() + 1);
  } else if (val < 256) {
    h.setBucket128To255(h.getBucket128To255() + 1);
  } else if (val < 512) {
    h.setBucket256To511(h.getBucket256To511() + 1);
  } else if (val < 1024) {
    h.setBucket512To1023(h.getBucket512To1023() + 1);
  } else if (val < 2048) {
    h.setBucket1024To2047(h.getBucket1024To2047() + 1);
  } else if (val < 4096) {
    h.setBucket2048To4095(h.getBucket2048To4095() + 1);
  } else if (val < 8192) {
    h.setBucket4096To8191(h.getBucket4096To8191() + 1);
  } else if (val < 16384) {
    h.setBucket8192To16383(h.getBucket8192To16383() + 1);
  } else if (val < 32768) {
    h.setBucket16384To32767(h.getBucket16384To32767() + 1);
  } else if (val < 65536) {
    h.setBucket32768To65535(h.getBucket32768To65535() + 1);
  } else if (val < 131072) {
    h.setBucket65536To131071(h.getBucket65536To131071() + 1);
  } else if (val < 262144) {
    h.setBucket131072To262143(h.getBucket131072To262143() + 1);
  } else if (val < 524288) {
    h.setBucket262144To524287(h.getBucket262144To524287() + 1);
  } else if (val < 1048576) {
    h.setBucket524288To1048575(h.getBucket524288To1048575() + 1);
  } else if (val < 2097152) {
    h.setBucket1048576To2097151(h.getBucket1048576To2097151() + 1);
  } else if (val < 4194304) {
    h.setBucket2097152To4194303(h.getBucket2097152To4194303() + 1);
  } else if (val < 8388608) {
    h.setBucket4194304To8388607(h.getBucket4194304To8388607() + 1);
  } else if (val < 16777216) {
    h.setBucket8388608To16777215(h.getBucket8388608To16777215() + 1);
  } else if (val < 33554432) {
    h.setBucket16777216To33554431(h.getBucket16777216To33554431() + 1);
  } else if (val < 67108864) {
    h.setBucket33554432To67108863(h.getBucket33554432To67108863() + 1);
  } else if (val < 134217728) {
    h.setBucket67108864To134217727(h.getBucket67108864To134217727() + 1);
  } else if (val < 268435456) {
    h.setBucket134217728To268435455(h.getBucket134217728To268435455() + 1);
  } else if (val < 536870912) {
    h.setBucket268435456To536870911(h.getBucket268435456To536870911() + 1);
  } else if (val < 1073741824) {
    h.setBucket536870912To1073741823(h.getBucket536870912To1073741823() + 1);
  } else {
    h.setBucket1073741824ToInf(h.getBucket1073741824ToInf() + 1);
  }
}

void LatencySummarizer::updatePercentHistogram(
  uint64_t count, uint64_t ofTotal, DistributionStats::Builder *summaryBuilder) {
  uint64_t val = count * 1000000 / ofTotal;
  uint64_t valForLog = val < 1 ? 1 : val;

  auto meanStats = summaryBuilder->getMeanStats();

  meanStats.setNumDataPoints(meanStats.getNumDataPoints() + 1);

  // Increment mean values.
  meanStats.setSum(meanStats.getSum() + val);

  meanStats.setSumOfSquares(meanStats.getSumOfSquares() + val * val);

  meanStats.setSumOfLogs(meanStats.getSumOfLogs() + log(valForLog));

  auto h = summaryBuilder->getPercentageHistogram();

  // Increment histogram bucket.
  if (val < 1) {
    h.setBucket0000000(h.getBucket0000000() + 1);
  } else if (val < 2) {
    h.setBucket0000001(h.getBucket0000001() + 1);
  } else if (val < 6) {
    h.setBucket0000002To0000005(h.getBucket0000002To0000005() + 1);
  } else if (val < 17) {
    h.setBucket0000006To0000016(h.getBucket0000006To0000016() + 1);
  } else if (val < 45) {
    h.setBucket0000017To0000044(h.getBucket0000017To0000044() + 1);
  } else if (val < 123) {
    h.setBucket0000045To0000122(h.getBucket0000045To0000122() + 1);
  } else if (val < 335) {
    h.setBucket0000123To0000334(h.getBucket0000123To0000334() + 1);
  } else if (val < 911) {
    h.setBucket0000335To0000910(h.getBucket0000335To0000910() + 1);
  } else if (val < 2473) {
    h.setBucket0000911To0002472(h.getBucket0000911To0002472() + 1);
  } else if (val < 6693) {
    h.setBucket0002473To0006692(h.getBucket0002473To0006692() + 1);
  } else if (val < 17986) {
    h.setBucket0006693To0017985(h.getBucket0006693To0017985() + 1);
  } else if (val < 47426) {
    h.setBucket0017986To0047425(h.getBucket0017986To0047425() + 1);
  } else if (val < 119203) {
    h.setBucket0047426To0119202(h.getBucket0047426To0119202() + 1);
  } else if (val < 268941) {
    h.setBucket0119203To0268940(h.getBucket0119203To0268940() + 1);
  } else if (val < 500000) {
    h.setBucket0268941To0499999(h.getBucket0268941To0499999() + 1);
  } else if (val < 731060) {
    h.setBucket0500000To0731059(h.getBucket0500000To0731059() + 1);
  } else if (val < 880798) {
    h.setBucket0731060To0880797(h.getBucket0731060To0880797() + 1);
  } else if (val < 952575) {
    h.setBucket0880798To0952574(h.getBucket0880798To0952574() + 1);
  } else if (val < 982015) {
    h.setBucket0952575To0982014(h.getBucket0952575To0982014() + 1);
  } else if (val < 993308) {
    h.setBucket0982015To0993307(h.getBucket0982015To0993307() + 1);
  } else if (val < 997528) {
    h.setBucket0993308To0997527(h.getBucket0993308To0997527() + 1);
  } else if (val < 999090) {
    h.setBucket0997528To0999089(h.getBucket0997528To0999089() + 1);
  } else if (val < 999666) {
    h.setBucket0999090To0999665(h.getBucket0999090To0999665() + 1);
  } else if (val < 999878) {
    h.setBucket0999666To0999877(h.getBucket0999666To0999877() + 1);
  } else if (val < 999956) {
    h.setBucket0999878To0999955(h.getBucket0999878To0999955() + 1);
  } else if (val < 999984) {
    h.setBucket0999956To0999983(h.getBucket0999956To0999983() + 1);
  } else if (val < 999995) {
    h.setBucket0999984To0999994(h.getBucket0999984To0999994() + 1);
  } else if (val < 999999) {
    h.setBucket0999995To0999998(h.getBucket0999995To0999998() + 1);
  } else if (val < 1000000) {
    h.setBucket0999999(h.getBucket0999999() + 1);
  } else {
    h.setBucket1000000(h.getBucket1000000() + 1);
  }
}

}  // namespace c8
