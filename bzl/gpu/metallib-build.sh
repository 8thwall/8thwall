#!/bin/bash --norc

# This is wrapper for calling 'metallib' through the 'xcrun' wrapper in XCode,
# which chooses the correct platform header and tools.

set -eu

# Source BAZEL_OUTPUT_BASE.
. ${WORKSPACE_ENV}

if [ $XCODE_VERSION -ge 16 ]; then
  METALLIB_PATH="$BAZEL_OUTPUT_BASE/$METAL_FOLDER/current/bin/metallib"
else
  METALLIB_PATH="$BAZEL_OUTPUT_BASE/$METAL_FOLDER/$PLATFORM/bin/metallib"
fi

$METALLIB_PATH $@
