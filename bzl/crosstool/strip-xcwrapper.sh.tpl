#!/bin/bash --norc

# The Bazel strip action uses the default shell env, so we need to manually add the toolchain env vars.
export WORKSPACE_ENV="{{WORKSPACE_ENV}}"
export RELATIVE_EXTERNAL_TOOLCHAINS_DIR="{{RELATIVE_EXTERNAL_TOOLCHAINS_DIR}}"
export XCTOOLCHAIN="{{XCTOOLCHAIN}}"

# Source BAZEL_OUTPUT_BASE.
. ${WORKSPACE_ENV}

$BAZEL_OUTPUT_BASE/$RELATIVE_EXTERNAL_TOOLCHAINS_DIR/$XCTOOLCHAIN/usr/bin/llvm-strip "$@"
