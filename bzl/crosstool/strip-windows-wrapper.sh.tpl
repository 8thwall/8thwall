#!/bin/bash --norc

set -eu

# The Bazel strip action uses the default shell env so it won't accept the environment variables we
# pass in, so we need to manually add the toolchain env vars.
export LLVM_USR="{{LLVM_USR}}"
export WORKSPACE_ENV="{{WORKSPACE_ENV}}"

# Source BAZEL_OUTPUT_BASE.
. ${WORKSPACE_ENV}

${BAZEL_OUTPUT_BASE}/${LLVM_USR}/bin/llvm-strip "$@"
