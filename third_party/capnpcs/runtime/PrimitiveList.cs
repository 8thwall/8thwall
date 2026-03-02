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

public class PrimitiveList {
  public class Void {
    public class Factory : ListFactory<Builder, Reader> {
      public Factory() : base(ElementSize.VOID) { }

      public override Reader constructReader(
        SegmentReader segment,
        int ptr,
        int elementCount,
        int step,
        int structDataSize,
        short structPointerCount,
        int nestingLimit) {
        return new Reader(
          segment, ptr, elementCount, step, structDataSize, structPointerCount, nestingLimit);
      }

      public override Builder constructBuilder(
        SegmentBuilder segment,
        int ptr,
        int elementCount,
        int step,
        int structDataSize,
        short structPointerCount) {
        return new Builder(segment, ptr, elementCount, step, structDataSize, structPointerCount);
      }
    }
    public static readonly Factory factory = new Factory();

    public class Reader : ListReader {
      public Reader(
        SegmentReader segment,
        int ptr,
        int elementCount,
        int step,
        int structDataSize,
        short structPointerCount,
        int nestingLimit)
        : base(segment, ptr, elementCount, step, structDataSize, structPointerCount, nestingLimit) {
      }

      public Capnp.Void get(int index) { return Capnp.Void.VOID; }
    }

    public class Builder : ListBuilder {
      public Builder(
        SegmentBuilder segment,
        int ptr,
        int elementCount,
        int step,
        int structDataSize,
        short structPointerCount)
        : base(segment, ptr, elementCount, step, structDataSize, structPointerCount) { }
    }
  }

  public class Boolean {
    public class Factory : ListFactory<Builder, Reader> {
      public Factory() : base(ElementSize.BIT) { }
      public override Reader constructReader(
        SegmentReader segment,
        int ptr,
        int elementCount,
        int step,
        int structDataSize,
        short structPointerCount,
        int nestingLimit) {
        return new Reader(
          segment, ptr, elementCount, step, structDataSize, structPointerCount, nestingLimit);
      }

      public override Builder constructBuilder(
        SegmentBuilder segment,
        int ptr,
        int elementCount,
        int step,
        int structDataSize,
        short structPointerCount) {
        return new Builder(segment, ptr, elementCount, step, structDataSize, structPointerCount);
      }
    }
    public static readonly Factory factory = new Factory();

    public class Reader : ListReader {
      public Reader(
        SegmentReader segment,
        int ptr,
        int elementCount,
        int step,
        int structDataSize,
        short structPointerCount,
        int nestingLimit)
        : base(segment, ptr, elementCount, step, structDataSize, structPointerCount, nestingLimit) {
      }

      public bool get(int index) { return _getBooleanElement(index); }
    }

    public class Builder : ListBuilder {
      public Builder(
        SegmentBuilder segment,
        int ptr,
        int elementCount,
        int step,
        int structDataSize,
        short structPointerCount)
        : base(segment, ptr, elementCount, step, structDataSize, structPointerCount) { }

      public bool get(int index) { return _getBooleanElement(index); }

      public void set(int index, bool value) { _setBooleanElement(index, value); }
    }
  }

  public class Byte {
    public class Factory : ListFactory<Builder, Reader> {
      public Factory() : base(ElementSize.BYTE) { }
      public override Reader constructReader(
        SegmentReader segment,
        int ptr,
        int elementCount,
        int step,
        int structDataSize,
        short structPointerCount,
        int nestingLimit) {
        return new Reader(
          segment, ptr, elementCount, step, structDataSize, structPointerCount, nestingLimit);
      }

      public override Builder constructBuilder(
        SegmentBuilder segment,
        int ptr,
        int elementCount,
        int step,
        int structDataSize,
        short structPointerCount) {
        return new Builder(segment, ptr, elementCount, step, structDataSize, structPointerCount);
      }
    }
    public static readonly Factory factory = new Factory();

    public class Reader : ListReader {
      public Reader(
        SegmentReader segment,
        int ptr,
        int elementCount,
        int step,
        int structDataSize,
        short structPointerCount,
        int nestingLimit)
        : base(segment, ptr, elementCount, step, structDataSize, structPointerCount, nestingLimit) {
      }

