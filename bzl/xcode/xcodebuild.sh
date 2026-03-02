#!/bin/bash

set -eu

OUTPUT_DIR="$(dirname $APP_OUTPUT)"
PROJECT_DIR="$(dirname $(dirname $(dirname $APP_OUTPUT)))"

BUILD_SETTINGS="
SRCROOT=$PWD/${XCODE_SRCROOT}
PROJECT_DIR=$PWD/${XCODE_SRCROOT}
ONLY_ACTIVE_ARCH=NO
MODULE_CACHE_DIR=\$(SRCROOT)/DerivedData/ModuleCache
OBJROOT=\$(SRCROOT)/Intermediates
DISABLE_MANUAL_TARGET_ORDER_BUILD_WARNING=1
SHARED_PRECOMPS_DIR=\$(SRCROOT)/Intermediates/PrecompiledHeaders
SYMROOT=\$(SRCROOT)/Products
USER_HEADER_SEARCH_PATHS=Frameworks/${PRODUCT_NAME}Deps.framework/Headers
PROJECT_TEMP_DIR=$PWD/${PROJECT_DIR}/Intermediates/${PRODUCT_NAME}.build
CONFIGURATION_BUILD_DIR=$PWD/${OUTPUT_DIR}
BUILT_PRODUCTS_DIR=$PWD/${OUTPUT_DIR}"

XCPRETTY="${XCPRETTY:-xcpretty}"
export PATH="${PATH:-/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin}"

# Kill any existing ibtoold. If not done, build will frequently intermittently
# fail when compiling storyboards and xib files.
# NOTE: the "|| :" allows script to continue to execute (see set -e) above,
# even if the process is not present.
pkill ibtoold || :

# Build the XCode project to output .app binary
echo "xcodebuild.sh xcodebuild -version: $(/usr/bin/xcodebuild -version)"
/usr/bin/xcodebuild "$@" -derivedDataPath "${PWD}/${PROJECT_DIR}/DerivedData" $BUILD_SETTINGS | "${XCPRETTY}" && exit ${PIPESTATUS[0]}
# Use this instead if you want full (verbose) clang output.
#/usr/bin/xcodebuild "$@" -derivedDataPath "${PWD}/${PROJECT_DIR}/DerivedData" $BUILD_SETTINGS

# Kill any ibtoold at the end. This service doesn't seem to do well existing.
pkill ibtoold || :
