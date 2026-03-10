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

class WireHelpers {

  public static Int64 logicalShift64(Int64 val, Int32 bytes) {
    return (Int64)(((UInt64)val) >> bytes);
  }

  public static Int32 logicalShift32(Int32 val, Int32 bytes) {
    return (Int32)(((UInt32)val) >> bytes);
  }

  public static Int32 roundBytesUpToWords(Int32 bytes) { return (bytes + 7) / 8; }

  public static Int32 roundBitsUpToBytes(Int32 bits) { return (bits + 7) / Constants.BITS_PER_BYTE; }

  public static Int32 roundBitsUpToWords(Int64 bits) {
    //# This code assumes 64-bit words.
    return (int)((bits + 63) / ((Int64)Constants.BITS_PER_WORD));
  }

  public class AllocateResult {
    public readonly Int32 ptr;
    public readonly Int32 rfOffset;
    public readonly SegmentBuilder segment;
    public AllocateResult(Int32 ptr, Int32 rfOffset, SegmentBuilder segment) {
      this.ptr = ptr;
      this.rfOffset = rfOffset;
      this.segment = segment;
    }
  }

  public static AllocateResult allocate(
    Int32 rfOffset,
    SegmentBuilder segment,
    Int32 amount,  // in words
    byte kind) {

    Int64 rf = segment.get(rfOffset);
    if (!WirePointer.isNull(rf)) {
      zeroObject(segment, rfOffset);
    }

    if (amount == 0 && kind == WirePointer.STRUCT) {
      WirePointer.setKindAndTargetForEmptyStruct(segment.buffer, rfOffset);
      return new AllocateResult(rfOffset, rfOffset, segment);
    }

    Int32 ptr = segment.allocate(amount);
    if (ptr == SegmentBuilder.FAILED_ALLOCATION) {
      //# Need to allocate in a new segment. We'll need to
      //# allocate an extra pointer worth of space to act as
      //# the landing pad for a far pointer.

      Int32 amountPlusRef = amount + Constants.POINTER_SIZE_IN_WORDS;
      BuilderArena.AllocateResult allocation = segment.getArena().allocate(amountPlusRef);

      //# Set up the original pointer to be a far pointer to
      //# the new segment.
      FarPointer.set(segment.buffer, rfOffset, false, allocation.offset);
      FarPointer.setSegmentId(segment.buffer, rfOffset, allocation.segment.id);

      //# Initialize the landing pad to indicate that the
      //# data immediately follows the pad.
      Int32 resultRefOffset = allocation.offset;
      Int32 ptr1 = allocation.offset + Constants.POINTER_SIZE_IN_WORDS;

      WirePointer.setKindAndTarget(allocation.segment.buffer, resultRefOffset, kind, ptr1);

      return new AllocateResult(ptr1, resultRefOffset, allocation.segment);
    } else {
      WirePointer.setKindAndTarget(segment.buffer, rfOffset, kind, ptr);
      return new AllocateResult(ptr, rfOffset, segment);
    }
  }

  public class FollowBuilderFarsResult {
    public readonly Int32 ptr;
    public readonly Int64 rf;
    public readonly SegmentBuilder segment;
    public FollowBuilderFarsResult(Int32 ptr, Int64 rf, SegmentBuilder segment) {
      this.ptr = ptr;
      this.rf = rf;
      this.segment = segment;
    }
  }

  public static FollowBuilderFarsResult followBuilderFars(
    Int64 rf, Int32 rfTarget, SegmentBuilder segment) {
    //# If `rf` is a far pointer, follow it. On return, `rf` will
    //# have been updated to poInt32 at a WirePointer that contains
    //# the type information about the target object, and a pointer
    //# to the object contents is returned. The caller must NOT use
    //# `rf->target()` as this may or may not actually return a
    //# valid pointer. `segment` is also updated to poInt32 at the
    //# segment which actually contains the object.
    //#
    //# If `rf` is not a far pointer, this simply returns
    //# `rfTarget`. Usually, `rfTarget` should be the same as
    //# `rf->target()`, but may not be in cases where `rf` is
    //# only a tag.

    if (WirePointer.kind(rf) == WirePointer.FAR) {
      SegmentBuilder resultSegment = segment.getArena().getSegment(FarPointer.getSegmentId(rf));

      Int32 padOffset = FarPointer.positionInSegment(rf);
      Int64 pad = resultSegment.get(padOffset);
      if (!FarPointer.isDoubleFar(rf)) {
        return new FollowBuilderFarsResult(WirePointer.target(padOffset, pad), pad, resultSegment);
      }

      //# Landing pad is another far pointer. It is followed by a
      //# tag describing the pointed-to object.
      Int32 rfOffset = padOffset + 1;
      rf = resultSegment.get(rfOffset);

      resultSegment = resultSegment.getArena().getSegment(FarPointer.getSegmentId(pad));
      return new FollowBuilderFarsResult(FarPointer.positionInSegment(pad), rf, resultSegment);
    } else {
      return new FollowBuilderFarsResult(rfTarget, rf, segment);
    }
  }

  public class FollowFarsResult {
    public readonly Int32 ptr;
    public readonly Int64 rf;
    public readonly SegmentReader segment;
    public FollowFarsResult(Int32 ptr, Int64 rf, SegmentReader segment) {
      this.ptr = ptr;
      this.rf = rf;
      this.segment = segment;
    }
  }

  public static FollowFarsResult followFars(Int64 rf, Int32 rfTarget, SegmentReader segment) {
    //# If the segment is null, this is an unchecked message,
    //# so there are no FAR pointers.
    if (segment != null && WirePointer.kind(rf) == WirePointer.FAR) {
      SegmentReader resultSegment = segment.arena.tryGetSegment(FarPointer.getSegmentId(rf));
      Int32 padOffset = FarPointer.positionInSegment(rf);
      Int64 pad = resultSegment.get(padOffset);

      // Int32 padWords = FarPointer.isDoubleFar(rf) ? 2 : 1;
      // TODO read limiting

      if (!FarPointer.isDoubleFar(rf)) {

        return new FollowFarsResult(WirePointer.target(padOffset, pad), pad, resultSegment);
      } else {
        //# Landing pad is another far pointer. It is
        //# followed by a tag describing the pointed-to
        //# object.

        Int64 tag = resultSegment.get(padOffset + 1);
        resultSegment = resultSegment.arena.tryGetSegment(FarPointer.getSegmentId(pad));
        return new FollowFarsResult(FarPointer.positionInSegment(pad), tag, resultSegment);
      }
    } else {
      return new FollowFarsResult(rfTarget, rf, segment);
    }
  }

