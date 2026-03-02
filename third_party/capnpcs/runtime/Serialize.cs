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
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace Capnp {

public class Serialize {

  static ByteBuffer makeByteBuffer(int bytes) {
    ByteBuffer result = ByteBuffer.allocate(bytes);
    result.order(ByteOrder.LITTLE_ENDIAN);
    // result.mark();
    return result;
  }

  public static void fillBuffer(ByteBuffer buffer, ReadableByteChannel bc) {
    while (buffer.hasRemaining()) {
      int r = bc.read(buffer);
      if (r < 0) {
        throw new ApplicationException("premature EOF");
      }
      // TODO check for r == 0 ?.
    }
  }

  public static MessageReader read(ReadableByteChannel bc) {
    return read(bc, ReaderOptions.DEFAULT_READER_OPTIONS);
  }

  public static MessageReader read(ReadableByteChannel bc, ReaderOptions options) {
    ByteBuffer firstWord = makeByteBuffer(Constants.BYTES_PER_WORD);
    fillBuffer(firstWord, bc);

    int segmentCount = 1 + firstWord.getInt(0);

    int segment0Size = 0;
    if (segmentCount > 0) {
      segment0Size = firstWord.getInt(4);
    }

    int totalWords = segment0Size;

    if (segmentCount > 512) {
      throw new ApplicationException("too many segments");
    }

    // in words
    List<Int32> moreSizes = new List<Int32>();

    if (segmentCount > 1) {
      ByteBuffer moreSizesRaw = makeByteBuffer(4 * (segmentCount & ~1));
      fillBuffer(moreSizesRaw, bc);
      for (int ii = 0; ii < segmentCount - 1; ++ii) {
        int size = moreSizesRaw.getInt(ii * 4);
        moreSizes.Add(size);
        totalWords += size;
      }
    }

    if (totalWords > options.traversalLimitInWords) {
      throw new DecodeException("Message size exceeds traversal limit.");
    }

    ByteBuffer allSegments = makeByteBuffer(totalWords * Constants.BYTES_PER_WORD);
    fillBuffer(allSegments, bc);

    ByteBuffer[] segmentSlices = new ByteBuffer[segmentCount];

    allSegments.rewind();
    segmentSlices[0] = allSegments.slice();
    segmentSlices[0].limit(segment0Size * Constants.BYTES_PER_WORD);
    segmentSlices[0].order(ByteOrder.LITTLE_ENDIAN);

    int offset = segment0Size;
    for (int ii = 1; ii < segmentCount; ++ii) {
      allSegments.position(offset * Constants.BYTES_PER_WORD);
      segmentSlices[ii] = allSegments.slice();
      segmentSlices[ii].limit(moreSizes[ii - 1] * Constants.BYTES_PER_WORD);
      segmentSlices[ii].order(ByteOrder.LITTLE_ENDIAN);
      offset += moreSizes[ii - 1];
    }

    return new MessageReader(segmentSlices, options);
  }

  public static MessageReader read(NativeByteArray bb) {
    return read(ByteBuffer.copyOf(bb));
  }

  public static MessageReader read(byte[] bb) {
    return read(ByteBuffer.wrap(bb));
  }

  public static MessageReader read(sbyte[] bb) {
    return read(ByteBuffer.wrap(bb));
  }

  public static MessageReader read(ByteBuffer bb) {
    return read(bb, ReaderOptions.DEFAULT_READER_OPTIONS);
  }

  /*
   * Upon return, `bb.position()` will be at the end of the message.
   */
  public static MessageReader read(ByteBuffer bb, ReaderOptions options) {
    bb.order(ByteOrder.LITTLE_ENDIAN);

    int segmentCount = 1 + bb.getInt();
    if (segmentCount > 512) {
      throw new ApplicationException("too many segments");
    }

    ByteBuffer[] segmentSlices = new ByteBuffer[segmentCount];

    int segmentSizesBase = bb.position();
    int segmentSizesSize = segmentCount * 4;

    int align = Constants.BYTES_PER_WORD - 1;
    int segmentBase = (segmentSizesBase + segmentSizesSize + align) & ~align;

    int totalWords = 0;

    for (int ii = 0; ii < segmentCount; ++ii) {
      int segmentSize = bb.getInt(segmentSizesBase + ii * 4);

      bb.position(segmentBase + totalWords * Constants.BYTES_PER_WORD);
      segmentSlices[ii] = bb.slice();
      segmentSlices[ii].limit(segmentSize * Constants.BYTES_PER_WORD);
      segmentSlices[ii].order(ByteOrder.LITTLE_ENDIAN);

      totalWords += segmentSize;
    }
    bb.position(segmentBase + totalWords * Constants.BYTES_PER_WORD);

    if (totalWords > options.traversalLimitInWords) {
      throw new DecodeException("Message size exceeds traversal limit.");
    }

    return new MessageReader(segmentSlices, options);
  }

  public static byte[] writeBytes(MessageBuilder message) {
    int size = (int)(Constants.BYTES_PER_WORD * computeSerializedSizeInWords(message));
    ArrayOutputStream outStream = new ArrayOutputStream(size);
    write(outStream, message);
    return outStream.getWriteBuffer().array();
  }

  public static sbyte[] writeSBytes(MessageBuilder message) {
    int size = (int)(Constants.BYTES_PER_WORD * computeSerializedSizeInWords(message));
    ArrayOutputStream outStream = new ArrayOutputStream(size, true);
    write(outStream, message);
    return (sbyte[]) ((Array) outStream.getWriteBuffer().array());
  }

  public static NativeByteArray allocNativeByteArrayAndWrite(MessageBuilder message) {
    int size = (int)(Constants.BYTES_PER_WORD * computeSerializedSizeInWords(message));
    ArrayOutputStream outStream = new ArrayOutputStream(size);
    write(outStream, message);

    NativeByteArray ret = new NativeByteArray();
    ret.size = size;
    ret.bytes = Marshal.AllocHGlobal(size);
    Marshal.Copy(outStream.getWriteBuffer().array(), 0, ret.bytes, ret.size);
    return ret;
  }

  public static void freeAllocedNativeByteArray(NativeByteArray arr) {
    Marshal.FreeHGlobal(arr.bytes);
  }

  public static long computeSerializedSizeInWords(MessageBuilder message) {
    ByteBuffer[] segments = message.getSegmentsForOutput();

    // From the capnproto documentation:
    // "When transmitting over a stream, the following should be sent..."
    long bytes = 0;
    // "(4 bytes) The number of segments, minus one..."
    bytes += 4;
    // "(N * 4 bytes) The size of each segment, in words."
    bytes += segments.Length * 4;
    // "(0 or 4 bytes) Padding up to the next word boundary."
    if (bytes % 8 != 0) {
      bytes += 4;
    }

    // The content of each segment, in order.
    for (int i = 0; i < segments.Length; ++i) {
      ByteBuffer s = segments[i];
      bytes += s.limit();
    }

    return bytes / Constants.BYTES_PER_WORD;
  }

  public static void write(WritableByteChannel outputChannel, MessageBuilder message) {
    ByteBuffer[] segments = message.getSegmentsForOutput();
    int tableSize = (segments.Length + 2) & (~1);

    ByteBuffer table = ByteBuffer.allocate(4 * tableSize);
    table.order(ByteOrder.LITTLE_ENDIAN);

    table.putInt(0, segments.Length - 1);

    for (int i = 0; i < segments.Length; ++i) {
      table.putInt(4 * (i + 1), segments[i].limit() / 8);
    }

    // Any padding is already zeroed.
    while (table.hasRemaining()) {
      outputChannel.write(table);
    }

    foreach (ByteBuffer buffer in segments) {
      while (buffer.hasRemaining()) {
        outputChannel.write(buffer);
      }
    }
  }
}

}  // namespace Capnp

