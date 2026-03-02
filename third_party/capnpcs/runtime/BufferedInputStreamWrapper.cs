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

public class BufferedInputStreamWrapper : BufferedInputStream {

  private readonly ReadableByteChannel inner;
  private readonly ByteBuffer buf;

  public BufferedInputStreamWrapper(ReadableByteChannel chan) {
    this.inner = chan;
    this.buf = ByteBuffer.allocate(8192);
    this.buf.limit(0);
  }

  public int read(ByteBuffer dst) {
    int numBytes = dst.remaining();
    if (numBytes < this.buf.remaining()) {
      //# Serve from the current buffer.
      ByteBuffer slice = this.buf.slice();
      slice.limit(numBytes);
      dst.put(slice);
      this.buf.position(this.buf.position() + numBytes);
      return numBytes;
    } else {
      //# Copy current available into destination.
      int fromFirstBuffer = this.buf.remaining();
      {
        ByteBuffer slice = this.buf.slice();
        slice.limit(fromFirstBuffer);
        dst.put(slice);
      }

      numBytes -= fromFirstBuffer;
      if (numBytes <= this.buf.capacity()) {
        //# Read the next buffer-full.
        this.buf.clear();
        int n = readAtLeast(this.inner, this.buf, numBytes);

        this.buf.rewind();
        ByteBuffer slice = this.buf.slice();
        slice.limit(numBytes);
        dst.put(slice);

        this.buf.limit(n);
        this.buf.position(numBytes);
        return fromFirstBuffer + numBytes;
      } else {
        //# Forward large read to the underlying stream.
        this.buf.clear();
        this.buf.limit(0);
        return fromFirstBuffer + readAtLeast(this.inner, dst, numBytes);
      }
    }
  }

  public ByteBuffer getReadBuffer() {
    if (this.buf.remaining() == 0) {
      this.buf.clear();
      int n = readAtLeast(this.inner, this.buf, 1);
      this.buf.rewind();
      this.buf.limit(n);
    }
    return this.buf;
  }

  public void close() { this.inner.close(); }

  public bool isOpen() { return this.inner.isOpen(); }

  public static int readAtLeast(ReadableByteChannel reader, ByteBuffer buf, int minBytes) {
    int numRead = 0;
    while (numRead < minBytes) {
      int res = reader.read(buf);
      if (res < 0) {
        throw new ApplicationException("premature EOF");
      }
      numRead += res;
    }
    return numRead;
  }
}

}  // namespace Capnp

