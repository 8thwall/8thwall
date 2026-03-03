#!/bin/bash

set -eu

XCPRETTY="${XCPRETTY:-xcpretty}"
export PATH="${PATH:-/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin}"

# Kill any existing ibtoold. If not done, build will frequently intermittently
# fail when compiling storyboards and xib files.
# NOTE: the "|| :" allows script to continue to execute (see set -e) above,
# even if the process is not present.
pkill ibtoold || :

# Export the xcode archive into a .ipa binary
echo "xcodeexport.sh xcodebuild -version: $(/usr/bin/xcodebuild -version)"
/usr/bin/xcodebuild -exportArchive -archivePath ${XCARCHIVE_PATH} -exportPath ${APP_OUTPUT_DIR} -exportOptionsPlist ${EXPORT_OPTIONS_PLIST_PATH} | "${XCPRETTY}" && exit ${PIPESTATUS[0]}
# Use this instead if you want full (verbose) clang output.
#/usr/bin/xcodebuild -exportArchive -archivePath ${XCARCHIVE_PATH} -exportPath ${APP_OUTPUT_DIR} -exportOptionsPlist ${EXPORT_OPTIONS_PLIST_PATH}

# Kill any ibtoold at the end. This service doesn't seem to do well existing.
pkill ibtoold || :
