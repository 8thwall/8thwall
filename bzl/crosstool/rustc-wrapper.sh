#!/bin/bash --norc
set -eu

# Source BAZEL_OUTPUT_BASE.
. "{{WORKSPACE_ENV}}"

export SDKROOT="${BAZEL_OUTPUT_BASE}/external/xcode{{xcode_version}}/Xcode_app/Contents/Developer/Platforms/{{platform}}.platform/Developer/SDKs/{{platform}}.sdk"

PREBUILT="${BAZEL_OUTPUT_BASE}/{{RUSTC}}"

$PREBUILT "$@"
