#!/bin/bash --norc

# The Bazel strip action uses the default shell env, so we need to manually add the toolchain env vars.
export LLVM_PREBUILT="{{LLVM_PREBUILT}}"
export WORKSPACE_ENV="{{WORKSPACE_ENV}}"

# Source BAZEL_OUTPUT_BASE.
. ${WORKSPACE_ENV}

PREBUILT="${BAZEL_OUTPUT_BASE}/${LLVM_PREBUILT}"

$PREBUILT/bin/llvm-strip "$@"
