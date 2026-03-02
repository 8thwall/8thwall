#if defined(ANDROID) || defined(_WIN32) || defined(JAVASCRIPT) || defined(LINUX)
#include <cblas.h>
#else
#include <Accelerate/Accelerate.h>
#endif
