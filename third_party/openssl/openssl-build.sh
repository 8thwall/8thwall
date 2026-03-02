#!/bin/bash --norc

# Cannot enable -e because it breaks in linux
# set -e
set -u

if [ ! -z "$TOOLCHAIN" ]; then
  export PATH=$TOOLCHAIN:$PATH
fi

external/openssl/Configure $CONFIGURE_FLAGS >/dev/null || external/openssl/Configure $CONFIGURE_FLAGS

# Execute the build.
# Forcing -j1 because anything higher break bazel which expects the output before that the Make
# completes the build
make -j1 >/dev/null || make

cp libcrypto.a $LIBCRYPTOP_PATH
cp libssl.a $LIBSSL_PATH
cp include/openssl/opensslconf.h $OPENSSL_H_PATH
# Copy opensslv.h from the source directory since it's not generated in build output
cp external/openssl/include/openssl/opensslv.h $OPENSSLV_H_PATH
