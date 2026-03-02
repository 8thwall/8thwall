#!/bin/bash --norc

set -eu

ARGS=( "$@" )
NUM_ARGS=$#
OUTPUT_NAME=''
DOCKER_RUN=0
XVFB=0
DOCKER_IMAGE=<REMOVED_BEFORE_OPEN_SOURCING>.dkr.ecr.us-west-2.amazonaws.com/amazonlinux8
METADATA_INDEX=

while [ $# -gt 0 ]
do
  case "$1" in
    -o)
      OUTPUT_INDEX=$(($NUM_ARGS - $# + 1))
      ;;
    -dockerrun)
      DOCKER_RUN=$(($NUM_ARGS - $#))
      ;;
    --xvfb)
      XVFB=$(($NUM_ARGS - $#))
      ;;
    -MF)
      METADATA_INDEX=$(($NUM_ARGS - $# + 1))
      ;;
  esac
  shift
done

OUTPUT_NAME=${ARGS[$OUTPUT_INDEX]}
OUTPUT_DIR=`dirname ${OUTPUT_NAME}`
BINARY=`basename ${OUTPUT_NAME}`

if [[ $DOCKER_RUN -gt 0 ]]; then
  unset ARGS[$DOCKER_RUN]
fi

if [[ $XVFB -gt 0 ]]; then
  unset ARGS[$XVFB]
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

ESCAPED_BAZEL_OUTPUT_BASE=$(echo "${BAZEL_OUTPUT_BASE}" | sed -e 's/[\/&]/\\&/g')

for i in ${!ARGS[@]}; do
    ARGS[i]=`echo ${ARGS[i]} | sed "s/BAZEL_OUTPUT_BASE/${ESCAPED_BAZEL_OUTPUT_BASE}/g"`
done

$BAZEL_OUTPUT_BASE/$LLVM_USR/bin/clang ${REWRITE_PATHS} ${ARGS[@]}

if [ ! -z "$METADATA_INDEX" ]; then
  # Rewrite the dependency file so that sources use relative paths to execroot.
  METADATA="${ARGS[$METADATA_INDEX]}"
  sed -i.bak "s|.*${ESCAPED_BAZEL_OUTPUT_BASE}|\/{{toolchain}}|" "${METADATA}"
  rm -f "${METADATA}.bak"
fi

if [[ $DOCKER_RUN -gt 0 ]]; then
  # Check that docker client is installed.
  hash docker 2>/dev/null || { echo >&2 "Missing docker. Try running 'brew install --cask docker'"; exit 1; }

  # Clean up previous image.
  docker rmi -f ${BINARY} &> /dev/null || true

  # Check that the ECR amazonlinux image is available locally.
  IMAGE=`docker images -q ${DOCKER_IMAGE}:latest`
  if [ -z "$IMAGE" ]; then
    echo
    echo "Missing amazonlinux Docker image. Try running the following commands:"
    echo "  aws ecr get-login-password --region us-west-2 | docker login --username AWS --password-stdin <REMOVED_BEFORE_OPEN_SOURCING>.dkr.ecr.us-west-2.amazonaws.com"
    echo "  docker pull <REMOVED_BEFORE_OPEN_SOURCING>.dkr.ecr.us-west-2.amazonaws.com/amazonlinux8"
    echo
    exit 1
  fi

# TODO(Nathan): get xvfb working so we can run this on our Mac.
  if [[ $XVFB -gt 0 ]]; then
    cat > ${OUTPUT_DIR}/Dockerfile << EOF
FROM ${DOCKER_IMAGE}:latest
RUN mkdir -p bin8
COPY ${BINARY} bin8/
ENTRYPOINT ["/bin/sh", "-c", "xvfb-run -a --server-args='-screen 0 640x480x24' bin8/${BINARY} \${C8_DOCKER_RUN_ARGS}"]
EOF
  else
    cat > ${OUTPUT_DIR}/Dockerfile << EOF
FROM ${DOCKER_IMAGE}:latest
RUN mkdir -p bin8
COPY ${BINARY} bin8/
ENTRYPOINT ["/bin/sh", "-c", "bin8/${BINARY} \${C8_DOCKER_RUN_ARGS}"]
EOF
  fi

  # Build a new image.
  # Use previous docker build system by setting DOCKER_BUILDKIT=0 in order for it to run correctly.
  # The -q will unfortunately quiet any errors that could happen during the docker build step but
  # if it works then it will return the image sha.
  BUILD_IMAGE_ID=`DOCKER_BUILDKIT=0 docker build -q -t ${BINARY} ${OUTPUT_DIR}`
  # Override the output with the docker run script.
  cat > $OUTPUT_NAME << EOF
#!/bin/bash --norc
set -eu
TAG=${BINARY}
IMAGE_ID=${BUILD_IMAGE_ID}
hash docker 2>/dev/null || { echo >&2 "Missing docker. Try running 'brew install --cask docker'"; exit 1; }

# We use $* in order to concatenate the arguments to the underlying executable being run inside
# the Docker container.  If we used $@, then we would have had to put the arguments in a string:
#    bazel run //path/to:docker-rule -- "--arg1 1 --arg2 2"
# Therefore, we're able to pass arguments in our standard format:
#   bazel run //path/to:docker-rule -- --arg1 1 --arg2 2
docker run --rm -t --env C8_DOCKER_RUN_ARGS="\$*" \$IMAGE_ID
EOF
fi
