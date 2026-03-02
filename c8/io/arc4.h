// Copyright (c) 2024 Niantic, Inc.
// Original Author: Nicholas Butko (nbutko@nianticlabs.com)

#include "c8/vector.h"

namespace c8 {

// Encode or decode a message using the ARC4 algorithm with the supplied key and skip.
Vector<uint8_t> arc4(
  const uint8_t *msg, int msgSize, const uint8_t *key, int keySize, int skip);

}  // namespace c8
