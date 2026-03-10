// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include <capnp/message.h>

#include <map>
#include <memory>

#include "c8/io/capnp-messages.h"
#include "c8/map.h"
#include "c8/stats/api/detail.capnp.h"
#include "c8/stats/api/summary.capnp.h"
#include "c8/stats/logging-context.h"
#include "c8/string.h"
#include "c8/vector.h"

namespace c8 {

// A LatencySummarizer stores a mapping from stat tags to event counts and
// latency distributions that can be exported in a compact form.
//
// Summaries are constructed from multiple LoggingDetail messages. Periodically,
// the summary can be exported for printing or logging. To avoid double counting
// the summarizer should be explicitly reset.
//
// Typical usage:
//
// /* Extract logging detail from a logging context. */
// MutableRootMessage<LoggingDetail> detail;
// auto detailBuilder = detail.builder();
// rootLoggingContext.exportDetail(detailBuilder);
//
// /* Add it to the current summary being built. */
// latencySummarizer.summarize(detailBuilder);
//
// /* Periodically export the summary for logging to stdout or server. */
// MutableRootMessage<LoggingSummary> summary;
// auto summaryBuilder = summary.builder();
// latencySummarizer.exportSummary(summaryBuilder);
//
// /* If logging to standard error, use format functions for prettier output. */
// std::cerr << LatencySummarizer::formatSummaryBrief(summaryBuilder);
//
// /* If logging to the server, we don't want to double count the same data
//    multiple times, so reset. */
// latencySummarizer.reset();
class LatencySummarizer {
public:
  // Format summary for a single event.
  static String briefSummaryForEvent(const EventSummary::Reader &event);
  // Format logging summaries.
  static String formatSummaryBrief(const LoggingSummary::Reader &summary);
  static String formatSummaryDetailed(const LoggingSummary::Reader &summary);
  static String formatSummaryFull(const LoggingSummary::Reader &summary);

  // Reset all the summarized counters back to zero.
  void reset();

  // Add the logging detail to the summary.
  void summarize(const LoggingDetail::Reader &detail);

  // Add a logging context directly.
  void summarize(const LoggingContext &rootLoggingContext);

  // Export the data summarized previously.
  void exportSummary(LoggingSummary::Builder *summaryBuilder) const;

  void logBriefSummary() const;
  void logDetailedSummary() const;
  void logFullSummary() const;

  ConstRootMessage<EventSummary> summary(const String &tag) const;

  // Default constructor.
  LatencySummarizer() = default;

  // Default move constructors.
  LatencySummarizer(LatencySummarizer &&) = default;
  LatencySummarizer &operator=(LatencySummarizer &&) = default;

  // Disallow copying.
  LatencySummarizer(const LatencySummarizer &) = delete;
  LatencySummarizer &operator=(const LatencySummarizer &) = delete;

private:
  TreeMap<String, std::unique_ptr<MutableRootMessage<EventSummary>>> statMap_;

  void summarizeOneEvent(const EventDetail::Reader &event);

  static void updateDistribution(
    const EventDetail::Reader &event, EventSummary::Builder *summaryBuilder);

  static void updatePositiveHistogram(uint64_t val, DistributionStats::Builder *summaryBuilder);
  static void updatePercentHistogram(
    uint64_t count, uint64_t ofTotal, DistributionStats::Builder *summaryBuilder);
};

}  // namespace c8
