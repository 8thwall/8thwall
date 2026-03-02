#!/bin/sh

# $1 pass is expected to be a <cuda-toolkit>/version.json file to verify if it exists
# $2 is a commented version of that file to be included a dummy header for depedency bazel check

echo "Searching in $1"

if ! [ -f "$1" ];
then
  if [ "$3" = "system" ];
  then
    echo "// CUDA system version" > $2
    echo "" >> $2
    exit 0
  fi
  echo "Cuda was not configured to save disk space. "
  echo "If you need it, please run a 'bazel clean' first and then use one of: "
  echo "'--//bzl/gpu:cuda-support=hermetic' or "
  echo "'--//bzl/gpu:cuda-support=system' arguments"
  echo "If you used 'system' and you still see this message it might be because "
  echo "you don't have cuda in this host system."
  exit 1
else
  echo "// Dump of version.json" > $2
  echo "" > $2
  cat $1 | while read i; do echo "// $i" >> $2; done
fi
