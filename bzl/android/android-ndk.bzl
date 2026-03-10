load("//bzl/android:android-version.bzl", "ANDROID_NDK_VERSION")

def android_ndk_runtime_libraries(target, **kwargs):
    native.filegroup(
        name = "{name}-dynamic-runtime-libraries".format(name = target),
        srcs = native.glob([
            "ndk/{ndk_ver}/sources/cxx-stl/llvm-libc++/libs/{arch}/*.so".format(ndk_ver = ANDROID_NDK_VERSION, arch = target),
        ]),
        **kwargs
    )

    native.filegroup(
        name = "{name}-static-runtime-libraries".format(name = target),
        # include all of the static runtime libraries, then ensure
        # libandroid_support.a is linked last.
        srcs = native.glob(
            [
                "ndk/{ndk_ver}/sources/cxx-stl/llvm-libc++/libs/{arch}/*.a".format(ndk_ver = ANDROID_NDK_VERSION, arch = target),
            ],
            exclude = [
                "ndk/{ndk_ver}/sources/cxx-stl/llvm-libc++/libs/{arch}/libandroid_support.a".format(ndk_ver = ANDROID_NDK_VERSION, arch = target),
            ],
        ) + native.glob([
            # This is a glob, since the support library only exists for the 32-bit architectures.
            "ndk/{ndk_ver}/sources/cxx-stl/llvm-libc++/libs/{arch}/libandroid_support.a".format(ndk_ver = ANDROID_NDK_VERSION, arch = target),
        ]),
        **kwargs
    )
