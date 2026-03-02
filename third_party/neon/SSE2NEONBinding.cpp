#include "SSE2NEONBinding.h"


#ifdef WIN32
#include <xmmintrin.h>
#include <emmintrin.h>

#include <malloc.h>
#include <crtdbg.h>
#define MEMALIGN ::memalign


#else
#include <stdlib.h>
#include <cstdlib>
#define MEMALIGN posix_memalign
#endif

namespace SSE2NEON
{

#ifdef WIN32
    void* platformAlignedAlloc(size_t size)
    {
        return _aligned_malloc(size, 16);
    }

    void platformAlignedFree(void* ptr)
    {
        _aligned_free(ptr);
    }

#else

    void* platformAlignedAlloc(size_t size)
    {
        return MEMALIGN(16, size);
    }

    void platformAlignedFree(void* ptr)
    {
        ::free(ptr);
    }

#endif

} // end of SSE2NEON namespace
