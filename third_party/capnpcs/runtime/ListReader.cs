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

public class ListReader {
  public interface Factory<T> {
    T constructReader(
      SegmentReader segment,
      int ptr,
      int elementCount,
      int step,
      int structDataSize,
      short structPointerCount,
      int nestingLimit);
  }

  public readonly SegmentReader segment;
  public readonly int ptr;  // byte offset to front of list
  public readonly int elementCount;
  public readonly int step;            // in bits
  public readonly int structDataSize;  // in bits
  public readonly short structPointerCount;
  public readonly int nestingLimit;

  public ListReader() {
    this.segment = null;
    this.ptr = 0;
    this.elementCount = 0;
    this.step = 0;
    this.structDataSize = 0;
    this.structPointerCount = 0;
    this.nestingLimit = 0x7fffffff;
  }

  public ListReader(
    SegmentReader segment,
    int ptr,
    int elementCount,
    int step,
    int structDataSize,
    short structPointerCount,
    int nestingLimit) {
    this.segment = segment;
    this.ptr = ptr;
    this.elementCount = elementCount;
    this.step = step;
    this.structDataSize = structDataSize;
    this.structPointerCount = structPointerCount;
    this.nestingLimit = nestingLimit;
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

  protected T _getStructElement<T> (StructReader.Factory<T> factory, int index) {
    // TODO check nesting limit

    long indexBit = (long)index * this.step;
    int structData = this.ptr + (int)(indexBit / Constants.BITS_PER_BYTE);
    int structPointers = structData + (this.structDataSize / Constants.BITS_PER_BYTE);

    return factory.constructReader(
      this.segment,
      structData,
      structPointers / 8,
      this.structDataSize,
      this.structPointerCount,
      this.nestingLimit - 1);
  }

  protected T _getPointerElement<T> (FromPointerReader<T> factory, int index) {
    return factory.fromPointerReader(
      this.segment,
      (this.ptr + (int)((long)index * this.step / Constants.BITS_PER_BYTE))
        / Constants.BYTES_PER_WORD,
      this.nestingLimit);
  }

  protected T _getPointerElement<T> (
    FromPointerReaderBlobDefault<T> factory,
    int index,
    ByteBuffer defaultBuffer,
    int defaultOffset,
    int defaultSize) {
    return factory.fromPointerReaderBlobDefault(
      this.segment,
      (this.ptr + (int)((long)index * this.step / Constants.BITS_PER_BYTE))
        / Constants.BYTES_PER_WORD,
      defaultBuffer,
      defaultOffset,
      defaultSize);
  }
}

}  // namespace Capnp

