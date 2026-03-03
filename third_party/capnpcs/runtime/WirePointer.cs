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

class WirePointer {
  public const byte STRUCT = 0;
  public const byte LIST = 1;
  public const byte FAR = 2;
  public const byte OTHER = 3;

  public static bool isNull(Int64 wirePointer) { return wirePointer == 0; }

  public static Int32 offsetAndKind(Int64 wirePointer) {
    return (Int32)(wirePointer & 0xffffffff);
  }

  public static byte kind(Int64 wirePointer) { return (byte)(offsetAndKind(wirePointer) & 3); }

  public static Int32 target(Int32 offset, Int64 wirePointer) {
    return offset + 1 + WireHelpers.logicalShift32(offsetAndKind(wirePointer), 2);
  }

  public static void setKindAndTarget(
    ByteBuffer buffer, Int32 offset, byte kind, Int32 targetOffset) {
    buffer.putInt(offset * 8, (((targetOffset - offset) - 1) << 2) | kind);
  }

  public static void setKindWithZeroOffset(ByteBuffer buffer, Int32 offset, byte kind) {
    buffer.putInt(offset * Constants.BYTES_PER_WORD, kind);
  }

  public static void setKindAndTargetForEmptyStruct(ByteBuffer buffer, Int32 offset) {
    //# This pointer points at an empty struct. Assuming the
    //# WirePointer itself is in-bounds, we can set the target to
    //# point either at the WirePointer itself or immediately after
    //# it. The latter would cause the WirePointer to be "null"
    //# (since for an empty struct the upper 32 bits are going to
    //# be zero). So we set an offset of -1, as if the struct were
    //# allocated immediately before this pointer, to distinguish
    //# it from null.

    buffer.putInt(offset * 8, unchecked((Int32)0xfffffffc));
  }

  public static void setOffsetAndKind(ByteBuffer buffer, Int32 offset, Int32 offsetAndKind) {
    buffer.putInt(offset * 8, offsetAndKind);
  }

  public static Int32 inlineCompositeListElementCount(Int64 wirePointer) {
    return WireHelpers.logicalShift32(offsetAndKind(wirePointer), 2);
  }

  public static void setKindAndInlineCompositeListElementCount(
    ByteBuffer buffer, Int32 offset, byte kind, Int32 elementCount) {
    buffer.putInt(offset * 8, (elementCount << 2) | kind);
  }

  public static Int32 upper32Bits(Int64 wirePointer) {
    return (Int32)WireHelpers.logicalShift64(wirePointer, 32);
  }
}

}  // namespace Capnp