      public byte get(int index) { return _getByteElement(index); }
    }

    public class Builder : ListBuilder {
      public Builder(
        SegmentBuilder segment,
        int ptr,
        int elementCount,
        int step,
        int structDataSize,
        short structPointerCount)
        : base(segment, ptr, elementCount, step, structDataSize, structPointerCount) { }

      public byte get(int index) { return _getByteElement(index); }

      public void set(int index, byte value) { _setByteElement(index, value); }
    }
  }

  public static class Short {
    public class Factory : ListFactory<Builder, Reader> {
      public Factory() : base(ElementSize.TWO_BYTES) { }
      public override Reader constructReader(
        SegmentReader segment,
        int ptr,
        int elementCount,
        int step,
        int structDataSize,
        short structPointerCount,
        int nestingLimit) {
        return new Reader(
          segment, ptr, elementCount, step, structDataSize, structPointerCount, nestingLimit);
      }

      public override Builder constructBuilder(
        SegmentBuilder segment,
        int ptr,
        int elementCount,
        int step,
        int structDataSize,
        short structPointerCount) {
        return new Builder(segment, ptr, elementCount, step, structDataSize, structPointerCount);
      }
    }
    public static readonly Factory factory = new Factory();

    public class Reader : ListReader {
      public Reader(
        SegmentReader segment,
        int ptr,
        int elementCount,
        int step,
        int structDataSize,
        short structPointerCount,
        int nestingLimit)
        : base(segment, ptr, elementCount, step, structDataSize, structPointerCount, nestingLimit) {
      }

      public short get(int index) { return _getShortElement(index); }
    }

    public class Builder : ListBuilder {
      public Builder(
        SegmentBuilder segment,
        int ptr,
        int elementCount,
        int step,
        int structDataSize,
        short structPointerCount)
        : base(segment, ptr, elementCount, step, structDataSize, structPointerCount) {}

      public short get(int index) { return _getShortElement(index); }

      public void set(int index, short value) { _setShortElement(index, value); }
    }
  }

  public class Int {
    public class Factory : ListFactory<Builder, Reader> {
      public Factory() : base(ElementSize.FOUR_BYTES) { }
      public override Reader constructReader(
        SegmentReader segment,
        int ptr,
        int elementCount,
        int step,
        int structDataSize,
        short structPointerCount,
        int nestingLimit) {
        return new Reader(
          segment, ptr, elementCount, step, structDataSize, structPointerCount, nestingLimit);
      }

      public override Builder constructBuilder(
        SegmentBuilder segment,
        int ptr,
        int elementCount,
        int step,
        int structDataSize,
        short structPointerCount) {
        return new Builder(segment, ptr, elementCount, step, structDataSize, structPointerCount);
      }
    }
    public static readonly Factory factory = new Factory();

    public class Reader : ListReader {
      public Reader(
        SegmentReader segment,
        int ptr,
        int elementCount,
        int step,
        int structDataSize,
        short structPointerCount,
        int nestingLimit)
        : base(segment, ptr, elementCount, step, structDataSize, structPointerCount, nestingLimit) {
      }

      public int get(int index) { return _getIntElement(index); }
    }

    public class Builder : ListBuilder {
      public Builder(
        SegmentBuilder segment,
        int ptr,
        int elementCount,
        int step,
        int structDataSize,
        short structPointerCount)
        : base(segment, ptr, elementCount, step, structDataSize, structPointerCount) { }

      public int get(int index) { return _getIntElement(index); }

      public void set(int index, int value) { _setIntElement(index, value); }
    }
  }

  public class Float {
    public class Factory : ListFactory<Builder, Reader> {
      public Factory() : base(ElementSize.FOUR_BYTES) { }
      public override Reader constructReader(
        SegmentReader segment,
        int ptr,
        int elementCount,
        int step,
        int structDataSize,
        short structPointerCount,
        int nestingLimit) {
        return new Reader(
          segment, ptr, elementCount, step, structDataSize, structPointerCount, nestingLimit);
      }

