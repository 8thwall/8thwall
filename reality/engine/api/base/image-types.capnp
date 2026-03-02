@0xf8e4ff06c42aa584;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("c8");

using Java = import "/capnp/java.capnp";
$Java.package("com.the8thwall.reality.engine.api.base");
$Java.outerClassname("ImageTypes");  # Must match this file's name!

using Cs = import "/capnp/cs.capnp";
$Cs.namespace("C8");

struct ImageUnion {
  oneOf :union {
    grayImagePointer @0 :GrayImagePointer;
    grayImageData @1 :GrayImageData;
    compressedImageData @2 :CompressedImageData;
  }
}

struct GrayImagePointer {
  rows @0 :Int32;
  cols @1 :Int32;
  bytesPerRow @2 :Int32;
  uInt8PixelDataPointer @3 :UInt64;
}

struct GrayImageData {
  rows @0 :Int32;
  cols @1 :Int32;
  bytesPerRow @2 :Int32;
  uInt8PixelData @3 :Data;
}

struct GrayImageData16 {
  rows @0 :Int32;
  cols @1 :Int32;
  bytesPerRow @2 :Int32;
  uInt16PixelData @3 :Data;
}

struct RGBAImageData {
  rows @0 :Int32;
  cols @1 :Int32;
  bytesPerRow @2 :Int32;
  uInt8PixelData @3 :Data;
}

struct CompressedImageData {
  enum Encoding {
    unspecified @0;
    rgb24 @1;
    rgb24InvertedY @2;
    jpgRgba @3;
    png @4;
  }

  width @0 :Int32;
  height @1 :Int32;
  data @2 :Data;
  encoding @3 :Encoding;
}

struct FloatImageData {
  rows @0 :Int32;
  cols @1 :Int32;
  elementsPerRow @2 :Int32;
  pixelData @3 :List(Float32);
}
