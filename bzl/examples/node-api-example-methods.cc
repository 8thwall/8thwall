#include <node_api.h>

#include "bzl/examples/example-methods.h"

extern "C" {

// Wrapper for the string method
napi_value exampleStringMethod(napi_env env, napi_callback_info info) {
  const char *result = c8_exampleString();

  napi_value str;
  napi_status status;
  status = napi_create_string_utf8(env, result, NAPI_AUTO_LENGTH, &str);
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Failed to create string");
    return nullptr;
  }

  return str;
}

// Wrapper for the int method
napi_value exampleIntMethod(napi_env env, napi_callback_info info) {
  int result = c8_exampleInt();

  napi_value num;
  napi_status status;
  status = napi_create_int32(env, result, &num);
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Failed to create int");
    return nullptr;
  }

  return num;
}

napi_value Init(napi_env env, napi_value exports) {
  napi_status status;

  napi_property_descriptor desc[] = {
    {"exampleStringMethod", 0, exampleStringMethod, 0, 0, 0, napi_default, 0},
    {"exampleIntMethod", 0, exampleIntMethod, 0, 0, 0, napi_default, 0}};

  status = napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Failed to define properties");
    return nullptr;
  }

  return exports;
}

NAPI_MODULE(NODE_API_EXAMPLE_METHODS, Init)

}  // extern "C"
