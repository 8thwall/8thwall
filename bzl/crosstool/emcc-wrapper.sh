#!/bin/bash --norc

set -eu

# Get the realpath.
function get_realpath() {
    local previous="$1"
    local next=$(readlink "${previous}")
    while [ -n "${next}" ]; do
        previous="${next}"
        next=$(readlink "${previous}")
    done
    echo "${previous}"
}

ARGS=( "$@" )

OUTPUT_INDEX=-1
NUM_ARGS=$#
OUTPUT_NAME=''
METADATA_FILE_INDEX=-1

RED='\033[1;31m'
NOCOLOR='\033[0m'

while [ $# -gt 0 ]
do
  case "$1" in
    -o) OUTPUT_INDEX=$(($NUM_ARGS - $# + 1));;
    -MF) METADATA_FILE_INDEX=$(($NUM_ARGS - $# + 1));;
    -pthread) if [[ "${NIA_WASM_PTHREAD}" -ne 1 ]]; then
      echo -e "${RED}ERROR:${NOCOLOR} flag -pthread is not currently supported for code8 wasm" 1>&2 && exit 1
    fi;;
    #--wasm-actually-link) ACTUALLY_LINK=1;;
  esac
  shift
done

# Source BAZEL_OUTPUT_BASE.
. ${WORKSPACE_ENV}

SANDBOX_EXECDIR="$PWD"
EXECDIR=$(echo $SANDBOX_EXECDIR | sed -e "s/\/sandbox\/[^\/]*\/[0-9]*//")
REWRITE_PATHS=
if [ "$EXECDIR" != "$SANDBOX_EXECDIR" ]; then
  # ex: $(basename $EXECDIR) -> 'niantic'.
  REWRITE_PATHS="-ffile-prefix-map=$SANDBOX_EXECDIR="
fi
REWRITE_PATHS="${REWRITE_PATHS} -ffile-prefix-map=$BAZEL_OUTPUT_BASE/external=external"

ESCAPED_BAZEL_OUTPUT_BASE=$(echo "${BAZEL_OUTPUT_BASE}" | sed -e 's/[\/&]/\\&/g')

for i in ${!ARGS[@]}; do
    ARGS[i]=`echo ${ARGS[i]} | sed "s/BAZEL_OUTPUT_BASE/${ESCAPED_BAZEL_OUTPUT_BASE}/g"`
done

OUTPUT_NAME=${ARGS[$OUTPUT_INDEX]}

RUN_WRAPPER="${RUN_WRAPPER:-}"
NIA_WASM_PTHREAD="${NIA_WASM_PTHREAD:-}"

EXECROOT=$BAZEL_OUTPUT_BASE/execroot
TOOLCHAIN_PACKAGE="external/emscripten"
TOOLCHAIN="${BAZEL_OUTPUT_BASE}/${TOOLCHAIN_PACKAGE}"
TOOLCHAIN_ABS=`get_realpath $TOOLCHAIN`
TOOLCHAIN_EXTERNAL=`dirname $TOOLCHAIN_ABS`
EXECROOT_ABS=`get_realpath $EXECROOT`

export EM_CACHE=${TOOLCHAIN}/cache
export EM_CONFIG=${TOOLCHAIN}/emscripten.config
export EM_EXCLUSIVE_CACHE_ACCESS=1
export EMCC_WASM_BACKEND=1
export EMCC_SKIP_SANITY_CHECK=1

if [[ $RUN_WRAPPER = "emrun" ]] ; then
# If emrun, ensure that we generate an html output.
ARGS[$OUTPUT_INDEX]="${OUTPUT_NAME}.html"
fi

# Do the compilation or linking.
${TOOLCHAIN}/upstream/emscripten/emcc ${REWRITE_PATHS} ${ARGS[@]}

if [[ $RUN_WRAPPER = "emrun" ]] ; then
# If emrun, generate a self-extracting shell script with the html to open in a browser..
DOTS=$(echo $OUTPUT_NAME | sed 's/[^\/]*/\.\./g')
  cat > $OUTPUT_NAME << EOF
#!/bin/bash

set -u

THIS=\${BASH_SOURCE[0]}
RUNFILES_DIR=\${RUNFILES_DIR:-\$0.runfiles}
RELATIVE_EMRUN=$DOTS/../$TOOLCHAIN_PACKAGE/upstream/emscripten/emrun
EMRUN="\$(dirname \$(readlink \${THIS} || echo \${THIS}))/\${RELATIVE_EMRUN}"

OFFSET_WORKER=\`awk '/^#__WORKER_JS_ARCHIVE_BELOW__/ {print NR + 1; exit 0; }' \$THIS\`

LOCAL_ARCHIVE=\`mktemp\`

TMP_DIR=\$(mktemp -d)
LOCAL_ARCHIVE="\$TMP_DIR/$(basename $OUTPUT_NAME).html"

sed -n '/^#__HTML_ARCHIVE_BELOW__$/,/^#__WORKER_JS_ARCHIVE_BELOW__$/{/^#__HTML_ARCHIVE_BELOW__$/!{/^#__WORKER_JS_ARCHIVE_BELOW__$/!p;};}' \$THIS > "\$LOCAL_ARCHIVE"

WORKER_LOCAL_ARCHIVE="\$TMP_DIR/$(basename $OUTPUT_NAME).worker.js"
tail -n+\$OFFSET_WORKER \$THIS > "\$WORKER_LOCAL_ARCHIVE"

\$EMRUN --browser=$BROWSER "\$LOCAL_ARCHIVE"
EXEC_RESULT=\$?

rm -r "\$LOCAL_ARCHIVE"
rm -r "\$WORKER_LOCAL_ARCHIVE"
exit \$EXEC_RESULT

#__HTML_ARCHIVE_BELOW__
EOF
cat "${OUTPUT_NAME}.html" >> $OUTPUT_NAME

# if there is worker file
if [[ $NIA_WASM_PTHREAD = 1 ]] ; then
  echo "#__WORKER_JS_ARCHIVE_BELOW__" >> $OUTPUT_NAME
  cat "${OUTPUT_NAME}.worker.js" >> $OUTPUT_NAME
fi

rm "${OUTPUT_NAME}.html"
fi

if [[ $RUN_WRAPPER = "nodejs" ]]; then
  # If this is a nodejs wrapper, create a two-line shebang that will run the file with the hermetic node toolchain.
  DOTS=$(echo $OUTPUT_NAME | sed 's/[^\/]*/\.\./g')
  WRAPPER=$DOTS/../external/nodejs_host/bin/node
  sed -i.bak '1 i\
#!/bin/bash\
'"'":"'"' //; exec "\$(dirname \$(readlink \${BASH_SOURCE[0]} || echo $\{BASH_SOURCE[0]}))/'"$WRAPPER"'" "\$0" "\$@"' $OUTPUT_NAME
  rm -f "${OUTPUT_NAME}.bak"
fi

if [ $METADATA_FILE_INDEX -ge 0 ]
then
  # Rewrite the dependency file so that sources use relative paths to execroot.
  METADATA_FILE=${ARGS[${METADATA_FILE_INDEX}]}
  sed -i.bak "s|$TOOLCHAIN_EXTERNAL|/\{\{toolchain\}\}/external|" "${METADATA_FILE}"
  sed -i.bak "s|$EXECROOT_ABS|/\{\{toolchain\}\}|" "${METADATA_FILE}"
  rm -f "${METADATA_FILE}.bak"
fi
