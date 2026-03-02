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

public class StructBuilder {
  public interface Factory<T> {
    T constructBuilder(
      SegmentBuilder segment, int data, int pointers, int dataSize, short pointerCount);
    StructSize structSize();
  }

  protected readonly SegmentBuilder segment;
  protected readonly int data;      // byte offset to data section
  protected readonly int pointers;  // word offset of pointer section
  protected readonly int dataSize;  // in bits
  protected readonly short pointerCount;

  public StructBuilder(
    SegmentBuilder segment, int data, int pointers, int dataSize, short pointerCount) {
    this.segment = segment;
    this.data = data;
    this.pointers = pointers;
    this.dataSize = dataSize;
    this.pointerCount = pointerCount;
  }

  protected bool _getBoolField(int offset) {
    int bitOffset = offset;
    int position = this.data + (bitOffset / 8);
    return (this.segment.buffer.get(position) & (1 << (bitOffset % 8))) != 0;
  }

  protected bool _getBoolField(int offset, bool mask) {
    return this._getBoolField(offset) ^ mask;
  }

  protected void _setBoolField(int offset, bool value) {
    int bitOffset = offset;
    byte bitnum = (byte)(bitOffset % 8);
    int position = this.data + (bitOffset / 8);
    byte oldValue = this.segment.buffer.get(position);
    this.segment.buffer.put(
      position, (byte)((oldValue & (~(1 << bitnum))) | ((value ? 1 : 0) << bitnum)));
  }

  protected void _setBoolField(int offset, bool value, bool mask) {
    this._setBoolField(offset, value ^ mask);
  }

  protected byte _getByteField(int offset) {
    return this.segment.buffer.get(this.data + offset);
  }

  protected byte _getByteField(int offset, byte mask) {
    return (byte)(this._getByteField(offset) ^ mask);
  }

  protected void _setByteField(int offset, byte value) {
    this.segment.buffer.put(this.data + offset, value);
  }

  protected void _setByteField(int offset, byte value, byte mask) {
    this._setByteField(offset, (byte)(value ^ mask));
  }

  protected short _getShortField(int offset) {
    return this.segment.buffer.getShort(this.data + offset * 2);
  }

  protected short _getShortField(int offset, short mask) {
    return (short)(this._getShortField(offset) ^ mask);
  }

  protected void _setShortField(int offset, short value) {
    this.segment.buffer.putShort(this.data + offset * 2, value);
  }

  protected void _setShortField(int offset, short value, short mask) {
    this._setShortField(offset, (short)(value ^ mask));
  }

  protected int _getIntField(int offset) {
    return this.segment.buffer.getInt(this.data + offset * 4);
  }

  protected int _getIntField(int offset, int mask) {
    return this._getIntField(offset) ^ mask;
  }

  protected void _setIntField(int offset, int value) {
    this.segment.buffer.putInt(this.data + offset * 4, value);
  }

  protected void _setIntField(int offset, int value, int mask) {
    this._setIntField(offset, value ^ mask);
  }

  protected long _getLongField(int offset) {
    return this.segment.buffer.getLong(this.data + offset * 8);
  }

  protected long _getLongField(int offset, long mask) {
    return this._getLongField(offset) ^ mask;
  }

  protected void _setLongField(int offset, long value) {
    this.segment.buffer.putLong(this.data + offset * 8, value);
  }

  protected void _setLongField(int offset, long value, long mask) {
    this._setLongField(offset, value ^ mask);
  }

  protected float _getFloatField(int offset) {
    return this.segment.buffer.getFloat(this.data + offset * 4);
  }

  protected float _getFloatField(int offset, int mask) {
    return BitConverter.ToSingle(
      BitConverter.GetBytes(this.segment.buffer.getInt(this.data + offset * 4) ^ mask),
      0);
  }

  protected void _setFloatField(int offset, float value) {
    this.segment.buffer.putFloat(this.data + offset * 4, value);
  }

  protected void _setFloatField(int offset, float value, int mask) {
    int floatAsInt = BitConverter.ToInt32(BitConverter.GetBytes(value), 0);
    this.segment.buffer.putInt(this.data + offset * 4, floatAsInt ^ mask);
  }

  protected double _getDoubleField(int offset) {
    return this.segment.buffer.getDouble(this.data + offset * 8);
  }

  protected double _getDoubleField(int offset, long mask) {
    return BitConverter.Int64BitsToDouble(this.segment.buffer.getLong(this.data + offset * 8) ^ mask);
  }

  protected void _setDoubleField(int offset, double value) {
    this.segment.buffer.putDouble(this.data + offset * 8, value);
  }

  protected void _setDoubleField(int offset, double value, long mask) {
    this.segment.buffer.putLong(this.data + offset * 8, BitConverter.DoubleToInt64Bits(value) ^ mask);
  }

  protected bool _pointerFieldIsNull(int ptrIndex) {
    return ptrIndex >= this.pointerCount
      || this.segment.buffer.getLong((this.pointers + ptrIndex) * Constants.BYTES_PER_WORD) == 0;
  }

  protected void _clearPointerField(int ptrIndex) {
    int pointer = this.pointers + ptrIndex;
    WireHelpers.zeroObject(this.segment, pointer);
    this.segment.buffer.putLong(pointer * 8, 0L);
  }

  protected T _getPointerField<T> (FromPointerBuilder<T> factory, int index) {
    return factory.fromPointerBuilder(this.segment, this.pointers + index);
  }

  protected T _getPointerField<T> (
    FromPointerBuilderRefDefault<T> factory,
    int index,
    SegmentReader defaultSegment,
    int defaultOffset) {
    return factory.fromPointerBuilderRefDefault(
      this.segment, this.pointers + index, defaultSegment, defaultOffset);
  }

  protected T _getPointerField<T> (
    FromPointerBuilderBlobDefault<T> factory,
    int index,
    ByteBuffer defaultBuffer,
    int defaultOffset,
    int defaultSize) {
    return factory.fromPointerBuilderBlobDefault(
      this.segment, this.pointers + index, defaultBuffer, defaultOffset, defaultSize);
  }

  protected T _initPointerField<T> (FromPointerBuilder<T> factory, int index, int elementCount) {
    return factory.initFromPointerBuilder(this.segment, this.pointers + index, elementCount);
  }

  protected void _setPointerField<Builder, Reader> (
    SetPointerBuilder<Builder, Reader> factory, int index, Reader value) {
    factory.setPointerBuilder(this.segment, this.pointers + index, value);
  }
}

}  // namespace Capnp

