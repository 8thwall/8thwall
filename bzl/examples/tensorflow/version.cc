#include <cstdio>

#include "tensorflow/lite/c/c_api.h"

int main() {
  auto tfVersion = TfLiteVersion();
  printf("Using TensorFlow Lite version %s\n", tfVersion);
  return 0;
}
