#include <pybind11/pybind11.h>

int mult(int a, int b) {return a * b;}

PYBIND11_MODULE(example_native, m) {
  m.doc() = "example native module";
  m.def("mult",
        &mult, "Multiply integer numbers");
}
