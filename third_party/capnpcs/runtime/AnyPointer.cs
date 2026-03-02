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

namespace Capnp {

public class AnyPointer {
  public class Factory : PointerFactory<Builder, Reader> {
    public Reader fromPointerReader(SegmentReader segment, int pointer, int nestingLimit) {
      return new Reader(segment, pointer, nestingLimit);
    }
    public Builder fromPointerBuilder(SegmentBuilder segment, int pointer) {
      return new Builder(segment, pointer);
    }
    public Builder
    initFromPointerBuilder(SegmentBuilder segment, int pointer, int elementCount) {
      Builder result = new Builder(segment, pointer);
      result.clear();
      return result;
    }
  }
  public static readonly Factory factory = new Factory();

  public class Reader {
    readonly SegmentReader segment;
    readonly int pointer;  // offset in words
    readonly int nestingLimit;

    public Reader(SegmentReader segment, int pointer, int nestingLimit) {
      this.segment = segment;
      this.pointer = pointer;
      this.nestingLimit = nestingLimit;
    }

    public bool isNull() {
      return WirePointer.isNull(
        this.segment.buffer.getLong(this.pointer * Constants.BYTES_PER_WORD));
    }

    public T getAs<T> (FromPointerReader<T> factory) {
      return factory.fromPointerReader(this.segment, this.pointer, this.nestingLimit);
    }
  }

  public class Builder {
    readonly SegmentBuilder segment;
    readonly int pointer;

    public Builder(SegmentBuilder segment, int pointer) {
      this.segment = segment;
      this.pointer = pointer;
    }

    public bool isNull() {
      return WirePointer.isNull(
        this.segment.buffer.getLong(this.pointer * Constants.BYTES_PER_WORD));
    }

    public T getAs<T> (FromPointerBuilder<T> factory) {
      return factory.fromPointerBuilder(this.segment, this.pointer);
    }

    public T initAs<T> (FromPointerBuilder<T> factory) {
      return factory.initFromPointerBuilder(this.segment, this.pointer, 0);
    }

    public T initAs<T> (FromPointerBuilder<T> factory, int elementCount) {
      return factory.initFromPointerBuilder(this.segment, this.pointer, elementCount);
    }

    public void setAs<T, U> (SetPointerBuilder<T, U> factory, U reader) {
      factory.setPointerBuilder(this.segment, this.pointer, reader);
    }

    public Reader asReader() { return new Reader(segment, pointer, 0x7fffffff); }

    public void clear() {
      WireHelpers.zeroObject(this.segment, this.pointer);
      this.segment.buffer.putLong(this.pointer * 8, 0L);
    }
  }
}

}  // namespace Capnp

