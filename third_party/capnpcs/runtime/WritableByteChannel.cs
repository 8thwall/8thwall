namespace Capnp {

public interface WritableByteChannel {
  int write(ByteBuffer src);
  bool isOpen();
  void close();
}

}