      public override Builder constructBuilder(
        SegmentBuilder segment,
        int ptr,
        int elementCount,
        int step,
        int structDataSize,
        short structPointerCount) {
        return new Builder(segment, ptr, elementCount, step, structDataSize, structPointerCount);
      }
    }
    public static readonly Factory factory = new Factory();

    public class Reader : ListReader {
      public Reader(
        SegmentReader segment,
        int ptr,
        int elementCount,
        int step,
        int structDataSize,
        short structPointerCount,
        int nestingLimit)
        : base(segment, ptr, elementCount, step, structDataSize, structPointerCount, nestingLimit) {
      }

      public float get(int index) { return _getFloatElement(index); }
    }

    public class Builder : ListBuilder {
      public Builder(
        SegmentBuilder segment,
        int ptr,
        int elementCount,
        int step,
        int structDataSize,
        short structPointerCount)
        : base(segment, ptr, elementCount, step, structDataSize, structPointerCount) {
      }

      public float get(int index) { return _getFloatElement(index); }

      public void set(int index, float value) { _setFloatElement(index, value); }
    }
  }

  public class Long {
    public class Factory : ListFactory<Builder, Reader> {
      public Factory() : base(ElementSize.EIGHT_BYTES) { }
      public override Reader constructReader(
        SegmentReader segment,
        int ptr,
        int elementCount,
        int step,
        int structDataSize,
        short structPointerCount,
        int nestingLimit) {
        return new Reader(
          segment, ptr, elementCount, step, structDataSize, structPointerCount, nestingLimit);
      }

      public override Builder constructBuilder(
        SegmentBuilder segment,
        int ptr,
        int elementCount,
        int step,
        int structDataSize,
        short structPointerCount) {
        return new Builder(segment, ptr, elementCount, step, structDataSize, structPointerCount);
      }
    }
    public static readonly Factory factory = new Factory();

    public class Reader : ListReader {
      public Reader(
        SegmentReader segment,
        int ptr,
        int elementCount,
        int step,
        int structDataSize,
        short structPointerCount,
        int nestingLimit)
        : base(segment, ptr, elementCount, step, structDataSize, structPointerCount, nestingLimit) {
      }

      public long get(int index) { return _getLongElement(index); }
    }

    public class Builder : ListBuilder {
      public Builder(
        SegmentBuilder segment,
        int ptr,
        int elementCount,
        int step,
        int structDataSize,
        short structPointerCount)
        : base(segment, ptr, elementCount, step, structDataSize, structPointerCount) {
      }

      public long get(int index) { return _getLongElement(index); }

      public void set(int index, long value) { _setLongElement(index, value); }
    }
  }

  public class Double {
    public class Factory : ListFactory<Builder, Reader> {
      public Factory() : base(ElementSize.EIGHT_BYTES) { }
      public override Reader constructReader(
        SegmentReader segment,
        int ptr,
        int elementCount,
        int step,
        int structDataSize,
        short structPointerCount,
        int nestingLimit) {
        return new Reader(
          segment, ptr, elementCount, step, structDataSize, structPointerCount, nestingLimit);
      }

      public override Builder constructBuilder(
        SegmentBuilder segment,
        int ptr,
        int elementCount,
        int step,
        int structDataSize,
        short structPointerCount) {
        return new Builder(segment, ptr, elementCount, step, structDataSize, structPointerCount);
      }
    }
    public static readonly Factory factory = new Factory();

    public class Reader : ListReader {
      public Reader(
        SegmentReader segment,
        int ptr,
        int elementCount,
        int step,
        int structDataSize,
        short structPointerCount,
        int nestingLimit)
        : base(segment, ptr, elementCount, step, structDataSize, structPointerCount, nestingLimit) {
      }

      public double get(int index) { return _getDoubleElement(index); }
    }

    public class Builder : ListBuilder {
      public Builder(
        SegmentBuilder segment,
        int ptr,
        int elementCount,
        int step,
        int structDataSize,
        short structPointerCount)
        : base(segment, ptr, elementCount, step, structDataSize, structPointerCount) {
      }

      public double get(int index) { return _getDoubleElement(index); }

      public void set(int index, double value) { _setDoubleElement(index, value); }
    }
  }
}

}  // namespace Capnp

