#!/bin/bash

set -eu

XCPRETTY="${XCPRETTY:-xcpretty}"
export PATH=$PATH:/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin

# Kill any existing ibtoold. If not done, build will frequently intermittently
# fail when compiling storyboards and xib files.
# NOTE: the "|| :" allows script to continue to execute (see set -e) above,
# even if the process is not present.
pkill ibtoold || :
# Archive the xcode project into a .xcarchive
echo "xcodearchive.sh xcodebuild -version: $(/usr/bin/xcodebuild -version)"
/usr/bin/xcodebuild "$@" -archivePath ${XCARCHIVE_PATH} archive | "${XCPRETTY}" && exit ${PIPESTATUS[0]}
# Use this instead if you want full (verbose) clang output.
#/usr/bin/xcodebuild "$@" -archivePath ${XCARCHIVE_PATH} archive

# Kill any ibtoold at the end. This service doesn't seem to do well existing.
pkill ibtoold || :
