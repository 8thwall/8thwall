#!/bin/bash

OUT=$1
shift

OUTPUT_DIR="$(dirname $APP_OUTPUT)"
PROJECT_DIR="$(dirname $(dirname $(dirname $APP_OUTPUT)))"

BUILD_SETTINGS="\
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
HOME=$PWD/${PROJECT_DIR}
BUILT_PRODUCTS_DIR=$PWD/${OUTPUT_DIR}"

echo "xcodebuildsettings.sh xcodebuild -version: $(/usr/bin/xcodebuild -version)"
/usr/bin/xcodebuild -showBuildSettings "$@" $BUILD_SETTINGS> ${OUT}