  public static void zeroObject(SegmentBuilder segment, Int32 rfOffset) {
    //# Zero out the pointed-to object. Use when the pointer is
    //# about to be overwritten making the target object no Int64er
    //# reachable.

    //# We shouldn't zero out external data linked into the message.
    if (!segment.isWritable())
      return;

    Int64 rf = segment.get(rfOffset);

    switch (WirePointer.kind(rf)) {
      case WirePointer.STRUCT:
      case WirePointer.LIST:
        zeroObject(segment, rf, WirePointer.target(rfOffset, rf));
        break;
      case WirePointer.FAR: {
        segment = segment.getArena().getSegment(FarPointer.getSegmentId(rf));
        if (segment.isWritable()) {  //# Don't zero external data.
          Int32 padOffset = FarPointer.positionInSegment(rf);
          Int64 pad = segment.get(padOffset);
          if (FarPointer.isDoubleFar(rf)) {
            SegmentBuilder otherSegment =
              segment.getArena().getSegment(FarPointer.getSegmentId(rf));
            if (otherSegment.isWritable()) {
              zeroObject(otherSegment, padOffset + 1, FarPointer.positionInSegment(pad));
            }
            segment.buffer.putLong(padOffset * 8, 0L);
            segment.buffer.putLong((padOffset + 1) * 8, 0L);

          } else {
            zeroObject(segment, padOffset);
            segment.buffer.putLong(padOffset * 8, 0L);
          }
        }

        break;
      }
      case WirePointer.OTHER: {
        // TODO
        break;
      }
      default: {
        throw new ApplicationException(
          "Bad WirePointer kind " + WirePointer.kind(rf) + " for ref " + rf);
      }
    }
  }

  public static void zeroObject(SegmentBuilder segment, Int64 tag, Int32 ptr) {
    //# We shouldn't zero out external data linked into the message.
    if (!segment.isWritable())
      return;

    switch (WirePointer.kind(tag)) {
      case WirePointer.STRUCT: {
        Int32 pointerSection = ptr + StructPointer.dataSize(tag);
        Int32 count = StructPointer.ptrCount(tag);
        for (Int32 ii = 0; ii < count; ++ii) {
          zeroObject(segment, pointerSection + ii);
        }
        memset(
          segment.buffer,
          ptr * Constants.BYTES_PER_WORD,
          (byte)0,
          StructPointer.wordSize(tag) * Constants.BYTES_PER_WORD);
        break;
      }
      case WirePointer.LIST: {
        switch (ListPointer.elementSize(tag)) {
          case ElementSize.VOID:
            break;
          case ElementSize.BIT:
          case ElementSize.BYTE:
          case ElementSize.TWO_BYTES:
          case ElementSize.FOUR_BYTES:
          case ElementSize.EIGHT_BYTES: {
            memset(
              segment.buffer,
              ptr * Constants.BYTES_PER_WORD,
              (byte)0,
              roundBitsUpToWords(
                ListPointer.elementCount(tag)
                * ElementSize.dataBitsPerElement(ListPointer.elementSize(tag)))
                * Constants.BYTES_PER_WORD);
            break;
          }
          case ElementSize.POINTER: {
            Int32 count = ListPointer.elementCount(tag);
            for (Int32 ii = 0; ii < count; ++ii) {
              zeroObject(segment, ptr + ii);
            }
            memset(
              segment.buffer,
              ptr * Constants.BYTES_PER_WORD,
              (byte)0,
              count * Constants.BYTES_PER_WORD);
            break;
          }
          case ElementSize.INLINE_COMPOSITE: {
            Int64 elementTag = segment.get(ptr);
            if (WirePointer.kind(elementTag) != WirePointer.STRUCT) {
              throw new ApplicationException("Don't know how to handle non-STRUCT inline composite.");
            }
            Int32 dataSize = StructPointer.dataSize(elementTag);
            Int32 pointerCount = StructPointer.ptrCount(elementTag);

            Int32 pos = ptr + Constants.POINTER_SIZE_IN_WORDS;
            Int32 count = WirePointer.inlineCompositeListElementCount(elementTag);
            for (Int32 ii = 0; ii < count; ++ii) {
              pos += dataSize;
              for (Int32 jj = 0; jj < pointerCount; ++jj) {
                zeroObject(segment, pos);
                pos += Constants.POINTER_SIZE_IN_WORDS;
              }
            }

            memset(
              segment.buffer,
              ptr * Constants.BYTES_PER_WORD,
              (byte)0,
              (StructPointer.wordSize(elementTag) * count + Constants.POINTER_SIZE_IN_WORDS)
                * Constants.BYTES_PER_WORD);
            break;
          }
          default: {
            throw new ApplicationException(
              "Bad element size " + ListPointer.elementSize(tag) + " for tag " + tag);
          }
        }
        break;
      }
      case WirePointer.FAR:
        throw new ApplicationException("Unexpected FAR pointer.");
      case WirePointer.OTHER:
        throw new ApplicationException("Unexpected OTHER pointer.");
    }
  }

  public static void zeroPointerAndFars(SegmentBuilder segment, Int32 rfOffset) {
    //# Zero out the pointer itself and, if it is a far pointer, zero the landing pad as well,
    //# but do not zero the object body. Used when upgrading.

    Int64 rf = segment.get(rfOffset);
    if (WirePointer.kind(rf) == WirePointer.FAR) {
      SegmentBuilder padSegment = segment.getArena().getSegment(FarPointer.getSegmentId(rf));
      if (padSegment.isWritable()) {  //# Don't zero external data.
        Int32 padOffset = FarPointer.positionInSegment(rf);
        padSegment.buffer.putLong(padOffset * Constants.BYTES_PER_WORD, 0L);
        if (FarPointer.isDoubleFar(rf)) {
          padSegment.buffer.putLong(padOffset * Constants.BYTES_PER_WORD + 1, 0L);
        }
      }
    }
    segment.put(rfOffset, 0L);
  }

  public static void transferPointer(
    SegmentBuilder dstSegment, Int32 dstOffset, SegmentBuilder srcSegment, Int32 srcOffset) {
    //# Make *dst poInt32 to the same object as *src. Both must reside in the same message, but can
    //# be in different segments.
    //#
    //# Caller MUST zero out the source pointer after calling this, to make sure no later code
    //# mistakenly thinks the source location still owns the object.  transferPointer() doesn't do
    //# this zeroing itself because many callers transfer several pointers in a loop then zero out
    //# the whole section.

    Int64 src = srcSegment.get(srcOffset);
    if (WirePointer.isNull(src)) {
      dstSegment.put(dstOffset, 0L);
    } else if (WirePointer.kind(src) == WirePointer.FAR) {
      //# Far pointers are position-independent, so we can just copy.
      dstSegment.put(dstOffset, srcSegment.get(srcOffset));
    } else {
      transferPointer(
        dstSegment, dstOffset, srcSegment, srcOffset, WirePointer.target(srcOffset, src));
    }
  }

