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
using System.Collections;
using System.Collections.Generic;

namespace Capnp {

public class BuilderArena : Arena {

  public enum AllocationStrategy { FIXED_SIZE, GROW_HEURISTICALLY };

  public const Int32 SUGGESTED_FIRST_SEGMENT_WORDS = 1024;
  public const AllocationStrategy SUGGESTED_ALLOCATION_STRATEGY =
    AllocationStrategy.GROW_HEURISTICALLY;

  private readonly List<SegmentBuilder> segments;

  public Int32 nextSize;
  public readonly AllocationStrategy allocationStrategy;

  public BuilderArena(Int32 firstSegmentSizeWords, AllocationStrategy allocationStrategy) {
    this.segments = new List<SegmentBuilder>();
    this.nextSize = firstSegmentSizeWords;
    this.allocationStrategy = allocationStrategy;
    SegmentBuilder segment0 = new SegmentBuilder(
      ByteBuffer.allocate(firstSegmentSizeWords * Constants.BYTES_PER_WORD), this);
    segment0.buffer.order(ByteOrder.LITTLE_ENDIAN);
    this.segments.Add(segment0);
  }

  public SegmentReader tryGetSegment(Int32 id) {
    if (id >= segments.Count) {
      throw new DecodeException("SegmentBuilder " + id + " does not exist out of " + segments.Count + " builders");
    }
    return this.segments[id];
  }
  public SegmentBuilder getSegment(Int32 id) {
    if (id >= segments.Count) {
      throw new DecodeException("SegmentBuilder " + id + " does not exist out of " + segments.Count + " builders");
    }
    return this.segments[id];
  }

  public void checkReadLimit(Int32 numBytes) {}

  public class AllocateResult {
    public readonly SegmentBuilder segment;

    // offset to the beginning the of allocated memory
    public readonly Int32 offset;

    public AllocateResult(SegmentBuilder segment, Int32 offset) {
      this.segment = segment;
      this.offset = offset;
    }
  }

  public AllocateResult allocate(Int32 amount) {

    Int32 len = this.segments.Count;
    // we allocate the first segment in the constructor.

    Int32 result = this.segments[len - 1].allocate(amount);
    if (result != SegmentBuilder.FAILED_ALLOCATION) {
      return new AllocateResult(this.segments[len - 1], result);
    }

    // allocate_owned_memory

    Int32 size = Math.Max(amount, this.nextSize);
    SegmentBuilder newSegment =
      new SegmentBuilder(ByteBuffer.allocate(size * Constants.BYTES_PER_WORD), this);

    switch (this.allocationStrategy) {
      case AllocationStrategy.GROW_HEURISTICALLY:
        this.nextSize += size;
        break;
      default:
        break;
    }

    // --------

    newSegment.buffer.order(ByteOrder.LITTLE_ENDIAN);
    newSegment.id = len;
    this.segments.Add(newSegment);

    return new AllocateResult(newSegment, newSegment.allocate(amount));
  }

  public ByteBuffer[] getSegmentsForOutput() {
    ByteBuffer[] result = new ByteBuffer[this.segments.Count];
    for (Int32 ii = 0; ii < this.segments.Count; ++ii) {
      SegmentBuilder segment = segments[ii];
      segment.buffer.rewind();
      ByteBuffer slice = segment.buffer.slice();
      slice.limit(segment.currentSize() * Constants.BYTES_PER_WORD);
      slice.order(ByteOrder.LITTLE_ENDIAN);
      result[ii] = slice;
    }
    return result;
  }
}

}  // namespace Capnp

