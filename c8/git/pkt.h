// Copyright (c) 2022 8th Wall, LLC.
// Original Author: Pawel Czarnecki (pawelczarnecki@nianticlabs.com)

#include <tuple>

#include "c8/process.h"
#include "c8/vector.h"

namespace c8 {

// maximum total (header+payload) length within one packet.
constexpr size_t MaxPacketLength = 65'516;

// PktReader and PktWriter own a process pipe for reading and writing, respectively.
// For testing, you may create a pipe in your test program and feed it data.

struct PktReader {

  PktReader(process::Pipe &&input) { _input = std::move(input); }

  // returns the size the header reported and fills the buffer with payload data.
  // does not strip any trailing LF.
  size_t readPacket(Vector<uint8_t> &dest);

  // same as readPacket but strips trailing LF, if present.
  size_t readTextPacket(Vector<uint8_t> &dest);

  // keeps eating packets, treating them as binary (not striping LFs)
  // and appending them to dest. dest is cleared before read begins.
  // until a flush packet is encountered.
  // clears the destination buffer before filling it.
  void readDataBlock(Vector<uint8_t> &dest);

private:
  process::Pipe _input;
};

struct PktWriter {

  PktWriter(process::Pipe &&output) { _output = std::move(output); }

  inline void writeFlush() { _output.writeBytes("0000", 4); }
  inline void writeDelim() { _output.writeBytes("0001", 4); }

  // writes a trailing LF. must be within payload size limits.
  void writeText(const String &src);

  // writes raw bytes, breaking up the payload as needed and finishing with a flush.
  void writeFullData(const Vector<uint8_t> &bytes);

private:
  process::Pipe _output;

  inline static const char *_makeHex4(size_t size) {
    static const char *hexMap = "0123456789abcdef";
    static char hex[5];
    for (int i = 3; i >= 0; i--) {
      hex[i] = hexMap[size % 16];
      size /= 16;
    }
    hex[4] = 0;
    return hex;
  }

  // automatically adds 4 to the payload size;
  inline void _writeDataHeader(size_t payloadSize) {
    _output.writeBytes(_makeHex4(payloadSize + 4), 4);
  }

  // adds header size (4) and trailing LF (1)
  inline void _writeTextHeader(size_t payloadSize) {
    _output.writeBytes(_makeHex4(payloadSize + 5), 4);
  }

  // finish off a text line with this
  inline void _writeLineFeed() { _output.writeBytes("\n", 1); }
};

}  // namespace c8
