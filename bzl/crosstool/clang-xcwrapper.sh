#!/bin/bash --norc

# This is wrapper for calling 'clang' through the 'xcrun' wrapper in XCode,
# which chooses the correct platform header and tools. In addition, it has
# facilities for rewriting dependency-file paths, rpaths, and OSO symbols to
# ensure relative paths, sandbox path stripping, and build repeatibility.

set -eu

# Not avaiable in linux
# INSTALL_NAME_TOOL="/usr/bin/install_name_tool"
# Hence defining after sourcing WORKSPACE_ENV

LIBS=
LIB_DIRS=
RPATH=
OUTPUT=
LINKING=1
METADATA=
SONAME_IN_LINUX_HOST=1
SONAME_INDEX=

INDEX=0
SHARED_INDEX=-1
USE_BUNDLE=
ARGS=( "$@" )

INDEX=0
SHARED_INDEX=-1
USE_BUNDLE=
ARGS=( "$@" )

for i in "$@"; do

    if [[ "${OUTPUT}" = "1" ]]; then
        OUTPUT=$i
    elif [[ "${METADATA}" = "1" ]]; then
        METADATA=$i
    elif [[ "$i" =~ ^-l(.*)$ ]]; then
        # lib
        LIBS="${BASH_REMATCH[1]} $LIBS"
    elif [[ "$i" =~ ^-L(.*)$ ]]; then
        # lib
        LIB_DIRS="${BASH_REMATCH[1]} $LIB_DIRS"
    elif [[ "$i" =~ ^-Wl,-rpath,@loader_path/(.*)$ ]]; then
        # rpath
        RPATH=${BASH_REMATCH[1]}
    elif [[ "$i" = "-o" ]]; then
        # output is coming
        OUTPUT=1
    elif [[ "$i" = "-MF" ]]; then
        # metadata is coming
        METADATA=1
    elif [[ "$i" = "-c" || "$i" = "-E" ]]; then
        # This is not a linking invocation.
        LINKING=
    elif [[ "$i" = "-shared" ]]; then
        # This is not a linking invocation.
        SHARED_INDEX=${INDEX}
    # Temporarily disabling bundle because of iOS errors
    # elif [[ "$i" = "-DGENERATE_SHARED_BUNDLE" ]]; then
    #     USE_BUNDLE=1
    elif [[ "$i" == *"-soname"* ]]; then
        unameOut="$(/usr/bin/uname -s)"
        if [[ "${unameOut}" = "Linux"* ]]; then
            # ensure -install_name is used instead fp -soname when compiling on Linux for OSX
            SONAME_INDEX=${INDEX}
            SONAME_IN_LINUX_HOST=1
        fi
    fi
    INDEX=$((INDEX+1))
done

# Call the C++ compiler
# ex: /private/var/tmp/_bazel_parismorgan/3af9ce7ad8de660749ce982d2e42b5a8/sandbox/darwin-sandbox/53/execroot/niantic
SANDBOX_EXECDIR=$PWD
# ex: /private/var/tmp/_bazel_parismorgan/3af9ce7ad8de660749ce982d2e42b5a8/execroot/niantic
EXECDIR=$(echo $SANDBOX_EXECDIR | sed -e "s/\/sandbox\/[^\/]*\/[0-9]*//")
REWRITE_SO_PATHS=
OSO_PREFIX=

if [ "$EXECDIR" != "$SANDBOX_EXECDIR" ]; then
  # ex: $(basename $EXECDIR) -> 'niantic'.
  REWRITE_SO_PATHS="-ffile-prefix-map=$SANDBOX_EXECDIR="
fi

if [[ "${LINKING}" = "1" ]]; then
  if [ "$USE_BUNDLE" == "1" ]; then
    ARGS[$SHARED_INDEX]="-bundle"
  fi
  OSO_PREFIX="-Wl,-oso_prefix,${SANDBOX_EXECDIR}/"

  # Replace -soname with -install_name
  if [ "${SONAME_IN_LINUX_HOST}" == "1" ]; then
    ARGS[$SONAME_INDEX]=${ARGS[$SONAME_INDEX]/"-soname"/"-install_name"}
  fi
fi

# Source BAZEL_OUTPUT_BASE.
. ${WORKSPACE_ENV}

# Install name tool from the toolchain
INSTALL_NAME_TOOL="${BAZEL_OUTPUT_BASE}/${LLVM_USR}/bin/llvm-install-name-tool"

$BAZEL_OUTPUT_BASE/$LLVM_USR/bin/clang -ffile-prefix-map=$BAZEL_OUTPUT_BASE/external=external --sysroot=$BAZEL_OUTPUT_BASE/$PLATFORM_SYSROOT "${ARGS[@]}" $REWRITE_SO_PATHS $OSO_PREFIX -B "$BAZEL_OUTPUT_BASE/$LLVM_USR/bin/llvm-"

if [ ! -z "$METADATA" ]
then
  # Rewrite the dependency file so that sources use relative paths to execroot.
  sed -i.bak "s|.*$BAZEL_OUTPUT_BASE/$RELATIVE_DEVELOPER_DIR|\/{{toolchain}}/$RELATIVE_DEVELOPER_DIR|" "${METADATA}"
  sed -i.bak "s|.*$BAZEL_OUTPUT_BASE/$RELATIVE_EXTERNAL_TOOLCHAINS_DIR|\/{{toolchain}}/$RELATIVE_EXTERNAL_TOOLCHAINS_DIR|" "${METADATA}"
  rm -f "${METADATA}.bak"
fi

# A convenient method to return the actual path even for non symlinks
# and multi-level symlinks.
function get_realpath() {
    local previous="$1"
    local next=$(readlink "${previous}")
    while [ -n "${next}" ]; do
        previous="${next}"
        next=$(readlink "${previous}")
    done
    echo "${previous}"
}


function get_library_path() {
    for libdir in ${LIB_DIRS}; do
        if [ -f ${libdir}/lib$1.dylib ]; then
            echo "${libdir}/lib$1.dylib"
        fi
    done
}

# Get the path of a lib inside a tool
function get_otool_path() {
    # the lib path is the path of the original lib relative to the workspace
    get_realpath $1 | sed 's|^.*/bazel-out/|bazel-out/|'
}

# Do replacements in the output
if [ -n "${RPATH}" ]; then
    for lib in ${LIBS}; do
        libpath=$(get_library_path ${lib})
        if [ -n "${libpath}" ]; then
            ${INSTALL_NAME_TOOL} -change $(get_otool_path "${libpath}") "@loader_path/${RPATH}/lib${lib}.dylib" "${OUTPUT}"
        fi
    done
fi
