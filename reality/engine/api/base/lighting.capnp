@0x88b77ac89dc4caec;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("c8");

using Java = import "/capnp/java.capnp";
$Java.package("com.the8thwall.reality.engine.api.base");
$Java.outerClassname("Lighting");  # Must match this file's name!

struct GlobalIllumination {
  # Measures the overall light content of the scene. -1 is completely underexposed (all dark); 0 is
  # well-exposed, and 1 is overexposed (all light).
  exposure @0: Float32;

  # Measures the overall color cast of the scene. 6500 is white while lower values are
  # reddish orange, and higher values have a blue cast. 1000 is an extreme reddish value, while
  # 27000 is an extreme blusish value.
  #
  # See https://en.wikipedia.org/wiki/Color_temperature for more info.
  temperature @1: Float32;
}
