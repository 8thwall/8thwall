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

public class ArrayOutputStream : BufferedOutputStream {

  public readonly ByteBuffer buf;

  public ArrayOutputStream(ByteBuffer buf) { this.buf = buf.duplicate(); }
  public ArrayOutputStream(int size) { this.buf = ByteBuffer.allocate(size); }
  public ArrayOutputStream(int size, bool signed) {
    this.buf = signed ? ByteBuffer.allocateSigned(size) : ByteBuffer.allocate(size);
  }

  public int write(ByteBuffer src) {
    int available = this.buf.remaining();
    int size = src.remaining();
    if (available < size) {
      throw new ApplicationException("backing buffer was not large enough");
    }
    this.buf.put(src);
    return size;
  }

  public ByteBuffer getWriteBuffer() { return this.buf; }

  public void close() { }

  public bool isOpen() { return true; }

  public void flush() {}
}

}  // namespace Capnp

