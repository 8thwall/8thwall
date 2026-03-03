#include <cublas_v2.h>

#include <cstdio>
#include <cstdlib>

#define CUBLAS_CHECK(expr)                                                     \
  do {                                                                         \
    cublasStatus_t err = (expr);                                               \
    if (err != CUBLAS_STATUS_SUCCESS) {                                        \
      fprintf(stderr, "CUBLAS Error: %d at %s:%d\n", err, __FILE__, __LINE__); \
      exit(err);                                                               \
    }                                                                          \
  } while (0)

int main() {
  cublasHandle_t handle;
  CUBLAS_CHECK(cublasCreate(&handle));
  CUBLAS_CHECK(cublasSetMathMode(handle, CUBLAS_DEFAULT_MATH));
  printf("cublas handle created\n");
  return 0;
}
