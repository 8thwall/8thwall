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

class StructPointer {
  public static short dataSize(long rf) {
    // in words.
    return (short)(WirePointer.upper32Bits(rf) & 0xffff);
  }

  public static short ptrCount(long rf) {
    return (short)WireHelpers.logicalShift32(WirePointer.upper32Bits(rf), 16);
  }

  public static int wordSize(long rf) { return (int)dataSize(rf) + (int)ptrCount(rf); }

  public static void setFromStructSize(ByteBuffer buffer, int offset, StructSize size) {
    buffer.putShort(8 * offset + 4, size.data);
    buffer.putShort(8 * offset + 6, size.pointers);
  }

  public static void set(ByteBuffer buffer, int offset, short dataSize, short pointerCount) {
    buffer.putShort(8 * offset + 4, dataSize);
    buffer.putShort(8 * offset + 6, pointerCount);
  }
}

}  // namespace Capnp

