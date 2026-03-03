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

public class ElementSize {
  public const byte VOID = 0;
  public const byte BIT = 1;
  public const byte BYTE = 2;
  public const byte TWO_BYTES = 3;
  public const byte FOUR_BYTES = 4;
  public const byte EIGHT_BYTES = 5;
  public const byte POINTER = 6;
  public const byte INLINE_COMPOSITE = 7;

  public static Int32 dataBitsPerElement(byte size) {
    switch (size) {
      case VOID:
        return 0;
      case BIT:
        return 1;
      case BYTE:
        return 8;
      case TWO_BYTES:
        return 16;
      case FOUR_BYTES:
        return 32;
      case EIGHT_BYTES:
        return 64;
      case POINTER:
        return 0;
      case INLINE_COMPOSITE:
        return 0;
      default:
        throw new ApplicationException("impossible field size: " + size);
    }
  }

  public static short pointersPerElement(byte size) {
    switch (size) {
      case POINTER:
        return 1;
      default:
        return 0;
    }
  }
}

}  // namespace Capnp

