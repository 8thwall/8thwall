// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Alvin Portillo (alvin@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":xr-log-preparer",
    "//c8/io:capnp-messages",
    "@com_google_googletest//:gtest_main",
    "@zlib//:zlib",
  };
}
cc_end(0xbbfd2462);

#include <capnp/serialize-packed.h>
#include <gtest/gtest.h>
#include <kj/io.h>
#include <zlib.h>

#include <chrono>
#include <thread>

#include "c8/c8-log.h"
#include "c8/io/capnp-messages.h"
#include "c8/protolog/api/log-request.capnp.h"
#include "reality/engine/logging/xr-log-preparer.h"

using MutableEventDetail = c8::MutableRootMessage<c8::EventDetail>;
using MutableLogRecordHeader = c8::MutableRootMessage<c8::LogRecordHeader>;
using MutableLoggingDetail = c8::MutableRootMessage<c8::LoggingDetail>;
using MutableLogRecord = c8::MutableRootMessage<c8::LogRecord>;

namespace c8 {

namespace {

constexpr char DEVICE_MANUFACTURER[] = "Manufacturer";
constexpr char DEVICE_MODEL[] = "Model";
constexpr char DEVICE_OS[] = "Operating System";
constexpr char DEVICE_OS_VERSION[] = "1.2.3";

constexpr char EVENT_NAME[] = "Event_Name";
constexpr char EVENT_ID[] = "Event_Id";
constexpr char EVENT_PARENT_ID[] = "Event_Parent_Id";
constexpr int64_t EVENT_START_TIME_MICROS = 1234;
constexpr int64_t EVENT_END_TIME_MICROS = 4444;
constexpr int64_t EVENT_COUNT = 1;
constexpr int64_t EVENT_OF_TOTAL_COUNT = 1;

void mockEventDetailData(EventDetail::Builder *detail) {
  detail->setEventName(EVENT_NAME);
  detail->setEventId(EVENT_ID);
  detail->setParentId(EVENT_PARENT_ID);
  detail->setStartTimeMicros(EVENT_START_TIME_MICROS);
  detail->setEndTimeMicros(EVENT_END_TIME_MICROS);
  detail->setPositiveCount(EVENT_COUNT);
  detail->setOfTotalPositiveCount(EVENT_OF_TOTAL_COUNT);
}

void mockLoggingDetailData(LoggingDetail::Builder *detail) {
  detail->initEvents(1);
  MutableEventDetail eventDetailMessage;
  EventDetail::Builder eventDetail = eventDetailMessage.builder();
  mockEventDetailData(&eventDetail);
  detail->getEvents().setWithCaveats(0, eventDetail);
}

void mockDeviceInfo(DeviceInfo::Builder *deviceInfo) {
  deviceInfo->setManufacturer(DEVICE_MANUFACTURER);
  deviceInfo->setModel(DEVICE_MODEL);
  deviceInfo->setOs(DEVICE_OS);
  deviceInfo->setOsVersion(DEVICE_OS_VERSION);
}

class MockTimer : public Timer {
public:
  void tick(int64_t microsToIncrement = 1) { nowMicros_ += microsToIncrement; }

  int64_t getNowMicros() const override { return nowMicros_; }

private:
  int64_t nowMicros_ = 1;
};

}  // namespace

class XRLogPreparerTest : public ::testing::Test {
protected:
  XRLogPreparer xrLogPreparer;

