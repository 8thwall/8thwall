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
using System.Globalization;

namespace Capnp {

public class EnumList {
  public static T clampOrdinal<T>(T[] values, short ordinal) {
    int index = ordinal;
    if (ordinal < 0 || ordinal >= values.Length) {
      index = values.Length - 1;
    }
    return values[index];
  }

  // Csharp doesn't allow Enum as a type constraint, so we use all of Enum's interface constraints
  //  as a proxy.
  public class Factory<T>
    : ListFactory<Builder<T>, Reader<T>> where T : IComparable, IConvertible, IFormattable {
    public T[] values;

    public Factory(Array values) : base(ElementSize.TWO_BYTES) {
      this.values = new T[values.Length];
      for (int i = 0; i < values.Length; ++i) {
        this.values[i] = (T)values.GetValue(i);
      }
    }

    public override Reader<T> constructReader(
      SegmentReader segment,
      int ptr,
      int elementCount,
      int step,
      int structDataSize,
      short structPointerCount,
      int nestingLimit) {
      return new Reader<T>(
        values, segment, ptr, elementCount, step, structDataSize, structPointerCount, nestingLimit);
    }

    public override Builder<T> constructBuilder(
      SegmentBuilder segment,
      int ptr,
      int elementCount,
      int step,
      int structDataSize,
      short structPointerCount) {
      return new Builder<T>(
        values, segment, ptr, elementCount, step, structDataSize, structPointerCount);
    }
  }

  public class Reader<T> : ListReader where T : IComparable, IConvertible, IFormattable {
    public readonly T[] values;

    public Reader(
      T[] values,
      SegmentReader segment,
      int ptr,
      int elementCount,
      int step,
      int structDataSize,
      short structPointerCount,
      int nestingLimit)
      : base(segment, ptr, elementCount, step, structDataSize, structPointerCount, nestingLimit) {
      this.values = values;
    }

    public T get(int index) { return clampOrdinal(this.values, _getShortElement(index)); }
  }

  public class Builder<T> : ListBuilder where T : IComparable, IConvertible, IFormattable {
    public readonly T[] values;

    public Builder(
      T[] values,
      SegmentBuilder segment,
      int ptr,
      int elementCount,
      int step,
      int structDataSize,
      short structPointerCount)
      : base(segment, ptr, elementCount, step, structDataSize, structPointerCount) {
      this.values = values;
    }

    public T get(int index) { return clampOrdinal(this.values, _getShortElement(index)); }

    public void set(int index, T value) {
      _setShortElement(index, value.ToInt16(CultureInfo.CurrentCulture));
    }
  }
}

}  // namespace Capnp

