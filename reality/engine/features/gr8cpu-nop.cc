// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)
//
// Gr8 features were originally forked from OpenCV's orb features.
//
// Software License Agreement (BSD License)
//
//  Copyright (c) 2009, Willow Garage, Inc.
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//   * Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above
//     copyright notice, this list of conditions and the following
//     disclaimer in the documentation and/or other materials provided
//     with the distribution.
//   * Neither the name of the Willow Garage nor the names of its
//     contributors may be used to endorse or promote products derived
//     from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
//  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
//  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
//  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
//  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
//  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
//  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
//  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.

#include "bzl/inliner/rules2.h"

cc_library {
  visibility = {
    "//reality/engine/features:__subpackages__",
  };
  hdrs = {
    "gr8cpu.h",
  };
  deps = {
    ":gr8cpu-interface",
    ":image-point",
    "//c8:exceptions",
    "//c8/pixels:pixels",
    "//c8/pixels:pixels",
  };
}
cc_end(0xb128cddf);

#include "reality/engine/features/gr8cpu.h"

#include "c8/exceptions.h"

namespace c8 {
namespace {
  class Gr8CpuNop : public Gr8CpuInterface {
public:
  ImagePoints detectAndCompute(YPlanePixels frame) override {
    C8_THROW("CPU feature extraction not supported on this platform");
  };
};
}

Gr8Cpu Gr8Cpu::create(
  int nfeatures,
  float scaleFactor,
  int nlevels,
  int edgeThreshold,
  int firstLevel,
  int wtaK,
  int scoreType,
  int fastThreshold) {
  return Gr8Cpu(new Gr8CpuNop());
}

}  // namespace c8