  public static void transferPointer(
    SegmentBuilder dstSegment,
    Int32 dstOffset,
    SegmentBuilder srcSegment,
    Int32 srcOffset,
    Int32 srcTargetOffset) {
    //# Like the other overload, but splits src into a tag and a target. Particularly useful for
    //# OrphanBuilder.

    Int64 src = srcSegment.get(srcOffset);
    Int64 srcTarget = srcSegment.get(srcTargetOffset);

    if (dstSegment == srcSegment) {
      //# Same segment, so create a direct pointer.

      if (WirePointer.kind(src) == WirePointer.STRUCT && StructPointer.wordSize(src) == 0) {
        WirePointer.setKindAndTargetForEmptyStruct(dstSegment.buffer, dstOffset);
      } else {
        WirePointer.setKindAndTarget(
          dstSegment.buffer, dstOffset, WirePointer.kind(src), srcTargetOffset);
      }
      // We can just copy the upper 32 bits.
      dstSegment.buffer.putInt(
        dstOffset * Constants.BYTES_PER_WORD + 4,
        srcSegment.buffer.getInt(srcOffset * Constants.BYTES_PER_WORD + 4));

    } else {
      //# Need to create a far pointer. Try to allocate it in the same segment as the source,
      //# so that it doesn't need to be a double-far.

      Int32 landingPadOffset = srcSegment.allocate(1);
      if (landingPadOffset == SegmentBuilder.FAILED_ALLOCATION) {
        //# Darn, need a double-far.

        BuilderArena.AllocateResult allocation = srcSegment.getArena().allocate(2);
        SegmentBuilder farSegment = allocation.segment;
        landingPadOffset = allocation.offset;

        FarPointer.set(farSegment.buffer, landingPadOffset, false, srcTargetOffset);
        FarPointer.setSegmentId(farSegment.buffer, landingPadOffset, srcSegment.id);

        WirePointer.setKindWithZeroOffset(
          farSegment.buffer, landingPadOffset + 1, WirePointer.kind(src));

        farSegment.buffer.putInt(
          (landingPadOffset + 1) * Constants.BYTES_PER_WORD + 4,
          srcSegment.buffer.getInt(srcOffset * Constants.BYTES_PER_WORD + 4));

        FarPointer.set(dstSegment.buffer, dstOffset, true, landingPadOffset);
        FarPointer.setSegmentId(dstSegment.buffer, dstOffset, farSegment.id);
      } else {
        //# Simple landing pad is just a pointer.
        WirePointer.setKindAndTarget(
          srcSegment.buffer, landingPadOffset, WirePointer.kind(srcTarget), srcTargetOffset);
        srcSegment.buffer.putInt(
          landingPadOffset * Constants.BYTES_PER_WORD + 4,
          srcSegment.buffer.getInt(srcOffset * Constants.BYTES_PER_WORD + 4));

        FarPointer.set(dstSegment.buffer, dstOffset, false, landingPadOffset);
        FarPointer.setSegmentId(dstSegment.buffer, dstOffset, srcSegment.id);
      }
    }
  }

  public static T initStructPointer<T> (
    StructBuilder.Factory<T> factory, Int32 rfOffset, SegmentBuilder segment, StructSize size) {
    AllocateResult allocation = allocate(rfOffset, segment, size.total(), WirePointer.STRUCT);
    StructPointer.setFromStructSize(allocation.segment.buffer, allocation.rfOffset, size);
    return factory.constructBuilder(
      allocation.segment,
      allocation.ptr * Constants.BYTES_PER_WORD,
      allocation.ptr + size.data,
      size.data * 64,
      size.pointers);
  }

  public static T getWritableStructPointer<T> (
    StructBuilder.Factory<T> factory,
    Int32 rfOffset,
    SegmentBuilder segment,
    StructSize size,
    SegmentReader defaultSegment,
    Int32 defaultOffset) {
    Int64 rf = segment.get(rfOffset);
    Int32 target = WirePointer.target(rfOffset, rf);
    if (WirePointer.isNull(rf)) {
      if (defaultSegment == null) {
        return initStructPointer(factory, rfOffset, segment, size);
      } else {
        throw new ApplicationException("unimplemented");
      }
    }
    FollowBuilderFarsResult resolved = followBuilderFars(rf, target, segment);

    short oldDataSize = StructPointer.dataSize(resolved.rf);
    short oldPointerCount = StructPointer.ptrCount(resolved.rf);
    Int32 oldPointerSection = resolved.ptr + oldDataSize;

    if (oldDataSize < size.data || oldPointerCount < size.pointers) {
      //# The space allocated for this struct is too small. Unlike with readers, we can't just
      //# run with it and do bounds checks at access time, because how would we handle writes?
      //# Instead, we have to copy the struct to a new space now.

      short newDataSize = (short)Math.Max(oldDataSize, size.data);
      short newPointerCount = (short)Math.Max(oldPointerCount, size.pointers);
      Int32 totalSize = newDataSize + newPointerCount * Constants.WORDS_PER_POINTER;

      //# Don't let allocate() zero out the object just yet.
      zeroPointerAndFars(segment, rfOffset);

      AllocateResult allocation = allocate(rfOffset, segment, totalSize, WirePointer.STRUCT);

      StructPointer.set(
        allocation.segment.buffer, allocation.rfOffset, newDataSize, newPointerCount);

      //# Copy data section.
      memcpy(
        allocation.segment.buffer,
        allocation.ptr * Constants.BYTES_PER_WORD,
        resolved.segment.buffer,
        resolved.ptr * Constants.BYTES_PER_WORD,
        oldDataSize * Constants.BYTES_PER_WORD);

      //# Copy pointer section.
      Int32 newPointerSection = allocation.ptr + newDataSize;
      for (Int32 ii = 0; ii < oldPointerCount; ++ii) {
        transferPointer(
          allocation.segment, newPointerSection + ii, resolved.segment, oldPointerSection + ii);
      }

      //# Zero out old location.  This has two purposes:
      //# 1) We don't want to leak the original contents of the struct when the message is written
      //#    out as it may contain secrets that the caller intends to remove from the new copy.
      //# 2) Zeros will be deflated by packing, making this dead memory almost-free if it ever
      //#    hits the wire.
      memset(
        resolved.segment.buffer,
        resolved.ptr * Constants.BYTES_PER_WORD,
        (byte)0,
        (oldDataSize + oldPointerCount * Constants.WORDS_PER_POINTER) * Constants.BYTES_PER_WORD);

      return factory.constructBuilder(
        allocation.segment,
        allocation.ptr * Constants.BYTES_PER_WORD,
        newPointerSection,
        newDataSize * Constants.BITS_PER_WORD,
        newPointerCount);
    } else {
      return factory.constructBuilder(
        resolved.segment,
        resolved.ptr * Constants.BYTES_PER_WORD,
        oldPointerSection,
        oldDataSize * Constants.BITS_PER_WORD,
        oldPointerCount);
    }
  }

  public static T initListPointer<T> (
    ListBuilder.Factory<T> factory,
    Int32 rfOffset,
    SegmentBuilder segment,
    Int32 elementCount,
    byte elementSize) {
    if (elementSize == ElementSize.INLINE_COMPOSITE) {
      throw new ApplicationException("Should have called initStructListPointer instead");
     }

    Int32 dataSize = ElementSize.dataBitsPerElement(elementSize);
    Int32 pointerCount = ElementSize.pointersPerElement(elementSize);
    Int32 step = dataSize + pointerCount * Constants.BITS_PER_POINTER;
    Int32 wordCount = roundBitsUpToWords((Int64)elementCount * (Int64)step);
    AllocateResult allocation = allocate(rfOffset, segment, wordCount, WirePointer.LIST);

    ListPointer.set(allocation.segment.buffer, allocation.rfOffset, elementSize, elementCount);

    return factory.constructBuilder(
      allocation.segment,
      allocation.ptr * Constants.BYTES_PER_WORD,
      elementCount,
      step,
      dataSize,
      (short)pointerCount);
  }

