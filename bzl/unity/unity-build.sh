#!/bin/bash

# Source BAZEL_OUTPUT_BASE.
. ${WORKSPACE_ENV}

echo ">>>>> Using the following WORKSPACE_ENV:"
cat ${WORKSPACE_ENV}

# Replace any $WORKSPACE in args with current directory.
NEWARGS=()
INDEX=0

echo ">>>>> Running Unity build with the following arguments @:"
echo "$@"

for ARG in "$@" ; do
  if [[ "$ARG" = "-projectPath" ]]; then
    PROJECT_PATH_INDEX=${INDEX}
  fi

  ARG="${ARG//\$WORKSPACE/${BAZEL_WORKSPACE}}"
  NEWARGS+=("${ARG}")

  INDEX=$((INDEX+1))
done

echo ">>>>> Running Unity build with the following arguments NEWARGS:"
echo "${NEWARGS[@]}"

# Give Unity access to certain common path directories, needed for tools like xcrun.
export PATH="/usr/bin:/bin:/usr/sbin:${PATH}"

export XDG_CONFIG_HOME="${PWD}/${OUT_DIR}/_xdg_config"
export XDG_CACHE_HOME="${PWD}/${OUT_DIR}/_xdg_cache"
export XDG_DATA_HOME="${PWD}/${OUT_DIR}/_xdg_data"
export XDG_STATE_HOME="${PWD}/${OUT_DIR}/_xdg_state"
export HOME="${PWD}/${OUT_DIR}/_home"

UNITY=${UNITY:-/Applications/Unity/Unity.app/Contents/MacOS/Unity}
PYTHON3=${PYTHON3:-python3}
UNITY_PROJECT_ROOT=${UNITY_PROJECT_ROOT:-.}
FORMATTER=./bzl/unity/unity-compile-formatter.py

# Checking if ${NEWARGS[$((PROJECT_PATH_INDEX+1))]} is a directory and exists
if [ ! -d "${NEWARGS[$((PROJECT_PATH_INDEX+1))]}" ]; then
  echo ">>>>> Error: Project path ${NEWARGS[$((PROJECT_PATH_INDEX+1))]} does not exist or is not a directory."
  exit 1
else 
  echo ">>>>> Project path ${NEWARGS[$((PROJECT_PATH_INDEX+1))]} exists."
fi

# Clean Library directory
rm -rf "${NEWARGS[$((PROJECT_PATH_INDEX+1))]}/Library"

# Run a Unity build.
echo ">>>>> Running Unity build with the following arguments:"
echo "${UNITY} ${NEWARGS[@]}"
${UNITY} "${NEWARGS[@]}" 2>&1 | tee unity.log 
# cat unity.log
# cat unity.log | ${PYTHON3} ${FORMATTER} && PS=${PIPESTATUS[0]}
# LOGFILE=$(echo "${NEWARGS[@]}" | perl -pe "s#.*?-logFile (.*?) .*#\1#g")
# if [ -f "$LOGFILE" ]
# then
#   cat $LOGFILE | ${PYTHON3} ${FORMATTER}
# fi
# exit $PS

# Uncomment and use this instead of the above line for raw (unformatted) output and debugging
${UNITY} "${NEWARGS[@]}" && exit ${PIPESTATUS[0]}
