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

public class ListBuilder {
  public interface Factory<T> {
    T constructBuilder(
      SegmentBuilder segment,
      int ptr,
      int elementCount,
      int step,
      int structDataSize,
      short structPointerCount);
  }

  readonly SegmentBuilder segment;
  readonly int ptr;  // byte offset to front of list
  readonly int elementCount;
  readonly int step;            // in bits
  readonly int structDataSize;  // in bits
  readonly short structPointerCount;

  public ListBuilder(
    SegmentBuilder segment,
    int ptr,
    int elementCount,
    int step,
    int structDataSize,
    short structPointerCount) {
    this.segment = segment;
    this.ptr = ptr;
    this.elementCount = elementCount;
    this.step = step;
    this.structDataSize = structDataSize;
    this.structPointerCount = structPointerCount;
  }

  public int size() { return this.elementCount; }

  protected bool _getBooleanElement(int index) {
    long bindex = (long)index * this.step;
    byte b = this.segment.buffer.get(this.ptr + (int)(bindex / Constants.BITS_PER_BYTE));
    return (b & (1 << ((int)(bindex % 8)))) != 0;
  }

  protected byte _getByteElement(int index) {
    return this.segment.buffer.get(
      this.ptr + (int)((long)index * this.step / Constants.BITS_PER_BYTE));
  }

  protected short _getShortElement(int index) {
    return this.segment.buffer.getShort(
      this.ptr + (int)((long)index * this.step / Constants.BITS_PER_BYTE));
  }

  protected int _getIntElement(int index) {
    return this.segment.buffer.getInt(
      this.ptr + (int)((long)index * this.step / Constants.BITS_PER_BYTE));
  }

  protected long _getLongElement(int index) {
    return this.segment.buffer.getLong(
      this.ptr + (int)((long)index * this.step / Constants.BITS_PER_BYTE));
  }

  protected float _getFloatElement(int index) {
    return this.segment.buffer.getFloat(
      this.ptr + (int)((long)index * this.step / Constants.BITS_PER_BYTE));
  }

  protected double _getDoubleElement(int index) {
    return this.segment.buffer.getDouble(
      this.ptr + (int)((long)index * this.step / Constants.BITS_PER_BYTE));
  }

  protected void _setBooleanElement(int index, bool value) {
    long bitOffset = index * this.step;
    byte bitnum = (byte)(bitOffset % 8);
    int position = (int)(this.ptr + (bitOffset / 8));
    byte oldValue = this.segment.buffer.get(position);
    this.segment.buffer.put(
      position, (byte)((oldValue & (~(1 << bitnum))) | ((value ? 1 : 0) << bitnum)));
  }

  protected void _setByteElement(int index, byte value) {
    this.segment.buffer.put(
      this.ptr + (int)((long)index * this.step / Constants.BITS_PER_BYTE), value);
  }

  protected void _setShortElement(int index, short value) {
    this.segment.buffer.putShort(
      this.ptr + (int)((long)index * this.step / Constants.BITS_PER_BYTE), value);
  }

  protected void _setIntElement(int index, int value) {
    this.segment.buffer.putInt(
      this.ptr + (int)((long)index * this.step / Constants.BITS_PER_BYTE), value);
  }

  protected void _setLongElement(int index, long value) {
    this.segment.buffer.putLong(
      this.ptr + (int)((long)index * this.step / Constants.BITS_PER_BYTE), value);
  }

  protected void _setFloatElement(int index, float value) {
    this.segment.buffer.putFloat(
      this.ptr + (int)((long)index * this.step / Constants.BITS_PER_BYTE), value);
  }

  protected void _setDoubleElement(int index, double value) {
    this.segment.buffer.putDouble(
      this.ptr + (int)((long)index * this.step / Constants.BITS_PER_BYTE), value);
  }

  protected T _getStructElement<T> (StructBuilder.Factory<T> factory, int index) {
    long indexBit = (long)index * this.step;
    int structData = this.ptr + (int)(indexBit / Constants.BITS_PER_BYTE);
    int structPointers = (structData + (this.structDataSize / 8)) / 8;

    return factory.constructBuilder(
      this.segment, structData, structPointers, this.structDataSize, this.structPointerCount);
  }

  protected T _getPointerElement<T> (FromPointerBuilder<T> factory, int index) {
    return factory.fromPointerBuilder(
      this.segment,
      (this.ptr + (int)((long)index * this.step / Constants.BITS_PER_BYTE))
        / Constants.BYTES_PER_WORD);
  }

  protected T
  _initPointerElement<T>(FromPointerBuilder<T> factory, int index, int elementCount) {
    return factory.initFromPointerBuilder(
      this.segment,
      (this.ptr + (int)((long)index * this.step / Constants.BITS_PER_BYTE))
        / Constants.BYTES_PER_WORD,
      elementCount);
  }

  protected void _setPointerElement<Builder, Reader> (
    SetPointerBuilder<Builder, Reader> factory, int index, Reader value) {
    factory.setPointerBuilder(
      this.segment,
      (this.ptr + (int)((long)index * this.step / Constants.BITS_PER_BYTE))
        / Constants.BYTES_PER_WORD,
      value);
  }
}

}  // namespace Capnp

