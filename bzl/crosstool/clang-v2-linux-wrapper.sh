#!/bin/bash --norc

set -eu

ARGS=( "$@" )
NUM_ARGS=$#
OUTPUT_NAME=''
DOCKER_RUN=0
XVFB=0
OUTPUT_INDEX=
METADATA_INDEX=

PATH=${PATH}:/bin:/usr/bin
while [ $# -gt 0 ]
do
  case "$1" in
    -o)
      OUTPUT_INDEX=$(($NUM_ARGS - $# + 1))
      ;;
    -MF)
      METADATA_INDEX=$(($NUM_ARGS - $# + 1))
      ;;
  esac
  shift
done

if [ -n "$OUTPUT_INDEX" ]; then
  OUTPUT_NAME=${ARGS[$OUTPUT_INDEX]}
  OUTPUT_DIR=`dirname ${OUTPUT_NAME}`
  BINARY=`basename ${OUTPUT_NAME}`
fi

if [[ $DOCKER_RUN -gt 0 ]]; then
  unset ARGS[$DOCKER_RUN]
fi

if [[ $XVFB -gt 0 ]]; then
  unset ARGS[$XVFB]
fi

# Source BAZEL_OUTPUT_BASE.
. ${WORKSPACE_ENV}

SANDBOX_EXECDIR="$PWD"
EXECDIR=$(echo $SANDBOX_EXECDIR | sed -e "s/\/sandbox\/[^\/]*\/[0-9]*//")
REWRITE_PATHS=
if [ "$EXECDIR" != "$SANDBOX_EXECDIR" ]; then
  # ex: $(basename $EXECDIR) -> 'niantic'.
  REWRITE_PATHS="-ffile-prefix-map=$SANDBOX_EXECDIR="
fi
REWRITE_PATHS="${REWRITE_PATHS} -ffile-prefix-map=$BAZEL_OUTPUT_BASE/external=external"

ESCAPED_BAZEL_OUTPUT_BASE=$(echo "${BAZEL_OUTPUT_BASE}" | sed -e 's/[\/&]/\\&/g')

for i in ${!ARGS[@]}; do
    ARGS[i]=`printf "%s" "${ARGS[i]}" | sed "s/BAZEL_OUTPUT_BASE/${ESCAPED_BAZEL_OUTPUT_BASE}/g"`
done

$BAZEL_OUTPUT_BASE/$LLVM_USR/bin/clang ${REWRITE_PATHS} "${ARGS[@]}"

if [ ! -z "$METADATA_INDEX" ]; then
  # Rewrite the dependency file so that sources use relative paths to execroot.
  METADATA="${ARGS[$METADATA_INDEX]}"
  sed -i.bak "s|.*${ESCAPED_BAZEL_OUTPUT_BASE}|\/{{deceive_bazel_header_deps}}|" "${METADATA}"
  rm -f "${METADATA}.bak"
fi
