load(
    "//bzl/android:android-version.bzl",
    "ANDROID_API_LEVELS",
    "ANDROID_BUILD_TOOLS_VERSION",
    "ANDROID_NDK_VERSION",
)

ANDROID_SDK_PACKAGES = [
    "build-tools;{version}".format(version = ANDROID_BUILD_TOOLS_VERSION),
    "extras;android;m2repository",
    "extras;google;m2repository",
    "ndk;%s" % ANDROID_NDK_VERSION,
    "platform-tools",
] + [
    "platforms;android-{api_level}".format(api_level = api_level)
    for api_level in ANDROID_API_LEVELS
]
