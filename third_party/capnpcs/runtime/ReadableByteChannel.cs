namespace Capnp {

public interface ReadableByteChannel {
  int read(ByteBuffer dst);
  bool isOpen();
  void close();
}

}
