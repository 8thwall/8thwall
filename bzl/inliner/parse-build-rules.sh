#!/usr/bin/env bash

WORKING_DIR=`dirname $0`
# If you get an error about missing greadlink, run `brew install coreutils`.
DREF_WORKING_DIR=`greadlink -f $WORKING_DIR`
BIN="${DREF_WORKING_DIR}/parse-build-rules"
CC="${CC:-llvm-g++}"

if [ $1 ]; then
  SRC_DIR=`dirname $1`
  pushd "${SRC_DIR}" > /dev/null
  WORKSPACE=`bazel info workspace`
  popd > /dev/null
else
  WORKSPACE="."
fi

COPTS="-std=c++14 -fexceptions -fcxx-exceptions -I${WORKSPACE}"
SYSTEM_INCLUDES=`${CC} -Wp,-v -x c++ - -fsyntax-only < /dev/null 2>&1 | sed -e '/#include <...> search starts here:/,/End of search list\./!d;//d' -e 's/^ *\([^ ]*\).*/-I\1/' | awk '{ printf "%s ", $0 }'`
$BIN "$@" -- ${COPTS} ${SYSTEM_INCLUDES}
