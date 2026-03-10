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

public class PackedInputStream : ReadableByteChannel {
  readonly BufferedInputStream inner;

  public PackedInputStream(BufferedInputStream input) { this.inner = input; }

  public int read(ByteBuffer outBuf) {

    int len = outBuf.remaining();
    if (len == 0) {
      return 0;
    }

    if (len % 8 != 0) {
      throw new ApplicationException("PackedInputStream reads must be word-aligned");
    }

    int outPtr = outBuf.position();
    int outEnd = outPtr + len;

    ByteBuffer inBuf = this.inner.getReadBuffer();

    while (true) {

      byte tag = 0;

      if (inBuf.remaining() < 10) {
        if (outBuf.remaining() == 0) {
          return len;
        }

        if (inBuf.remaining() == 0) {
          inBuf = this.inner.getReadBuffer();
          continue;
        }

        //# We have at least 1, but not 10, bytes available. We need to read
        //# slowly, doing a bounds check on each byte.

        tag = inBuf.get();

        for (int i = 0; i < 8; ++i) {
          if ((tag & (1 << i)) != 0) {
            if (inBuf.remaining() == 0) {
              inBuf = this.inner.getReadBuffer();
            }
            outBuf.put(inBuf.get());
          } else {
            outBuf.put((byte)0);
          }
        }

        if (inBuf.remaining() == 0 && (tag == 0 || tag == (byte)0xff)) {
          inBuf = this.inner.getReadBuffer();
        }
      } else {

        tag = inBuf.get();
        for (int n = 0; n < 8; ++n) {
          bool isNonzero = (tag & (1 << n)) != 0;
          outBuf.put((byte)(inBuf.get() & (isNonzero ? -1 : 0)));
          inBuf.position(inBuf.position() + (isNonzero ? 0 : -1));
        }
      }

      if (tag == 0) {
        if (inBuf.remaining() == 0) {
          throw new ApplicationException("Should always have non-empty buffer here.");
        }

        int runLength = (0xff & (int)inBuf.get()) * 8;

        if (runLength > outEnd - outPtr) {
          throw new ApplicationException("Packed input did not end cleanly on a segment boundary");
        }

        for (int i = 0; i < runLength; ++i) {
          outBuf.put((byte)0);
        }
      } else if (tag == (byte)0xff) {

        int runLength = (0xff & (int)inBuf.get()) * 8;

        if (inBuf.remaining() >= runLength) {
          //# Fast path.
          ByteBuffer slice = inBuf.slice();
          slice.limit(runLength);
          outBuf.put(slice);
          inBuf.position(inBuf.position() + runLength);
        } else {
          //# Copy over the first buffer, then do one big read for the rest.
          runLength -= inBuf.remaining();
          outBuf.put(inBuf);

          ByteBuffer slice = outBuf.slice();
          slice.limit(runLength);

          this.inner.read(slice);
          outBuf.position(outBuf.position() + runLength);

          if (outBuf.remaining() == 0) {
            return len;
          } else {
            inBuf = this.inner.getReadBuffer();
            continue;
          }
        }
      }

      if (outBuf.remaining() == 0) {
        return len;
      }
    }
  }

  public void close() { inner.close(); }

  public bool isOpen() { return inner.isOpen(); }
}

}  // namespace Capnp

