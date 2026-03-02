#!/bin/bash
set -e

function verify_argument() {
  local arg=$1
  local arg_name=$2
  local optional_flag=$3

  if [ "$optional_flag" != "optional" ] && [ -z "$arg" ]; then
    echo "Error: Missing $arg_name argument." >&2
    return 1
  fi

  echo "$arg"
}

# The output directory for our framework, i.e. "foo/bar.framework".
FRAMEWORK_OUTPUT_PATH="$(verify_argument "$1" "framework_output_path")"
echo "Building framework at $FRAMEWORK_OUTPUT_PATH"
# The Info.plist template file path.
BINARY_PATH="$(verify_argument "$2" "binary_path")"
echo "Using binary at $BINARY_PATH"
# The Info.plist template file path.
INFO_PLIST_TEMPLATE="$(verify_argument "$3" "info_plist_template")"
# The name of the framework, e.g. "FooBar".
FRAMEWORK_NAME="$(verify_argument "$4" "framework_name")"
# The framework bundle identifier, e.g. "com.the8thwall.foo".
BUNDLE_IDENTIFIER="$(verify_argument "$5" "bundle_identifier")"
# The version of the framework, e.g. "1.0.0.123".
FRAMEWORK_VERSION="$(verify_argument "$6" "framework_version")"
# The short version of the framework, e.g. "1.0.0"
FRAMEWORK_VERSION_SHORT="$(verify_argument "$7" "framework_version_short")"
# The minimum OS version for the framework, e.g. "14.0" for iOS or "10.15" for macOS.
MIN_OS_VERSION="$(verify_argument "$8" "min_os_version")"

mkdir -p "$FRAMEWORK_OUTPUT_PATH"

INFO_PLIST_PATH="$FRAMEWORK_OUTPUT_PATH/Info.plist"

echo "INFO_PLIST_TEMPLATE: $INFO_PLIST_TEMPLATE"
echo "Writing Info.plist to $INFO_PLIST_PATH..."
sed -e "s|\$FRAMEWORK_NAME|$FRAMEWORK_NAME|g" \
    -e "s|\$BUNDLE_IDENTIFIER|$BUNDLE_IDENTIFIER|g" \
    -e "s|\$FRAMEWORK_VERSION_SHORT|$FRAMEWORK_VERSION_SHORT|g" \
    -e "s|\$FRAMEWORK_VERSION|$FRAMEWORK_VERSION|g" \
    -e "s|\$MIN_OS_VERSION|$MIN_OS_VERSION|g" \
    "$INFO_PLIST_TEMPLATE" > "$INFO_PLIST_PATH"

echo "Copying $BINARY_PATH binary to $FRAMEWORK_OUTPUT_PATH/$FRAMEWORK_NAME"

cp "$BINARY_PATH" "$FRAMEWORK_OUTPUT_PATH/$FRAMEWORK_NAME"
