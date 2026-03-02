#pragma once

namespace c8flann {

#if (defined WIN32 || defined _WIN32 || defined WINCE) && defined CVAPI_EXPORTS
__declspec(dllexport)
#endif
  void dummyfunc();
}
