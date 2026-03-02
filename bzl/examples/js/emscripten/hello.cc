#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/fetch.h>

#include "c8/symbol-visibility.h"

extern "C" {

C8_PUBLIC int c8EmAsm_getAnswer() { return 42; }

}  // EXTERN "C"
