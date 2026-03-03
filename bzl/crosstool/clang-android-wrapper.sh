#!/bin/bash --norc

set -eu

ARGS=( "$@" )
OUTPUT_NAME=''
NUM_ARGS=$#
ADB_RUN=0
OUTPUT_INDEX=
METADATA_INDEX=
SOLIB_RPATHS=()
SOLIB_DIRS=()
RPATH_PREFIX="-Wl,-rpath,\$EXEC_ORIGIN/"
: "${ADB:=}"

while [ $# -gt 0 ]
do
  case "$1" in
    $RPATH_PREFIX*)
      SOLIB_RPATHS+=( ${1#$RPATH_PREFIX} )
      ;;
    -o)
      OUTPUT_INDEX=$(($NUM_ARGS - $# + 1))
      ;;
    -MF)
      METADATA_INDEX=$(($NUM_ARGS - $# + 1))
      ;;
    --run-with-adb)
      ADB_RUN=$(($NUM_ARGS - $#))
      ;;
  esac
  shift
done

if [ ! -z "$OUTPUT_INDEX" ]; then

OUTPUT_NAME=${ARGS[$OUTPUT_INDEX]}
PACKAGE_DIR=$(dirname $OUTPUT_NAME | cut -d'/' -f4-)

for rpath in ${SOLIB_RPATHS[@]+"${SOLIB_RPATHS[@]}"}
do
     SOLIB_DIRS+=( "\"${PACKAGE_DIR}/$rpath\"" )
done

fi

if [[ $ADB_RUN -gt 0 ]]; then
  unset ARGS[$ADB_RUN]

  for i in ${!ARGS[@]}; do
    if [[ ${ARGS[$i]} == $RPATH_PREFIX* ]]; then
      ARGS[$i]="-Wl,-rpath,\$ORIGIN/$(basename ${ARGS[$i]#$RPATH_PREFIX})"
    fi
  done
fi

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

PREBUILT="${BAZEL_OUTPUT_BASE}/${LLVM_PREBUILT}"

# Compile or Link.
${PREBUILT}/bin/clang --gcc-toolchain=${PREBUILT} --sysroot=${PREBUILT}/sysroot ${REWRITE_PATHS} ${ARGS[@]}

if [ ! -z "$METADATA_INDEX" ]
then
  # Rewrite the dependency file so that sources use relative paths to execroot.
  METADATA=${ARGS[$METADATA_INDEX]}
  sed -i.bak "s|.*$BAZEL_OUTPUT_BASE|\/{{toolchain}}|" "${METADATA}"
  rm -f "${METADATA}.bak"

fi

TMP_OUTPUT_NAME=$OUTPUT_NAME-adb-run-sh

if [ $ADB_RUN -gt 0 ] && [ ! -z "$ADB" ]; then
  cat > $TMP_OUTPUT_NAME << EOL
#!/bin/bash

THIS=\${BASH_SOURCE[0]}
RUNFILES_DIR=\${RUNFILES_DIR:-\$0.runfiles}

ADB="${BAZEL_OUTPUT_BASE}/${ADB}"

SOLIB_DIRS=( ${SOLIB_DIRS[@]+"${SOLIB_DIRS[@]}"} )

REMOTE_TMP_PREFIX="/data/local/tmp"
REMOTE_TMPDIR=\$REMOTE_TMP_PREFIX/androidrun.\`date +%s\`-\$RANDOM
\$ADB shell "mkdir \${REMOTE_TMPDIR}"

if (( \$? )); then
  # Failed to create tmp directory.
  exit 1
fi

BIN_BASE=\`basename \$THIS\`
REMOTE_BIN=\$REMOTE_TMPDIR/\$BIN_BASE
REMOTE_ARCHIVE=\$REMOTE_BIN.gz

OFFSET=\`awk '/^#__BASE64_ARCHIVE_BELOW__/ {print NR + 1; exit 0; }' \$THIS\`

LOCAL_ARCHIVE=\`mktemp\`

tail -n+\$OFFSET \$THIS | base64 -D > \$LOCAL_ARCHIVE

# Push the binary to the phone.
\$ADB push \$LOCAL_ARCHIVE \$REMOTE_ARCHIVE > /dev/null

LOCAL_TMPDIR=$(mktemp -d -t androidrun)
if [ -z "\$LOCAL_TMPDIR" ]; then
  # Failed to create local tmp directory.
  exit 1
fi

# This converts symlinks in runfiles to real files
rsync -aL \$RUNFILES_DIR \$LOCAL_TMPDIR/
# Copy the runfiles
\$ADB push \$LOCAL_TMPDIR/. \$REMOTE_TMPDIR
rm -rf \$LOCAL_TMPDIR

# Push any dynamic libraries to the phone.
for rpath in \${SOLIB_DIRS[@]+"\${SOLIB_DIRS[@]}"}
do
     SRC_DIR="\$RUNFILES_DIR/$(basename $PWD)/\${rpath}"
     DST_DIR="\${REMOTE_TMPDIR}/\$(basename \$rpath)"
     \$ADB shell "mkdir \$DST_DIR"
     find -L \$SRC_DIR -type f -name '*.so' -exec \$ADB push {} \$DST_DIR \\; > /dev/null
done

EXEC_RESULT=1

if [ \$? -eq 0 ]; then
  # Unzip the binary.
  \$ADB shell "gzip -d < \$REMOTE_ARCHIVE > \$REMOTE_BIN"

  \$ADB shell chmod 755 \$REMOTE_BIN

  # Run the executable, if the base path is under runfiles then run in that directory on the device.
  if [[ "\$PWD" =~ .*"\$BIN_BASE.runfiles/_main" ]]; then
    \$ADB shell "cd '\${REMOTE_TMPDIR}/\$BIN_BASE.runfiles/_main' && '\${REMOTE_BIN}' \${@}"
  else
    \$ADB shell exec "\${REMOTE_BIN} \${@}"
  fi
  EXEC_RESULT=\$?
fi

rm -f \$LOCAL_ARCHIVE

# Clean up.
\$ADB shell rm -rf \$REMOTE_TMPDIR

exit \$EXEC_RESULT

#__BASE64_ARCHIVE_BELOW__
EOL
  gzip -c $OUTPUT_NAME | base64 >> $TMP_OUTPUT_NAME
  mv -f $TMP_OUTPUT_NAME $OUTPUT_NAME
fi
