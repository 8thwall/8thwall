#!/bin/bash --norc

set -eu

# Source BAZEL_OUTPUT_BASE.
. ${WORKSPACE_ENV}

${BAZEL_OUTPUT_BASE}/${LLVM_USR}/bin/llvm-ar "$@"
