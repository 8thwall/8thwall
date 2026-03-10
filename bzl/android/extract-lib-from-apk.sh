#!/bin/bash

NAME=$1
APK=$2
OUTPUT_DIR=$3
OUTPUT_NAME=$4
UNZIP_DIR=$OUTPUT_DIR/$NAME.apk-files

rm -rf $UNZIP_DIR
unzip -q $APK -d $OUTPUT_DIR/$NAME.apk-files
ARCHS=`ls $UNZIP_DIR/lib | xargs`
NUM_ARCHS=`echo $ARCHS | awk '{print NF}'`

INPUTS=""
if [ $NUM_ARCHS == 1 ]; then
  # If only one architecture, copy it to the output.
  mv $UNZIP_DIR/lib/$ARCHS/lib*.so $OUTPUT_DIR/$OUTPUT_NAME
elif [ $NUM_ARCHS > 1 ]; then
  # If more than one architecture, create a fat library with lipo.
  for ARCH in $ARCHS; do
    case $ARCH in
      armeabi) LLVM_ARCH="arm";;
      armeabi-v7a) LLVM_ARCH="armv7";;
      arm64-v8a) LLVM_ARCH="arm64";;
      x86) LLVM_ARCH="i686";;
      x86_64) LLVM_ARCH="x86_64";;
      *) LLVM_ARCH=$ARCH;;
    esac
    INPUTS+="-arch $LLVM_ARCH $UNZIP_DIR/lib/$ARCH/lib*.so "
  done
  lipo -create $INPUTS -o $OUTPUT_DIR/$OUTPUT_NAME
fi

rm -rf $UNZIP_DIR

