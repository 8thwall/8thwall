// Copyright (c) 2013-2014 Sandstorm Development Group, Inc. and contributors
// Licensed under the MIT License:
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

using System;
using System.Collections.Generic;

namespace Capnp {

public class ReaderArena : Arena {

  public long limit;
  // current limit

  private readonly List<SegmentReader> segments;

  public ReaderArena(ByteBuffer[] segmentSlices, long traversalLimitInWords) {
    this.limit = traversalLimitInWords;
    this.segments = new List<SegmentReader>();
    for (int ii = 0; ii < segmentSlices.Length; ++ii) {
      this.segments.Add(new SegmentReader(segmentSlices[ii], this));
    }
  }

  public SegmentReader tryGetSegment(int id) {
    if (id >= segments.Count) {
      throw new DecodeException("SegmentReader " + id + " does not exist out of " + segments.Count + " readers");
    }
    return segments[id];
  }

  public void checkReadLimit(int numBytes) {
    if (numBytes > limit) {
      throw new DecodeException("Read limit exceeded.");
    } else {
      limit -= numBytes;
    }
  }
}

}  // namespace Capnp