  virtual ~XRLogPreparerTest() throw() {}
};

TEST_F(XRLogPreparerTest, TestPrepareLogForUpload) {
  // NOTE: The outputs of this test reflect that the logs prepared are a combination of the
  // provided DeviceInfo and LogSummary in the format of the LogRecord capnp struct.
  MutableLoggingDetail detailMessage;
  LoggingDetail::Builder loggingDetail = detailMessage.builder();
  mockLoggingDetailData(&loggingDetail);

  LatencySummarizer summarizer;
  summarizer.summarize(loggingDetail);

  MutableLogRecordHeader logRecordHeaderMessage;
  LogRecordHeader::Builder logHeader = logRecordHeaderMessage.builder();
  auto deviceInfo = logHeader.getDevice().getDeviceInfo();
  mockDeviceInfo(&deviceInfo);

  auto logRecordBytes = xrLogPreparer.prepareLogForUpload(logHeader.asReader(), &summarizer);

  auto bufferCapacity = 1500ul;
  uint8_t *uncompressedBuf = new uint8_t[bufferCapacity];

  auto result =
    uncompress(uncompressedBuf, &bufferCapacity, logRecordBytes->data(), logRecordBytes->size());

  EXPECT_EQ(Z_OK, result);

  kj::ArrayPtr<uint8_t> uncompressedBytes(uncompressedBuf, bufferCapacity);
  kj::ArrayInputStream inputStream(uncompressedBytes);
  capnp::PackedMessageReader logRecord(inputStream);
  LogServiceRequest::Reader requestReader = logRecord.getRoot<LogServiceRequest>();
  auto logRecordReader = requestReader.getRecords()[0];

  // Test that the device info mocked earlier is what's added to the log.
  auto logDeviceInfo = logRecordReader.getHeader().getDevice().getDeviceInfo();
  EXPECT_STREQ(DEVICE_MANUFACTURER, logDeviceInfo.getManufacturer().cStr());
  EXPECT_STREQ(DEVICE_MODEL, logDeviceInfo.getModel().cStr());
  EXPECT_STREQ(DEVICE_OS, logDeviceInfo.getOs().cStr());
  EXPECT_STREQ(DEVICE_OS_VERSION, logDeviceInfo.getOsVersion().cStr());

  // We only mocked one event, so there should only be one summarized.
  auto logSummary = logRecordReader.getRealityEngine().getStats().getSummary();
  EXPECT_EQ(1, logSummary.getEvents().size());

  // Test that the one event matches the one we mocked.
  auto eventSummary = logSummary.getEvents()[0];
  EXPECT_STREQ(EVENT_NAME, eventSummary.getEventName().cStr());
  EXPECT_EQ(EVENT_COUNT, eventSummary.getEventCount());

  delete[] uncompressedBuf;
}

TEST_F(XRLogPreparerTest, Times) {
  XRSessionStats stats;
  for (const auto &metric : metrics()) {
    stats.timer(metric).on();
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(2));

  MutableLogRecord logRecord;
  LogRecord::Builder logRecordBuilder = logRecord.builder();
  stats.exportRecord(logRecordBuilder);
  auto dwellTime = logRecordBuilder.getDwellTime();

  EXPECT_GT(dwellTime.getPageMillis(), 0);
  EXPECT_GT(dwellTime.getCameraMillis(), 0);
  EXPECT_GT(dwellTime.getDesktop3DMillis(), 0);
  EXPECT_GT(dwellTime.getWorldEffectMillis(), 0);
  EXPECT_GT(dwellTime.getWorldEffectResponsiveMillis(), 0);
  EXPECT_GT(dwellTime.getWorldEffectAbsoluteMillis(), 0);
  EXPECT_GT(dwellTime.getFaceEffectTrackingTime().getTotalMillis(), 0);
  EXPECT_GT(dwellTime.getArMillis(), 0);
  EXPECT_GT(dwellTime.getVrMillis(), 0);
  EXPECT_GT(dwellTime.getImageTargetTrackingTime().getTotalMillis(), 0);
  EXPECT_GT(dwellTime.getCurvyImageTargetTrackingTime().getTotalMillis(), 0);
  EXPECT_GT(dwellTime.getVpsTrackingTime().getTotalMillis(), 0);
  EXPECT_GT(dwellTime.getVpsTrackingTime().getTimeToNodeMillis(), 0);
  EXPECT_GT(dwellTime.getVpsTrackingTime().getNodeMillis(), 0);
  EXPECT_GT(dwellTime.getVpsTrackingTime().getTimeToWayspotAnchorMillis(), 0);
  EXPECT_GT(dwellTime.getVpsTrackingTime().getWayspotAnchorMillis(), 0);
}

TEST_F(XRLogPreparerTest, VpsTrackingTime_TimeToFallbackMillis_NoFallbackOccurred) {
  {
    XRSessionStats stats;

    // VPS enabled. Start timer.
    stats.timer("vpsTimeToFallback").on();

    std::this_thread::sleep_for(std::chrono::milliseconds(2));

    MutableLogRecord logRecord;
    LogRecord::Builder logRecordBuilder = logRecord.builder();
    stats.exportRecord(logRecordBuilder);
    auto dwellTime = logRecordBuilder.getDwellTime();

    EXPECT_EQ(dwellTime.getVpsTrackingTime().getTimeToFallbackMillis(), 0);
  }
  {
    XRSessionStats stats;

    // VPS disabled. Timer never started.

    std::this_thread::sleep_for(std::chrono::milliseconds(2));

    MutableLogRecord logRecord;
    LogRecord::Builder logRecordBuilder = logRecord.builder();
    stats.exportRecord(logRecordBuilder);
    auto dwellTime = logRecordBuilder.getDwellTime();

    EXPECT_EQ(dwellTime.getVpsTrackingTime().getTimeToFallbackMillis(), 0);
  }
}

TEST_F(XRLogPreparerTest, VpsTrackingTime_TimeToFallbackMillis_FallbackOccurred) {
  XRSessionStats stats;

  // VPS enabled. Start timer.
  stats.timer("vpsTimeToFallback").on();

  std::this_thread::sleep_for(std::chrono::milliseconds(2));

  // Fallback occurred. Stop timer.
  stats.timer("vpsTimeToFallback").off();

  MutableLogRecord logRecord;
  LogRecord::Builder logRecordBuilder = logRecord.builder();
  stats.exportRecord(logRecordBuilder);
  auto dwellTime = logRecordBuilder.getDwellTime();

  EXPECT_GT(dwellTime.getVpsTrackingTime().getTimeToFallbackMillis(), 0);
}

TEST_F(XRLogPreparerTest, Increment) {
  XRSessionStats stats;
  stats.incrementFacesFound();
  stats.incrementImageTargetsFound();
  stats.incrementImageTargetsFoundUnique();
  stats.incrementWayspotAnchorsFoundUnique();
  stats.incrementWayspotAnchorsFound();
  stats.incrementWayspotAnchorsUpdated();
  stats.incrementNodesFoundUnique();
  stats.incrementNodesFound();
  stats.incrementNodesUpdated();

  MutableLogRecord logRecord;
  LogRecord::Builder logRecordBuilder = logRecord.builder();
  stats.exportRecord(logRecordBuilder);

  EXPECT_EQ(logRecordBuilder.getEventCounts().getFacesFound().getTotal(), 1);
  EXPECT_EQ(logRecordBuilder.getEventCounts().getImageTargetsFound().getTotal(), 1);
  EXPECT_EQ(logRecordBuilder.getEventCounts().getImageTargetsFound().getUnique(), 1);
  EXPECT_EQ(logRecordBuilder.getEventCounts().getWayspotAnchorsFound().getUnique(), 1);
  EXPECT_EQ(logRecordBuilder.getEventCounts().getWayspotAnchorsFound().getTotal(), 1);
  EXPECT_EQ(logRecordBuilder.getEventCounts().getWayspotAnchorsFound().getUpdated(), 1);
  EXPECT_EQ(logRecordBuilder.getEventCounts().getNodesFound().getUnique(), 1);
  EXPECT_EQ(logRecordBuilder.getEventCounts().getNodesFound().getTotal(), 1);
  EXPECT_EQ(logRecordBuilder.getEventCounts().getNodesFound().getUpdated(), 1);
}

TEST_F(XRLogPreparerTest, IncrementThenReset) {
  XRSessionStats stats;
  stats.incrementFacesFound();
  stats.incrementImageTargetsFound();
  stats.incrementImageTargetsFoundUnique();
  stats.incrementWayspotAnchorsFound();
  stats.incrementWayspotAnchorsFoundUnique();
  stats.incrementNodesFound();
  stats.incrementNodesFoundUnique();
  stats.reset();

  MutableLogRecord logRecord;
  LogRecord::Builder logRecordBuilder = logRecord.builder();
  stats.exportRecord(logRecordBuilder);

  EXPECT_EQ(logRecordBuilder.getEventCounts().getFacesFound().getTotal(), 0);
  EXPECT_EQ(logRecordBuilder.getEventCounts().getImageTargetsFound().getTotal(), 0);
  EXPECT_EQ(logRecordBuilder.getEventCounts().getImageTargetsFound().getUnique(), 0);
  EXPECT_EQ(logRecordBuilder.getEventCounts().getWayspotAnchorsFound().getTotal(), 0);
  EXPECT_EQ(logRecordBuilder.getEventCounts().getWayspotAnchorsFound().getUnique(), 0);
  EXPECT_EQ(logRecordBuilder.getEventCounts().getNodesFound().getTotal(), 0);
  EXPECT_EQ(logRecordBuilder.getEventCounts().getNodesFound().getUnique(), 0);
}

TEST_F(XRLogPreparerTest, Current) {
  MockTimer t;
  t.on();
  EXPECT_EQ(t.current(), 0);

  t.tick();
  EXPECT_EQ(t.current(), 1);

  t.tick();
  EXPECT_EQ(t.current(), 2);

  t.tick(2);
  EXPECT_EQ(t.current(), 4);

  t.tick(40);
  EXPECT_EQ(t.current(), 44);
}

TEST_F(XRLogPreparerTest, IsOff) {
  MockTimer t;
  EXPECT_TRUE(t.isOff());
  EXPECT_FALSE(t.isOn());

  t.on();
  EXPECT_FALSE(t.isOff());
  EXPECT_TRUE(t.isOn());

  t.off();
  EXPECT_TRUE(t.isOff());
  EXPECT_FALSE(t.isOn());

  t.on();
  EXPECT_FALSE(t.isOff());
  EXPECT_TRUE(t.isOn());
}

TEST_F(XRLogPreparerTest, IsPaused) {
  MockTimer t;
  t.on();
  EXPECT_FALSE(t.isPaused());

  t.pause();
  EXPECT_TRUE(t.isPaused());

  t.unpause();
  EXPECT_FALSE(t.isPaused());
}

TEST_F(XRLogPreparerTest, IsOffIsPausedCombinations) {
  MockTimer t;
  {
    // on + unpaused -> off
    t.on();
    t.unpause();
    t.off();
    EXPECT_TRUE(t.isOff());
    EXPECT_FALSE(t.isPaused());

    // on + unpaused -> pause
    t.on();
    t.unpause();
    t.pause();
    EXPECT_FALSE(t.isOff());
    EXPECT_TRUE(t.isPaused());
  }
  {
    // off + unpaused -> on
    t.off();
    t.unpause();
    t.on();
    EXPECT_FALSE(t.isOff());
    EXPECT_FALSE(t.isPaused());

    // off + unpaused -> paused
    t.off();
    t.unpause();
    t.pause();
    EXPECT_TRUE(t.isOff());
    EXPECT_TRUE(t.isPaused());
  }
  {
    // on + paused -> off
    t.on();
    t.pause();
    t.off();
    EXPECT_TRUE(t.isOff());
    EXPECT_TRUE(t.isPaused());

    // on + paused -> unpaused
    t.on();
    t.pause();
    t.unpause();
    EXPECT_FALSE(t.isOff());
    EXPECT_FALSE(t.isPaused());
  }
  {
    // off + paused -> on
    t.off();
    t.pause();
    t.on();
    EXPECT_FALSE(t.isOff());
    EXPECT_TRUE(t.isPaused());

    // off + paused -> pause
    t.off();
    t.pause();
    t.pause();
    EXPECT_TRUE(t.isOff());
    EXPECT_TRUE(t.isPaused());
  }
}

TEST_F(XRLogPreparerTest, OnOff) {
  MockTimer t;

  t.on();
  EXPECT_EQ(t.current(), 0);

  t.tick();
  EXPECT_EQ(t.current(), 1);

  t.off();
  EXPECT_EQ(t.current(), 1);

  t.tick();
  EXPECT_EQ(t.current(), 1);

  t.on();
  EXPECT_EQ(t.current(), 1);

  t.tick();
  EXPECT_EQ(t.current(), 2);
}

TEST_F(XRLogPreparerTest, OnWhilePaused) {
  MockTimer t;
  t.on();
  EXPECT_FALSE(t.isOff());
  EXPECT_FALSE(t.isPaused());
  EXPECT_EQ(t.current(), 0);

  t.tick();
  EXPECT_FALSE(t.isOff());
  EXPECT_FALSE(t.isPaused());
  EXPECT_EQ(t.current(), 1);

  t.off();
  EXPECT_TRUE(t.isOff());
  EXPECT_FALSE(t.isPaused());
  EXPECT_EQ(t.current(), 1);

  t.tick();
  EXPECT_TRUE(t.isOff());
  EXPECT_FALSE(t.isPaused());
  EXPECT_EQ(t.current(), 1);

  t.pause();
  EXPECT_TRUE(t.isOff());
  EXPECT_TRUE(t.isPaused());
  EXPECT_EQ(t.current(), 1);

  t.tick();
  EXPECT_TRUE(t.isOff());
  EXPECT_TRUE(t.isPaused());
  EXPECT_EQ(t.current(), 1);

  t.on();
  EXPECT_FALSE(t.isOff());
  EXPECT_TRUE(t.isPaused());
  EXPECT_EQ(t.current(), 1);

  t.tick();
  EXPECT_FALSE(t.isOff());
  EXPECT_TRUE(t.isPaused());
  EXPECT_EQ(t.current(), 1);

  t.unpause();
  EXPECT_FALSE(t.isOff());
  EXPECT_FALSE(t.isPaused());
  EXPECT_EQ(t.current(), 1);

  t.tick();
  EXPECT_FALSE(t.isOff());
  EXPECT_FALSE(t.isPaused());
  EXPECT_EQ(t.current(), 2);
}

TEST_F(XRLogPreparerTest, OffWhilePaused) {
  MockTimer t;
  t.on();
  EXPECT_FALSE(t.isOff());
  EXPECT_FALSE(t.isPaused());
  EXPECT_EQ(t.current(), 0);

  t.tick();
  EXPECT_FALSE(t.isOff());
  EXPECT_FALSE(t.isPaused());
  EXPECT_EQ(t.current(), 1);

  t.pause();
  EXPECT_FALSE(t.isOff());
  EXPECT_TRUE(t.isPaused());
  EXPECT_EQ(t.current(), 1);

  t.tick();
  EXPECT_FALSE(t.isOff());
  EXPECT_TRUE(t.isPaused());
  EXPECT_EQ(t.current(), 1);

  t.off();
  EXPECT_TRUE(t.isOff());
  EXPECT_TRUE(t.isPaused());
  EXPECT_EQ(t.current(), 1);

  t.tick();
  EXPECT_TRUE(t.isOff());
  EXPECT_TRUE(t.isPaused());
  EXPECT_EQ(t.current(), 1);
}

TEST_F(XRLogPreparerTest, PauseUnpauseWhileOn) {
  // If you are on while paused, it won't accrue anything until you unpause(), then it will accrue.
  // If you pause() and then unpause() something that's on, it will end up on.
  MockTimer t;
  t.on();
  EXPECT_FALSE(t.isOff());
  EXPECT_FALSE(t.isPaused());
  EXPECT_EQ(t.current(), 0);

  t.tick();
  EXPECT_FALSE(t.isOff());
  EXPECT_FALSE(t.isPaused());
  EXPECT_EQ(t.current(), 1);

  t.pause();
  EXPECT_FALSE(t.isOff());
  EXPECT_TRUE(t.isPaused());
  EXPECT_EQ(t.current(), 1);

  t.tick();
  EXPECT_FALSE(t.isOff());
  EXPECT_TRUE(t.isPaused());
  EXPECT_EQ(t.current(), 1);

  t.unpause();
  EXPECT_FALSE(t.isOff());
  EXPECT_FALSE(t.isPaused());
  EXPECT_EQ(t.current(), 1);

  t.tick();
  EXPECT_FALSE(t.isOff());
  EXPECT_FALSE(t.isPaused());
  EXPECT_EQ(t.current(), 2);
}

TEST_F(XRLogPreparerTest, PauseUnpauseWhileOff) {
  // If you are off while paused, when you unpause(), it won't acrue anything until you on().
  // If you pause() and then unpause() something that's off, it will end up off.
  MockTimer t;
  t.on();
  t.tick();
  EXPECT_FALSE(t.isOff());
  EXPECT_FALSE(t.isPaused());
  EXPECT_EQ(t.current(), 1);

  t.off();
  EXPECT_TRUE(t.isOff());
  EXPECT_FALSE(t.isPaused());
  EXPECT_EQ(t.current(), 1);

  t.pause();
  EXPECT_TRUE(t.isOff());
  EXPECT_TRUE(t.isPaused());
  EXPECT_EQ(t.current(), 1);

  t.tick();
  EXPECT_TRUE(t.isOff());
  EXPECT_TRUE(t.isPaused());
  EXPECT_EQ(t.current(), 1);

  t.unpause();
  EXPECT_TRUE(t.isOff());
  EXPECT_FALSE(t.isPaused());
  EXPECT_EQ(t.current(), 1);

  t.tick();
  EXPECT_TRUE(t.isOff());
  EXPECT_FALSE(t.isPaused());
  EXPECT_EQ(t.current(), 1);
}

TEST_F(XRLogPreparerTest, ResetWhileOn) {
  // This is the main way in which engine code calls the timer.
  MockTimer t;

  t.on();
  EXPECT_FALSE(t.isOff());
  EXPECT_EQ(t.current(), 0);

  t.tick();
  EXPECT_EQ(t.current(), 1);

  t.reset();
  EXPECT_EQ(t.current(), 0);

  t.tick();
  EXPECT_EQ(t.current(), 1);

  t.off();
  EXPECT_TRUE(t.isOff());
  EXPECT_EQ(t.current(), 1);
}

TEST_F(XRLogPreparerTest, ResetWhileOff) {
  MockTimer t;

  t.on();
  t.tick();
  EXPECT_FALSE(t.isOff());
  EXPECT_EQ(t.current(), 1);

  t.off();
  EXPECT_TRUE(t.isOff());
  EXPECT_EQ(t.current(), 1);

  t.reset();
  EXPECT_TRUE(t.isOff());
  EXPECT_EQ(t.current(), 0);

  t.tick();
  EXPECT_TRUE(t.isOff());
  EXPECT_EQ(t.current(), 0);
}

TEST_F(XRLogPreparerTest, ResetWhileOnAndPaused) {
  MockTimer t;

  t.on();
  t.tick();
  EXPECT_FALSE(t.isOff());
  EXPECT_FALSE(t.isPaused());
  EXPECT_EQ(t.current(), 1);

  t.pause();
  EXPECT_FALSE(t.isOff());
  EXPECT_TRUE(t.isPaused());
  EXPECT_EQ(t.current(), 1);

  t.tick();
  EXPECT_FALSE(t.isOff());
  EXPECT_TRUE(t.isPaused());
  EXPECT_EQ(t.current(), 1);

  t.reset();
  EXPECT_FALSE(t.isOff());
  EXPECT_TRUE(t.isPaused());
  EXPECT_EQ(t.current(), 0);

  t.tick();
  EXPECT_FALSE(t.isOff());
  EXPECT_TRUE(t.isPaused());
  EXPECT_EQ(t.current(), 0);

  t.unpause();
  EXPECT_FALSE(t.isOff());
  EXPECT_FALSE(t.isPaused());
  EXPECT_EQ(t.current(), 0);

  t.tick();
  EXPECT_FALSE(t.isOff());
  EXPECT_FALSE(t.isPaused());
  EXPECT_EQ(t.current(), 1);
}

TEST_F(XRLogPreparerTest, ResetWhileOffAndPaused) {
  MockTimer t;

  t.on();
  t.tick();
  EXPECT_FALSE(t.isOff());
  EXPECT_FALSE(t.isPaused());
  EXPECT_EQ(t.current(), 1);

  t.off();
  EXPECT_TRUE(t.isOff());
  EXPECT_FALSE(t.isPaused());
  EXPECT_EQ(t.current(), 1);

  t.tick();
  EXPECT_TRUE(t.isOff());
  EXPECT_FALSE(t.isPaused());
  EXPECT_EQ(t.current(), 1);

  t.pause();
  EXPECT_TRUE(t.isOff());
  EXPECT_TRUE(t.isPaused());
  EXPECT_EQ(t.current(), 1);

  t.tick();
  EXPECT_TRUE(t.isOff());
  EXPECT_TRUE(t.isPaused());
  EXPECT_EQ(t.current(), 1);

  t.reset();
  EXPECT_TRUE(t.isOff());
  EXPECT_TRUE(t.isPaused());
  EXPECT_EQ(t.current(), 0);

  t.on();
  EXPECT_FALSE(t.isOff());
  EXPECT_TRUE(t.isPaused());
  EXPECT_EQ(t.current(), 0);

  t.tick();
  EXPECT_FALSE(t.isOff());
  EXPECT_TRUE(t.isPaused());
  EXPECT_EQ(t.current(), 0);
}

TEST_F(XRLogPreparerTest, PauseUnpauseMultipleTimesThenResetWhilePaused) {
  MockTimer t;

  t.on();
  t.tick();
  EXPECT_FALSE(t.isOff());
  EXPECT_FALSE(t.isPaused());
  EXPECT_EQ(t.current(), 1);

  t.pause();
  EXPECT_FALSE(t.isOff());
  EXPECT_TRUE(t.isPaused());
  EXPECT_EQ(t.current(), 1);

  t.tick();
  EXPECT_FALSE(t.isOff());
  EXPECT_TRUE(t.isPaused());
  EXPECT_EQ(t.current(), 1);

  t.unpause();
  EXPECT_FALSE(t.isOff());
  EXPECT_FALSE(t.isPaused());
  EXPECT_EQ(t.current(), 1);

  t.tick();
  EXPECT_FALSE(t.isOff());
  EXPECT_FALSE(t.isPaused());
  EXPECT_EQ(t.current(), 2);

  t.pause();
  EXPECT_FALSE(t.isOff());
  EXPECT_TRUE(t.isPaused());
  EXPECT_EQ(t.current(), 2);

  t.tick();
  EXPECT_FALSE(t.isOff());
  EXPECT_TRUE(t.isPaused());
  EXPECT_EQ(t.current(), 2);

  t.unpause();
  EXPECT_FALSE(t.isOff());
  EXPECT_FALSE(t.isPaused());
  EXPECT_EQ(t.current(), 2);

  t.tick();
  EXPECT_FALSE(t.isOff());
  EXPECT_FALSE(t.isPaused());
  EXPECT_EQ(t.current(), 3);

  t.pause();
  EXPECT_FALSE(t.isOff());
  EXPECT_TRUE(t.isPaused());
  EXPECT_EQ(t.current(), 3);

  t.tick();
  EXPECT_FALSE(t.isOff());
  EXPECT_TRUE(t.isPaused());
  EXPECT_EQ(t.current(), 3);

  t.reset();
  EXPECT_FALSE(t.isOff());
  EXPECT_TRUE(t.isPaused());
  EXPECT_EQ(t.current(), 0);

  t.tick();
  EXPECT_FALSE(t.isOff());
  EXPECT_TRUE(t.isPaused());
  EXPECT_EQ(t.current(), 0);

  t.unpause();
  EXPECT_FALSE(t.isOff());
  EXPECT_FALSE(t.isPaused());
  EXPECT_EQ(t.current(), 0);

  t.tick();
  EXPECT_FALSE(t.isOff());
  EXPECT_FALSE(t.isPaused());
  EXPECT_EQ(t.current(), 1);
}

}  // namespace c8
