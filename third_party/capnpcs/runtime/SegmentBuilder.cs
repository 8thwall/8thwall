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

public class SegmentBuilder : SegmentReader {

  public const Int32 FAILED_ALLOCATION = -1;

  public Int32 pos = 0;  // in words
  public Int32 id = 0;

  public SegmentBuilder(ByteBuffer buf, Arena arena) : base(buf, arena) { }

  // the total number of words the buffer can hold
  private Int32 capacity() {
    this.buffer.rewind();
    return this.buffer.remaining() / 8;
  }

  // return how many words have already been allocated
  public Int32 currentSize() { return this.pos; }

  /*
     Allocate `amount` words.
   */
  public Int32 allocate(Int32 amount) {
    if (amount < 0) {
      throw new ApplicationException("tried to allocate a negative number of words");
    }

    if (amount > this.capacity() - this.currentSize()) {
      return FAILED_ALLOCATION;  // no space left;
    } else {
      Int32 result = this.pos;
      this.pos += amount;
      return result;
    }
  }

  public BuilderArena getArena() { return (BuilderArena)this.arena; }

  public bool isWritable() {
    // TODO support external non-writable segments
    return true;
  }

  public void put(Int32 index, Int64 value) {
    buffer.putLong(index * Constants.BYTES_PER_WORD, value);
  }
}

}  // namespace Capnp

