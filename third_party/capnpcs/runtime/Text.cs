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

public class Text {
  public class Factory
    : FromPointerReaderBlobDefault<Reader>,
      FromPointerBuilderBlobDefault<Builder>,
      PointerFactory<Builder, Reader>,
      SetPointerBuilder<Builder, Reader> {
    public Reader fromPointerReaderBlobDefault(
      SegmentReader segment,
      int pointer,
      ByteBuffer defaultBuffer,
      int defaultOffset,
      int defaultSize) {
      return WireHelpers.readTextPointer(
        segment, pointer, defaultBuffer, defaultOffset, defaultSize);
    }

    public Reader fromPointerReader(SegmentReader segment, int pointer, int nestingLimit) {
      return WireHelpers.readTextPointer(segment, pointer, null, 0, 0);
    }

    public Builder fromPointerBuilderBlobDefault(
      SegmentBuilder segment,
      int pointer,
      ByteBuffer defaultBuffer,
      int defaultOffset,
      int defaultSize) {
      return WireHelpers.getWritableTextPointer(
        pointer, segment, defaultBuffer, defaultOffset, defaultSize);
    }

    public Builder fromPointerBuilder(SegmentBuilder segment, int pointer) {
      return WireHelpers.getWritableTextPointer(pointer, segment, null, 0, 0);
    }

    public Builder initFromPointerBuilder(SegmentBuilder segment, int pointer, int size) {
      return WireHelpers.initTextPointer(pointer, segment, size);
    }

    public void setPointerBuilder(SegmentBuilder segment, int pointer, Reader value) {
      WireHelpers.setTextPointer(pointer, segment, value);
    }
  }
  public static Factory factory = new Factory();

  public class Reader {
    public readonly ByteBuffer buffer;
    public readonly int offset;  // in bytes
    public readonly int size_;    // in bytes, not including NUL terminator

    public Reader() {
      // TODO what about the null terminator?
      this.buffer = ByteBuffer.allocate(0);
      this.offset = 0;
      this.size_ = 0;
    }

    public Reader(ByteBuffer buffer, int offset, int size) {
      this.buffer = buffer;
      this.offset = offset * 8;
      this.size_ = size;
    }

    public Reader(String value) {
      try {
        byte[] bytes = System.Text.Encoding.UTF8.GetBytes(value);
        this.buffer = ByteBuffer.wrap(bytes);
        this.offset = 0;
        this.size_ = bytes.Length;
      } catch (Exception e) {
        throw new ApplicationException("UTF-8 is unsupported", e);
      }
    }

    public int size() { return this.size_; }

    public ByteBuffer asByteBuffer() {
      ByteBuffer dup = this.buffer.asReadOnlyBuffer();
      dup.position(this.offset);
      ByteBuffer result = dup.slice();
      result.limit(this.size_);
      return result;
    }

    public override String ToString() {
      byte[] bytes = new byte[this.size()];

      ByteBuffer dup = this.buffer.duplicate();
      dup.position(this.offset);
      dup.get(bytes, 0, this.size_);

      try {
        return System.Text.Encoding.UTF8.GetString(bytes);
      } catch (Exception e) {
        throw new ApplicationException("UTF-8 is unsupported", e);
      }
    }
  }

  public class Builder {
    public readonly ByteBuffer buffer;
    public readonly int offset;  // in bytes
    public readonly int size;    // in bytes

    public Builder() {
      this.buffer = ByteBuffer.allocate(0);
      this.offset = 0;
      this.size = 0;
    }

    public Builder(ByteBuffer buffer, int offset, int size) {
      this.buffer = buffer;
      this.offset = offset;
      this.size = size;
    }

    public ByteBuffer asByteBuffer() {
      ByteBuffer dup = this.buffer.duplicate();
      dup.position(this.offset);
      ByteBuffer result = dup.slice();
      result.limit(this.size);
      return result;
    }

    public override String ToString() {
      byte[] bytes = new byte[this.size];

      ByteBuffer dup = this.buffer.duplicate();
      dup.position(this.offset);
      dup.get(bytes, 0, this.size);

      try {
        return System.Text.Encoding.UTF8.GetString(bytes);
      } catch (Exception e) {
        throw new ApplicationException("UTF-8 is unsupported", e);
      }
    }
  }
}

}  // namespace Capnp

