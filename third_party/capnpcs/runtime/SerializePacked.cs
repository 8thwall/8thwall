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

public class SerializePacked {

  public static MessageReader read(BufferedInputStream input) {
    return read(input, ReaderOptions.DEFAULT_READER_OPTIONS);
  }

  public static MessageReader read(BufferedInputStream input, ReaderOptions options) {
    PackedInputStream packedInput = new PackedInputStream(input);
    return Serialize.read(packedInput, options);
  }

  public static MessageReader readFromUnbuffered(ReadableByteChannel input) {
    return readFromUnbuffered(input, ReaderOptions.DEFAULT_READER_OPTIONS);
  }

  public static MessageReader readFromUnbuffered(ReadableByteChannel input, ReaderOptions options) {
    PackedInputStream packedInput = new PackedInputStream(new BufferedInputStreamWrapper(input));
    return Serialize.read(packedInput, options);
  }

  public static void write(BufferedOutputStream output, MessageBuilder message) {
    PackedOutputStream packedOutputStream = new PackedOutputStream(output);
    Serialize.write(packedOutputStream, message);
  }

  public static void writeToUnbuffered(WritableByteChannel output, MessageBuilder message) {
    BufferedOutputStreamWrapper buffered = new BufferedOutputStreamWrapper(output);
    write(buffered, message);
    buffered.flush();
  }
}

}  // namespace Capnp

