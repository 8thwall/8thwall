#!/bin/bash --norc

set -eu

# Source BAZEL_OUTPUT_BASE.
. ${WORKSPACE_ENV}

ESCAPED_BAZEL_OUTPUT_BASE=$(echo "${BAZEL_OUTPUT_BASE}" | sed -e 's/[\/&]/\\&/g')

for i in ${!ARGS[@]}; do
    ARGS[i]=`echo ${ARGS[i]} | sed "s/BAZEL_OUTPUT_BASE/${ESCAPED_BAZEL_OUTPUT_BASE}/g"`
done

${BAZEL_OUTPUT_BASE}/${LLVM_USR}/bin/llvm-ar "$@"
