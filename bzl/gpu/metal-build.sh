#!/bin/bash --norc

# This is wrapper for calling 'metal' through the 'xcrun' wrapper in XCode,
# which chooses the correct platform header and tools.

set -eu

# Source BAZEL_OUTPUT_BASE.
. ${WORKSPACE_ENV}

# Setup sysroot
SYSROOT="$BAZEL_OUTPUT_BASE/$WORKSPACE_ROOT/Xcode_app/Contents/Developer/Platforms/$SDK.platform/Developer/SDKs/$SDK.sdk"

if [ $XCODE_VERSION -ge 16 ]; then
  METAL_PATH="$BAZEL_OUTPUT_BASE/$METAL_FOLDER/current/bin/metal"
else
  METAL_PATH="$BAZEL_OUTPUT_BASE/$METAL_FOLDER/$PLATFORM/bin/metal"
fi

$METAL_PATH -isysroot $SYSROOT "$@"
