// Copyright (c) 2022 8th Wall, LLC.
// Original Author: Pawel Czarnecki (pawelczarnecki@nianticlabs.com)

// Relevant context:
// https://github.com/git-lfs/pktline

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":pkt",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x1c0ac9a6);

#include <thread>

#include "c8/git/pkt.h"
#include "gmock/gmock.h"

namespace c8 {
class PktReaderWriterTest : public ::testing::Test {
public:
  PktReaderWriterTest(){};
};

inline String bytesToString(const Vector<uint8_t> &bytes) {
  return {bytes.begin(), bytes.begin() + bytes.size()};
}

TEST_F(PktReaderWriterTest, Reading) {
  auto [readerPipe, writerPipe] = process::Pipe::makeConnectedPair();
  PktReader pktReader(std::move(readerPipe));
  Vector<uint8_t> readPayload;
  size_t packetLength;

  // flush
  writerPipe.writeBytes("0000", 4);
  packetLength = pktReader.readPacket(readPayload);
  EXPECT_EQ(packetLength, 0);
  EXPECT_EQ(readPayload.size(), 0);

  // delim
  writerPipe.writeBytes("0001", 4);
  packetLength = pktReader.readPacket(readPayload);
  EXPECT_EQ(packetLength, 1);
  EXPECT_EQ(readPayload.size(), 0);

  // empty
  writerPipe.writeBytes("0004", 4);
  packetLength = pktReader.readPacket(readPayload);
  EXPECT_EQ(packetLength, 4);
  EXPECT_EQ(readPayload.size(), 0);

  // there are 16 hex digits, plus 1 newline character plus 4 bytes header = 21 total
  writerPipe.writeBytes("00150123456789abcdef\n", 21);
  packetLength = pktReader.readPacket(readPayload);
  EXPECT_EQ(packetLength, 21);
  EXPECT_EQ(readPayload.size(), 17);  // newline should not get stripped.

  writerPipe.writeBytes("00150123456789abcdef\n", 21);
  packetLength = pktReader.readTextPacket(readPayload);
  EXPECT_EQ(packetLength, 21);        // size includes header and trailing newline.
  EXPECT_EQ(readPayload.size(), 16);  // newline gets stripped.
}

TEST_F(PktReaderWriterTest, Writing) {
  auto [readerPipe, writerPipe] = process::Pipe::makeConnectedPair();
  PktWriter pktWriter(std::move(writerPipe));
  Vector<uint8_t> readBuffer;
  String readBufferText;

  pktWriter.writeFlush();
  pktWriter.writeDelim();
  pktWriter.writeText("abc123");

  readerPipe >> readBuffer;
  readBufferText = bytesToString(readBuffer);

  EXPECT_EQ(
    readBufferText,
    "0000"
    "0001"
    "000b"
    "abc123\n");

  pktWriter.writeFlush();
  pktWriter.writeText("");
  readerPipe >> readBuffer;
  EXPECT_EQ(
    bytesToString(readBuffer),
    "0000"
    "0005\n");

  Vector<uint8_t> testData = {3, 1, 8, 9, 4, 1, 3, 2, 9};
  pktWriter.writeFullData(testData);
  readerPipe >> readBuffer;
  // pktsize = 13 (0xd) and flush packet at the end.
  Vector<uint8_t> expected = {'0', '0', '0', 'd', 3, 1, 8, 9, 4, 1, 3, 2, 9, '0', '0', '0', '0'};
  EXPECT_EQ(readBuffer, expected);
}

TEST_F(PktReaderWriterTest, ReadingWritingBlobs) {
  const size_t testDataSize = 64 * 10240 * 1024;  // about 64 megabytes.
  Vector<uint8_t> testData;
  testData.resize(testDataSize);
  for (auto i = 0; i < testDataSize; i++) {
    testData[i] = i;
  }

  auto [readerPipe, writerPipe] = process::Pipe::makeConnectedPair();
  PktReader pktReader(std::move(readerPipe));
  PktWriter pktWriter(std::move(writerPipe));

  // since the pipe has limited size, it'll block until the reader reads from it
  // so we launch it in a parallel thread.
  // we're going to send this block three times.
  std::thread writer([&testData, &pktWriter] {
    pktWriter.writeFullData(testData);
    pktWriter.writeText("hello world :)");
    pktWriter.writeFullData(testData);
    pktWriter.writeFullData(testData);
    pktWriter.writeDelim();
  });

  Vector<uint8_t> recvData;
  String recvText;

  pktReader.readDataBlock(recvData);
  EXPECT_EQ(testData, recvData);

  pktReader.readTextPacket(recvData);
  EXPECT_EQ(bytesToString(recvData), "hello world :)");

  pktReader.readDataBlock(recvData);
  EXPECT_EQ(testData, recvData);

  pktReader.readDataBlock(recvData);
  EXPECT_EQ(testData, recvData);

  EXPECT_EQ(pktReader.readTextPacket(recvData), 1);  // returns size, 1 for delim
  writer.join();
}

}  // namespace c8
