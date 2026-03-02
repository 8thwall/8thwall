using System;
using System.IO;
using System.Runtime.InteropServices;

namespace Capnp {

public enum ByteOrder {
  LITTLE_ENDIAN,
};

public class ByteBuffer {
  private MemoryStream buffer_;
  private BinaryReader reader_;
  private BinaryWriter writer_;
  private int first_;  // Position 0 of this byte buffer in its backing array.

  private ByteBuffer(Int32 bytes, bool signed) {
    buffer_ = signed
      ? new MemoryStream((Byte[]) ((Array) new SByte[bytes]), 0, bytes, true, true)
      : new MemoryStream(new Byte[bytes], 0, bytes, true, true);
    reader_ = new BinaryReader(buffer_);
    writer_ = new BinaryWriter(buffer_);

    // Initialize to a buffer of zeros with length bytes and position 0.
    buffer_.Position = 0;
    for (Int32 i = 0; i < bytes; ++i) {
      writer_.Write((Byte) 0);
    }
    buffer_.Position = 0;
    first_ = 0;
  }

  private ByteBuffer(MemoryStream buffer, int first) {
    buffer_ = buffer;
    reader_ = new BinaryReader(buffer_);
    writer_ = new BinaryWriter(buffer_);
    first_ = first;
  }

  public Byte[] array() {
    return buffer_.GetBuffer();
  }

  public Byte get() {
    return reader_.ReadByte();
  }

  public void put(Byte value) {
    writer_.Write(value);
  }

  public Byte get(Int32 index) {
    Int32 op = position();
    position(index);
    Byte b = reader_.ReadByte();
    position(op);
    return b;
  }

  public void put(Int32 index, Byte value) {
    Int32 op = position();
    position(index);
    writer_.Write(value);
    position(op);
  }

  public void get(Byte[] output, Int32 offset, Int32 count) {
    buffer_.Read(output, offset, count);
  }

  public void data(Byte[] output) {
    Int32 op = position();
    position(0);
    buffer_.Read(output, 0, limit());
    position(op);
  }

  public void put(ByteBuffer toCopy) {
    while (toCopy.buffer_.Position < toCopy.buffer_.Length) {
      writer_.Write(toCopy.reader_.ReadByte());
    }
  }

  public Int16 getShort(Int32 index) {
    Int32 op = position();
    position(index);
    Int16 s = reader_.ReadInt16();
    position(op);
    return s;
  }

  public void putShort(Int32 index, Int16 value) {
    Int32 op = position();
    position(index);
    writer_.Write(value);
    position(op);
  }

  public Int32 getInt() {
    return reader_.ReadInt32();
  }

  public Int32 getInt(Int32 index) {
    Int32 op = position();
    position(index);
    int i = reader_.ReadInt32();
    position(op);
    return i;
  }

  public void putInt(Int32 index, Int32 value) {
    Int32 op = position();
    position(index);
    writer_.Write(value);
    position(op);
  }

  public Int64 getLong(Int32 index) {
    Int32 op = position();
    position(index);
    Int64 l = reader_.ReadInt64();
    position(op);
    return l;
  }

  public void putLong(Int32 index, Int64 value) {
    Int32 op = position();
    position(index);
    writer_.Write(value);
    position(op);
  }

  public float getFloat(Int32 index) {
    Int32 op = position();
    position(index);
    float f = reader_.ReadSingle();
    position(op);
    return f;
  }

  public void putFloat(Int32 index, float value) {
    Int32 op = position();
    position(index);
    writer_.Write(value);
    position(op);
  }

  public double getDouble(Int32 index) {
    Int32 op = position();
    position(index);
    double d = reader_.ReadDouble();
    position(op);
    return d;
  }

  public void putDouble(Int32 index, double value) {
    Int32 op = position();
    position(index);
    writer_.Write(value);
    position(op);
  }

  public void rewind() {
    position(0);
  }

  public Int32 remaining() {
    return (Int32)(buffer_.Length - buffer_.Position);
  }

  public bool hasRemaining() {
    return remaining() > 0;
  }

  public void order(ByteOrder order) {
    // NOP, only litte-endian is supported.
  }

  public void clear() {
    position(0);
    buffer_.SetLength(0);
  }

  public int capacity() {
    return (int)buffer_.Capacity - first_;
  }

  public Int32 position() {
    return (Int32)buffer_.Position - first_;
  }

  public void position(Int32 newPosition) {
    buffer_.Position = newPosition + first_;
  }

  public Int32 limit() {
    return (Int32)buffer_.Length - first_;
  }

  public void limit(Int32 newLimit) {
    if (newLimit > capacity()) {
      throw new ApplicationException("Limit " + newLimit + " above capacity " + capacity());
    }
    if (position() > newLimit) {
      position(newLimit);
    }
    buffer_.SetLength(newLimit + first_);
  }

  public ByteBuffer slice() {
    Int32 pos = (Int32)buffer_.Position;
    Int32 len = (Int32)buffer_.Length;

    Byte[] buffer = buffer_.GetBuffer();

    MemoryStream sliceStream = new MemoryStream(buffer, 0, buffer.Length, writer_ != null, true);

    ByteBuffer newBuffer = new ByteBuffer(sliceStream, pos);
    newBuffer.limit(len - pos);

    if (writer_ == null) {
      newBuffer.writer_ = null;
    }

    return newBuffer;
  }

  public ByteBuffer duplicate() {
    Int32 pos = (Int32)buffer_.Position;
    Int32 len = (Int32)buffer_.Length;
    Byte[] buffer = buffer_.GetBuffer();
    MemoryStream dupStream = new MemoryStream(buffer, 0, buffer.Length, writer_ != null, true);
    dupStream.SetLength(len);
    dupStream.Position = pos;
    ByteBuffer newBuffer = new ByteBuffer(dupStream, first_);
    if (writer_ == null) {
      newBuffer.writer_ = null;
    }
    return newBuffer;
  }

  public ByteBuffer asReadOnlyBuffer() {
    ByteBuffer readOnly = new ByteBuffer(buffer_, first_);
    readOnly.writer_ = null;
    return readOnly;
  }

  public static ByteBuffer allocate(Int32 bytes) {
    return new ByteBuffer(bytes, false);
  }

  public static ByteBuffer allocateSigned(Int32 bytes) {
    return new ByteBuffer(bytes, true);
  }

  public static ByteBuffer wrap(Byte[] buffer) {
    MemoryStream wrapStream = new MemoryStream(buffer, 0, buffer.Length, true, true);
    wrapStream.SetLength(buffer.Length);
    wrapStream.Position = 0;
    return new ByteBuffer(wrapStream, 0);
  }

  public static ByteBuffer wrap(SByte[] buffer) {
    Byte[] unsignedBuffer = (Byte[]) ((Array) buffer);
    return wrap(unsignedBuffer);
  }

  public static ByteBuffer copyOf(NativeByteArray buffer) {
    byte[] bytes = new byte[buffer.size];
    Marshal.Copy(buffer.bytes, bytes, 0, buffer.size);
    return wrap(bytes);
  }
}

}  // namespace Capnp
