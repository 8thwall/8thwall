// Copyright (c) 2024 Niantic, Inc.
// Original Author: Nicholas Butko (nbutko@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "arc4.h",
  };
  deps = {
    "//c8:vector",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0xad24a444);

#include <array>
#include <numeric>

#include "c8/io/arc4.h"

namespace c8 {

Vector<uint8_t> arc4(const uint8_t *msg, int msgSize, const uint8_t *key, int keySize, int skip) {
  if (msgSize == 0 || keySize == 0) {
    return {};
  }

  std::array<uint8_t, 256> state;
  std::iota(state.begin(), state.end(), 0);

  for (int i = 0, j = 0; i < 256; ++i) {
    j = (j + state[i] + key[i % keySize]) & 0xff;
    std::swap(state[i], state[j]);
  }

  Vector<uint8_t> output(msgSize);
  for (int n = -skip, i = 0, j = 0; n < msgSize; ++n) {
    i = (i + 1) & 0xff;
    j = (j + state[i]) & 0xff;
    std::swap(state[i], state[j]);
    if (n < 0) {
      continue;  // Skip the first `skip` pseudorandom numbers.
    }
    output[n] = msg[n] ^ state[(state[i] + state[j]) & 0xff];
  }

  return output;
}

}  // namespace c8
