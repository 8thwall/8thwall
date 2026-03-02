@0xae336800722d02bf;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("c8");

using Java = import "/capnp/java.capnp";
$Java.package("com.the8thwall.c8.io");
$Java.outerClassname("IoTest");  # Must match this file's name!

struct TestStruct {
  name @0 :Text;
  int @1 :Int32;
  float @2 :Float32;
  listStruct @3: List(SimpleStruct);
  nestedStruct @4: InnerStruct;
}

struct SimpleStruct {
  name @0: Text;
}

struct InnerStruct {
  data @0: Data;
  listStruct @1: List(SimpleStruct);
}
