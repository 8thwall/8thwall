// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/examples/example-methods.h"

#ifdef ANDROID
#include <jni.h>

extern "C" {

JNIEXPORT jstring JNICALL
Java_com_nianticlabs_bzl_examples_android_ExampleMethods_exampleString(JNIEnv *env) {
  return env->NewStringUTF(c8_exampleString());
}

JNIEXPORT jint JNICALL
Java_com_nianticlabs_bzl_examples_android_ExampleMethods_exampleInt(JNIEnv *env) {
  return c8_exampleInt();
}

}  // extern "C"
#endif  // ANDROID
