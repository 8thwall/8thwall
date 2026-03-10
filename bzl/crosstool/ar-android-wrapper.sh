#!/bin/bash --norc

# Source BAZEL_OUTPUT_BASE.
. ${WORKSPACE_ENV}

PREBUILT="${BAZEL_OUTPUT_BASE}/${LLVM_PREBUILT}"

$PREBUILT/bin/llvm-ar "$@"
