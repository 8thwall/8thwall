// 'Hello World!' sample application that throws.
//
// Example Usage
//   bazel run //bzl/examples:hello-exceptions

#include <cstdio>

int main() {
  printf("Hello, World!\n");
  try {
    throw 1;
  } catch (...) { }
}
