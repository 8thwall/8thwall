#include "rust-ffi.h"

#include <cstdio>
#include <cstdlib>

int main() {
  printf("2 + 2 is %d!\n", rust_ffi_add(2, 2));
  const char a[] = "Hello";
  const char b[] = " World!";
  char *c = rust_ffi_str_cat(a, b);
  printf("'%s' + '%s' is '%s'\n", a, b, c);
  free(c);
  return 0;
}