  public static T initStructListPointer<T> (
    ListBuilder.Factory<T> factory,
    Int32 rfOffset,
    SegmentBuilder segment,
    Int32 elementCount,
    StructSize elementSize) {
    Int32 wordsPerElement = elementSize.total();

    //# Allocate the list, prfixed by a single WirePointer.
    Int32 wordCount = elementCount * wordsPerElement;
    AllocateResult allocation =
      allocate(rfOffset, segment, Constants.POINTER_SIZE_IN_WORDS + wordCount, WirePointer.LIST);

    //# Initialize the pointer.
    ListPointer.setInlineComposite(allocation.segment.buffer, allocation.rfOffset, wordCount);
    WirePointer.setKindAndInlineCompositeListElementCount(
      allocation.segment.buffer, allocation.ptr, WirePointer.STRUCT, elementCount);
    StructPointer.setFromStructSize(allocation.segment.buffer, allocation.ptr, elementSize);

    return factory.constructBuilder(
      allocation.segment,
      (allocation.ptr + 1) * Constants.BYTES_PER_WORD,
      elementCount,
      wordsPerElement * Constants.BITS_PER_WORD,
      elementSize.data * Constants.BITS_PER_WORD,
      elementSize.pointers);
  }

  public static T getWritableListPointer<T> (
    ListBuilder.Factory<T> factory,
    Int32 origRefOffset,
    SegmentBuilder origSegment,
    byte elementSize,
    SegmentReader defaultSegment,
    Int32 defaultOffset) {
    if (elementSize == ElementSize.INLINE_COMPOSITE) {
      throw new ApplicationException("Use getWritableStructListPointer() for struct lists");
    }

    Int64 origRef = origSegment.get(origRefOffset);
    Int32 origRefTarget = WirePointer.target(origRefOffset, origRef);

    if (WirePointer.isNull(origRef)) {
      throw new ApplicationException("unimplemented");
    }

    //# We must verify that the pointer has the right size. Unlike
    //# in getWritableStructListPointer(), we never need to
    //# "upgrade" the data, because this method is called only for
    //# non-struct lists, and there is no allowed upgrade path *to*
    //# a non-struct list, only *from* them.

    FollowBuilderFarsResult resolved = followBuilderFars(origRef, origRefTarget, origSegment);

    if (WirePointer.kind(resolved.rf) != WirePointer.LIST) {
      throw new DecodeException(
        "Called getList{Field,Element}() but existing pointer is not a list");
    }

    byte oldSize = ListPointer.elementSize(resolved.rf);

    if (oldSize == ElementSize.INLINE_COMPOSITE) {
      //# The existing element size is InlineComposite, which
      //# means that it is at least two words, which makes it
      //# bigger than the expected element size. Since fields can
      //# only grow when upgraded, the existing data must have
      //# been written with a newer version of the protocol. We
      //# therfore never need to upgrade the data in this case,
      //# but we do need to validate that it is a valid upgrade
      //# from what we expected.
      throw new ApplicationException("unimplemented");
    } else {
      Int32 dataSize = ElementSize.dataBitsPerElement(oldSize);
      Int32 pointerCount = ElementSize.pointersPerElement(oldSize);

      if (dataSize < ElementSize.dataBitsPerElement(elementSize)) {
        throw new DecodeException("Existing list value is incompatible with expected type.");
      }
      if (pointerCount < ElementSize.pointersPerElement(elementSize)) {
        throw new DecodeException("Existing list value is incompatible with expected type.");
      }

      Int32 step = dataSize + pointerCount * Constants.BITS_PER_POINTER;

      return factory.constructBuilder(
        resolved.segment,
        resolved.ptr * Constants.BYTES_PER_WORD,
        ListPointer.elementCount(resolved.rf),
        step,
        dataSize,
        (short)pointerCount);
    }
  }

