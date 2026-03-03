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

namespace Capnp {

class FarPointer {
  public static Int32 getSegmentId(Int64 r) { return WirePointer.upper32Bits(r); }

  public static Int32 positionInSegment(Int64 r) {
    return WireHelpers.logicalShift32(WirePointer.offsetAndKind(r), 3);
  }

  public static bool isDoubleFar(Int64 r) {
    return (WireHelpers.logicalShift32(WirePointer.offsetAndKind(r), 2) & 1) != 0;
  }

  public static void setSegmentId(ByteBuffer buffer, Int32 offset, Int32 segmentId) {
    buffer.putInt(8 * offset + 4, segmentId);
  }

  public static void set(ByteBuffer buffer, Int32 offset, bool isDoubleFar, Int32 pos) {
    Int32 idf = isDoubleFar ? 1 : 0;
    WirePointer.setOffsetAndKind(buffer, offset, (pos << 3) | (idf << 2) | WirePointer.FAR);
  }
}

}  // namespace Capnp

