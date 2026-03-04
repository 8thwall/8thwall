// Copyright (c) 2022 8th Wall, LLC.
// Original Author: Pawel Czarnecki (pawelczarnecki@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "pkt.h",
  };
  visibility = {
    "//visibility:private",
  };
  deps = {
    "//c8:process",
    "//c8:vector",
  };
}
cc_end(0x416dba43);

#include "c8/git/pkt.h"

using namespace c8;

namespace {
size_t parseHex4(const Vector<uint8_t> &bytes) {
  size_t res = 0;
  for (int i = 0; i < 4; i++) {
    auto byte = bytes[i];
    auto pos = 3 - i;
    size_t multiplier = std::pow(16, pos);

    if (byte >= '0' && byte <= '9') {
      res += (byte - '0') * multiplier;
    } else if (byte >= 'a' && byte <= 'f') {
      res += (byte - 'a' + 10) * multiplier;
    } else if (byte >= 'A' && byte <= 'F') {
      res += (byte - 'A' + 10) * multiplier;
    } else {
      // todo(pawel) report error... right now we ignore malformed input
    }
  }
  return res;
}
}  // namespace

// returns the size the header reported and fills the buffer with payload data.
size_t PktReader::readPacket(Vector<uint8_t> &dest) {
  bool eof = _input.readBytes(dest, 4);
  if (eof) {
    // TODO(pawel) figure out how to handle this.
    C8Log("PktReader::readPacket() eof unhandled");
  }
  if (dest.size() < 4) {
    // todo(pawel) something better.
    C8_THROW("unexpected end of input");
  }

  size_t packetSize = parseHex4(dest);

  if (packetSize == 0 || packetSize == 1) {
    dest.resize(0);
    return packetSize;
  }

  if (packetSize < 4) {
    // todo(pawel) something better
    C8_THROW("unexpected payload size");
  }

  // this blocks until it has read as many bytes as we requested.
  _input.readBytes(dest, packetSize - 4);

  if (dest.size() != packetSize - 4) {
    // todo(pawel) should we try a read here?
    // should
    C8_THROW("not enough input bytes for payload size");
  }

  return packetSize;
}

void PktReader::readDataBlock(Vector<uint8_t> &dest) {
  Vector<uint8_t> buffer;
  dest.clear();
  size_t packetSize{SIZE_T_MAX};
  do {
    packetSize = readPacket(buffer);
    dest.insert(dest.end(), buffer.begin(), buffer.end());
  } while (packetSize != 0);  // size 0 signifies the flush packet.
}

size_t PktReader::readTextPacket(Vector<uint8_t> &dest) {
  auto packetSize = readPacket(dest);
  if (packetSize > 4 && dest[dest.size() - 1] == '\n') {
    dest.resize(dest.size() - 1);
  }
  // the git-lfs/pktline implementation does not modify the returned size.
  // this means dest.size() + 4 does not equal packetSize if LF was stripped.
  // https://github.com/git-lfs/pktline/blob/main/pkt_line.go#L91
  return packetSize;
}

void PktWriter::writeText(const String &src) {
  if (src.size() >= MaxPacketLength - 5) {
    C8_THROW("PktWriter::writeText() string too large to fit in single pkt");
  }
  _writeTextHeader(src.size());
  _output << src;
  _writeLineFeed();
}

void PktWriter::writeFullData(const Vector<uint8_t> &bytes) {
  const uint8_t *beginWrite{bytes.data()};
  const uint8_t *endWrite{beginWrite + bytes.size()};

  while (beginWrite < endWrite) {
    size_t payloadSize = std::min((size_t)(endWrite - beginWrite), MaxPacketLength - 4);
    _writeDataHeader(payloadSize);
    beginWrite = _output.writeBytes(beginWrite, payloadSize);
  };
  writeFlush();
}
