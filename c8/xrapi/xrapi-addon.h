#include <android_native_app_glue.h>
#include <uv.h>
#include <v8.h>

#include <optional>

#include "c8/c8-log.h"
#include "c8/string.h"

namespace c8 {

v8::Local<v8::Object> initXrapiAddon(
  android_app *android_app, v8::Isolate *isolate, v8::Local<v8::Context> context, uv_loop_t *loop);

}  // namespace c8
