#!/bin/bash --norc

set -eu

# Compiles a set of c# files into a .dll assemply, linking against unity libraries as needed.
#
# Usage:
#  unity-csharp-dll.sh -out:/path/to/OutputAssembly.dll  /path/to/Source1.cs /pat/to/Source2.cs

# Source BAZEL_EXECUTION_ROOT and BAZEL_OUTPUT_BASE
. ${WORKSPACE_ENV}

# Replace any $WORKSPACE AND $EXEC_ROOT_PATH in args with current directory.
NEWARGS=()

for ARG in "$@" ; do
  ARG="${ARG//\$WORKSPACE/${PWD}}"
  ARG="${ARG//\$EXEC_ROOT_PATH/${BAZEL_EXECUTION_ROOT}}"
  NEWARGS+=("${ARG}")
done

MONO=${BAZEL_OUTPUT_BASE}/${MONO}

MONO_BIN=${MONO}/bin/mono
MCS=${MONO}/lib/mono/4.5/mcs.exe
CONTENTS_DIR=$(dirname $(dirname $UNITY))
UNITY_MANAGED="${CONTENTS_DIR}/Managed"
MCS_ARGS="-r:${UNITY_MANAGED}/UnityEngine.dll -debug- -optimize+ -r:${UNITY_MANAGED}/UnityEditor.dll -target:library -sdk:2"

# The following mimics what happens in the /bin/mcs script, but with correct paths to the install location
export PATH=$PATH:${MONO}/bin
export PKG_CONFIG_PATH=${MONO}/lib/pkgconfig:${MONO}/share/pkgconfig

# Run the mono mcs build.
$MONO_BIN $MCS ${MCS_ARGS} "${NEWARGS[@]}"
