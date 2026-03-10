#!/bin/bash --norc

set -eu

ARGS=( "$@" )
VALIDATOR=${VALIDATOR:-glslangValidator}
OPTIMIZER=${OPTIMIZER:-spirv-opt}

COMPILE=1

VALIDATOR_ARGS=()
OPTIMIZER_ARGS=()

OUTPUT=""

while [ $# -gt 0 ]
do
  case "$1" in
    -o)
      shift
      OUTPUT=("$1")
      ;;
    -O*)
      OPTIMIZER_ARGS+=("$1")
      ;;
    -Wo,*)
      OPTIMIZER_ARGS+=("${$1:4}")
      ;;
    --target-env=*)
      VALIDATOR_ARGS+=("$1")
      OPTIMIZER_ARGS+=("$1")
      ;;
    -D*)
      VALIDATOR_ARGS+=("$1")
      ;;
    *)
      VALIDATOR_ARGS+=("$1")
      ;;
  esac
  shift
done

SAFE_VAL_ARGS=${VALIDATOR_ARGS[@]+"${VALIDATOR_ARGS[@]}"}
SAFE_OPT_ARGS=${OPTIMIZER_ARGS[@]+"${OPTIMIZER_ARGS[@]}"}

# Actual compilation and optimization.
UNOPT="${OUTPUT%.*}-unoptimized.spv"
${VALIDATOR} ${SAFE_VAL_ARGS} -o $UNOPT
${OPTIMIZER} $UNOPT -o ${OUTPUT} ${SAFE_OPT_ARGS}
rm $UNOPT
