#!/bin/bash --norc
set -eu

# We establish the runfiles within the build tree, and get the npm bin
export RUNFILES_DIR=${RUNFILES_DIR:-$PWD/$0.runfiles}
NPM_BIN=${NPM_BIN:-$RUNFILES_DIR/niantic/bzl/node/npm}
NPM_BIN="$(pwd)/$NPM_BIN"

# Create a temp dir to copy all files to. This is to circumnavigate the
# issue that npm ignores symlinks and the build tree is full of them.
# rsync will copy not the symlinks but the sources themselves of the
# symlinks. We will do the packing in this temp dir
mkdir -p $PACKING_DIR
rsync -xkLRr "$@" $PACKING_DIR

# Hold on to the tarball designated output that we declare_files for in
# the .bzl impl
OUTPUT=$PWD/$OUTPUT

# Hold on to the npm cache that will be needed when we move into the
# packing dir
export npm_config_cache=$PWD/
cd $PACKING_DIR/$DIR

# Do the pack
tarball=$("$NPM_BIN" pack --silent)

# Move the tarball back to the OUTPUT so bazel can grab it
mv $tarball $OUTPUT
