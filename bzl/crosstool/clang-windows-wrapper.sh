#!/bin/bash --norc

set -eu

ARGS=( "$@" )
NUM_ARGS=$#
OUTPUT_NAME=''
WINRUN=0
METADATA_INDEX=
: "${WINE64:=}"

while [ $# -gt 0 ]
do
  case "$1" in
    -o)
      OUTPUT_INDEX=$(($NUM_ARGS - $# + 1))
      ;;
    -MF)
      METADATA_INDEX=$(($NUM_ARGS - $# + 1))
      ;;
  esac
  shift
done

OUTPUT_NAME=${ARGS[$OUTPUT_INDEX]}

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

$BAZEL_OUTPUT_BASE/$LLVM_USR/bin/clang ${REWRITE_PATHS} ${ARGS[@]}

if [ ! -z "$METADATA_INDEX" ]; then
  # Rewrite the dependency file so that sources use relative paths to execroot.
  METADATA="${ARGS[$METADATA_INDEX]}"
  sed -i.bak "s|.*${ESCAPED_BAZEL_OUTPUT_BASE}|\/{{deceive_bazel_header_deps}}/|" "${METADATA}"
  rm -f "${METADATA}.bak"
fi

if [ ! -z $WINE64 ]; then

  TMP_OUTPUT_NAME=$OUTPUT_NAME-windows-run-sh
  cat > $TMP_OUTPUT_NAME << EOF
#!/bin/bash --norc
set -eu
WINE64="${BAZEL_OUTPUT_BASE}/${WINE64}"
hash \$WINE64 2>/dev/null || { echo >&2 "Missing wine64. Try running 'brew cask install wine-stable'"; exit 1; }
THIS=\${BASH_SOURCE[0]}
OFFSET=\`awk '/^#__BASE64_ARCHIVE_BELOW__/ {print NR + 1; exit 0; }' \$THIS\`
LOCAL_ARCHIVE=\`mktemp\`
tail -n+\$OFFSET \$THIS | base64 -D > \$LOCAL_ARCHIVE
chmod u+x \$LOCAL_ARCHIVE

# Rewrite XML_OUTPUT_FILE env variable used for gtest output as defaults exceed windows path character limits.
: "\${XML_OUTPUT_FILE:=}"
ORIGINAL_XML_OUTPUT_FILE=\${XML_OUTPUT_FILE:=}
if [ ! -z "\$XML_OUTPUT_FILE" ]; then
  export XML_OUTPUT_FILE=\$(mktemp)
fi

if [ \$? -eq 0 ]; then
  # Run the binary.
  WINEDEBUG=-all exec \$WINE64 \$LOCAL_ARCHIVE "\$@"

  if [ ! -z "\$XML_OUTPUT_FILE" ] && [ -f "\$XML_OUTPUT_FILE" ]; then
    # Copy the xml output to its expected location.
    mv "\$XML_OUTPUT_FILE" "\$ORIGINAL_XML_OUTPUT_FILE"
  fi
fi

#__BASE64_ARCHIVE_BELOW__
EOF
  base64 < $OUTPUT_NAME >> $TMP_OUTPUT_NAME
  mv -f $TMP_OUTPUT_NAME $OUTPUT_NAME
fi
