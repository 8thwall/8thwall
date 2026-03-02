#!/bin/bash

OUTPUT=$1
DIR=$2

# Skip to the first file param.
shift
shift

# Remove any previous manifest
rm -f $OUTPUT

for arg
do
  ASSET=$DIR/$arg
  SIZE=`wc -c < "$ASSET" | tr -d '[:space:]'`
  SHA1=`openssl sha1 < "$ASSET" | cut -d' ' -f2`
  echo "$arg $SIZE $SHA1" >> $OUTPUT
done
