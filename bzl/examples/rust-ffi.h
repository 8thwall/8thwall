#include <cstdio>
#include <cstdlib>

extern "C" {
int32_t rust_ffi_add(int32_t a, int32_t b);

char *rust_ffi_str_cat(const char *a, const char *b);
}
