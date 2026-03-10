#include "bzl/examples/export-methods.h"
#include "bzl/examples/example-methods.h"

extern "C" {

C8_PUBLIC int c8_exampleIntMethod() { return c8_exampleInt(); }

C8_PUBLIC const char *c8_exampleStringMethod() { return c8_exampleString(); }

}  // extern "C"
