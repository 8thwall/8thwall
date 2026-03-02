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

using System.Collections;
using System.Collections.Generic;

namespace Capnp {

public class StructList {
  public class Factory<ElementBuilder, ElementReader>
    : ListFactory<Builder<ElementBuilder>, Reader<ElementReader>>
    where ElementBuilder : StructBuilder
    where ElementReader : StructReader {

    public StructFactory<ElementBuilder, ElementReader> factory;

    public Factory(StructFactory<ElementBuilder, ElementReader> factory)
      : base(ElementSize.INLINE_COMPOSITE) {
      this.factory = factory;
    }

    public override Reader<ElementReader> constructReader(
      SegmentReader segment,
      int ptr,
      int elementCount,
      int step,
      int structDataSize,
      short structPointerCount,
      int nestingLimit) {
      return new Reader<ElementReader>(
        factory,
        segment,
        ptr,
        elementCount,
        step,
        structDataSize,
        structPointerCount,
        nestingLimit);
    }

    public override Builder<ElementBuilder> constructBuilder(
      SegmentBuilder segment,
      int ptr,
      int elementCount,
      int step,
      int structDataSize,
      short structPointerCount) {
      return new Builder<ElementBuilder>(
        factory, segment, ptr, elementCount, step, structDataSize, structPointerCount);
    }

    public override Builder<ElementBuilder> fromPointerBuilderRefDefault(
      SegmentBuilder segment, int pointer, SegmentReader defaultSegment, int defaultOffset) {
      return WireHelpers.getWritableStructListPointer(
        this, pointer, segment, factory.structSize(), defaultSegment, defaultOffset);
    }

    public override Builder<ElementBuilder> fromPointerBuilder(
      SegmentBuilder segment, int pointer) {
      return WireHelpers.getWritableStructListPointer(
        this, pointer, segment, factory.structSize(), null, 0);
    }

    public override Builder<ElementBuilder> initFromPointerBuilder(
      SegmentBuilder segment, int pointer, int elementCount) {
      return WireHelpers.initStructListPointer(
        this, pointer, segment, elementCount, factory.structSize());
    }
  }

  public class Reader<T> : ListReader, IEnumerable<T> {
    public readonly StructReader.Factory<T> factory;

    public Reader(
      StructReader.Factory<T> factory,
      SegmentReader segment,
      int ptr,
      int elementCount,
      int step,
      int structDataSize,
      short structPointerCount,
      int nestingLimit)
      : base(segment, ptr, elementCount, step, structDataSize, structPointerCount, nestingLimit) {      ;
      this.factory = factory;
    }

    public T get(int index) { return _getStructElement(factory, index); }

    public IEnumerator<T> GetEnumerator() {
      for (int idx = 0; idx < this.size(); ++idx) {
        yield return _getStructElement(factory, idx);
      }
    }

    IEnumerator IEnumerable.GetEnumerator() {
      return GetEnumerator();
    }
  }

  public class Builder<T> : ListBuilder, IEnumerable<T> {
    public readonly StructBuilder.Factory<T> factory;

    public Builder(
      StructBuilder.Factory<T> factory,
      SegmentBuilder segment,
      int ptr,
      int elementCount,
      int step,
      int structDataSize,
      short structPointerCount)
      : base(segment, ptr, elementCount, step, structDataSize, structPointerCount) {
      this.factory = factory;
    }

    public T get(int index) { return _getStructElement(factory, index); }

    public IEnumerator<T> GetEnumerator() {
      for (int idx = 0; idx < this.size(); ++idx) {
        yield return _getStructElement(factory, idx);
      }
    }

    IEnumerator IEnumerable.GetEnumerator() {
      return GetEnumerator();
    }
  }
}


}  // namespace Capnp

