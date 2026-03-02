#!/bin/bash --norc

set -eu

# Source BAZEL_OUTPUT_BASE.
. ${WORKSPACE_ENV}

TOOLCHAIN="${BAZEL_OUTPUT_BASE}/external/emscripten"

export EM_CACHE=${TOOLCHAIN}/cache
export EM_CONFIG=${TOOLCHAIN}/emscripten.config
export EM_EXCLUSIVE_CACHE_ACCESS=1
export EMCC_WASM_BACKEND=1
export EMCC_SKIP_SANITY_CHECK=1

${TOOLCHAIN}/upstream/emscripten/emar $@
