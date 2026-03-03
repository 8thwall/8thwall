@0xac605faf7f767b21;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("c8");

using Java = import "/capnp/java.capnp";
$Java.package("com.the8thwall.reality.engine.features.api");
$Java.outerClassname("Descriptors");  # Must match this file's name!

struct DescriptorDictionary {
  # Number of bytes in one descriptor.
  descriptorSize @0 :UInt32;

  # All of the descriptor data, concatenated into one blob. Size should be
  # equal to numDescriptors * descriptorSize.
  data @1 :Data;
}

struct PcaBasisData {
  # Number of floats in one basis vector.
  basisVectorSize @0 :UInt32;

  # All basis vectors concatenated in a block of memory in COLUMN-MAJOR order.
  basis @1 :List(Float32);

  # Amount to translate all input samples.
  # Dimensionality is equal to basisVectorSize.
  translation @2 :List(Float32);

  # Scale factor to apply to input samples.
  # Dimensionality is equal to basisVectorSize.
  scale @3 :List(Float32);

  # Scale factor to apply to projection for whitening.
  # Dimensionality is equal to basis.size() / basisVectorSize.
  whitening @4 :List(Float32);
}

struct BinaryPcaData {
  # Number of bits in one basis vector.
  basisVectorSize @0 :UInt32;

  # All basis vectors concatenated into a block of memory in ROW-MAJOR order.
  # Each Uint64 contains a block of 64-bits, of which each bit is a different
  # vector component.
  basis @1 :List(UInt64);

  # LookUpTable (LUT) of translations. Contains 256 or fewer elements.
  translationLut @2 :List(Float32);

  # Indexes of translations, which will be applied to all input samples before binarization.
  # Dimensionality is equal to basisVectorSize.
  translationIdx @3 :List(UInt8);

  # Scale factor to apply to projection for whitening.
  # Dimensionality is equal to (64 * basis.size()) / basisVectorSize.
  whitening @4 :List(Float32);
}
