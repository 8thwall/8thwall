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

public class StructReader {
  public interface Factory<T> {
    T constructReader(
      SegmentReader segment,
      int data,
      int pointers,
      int dataSize,
      short pointerCount,
      int nestingLimit);
  }

  public readonly SegmentReader segment;
  public readonly int data;      // byte offset to data section
  public readonly int pointers;  // word offset of pointer section
  public readonly int dataSize;  // in bits
  public readonly short pointerCount;
  public readonly int nestingLimit;

  public StructReader() {
    this.segment = SegmentReader.EMPTY;
    this.data = 0;
    this.pointers = 0;
    this.dataSize = 0;
    this.pointerCount = 0;
    this.nestingLimit = 0x7fffffff;
  }

  public StructReader(
    SegmentReader segment,
    int data,
    int pointers,
    int dataSize,
    short pointerCount,
    int nestingLimit) {
    this.segment = segment;
    this.data = data;
    this.pointers = pointers;
    this.dataSize = dataSize;
    this.pointerCount = pointerCount;
    this.nestingLimit = nestingLimit;
  }

  protected bool _getBoolField(int offset) {
    // XXX should use unsigned operations
    if (offset < this.dataSize) {
      byte b = this.segment.buffer.get(this.data + offset / 8);

      return (b & (1 << (offset % 8))) != 0;
    } else {
      return false;
    }
  }

  protected bool _getBoolField(int offset, bool mask) {
    return this._getBoolField(offset) ^ mask;
  }

  protected byte _getByteField(int offset) {
    if ((offset + 1) * 8 <= this.dataSize) {
      return this.segment.buffer.get(this.data + offset);
    } else {
      return 0;
    }
  }

  protected byte _getByteField(int offset, byte mask) {
    return (byte)(this._getByteField(offset) ^ mask);
  }

  protected short _getShortField(int offset) {
    if ((offset + 1) * 16 <= this.dataSize) {
      return this.segment.buffer.getShort(this.data + offset * 2);
    } else {
      return 0;
    }
  }

  protected short _getShortField(int offset, short mask) {
    return (short)(this._getShortField(offset) ^ mask);
  }

  protected int _getIntField(int offset) {
    if ((offset + 1) * 32 <= this.dataSize) {
      return this.segment.buffer.getInt(this.data + offset * 4);
    } else {
      return 0;
    }
  }

  protected int _getIntField(int offset, int mask) {
    return this._getIntField(offset) ^ mask;
  }

  protected long _getLongField(int offset) {
    if ((offset + 1) * 64 <= this.dataSize) {
      return this.segment.buffer.getLong(this.data + offset * 8);
    } else {
      return 0;
    }
  }

  protected long _getLongField(int offset, long mask) {
    return this._getLongField(offset) ^ mask;
  }

  protected float _getFloatField(int offset) {
    if ((offset + 1) * 32 <= this.dataSize) {
      return this.segment.buffer.getFloat(this.data + offset * 4);
    } else {
      return 0;
    }
  }

  protected float _getFloatField(int offset, int mask) {
    int floatAsInt = this.segment.buffer.getInt(this.data + offset * 4) ^ mask;
    if ((offset + 1) * 32 <= this.dataSize) {
      return BitConverter.ToSingle(BitConverter.GetBytes(floatAsInt), 0);
    } else {
      return BitConverter.ToSingle(BitConverter.GetBytes(mask), 0);
    }
  }

  protected double _getDoubleField(int offset) {
    if ((offset + 1) * 64 <= this.dataSize) {
      return this.segment.buffer.getDouble(this.data + offset * 8);
    } else {
      return 0;
    }
  }

  protected double _getDoubleField(int offset, long mask) {
    if ((offset + 1) * 64 <= this.dataSize) {
      return BitConverter.Int64BitsToDouble(this.segment.buffer.getLong(this.data + offset * 8) ^ mask);
    } else {
      return BitConverter.Int64BitsToDouble(mask);
    }
  }

  protected bool _pointerFieldIsNull(int ptrIndex) {
    return ptrIndex >= this.pointerCount
      || this.segment.buffer.getLong((this.pointers + ptrIndex) * Constants.BYTES_PER_WORD) == 0;
  }

  protected T _getPointerField<T> (FromPointerReader<T> factory, int ptrIndex) {
    if (ptrIndex < this.pointerCount) {
      return factory.fromPointerReader(this.segment, this.pointers + ptrIndex, this.nestingLimit);
    } else {
      return factory.fromPointerReader(SegmentReader.EMPTY, 0, this.nestingLimit);
    }
  }

  protected T _getPointerField<T> (
    FromPointerReaderRefDefault<T> factory,
    int ptrIndex,
    SegmentReader defaultSegment,
    int defaultOffset) {
    if (ptrIndex < this.pointerCount) {
      return factory.fromPointerReaderRefDefault(
        this.segment, this.pointers + ptrIndex, defaultSegment, defaultOffset, this.nestingLimit);
    } else {
      return factory.fromPointerReaderRefDefault(
        SegmentReader.EMPTY, 0, defaultSegment, defaultOffset, this.nestingLimit);
    }
  }

  protected T _getPointerField<T> (
    FromPointerReaderBlobDefault<T> factory,
    int ptrIndex,
    ByteBuffer defaultBuffer,
    int defaultOffset,
    int defaultSize) {
    if (ptrIndex < this.pointerCount) {
      return factory.fromPointerReaderBlobDefault(
        this.segment, this.pointers + ptrIndex, defaultBuffer, defaultOffset, defaultSize);
    } else {
      return factory.fromPointerReaderBlobDefault(
        SegmentReader.EMPTY, 0, defaultBuffer, defaultOffset, defaultSize);
    }
  }
}

}  // namespace Capnp

