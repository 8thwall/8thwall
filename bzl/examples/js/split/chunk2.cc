#ifdef __EMSCRIPTEN__

#include <emscripten.h>
#include <stdio.h>

#include "c8/symbol-visibility.h"

extern "C" {

C8_PUBLIC
int c8EmAsm_getValue() { return 42; }

}  // extern "C"

#endif
