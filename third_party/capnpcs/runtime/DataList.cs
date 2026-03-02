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

public class DataList {
  public class Factory : ListFactory<Builder, Reader> {
    public Factory() : base(ElementSize.POINTER) { }

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

  public class Reader : ListReader, IEnumerable<Data.Reader> {
    public Reader(
      SegmentReader segment,
      int ptr,
      int elementCount,
      int step,
      int structDataSize,
      short structPointerCount,
      int nestingLimit)
      : base(segment, ptr, elementCount, step, structDataSize, structPointerCount, nestingLimit){ }

    public Data.Reader get(int index) { return _getPointerElement(Data.factory, index); }

    public IEnumerator<Data.Reader> GetEnumerator() {
      for (int idx = 0; idx < this.size(); ++idx) {
        yield return this._getPointerElement(Data.factory, idx);
      }
    }

    IEnumerator IEnumerable.GetEnumerator() {
      return GetEnumerator();
    }
  }

  public class Builder : ListBuilder, IEnumerable<Data.Builder> {

    public Builder(
      SegmentBuilder segment,
      int ptr,
      int elementCount,
      int step,
      int structDataSize,
      short structPointerCount)
      : base(segment, ptr, elementCount, step, structDataSize, structPointerCount) { }

    public Data.Builder get(int index) { return _getPointerElement(Data.factory, index); }

    public void set(int index, Data.Reader value) {
      _setPointerElement(Data.factory, index, value);
    }

    public IEnumerator<Data.Builder> GetEnumerator() {
      for (int idx = 0; idx < this.size(); ++idx) {
        yield return this._getPointerElement(Data.factory, idx);
      }
    }

    IEnumerator IEnumerable.GetEnumerator() {
      return GetEnumerator();
    }
  }
}

}  // namespace Capnp