  public static T getWritableStructListPointer<T> (
    ListBuilder.Factory<T> factory,
    Int32 origRefOffset,
    SegmentBuilder origSegment,
    StructSize elementSize,
    SegmentReader defaultSegment,
    Int32 defaultOffset) {
    Int64 origRef = origSegment.get(origRefOffset);
    Int32 origRefTarget = WirePointer.target(origRefOffset, origRef);

    if (WirePointer.isNull(origRef)) {
      throw new ApplicationException("unimplemented");
    }

    //# We must verify that the pointer has the right size and potentially upgrade it if not.

    FollowBuilderFarsResult resolved = followBuilderFars(origRef, origRefTarget, origSegment);
    if (WirePointer.kind(resolved.rf) != WirePointer.LIST) {
      throw new DecodeException(
        "Called getList{Field,Element}() but existing pointer is not a list");
    }

    byte oldSize = ListPointer.elementSize(resolved.rf);

    if (oldSize == ElementSize.INLINE_COMPOSITE) {
      //# Existing list is INLINE_COMPOSITE, but we need to verify that the sizes match.
      Int64 oldTag = resolved.segment.get(resolved.ptr);
      Int32 oldPtr = resolved.ptr + Constants.POINTER_SIZE_IN_WORDS;
      if (WirePointer.kind(oldTag) != WirePointer.STRUCT) {
        throw new DecodeException("INLINE_COMPOSITE list with non-STRUCT elements not supported.");
      }
      Int32 oldDataSize = StructPointer.dataSize(oldTag);
      short oldPointerCount = StructPointer.ptrCount(oldTag);
      Int32 oldStep = (oldDataSize + oldPointerCount * Constants.POINTER_SIZE_IN_WORDS);
      Int32 elementCount = WirePointer.inlineCompositeListElementCount(oldTag);

      if (oldDataSize >= elementSize.data && oldPointerCount >= elementSize.pointers) {
        //# Old size is at least as large as we need. Ship it.
        return factory.constructBuilder(
          resolved.segment,
          oldPtr * Constants.BYTES_PER_WORD,
          elementCount,
          oldStep * Constants.BITS_PER_WORD,
          oldDataSize * Constants.BITS_PER_WORD,
          oldPointerCount);
      }

      //# The structs in this list are smaller than expected, probably written using an older
      //# version of the protocol. We need to make a copy and expand them.

      short newDataSize = (short)Math.Max(oldDataSize, elementSize.data);
      short newPointerCount = (short)Math.Max(oldPointerCount, elementSize.pointers);
      Int32 newStep = newDataSize + newPointerCount * Constants.WORDS_PER_POINTER;
      Int32 totalSize = newStep * elementCount;

      //# Don't let allocate() zero out the object just yet.
      zeroPointerAndFars(origSegment, origRefOffset);

      AllocateResult allocation = allocate(
        origRefOffset, origSegment, totalSize + Constants.POINTER_SIZE_IN_WORDS, WirePointer.LIST);

      ListPointer.setInlineComposite(allocation.segment.buffer, allocation.rfOffset, totalSize);

      // Int64 tag =
      // Keep side-effects of no-op assignment from capnp-java.
      allocation.segment.get(allocation.ptr);
      WirePointer.setKindAndInlineCompositeListElementCount(
        allocation.segment.buffer, allocation.ptr, WirePointer.STRUCT, elementCount);
      StructPointer.set(allocation.segment.buffer, allocation.ptr, newDataSize, newPointerCount);
      Int32 newPtr = allocation.ptr + Constants.POINTER_SIZE_IN_WORDS;

      Int32 src = oldPtr;
      Int32 dst = newPtr;
      for (Int32 ii = 0; ii < elementCount; ++ii) {
        //# Copy data section.
        memcpy(
          allocation.segment.buffer,
          dst * Constants.BYTES_PER_WORD,
          resolved.segment.buffer,
          src * Constants.BYTES_PER_WORD,
          oldDataSize * Constants.BYTES_PER_WORD);

        //# Copy pointer section.
        Int32 newPointerSection = dst + newDataSize;
        Int32 oldPointerSection = src + oldDataSize;
        for (Int32 jj = 0; jj < oldPointerCount; ++jj) {
          transferPointer(
            allocation.segment, newPointerSection + jj, resolved.segment, oldPointerSection + jj);
        }

        dst += newStep;
        src += oldStep;
      }

      //# Zero out old location. See explanation in getWritableStructPointer().
      //# Make sure to include the tag word.
      memset(
        resolved.segment.buffer,
        resolved.ptr * Constants.BYTES_PER_WORD,
        (byte)0,
        (1 + oldStep * elementCount) * Constants.BYTES_PER_WORD);

      return factory.constructBuilder(
        allocation.segment,
        newPtr * Constants.BYTES_PER_WORD,
        elementCount,
        newStep * Constants.BITS_PER_WORD,
        newDataSize * Constants.BITS_PER_WORD,
        newPointerCount);
    } else {
      //# We're upgrading from a non-struct list.

      Int32 oldDataSize = ElementSize.dataBitsPerElement(oldSize);
      Int32 oldPointerCount = ElementSize.pointersPerElement(oldSize);
      Int32 oldStep = oldDataSize + oldPointerCount * Constants.BITS_PER_POINTER;
      Int32 elementCount = ListPointer.elementCount(origRef);

      if (oldSize == ElementSize.VOID) {
        //# Nothing to copy, just allocate a new list.
        return initStructListPointer(
          factory, origRefOffset, origSegment, elementCount, elementSize);
      } else {
        //# Upgrading to an inline composite list.

        if (oldSize == ElementSize.BIT) {
          throw new ApplicationException(
            "Found bit list where struct list was expected; "
            + "upgrading boolean lists to struct is no longer supported.");
        }

        short newDataSize = elementSize.data;
        short newPointerCount = elementSize.pointers;

        if (oldSize == ElementSize.POINTER) {
          newPointerCount = Math.Max(newPointerCount, (short)1);
        } else {
          //# Old list contains data elements, so we need at least 1 word of data.
          newDataSize = Math.Max(newDataSize, (short)1);
        }

        Int32 newStep = (newDataSize + newPointerCount * Constants.WORDS_PER_POINTER);
        Int32 totalWords = elementCount * newStep;

        //# Don't let allocate() zero out the object just yet.
        zeroPointerAndFars(origSegment, origRefOffset);

        AllocateResult allocation = allocate(
          origRefOffset,
          origSegment,
          totalWords + Constants.POINTER_SIZE_IN_WORDS,
          WirePointer.LIST);

        ListPointer.setInlineComposite(allocation.segment.buffer, allocation.rfOffset, totalWords);

        // Int64 tag =
      // Int64 tag =
      // Keep side-effects of no-op assignment from capnp-java.
        allocation.segment.get(allocation.ptr);
        WirePointer.setKindAndInlineCompositeListElementCount(
          allocation.segment.buffer, allocation.ptr, WirePointer.STRUCT, elementCount);
        StructPointer.set(allocation.segment.buffer, allocation.ptr, newDataSize, newPointerCount);
        Int32 newPtr = allocation.ptr + Constants.POINTER_SIZE_IN_WORDS;

        if (oldSize == ElementSize.POINTER) {
          Int32 dst = newPtr + newDataSize;
          Int32 src = resolved.ptr;
          for (Int32 ii = 0; ii < elementCount; ++ii) {
            transferPointer(origSegment, dst, resolved.segment, src);
            dst += newStep / Constants.WORDS_PER_POINTER;
            src += 1;
          }
        } else {
          Int32 dst = newPtr;
          Int32 srcByteOffset = resolved.ptr * Constants.BYTES_PER_WORD;
          Int32 oldByteStep = oldDataSize / Constants.BITS_PER_BYTE;
          for (Int32 ii = 0; ii < elementCount; ++ii) {
            memcpy(
              allocation.segment.buffer,
              dst * Constants.BYTES_PER_WORD,
              resolved.segment.buffer,
              srcByteOffset,
              oldByteStep);
            srcByteOffset += oldByteStep;
            dst += newStep;
          }
        }

        //# Zero out old location. See explanation in getWritableStructPointer().
        memset(
          resolved.segment.buffer,
          resolved.ptr * Constants.BYTES_PER_WORD,
          (byte)0,
          roundBitsUpToBytes(oldStep * elementCount));

        return factory.constructBuilder(
          allocation.segment,
          newPtr * Constants.BYTES_PER_WORD,
          elementCount,
          newStep * Constants.BITS_PER_WORD,
          newDataSize * Constants.BITS_PER_WORD,
          newPointerCount);
      }
    }
  }

  // size is in bytes
  public static Text.Builder initTextPointer(Int32 rfOffset, SegmentBuilder segment, Int32 size) {
    //# The byte list must include a NUL terminator.
    Int32 byteSize = size + 1;

    //# Allocate the space.
    AllocateResult allocation =
      allocate(rfOffset, segment, roundBytesUpToWords(byteSize), WirePointer.LIST);

    //# Initialize the pointer.
    ListPointer.set(allocation.segment.buffer, allocation.rfOffset, ElementSize.BYTE, byteSize);

    return new Text.Builder(
      allocation.segment.buffer, allocation.ptr * Constants.BYTES_PER_WORD, size);
  }

  public static Text.Builder setTextPointer(Int32 rfOffset, SegmentBuilder segment, Text.Reader value) {
    Text.Builder builder = initTextPointer(rfOffset, segment, value.size());

    ByteBuffer slice = value.buffer.duplicate();
    slice.position(value.offset);
    slice.limit(value.offset + value.size());
    builder.buffer.position(builder.offset);
    builder.buffer.put(slice);
    return builder;
  }

