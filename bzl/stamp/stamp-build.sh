#!/bin/bash

# STABLE_ keys will cause a rebuild of stamp rules every time.

C8_VERSION="0.0.0.0"
C8_VERSION_FILE=".c8version"

if [ -f $C8_VERSION_FILE ]; then
  C8_VERSION=$(<$C8_VERSION_FILE)
fi

echo "STABLE_C8_RELEASE_BUILD_ID ${C8_VERSION}"