  public static Text.Builder getWritableTextPointer(
    Int32 rfOffset,
    SegmentBuilder segment,
    ByteBuffer defaultBuffer,
    Int32 defaultOffset,
    Int32 defaultSize) {
    Int64 rf = segment.get(rfOffset);

    if (WirePointer.isNull(rf)) {
      if (defaultBuffer == null) {
        return new Text.Builder();
      } else {
        Text.Builder builder = initTextPointer(rfOffset, segment, defaultSize);
        // TODO is there a way to do this with bulk methods?
        for (Int32 i = 0; i < builder.size; ++i) {
          builder.buffer.put(builder.offset + i, defaultBuffer.get(defaultOffset * 8 + i));
        }
        return builder;
      }
    }

    Int32 rfTarget = WirePointer.target(rfOffset, rf);
    FollowBuilderFarsResult resolved = followBuilderFars(rf, rfTarget, segment);

    if (WirePointer.kind(resolved.rf) != WirePointer.LIST) {
      throw new DecodeException(
        "Called getText{Field,Element} but existing pointer is not a list.");
    }
    if (ListPointer.elementSize(resolved.rf) != ElementSize.BYTE) {
      throw new DecodeException(
        "Called getText{Field,Element} but existing list pointer is not byte-sized.");
    }

    Int32 size = ListPointer.elementCount(resolved.rf);
    if (
      size == 0
      || resolved.segment.buffer.get(resolved.ptr * Constants.BYTES_PER_WORD + size - 1) != 0) {
      throw new DecodeException("Text blob missing NUL terminator.");
    }
    return new Text.Builder(
      resolved.segment.buffer, resolved.ptr * Constants.BYTES_PER_WORD, size - 1);
  }

  // size is in bytes
  public static Data.Builder initDataPointer(Int32 rfOffset, SegmentBuilder segment, Int32 size) {
    //# Allocate the space.
    AllocateResult allocation =
      allocate(rfOffset, segment, roundBytesUpToWords(size), WirePointer.LIST);

    //# Initialize the pointer.
    ListPointer.set(allocation.segment.buffer, allocation.rfOffset, ElementSize.BYTE, size);

    return new Data.Builder(
      allocation.segment.buffer, allocation.ptr * Constants.BYTES_PER_WORD, size);
  }

  public static Data.Builder setDataPointer(
    Int32 rfOffset, SegmentBuilder segment, Data.Reader value) {
    Data.Builder builder = initDataPointer(rfOffset, segment, value.size());

    // TODO is there a way to do this with bulk methods?
    for (Int32 i = 0; i < builder.size; ++i) {
      builder.buffer.put(builder.offset + i, value.buffer.get(value.offset + i));
    }
    return builder;
  }

  public static Data.Builder getWritableDataPointer(
    Int32 rfOffset,
    SegmentBuilder segment,
    ByteBuffer defaultBuffer,
    Int32 defaultOffset,
    Int32 defaultSize) {
    Int64 rf = segment.get(rfOffset);

    if (WirePointer.isNull(rf)) {
      if (defaultBuffer == null) {
        return new Data.Builder();
      } else {
        Data.Builder builder = initDataPointer(rfOffset, segment, defaultSize);
        // TODO is there a way to do this with bulk methods?
        for (Int32 i = 0; i < builder.size; ++i) {
          builder.buffer.put(builder.offset + i, defaultBuffer.get(defaultOffset * 8 + i));
        }
        return builder;
      }
    }

    Int32 rfTarget = WirePointer.target(rfOffset, rf);
    FollowBuilderFarsResult resolved = followBuilderFars(rf, rfTarget, segment);

    if (WirePointer.kind(resolved.rf) != WirePointer.LIST) {
      throw new DecodeException(
        "Called getData{Field,Element} but existing pointer is not a list.");
    }
    if (ListPointer.elementSize(resolved.rf) != ElementSize.BYTE) {
      throw new DecodeException(
        "Called getData{Field,Element} but existing list pointer is not byte-sized.");
    }

    return new Data.Builder(
      resolved.segment.buffer,
      resolved.ptr * Constants.BYTES_PER_WORD,
      ListPointer.elementCount(resolved.rf));
  }

  public static T readStructPointer<T> (
    StructReader.Factory<T> factory,
    SegmentReader segment,
    Int32 rfOffset,
    SegmentReader defaultSegment,
    Int32 defaultOffset,
    Int32 nestingLimit) {
    Int64 rf = segment.get(rfOffset);
    if (WirePointer.isNull(rf)) {
      if (defaultSegment == null) {
        return factory.constructReader(SegmentReader.EMPTY, 0, 0, 0, (short)0, 0x7fffffff);
      } else {
        segment = defaultSegment;
        rfOffset = defaultOffset;
        rf = segment.get(rfOffset);
      }
    }

    if (nestingLimit <= 0) {
      throw new DecodeException("Message is too deeply nested or contains cycles.");
    }

    Int32 rfTarget = WirePointer.target(rfOffset, rf);
    FollowFarsResult resolved = followFars(rf, rfTarget, segment);

    Int32 dataSizeWords = StructPointer.dataSize(resolved.rf);

    if (WirePointer.kind(resolved.rf) != WirePointer.STRUCT) {
      throw new DecodeException(
        "Message contains non-struct pointer where struct pointer was expected.");
    }

    resolved.segment.arena.checkReadLimit(StructPointer.wordSize(resolved.rf));

    return factory.constructReader(
      resolved.segment,
      resolved.ptr * Constants.BYTES_PER_WORD,
      (resolved.ptr + dataSizeWords),
      dataSizeWords * Constants.BITS_PER_WORD,
      StructPointer.ptrCount(resolved.rf),
      nestingLimit - 1);
  }

  public static SegmentBuilder setStructPointer(
    SegmentBuilder segment, Int32 rfOffset, StructReader value) {
    short dataSize = (short)roundBitsUpToWords(value.dataSize);
    Int32 totalSize = dataSize + value.pointerCount * Constants.POINTER_SIZE_IN_WORDS;

    AllocateResult allocation = allocate(rfOffset, segment, totalSize, WirePointer.STRUCT);
    StructPointer.set(
      allocation.segment.buffer, allocation.rfOffset, dataSize, value.pointerCount);

    if (value.dataSize == 1) {
      throw new ApplicationException("single bit case not handled");
    } else {
      memcpy(
        allocation.segment.buffer,
        allocation.ptr * Constants.BYTES_PER_WORD,
        value.segment.buffer,
        value.data,
        value.dataSize / Constants.BITS_PER_BYTE);
    }

    Int32 pointerSection = allocation.ptr + dataSize;
    for (Int32 i = 0; i < value.pointerCount; ++i) {
      copyPointer(
        allocation.segment,
        pointerSection + i,
        value.segment,
        value.pointers + i,
        value.nestingLimit);
    }
    return allocation.segment;
  }

  public static SegmentBuilder setListPointer(SegmentBuilder segment, Int32 rfOffset, ListReader value) {
    Int32 totalSize = roundBitsUpToWords(value.elementCount * value.step);

    if (value.step <= Constants.BITS_PER_WORD) {
      //# List of non-structs.
      AllocateResult allocation = allocate(rfOffset, segment, totalSize, WirePointer.LIST);

      if (value.structPointerCount == 1) {
        //# List of pointers.
        ListPointer.set(
          allocation.segment.buffer, allocation.rfOffset, ElementSize.POINTER, value.elementCount);
        for (Int32 i = 0; i < value.elementCount; ++i) {
          copyPointer(
            allocation.segment,
            allocation.ptr + i,
            value.segment,
            value.ptr / Constants.BYTES_PER_WORD + i,
            value.nestingLimit);
        }
      } else {
        //# List of data.
        byte elementSize = ElementSize.VOID;
        switch (value.step) {
          case 0:
            elementSize = ElementSize.VOID;
            break;
          case 1:
            elementSize = ElementSize.BIT;
            break;
          case 8:
            elementSize = ElementSize.BYTE;
            break;
          case 16:
            elementSize = ElementSize.TWO_BYTES;
            break;
          case 32:
            elementSize = ElementSize.FOUR_BYTES;
            break;
          case 64:
            elementSize = ElementSize.EIGHT_BYTES;
            break;
          default:
            throw new ApplicationException("invalid list step size: " + value.step);
        }

        ListPointer.set(
          allocation.segment.buffer, allocation.rfOffset, elementSize, value.elementCount);
        memcpy(
          allocation.segment.buffer,
          allocation.ptr * Constants.BYTES_PER_WORD,
          value.segment.buffer,
          value.ptr,
          totalSize * Constants.BYTES_PER_WORD);
      }
      return allocation.segment;
    } else {
      //# List of structs.
      AllocateResult allocation =
        allocate(rfOffset, segment, totalSize + Constants.POINTER_SIZE_IN_WORDS, WirePointer.LIST);
      ListPointer.setInlineComposite(allocation.segment.buffer, allocation.rfOffset, totalSize);
      short dataSize = (short)roundBitsUpToWords(value.structDataSize);
      short pointerCount = value.structPointerCount;

      WirePointer.setKindAndInlineCompositeListElementCount(
        allocation.segment.buffer, allocation.ptr, WirePointer.STRUCT, value.elementCount);
      StructPointer.set(allocation.segment.buffer, allocation.ptr, dataSize, pointerCount);

      Int32 dstOffset = allocation.ptr + Constants.POINTER_SIZE_IN_WORDS;
      Int32 srcOffset = value.ptr / Constants.BYTES_PER_WORD;

      for (Int32 i = 0; i < value.elementCount; ++i) {
        memcpy(
          allocation.segment.buffer,
          dstOffset * Constants.BYTES_PER_WORD,
          value.segment.buffer,
          srcOffset * Constants.BYTES_PER_WORD,
          value.structDataSize / Constants.BITS_PER_BYTE);
        dstOffset += dataSize;
        srcOffset += dataSize;

        for (Int32 j = 0; j < pointerCount; ++j) {
          copyPointer(allocation.segment, dstOffset, value.segment, srcOffset, value.nestingLimit);
          dstOffset += Constants.POINTER_SIZE_IN_WORDS;
          srcOffset += Constants.POINTER_SIZE_IN_WORDS;
        }
      }
      return allocation.segment;
    }
  }

  public static void memset(ByteBuffer dstBuffer, Int32 dstByteOffset, byte value, Int32 length) {
    // TODO we can probably do this faster
    for (Int32 ii = dstByteOffset; ii < dstByteOffset + length; ++ii) {
      dstBuffer.put(ii, value);
    }
  }

  public static void memcpy(
    ByteBuffer dstBuffer, Int32 dstByteOffset, ByteBuffer srcBuffer, Int32 srcByteOffset, Int32 length) {
    ByteBuffer dstDup = dstBuffer.duplicate();
    dstDup.position(dstByteOffset);
    dstDup.limit(dstByteOffset + length);
    ByteBuffer srcDup = srcBuffer.duplicate();
    srcDup.position(srcByteOffset);
    srcDup.limit(srcByteOffset + length);
    dstDup.put(srcDup);
  }

  public static SegmentBuilder copyPointer(
    SegmentBuilder dstSegment,
    Int32 dstOffset,
    SegmentReader srcSegment,
    Int32 srcOffset,
    Int32 nestingLimit) {
    // Deep-copy the object pointed to by src into dst.  It turns out we can't reuse
    // readStructPointer(), etc. because they do type checking whereas here we want to accept any
    // valid pointer.

    Int64 srcRef = srcSegment.get(srcOffset);

    if (WirePointer.isNull(srcRef)) {
      dstSegment.buffer.putLong(dstOffset * 8, 0L);
      return dstSegment;
    }

    Int32 srcTarget = WirePointer.target(srcOffset, srcRef);
    FollowFarsResult resolved = followFars(srcRef, srcTarget, srcSegment);

    switch (WirePointer.kind(resolved.rf)) {
      case WirePointer.STRUCT:
        if (nestingLimit <= 0) {
          throw new DecodeException(
            "Message is too deeply nested or contains cycles. See org.capnproto.ReaderOptions.");
        }
        resolved.segment.arena.checkReadLimit(StructPointer.wordSize(resolved.rf));
        return setStructPointer(
          dstSegment,
          dstOffset,
          new StructReader(
            resolved.segment,
            resolved.ptr * Constants.BYTES_PER_WORD,
            resolved.ptr + StructPointer.dataSize(resolved.rf),
            StructPointer.dataSize(resolved.rf) * Constants.BITS_PER_WORD,
            StructPointer.ptrCount(resolved.rf),
            nestingLimit - 1));
      case WirePointer.LIST:
        byte elementSize = ListPointer.elementSize(resolved.rf);
        if (nestingLimit <= 0) {
          throw new DecodeException(
            "Message is too deeply nested or contains cycles. See org.capnproto.ReaderOptions.");
        }
        if (elementSize == ElementSize.INLINE_COMPOSITE) {
          Int32 wordCount = ListPointer.inlineCompositeWordCount(resolved.rf);
          Int64 tag = resolved.segment.get(resolved.ptr);
          Int32 ptr = resolved.ptr + 1;

          resolved.segment.arena.checkReadLimit(wordCount + 1);

          if (WirePointer.kind(tag) != WirePointer.STRUCT) {
            throw new DecodeException(
              "INLINE_COMPOSITE lists of non-STRUCT type are not supported.");
          }

          Int32 elementCount = WirePointer.inlineCompositeListElementCount(tag);
          Int32 wordsPerElement = StructPointer.wordSize(tag);
          if ((Int64)wordsPerElement * elementCount > wordCount) {
            throw new DecodeException("INLINE_COMPOSITE list's elements overrun its word count.");
          }

          if (wordsPerElement == 0) {
            // Watch out for lists of zero-sized structs, which can claim to be arbitrarily
            // large without having sent actual data.
            resolved.segment.arena.checkReadLimit(elementCount);
          }

          return setListPointer(
            dstSegment,
            dstOffset,
            new ListReader(
              resolved.segment,
              ptr * Constants.BYTES_PER_WORD,
              elementCount,
              wordsPerElement * Constants.BITS_PER_WORD,
              StructPointer.dataSize(tag) * Constants.BITS_PER_WORD,
              StructPointer.ptrCount(tag),
              nestingLimit - 1));
        } else {
          Int32 dataSize = ElementSize.dataBitsPerElement(elementSize);
          short pointerCount = ElementSize.pointersPerElement(elementSize);
          Int32 step = dataSize + pointerCount * Constants.BITS_PER_POINTER;
          Int32 elementCount = ListPointer.elementCount(resolved.rf);
          Int32 wordCount = roundBitsUpToWords((Int64)elementCount * step);

          resolved.segment.arena.checkReadLimit(wordCount);

          if (elementSize == ElementSize.VOID) {
            // Watch out for lists of void, which can claim to be arbitrarily large without
            // having sent actual data.
            resolved.segment.arena.checkReadLimit(elementCount);
          }

          return setListPointer(
            dstSegment,
            dstOffset,
            new ListReader(
              resolved.segment,
              resolved.ptr * Constants.BYTES_PER_WORD,
              elementCount,
              step,
              dataSize,
              pointerCount,
              nestingLimit - 1));
        }

      case WirePointer.FAR:
        throw new DecodeException("Unexpected FAR pointer.");
      case WirePointer.OTHER:
        throw new ApplicationException("copyPointer is unimplemented for OTHER pointers");
    }
    throw new ApplicationException("unreachable");
  }

  public static T readListPointer<T> (
    ListReader.Factory<T> factory,
    SegmentReader segment,
    Int32 rfOffset,
    SegmentReader defaultSegment,
    Int32 defaultOffset,
    byte expectedElementSize,
    Int32 nestingLimit) {

    Int64 rf = segment.get(rfOffset);

    if (WirePointer.isNull(rf)) {
      if (defaultSegment == null) {
        return factory.constructReader(SegmentReader.EMPTY, 0, 0, 0, 0, (short)0, 0x7fffffff);
      } else {
        segment = defaultSegment;
        rfOffset = defaultOffset;
        rf = segment.get(rfOffset);
      }
    }

    if (nestingLimit <= 0) {
      throw new ApplicationException("nesting limit exceeded");
    }

    Int32 rfTarget = WirePointer.target(rfOffset, rf);

    FollowFarsResult resolved = followFars(rf, rfTarget, segment);

    byte elementSize = ListPointer.elementSize(resolved.rf);
    switch (elementSize) {
      case ElementSize.INLINE_COMPOSITE: {
        Int32 wordCount = ListPointer.inlineCompositeWordCount(resolved.rf);

        Int64 tag = resolved.segment.get(resolved.ptr);
        Int32 ptr = resolved.ptr + 1;

        resolved.segment.arena.checkReadLimit(wordCount + 1);

        Int32 size = WirePointer.inlineCompositeListElementCount(tag);

        Int32 wordsPerElement = StructPointer.wordSize(tag);

        if ((Int64)size * wordsPerElement > wordCount) {
          throw new DecodeException("INLINE_COMPOSITE list's elements overrun its word count.");
        }

        if (wordsPerElement == 0) {
          // Watch out for lists of zero-sized structs, which can claim to be arbitrarily
          // large without having sent actual data.
          resolved.segment.arena.checkReadLimit(size);
        }

        // TODO check whether the size is compatible

        return factory.constructReader(
          resolved.segment,
          ptr * Constants.BYTES_PER_WORD,
          size,
          wordsPerElement * Constants.BITS_PER_WORD,
          StructPointer.dataSize(tag) * Constants.BITS_PER_WORD,
          StructPointer.ptrCount(tag),
          nestingLimit - 1);
      }
      default: {
        //# This is a primitive or pointer list, but all such
        //# lists can also be interpreted as struct lists. We
        //# need to compute the data size and pointer count for
        //# such structs.
        Int32 dataSize = ElementSize.dataBitsPerElement(ListPointer.elementSize(resolved.rf));
        Int32 pointerCount = ElementSize.pointersPerElement(ListPointer.elementSize(resolved.rf));
        Int32 elementCount = ListPointer.elementCount(resolved.rf);
        Int32 step = dataSize + pointerCount * Constants.BITS_PER_POINTER;

        resolved.segment.arena.checkReadLimit(roundBitsUpToWords(elementCount * step));

        if (elementSize == ElementSize.VOID) {
          // Watch out for lists of void, which can claim to be arbitrarily large without
          // having sent actual data.
          resolved.segment.arena.checkReadLimit(elementCount);
        }

        //# Verify that the elements are at least as large as
        //# the expected type. Note that if we expected
        //# InlineComposite, the expected sizes here will be
        //# zero, because bounds checking will be performed at
        //# field access time. So this check here is for the
        //# case where we expected a list of some primitive or
        //# pointer type.

        Int32 expectedDataBitsPerElement = ElementSize.dataBitsPerElement(expectedElementSize);
        Int32 expectedPointersPerElement = ElementSize.pointersPerElement(expectedElementSize);

        if (expectedDataBitsPerElement > dataSize) {
          throw new DecodeException("Message contains list with incompatible element type.");
        }

        if (expectedPointersPerElement > pointerCount) {
          throw new DecodeException("Message contains list with incompatible element type.");
        }

        return factory.constructReader(
          resolved.segment,
          resolved.ptr * Constants.BYTES_PER_WORD,
          ListPointer.elementCount(resolved.rf),
          step,
          dataSize,
          (short)pointerCount,
          nestingLimit - 1);
      }
    }
  }

  public static Text.Reader readTextPointer(
    SegmentReader segment,
    Int32 rfOffset,
    ByteBuffer defaultBuffer,
    Int32 defaultOffset,
    Int32 defaultSize) {
    Int64 rf = segment.get(rfOffset);

    if (WirePointer.isNull(rf)) {
      if (defaultBuffer == null) {
        return new Text.Reader();
      } else {
        return new Text.Reader(defaultBuffer, defaultOffset, defaultSize);
      }
    }

    Int32 rfTarget = WirePointer.target(rfOffset, rf);

    FollowFarsResult resolved = followFars(rf, rfTarget, segment);

    Int32 size = ListPointer.elementCount(resolved.rf);

    if (WirePointer.kind(resolved.rf) != WirePointer.LIST) {
      throw new DecodeException("Message contains non-list pointer where text was expected.");
    }

    if (ListPointer.elementSize(resolved.rf) != ElementSize.BYTE) {
      throw new DecodeException(
        "Message contains list pointer of non-bytes where text was expected.");
    }

    resolved.segment.arena.checkReadLimit(roundBytesUpToWords(size));

    if (size == 0 || resolved.segment.buffer.get(8 * resolved.ptr + size - 1) != 0) {
      throw new DecodeException("Message contains text that is not NUL-terminated.");
    }

    return new Text.Reader(resolved.segment.buffer, resolved.ptr, size - 1);
  }

  public static Data.Reader readDataPointer(
    SegmentReader segment,
    Int32 rfOffset,
    ByteBuffer defaultBuffer,
    Int32 defaultOffset,
    Int32 defaultSize) {
    Int64 rf = segment.get(rfOffset);

    if (WirePointer.isNull(rf)) {
      if (defaultBuffer == null) {
        return new Data.Reader();
      } else {
        return new Data.Reader(defaultBuffer, defaultOffset, defaultSize);
      }
    }

    Int32 rfTarget = WirePointer.target(rfOffset, rf);

    FollowFarsResult resolved = followFars(rf, rfTarget, segment);

    Int32 size = ListPointer.elementCount(resolved.rf);

    if (WirePointer.kind(resolved.rf) != WirePointer.LIST) {
      throw new DecodeException("Message contains non-list pointer where data was expected.");
    }

    if (ListPointer.elementSize(resolved.rf) != ElementSize.BYTE) {
      throw new DecodeException(
        "Message contains list pointer of non-bytes where data was expected.");
    }

    resolved.segment.arena.checkReadLimit(roundBytesUpToWords(size));

    return new Data.Reader(resolved.segment.buffer, resolved.ptr, size);
  }
}

}  // namespace Capnp

