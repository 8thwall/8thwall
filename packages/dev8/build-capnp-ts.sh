set -e

mkdir -p src/capnp/c8/protolog/api
mkdir -p src/capnp/c8/stats/api
mkdir -p src/capnp/reality/engine/api/device
mkdir -p src/capnp/reality/engine/api/base
mkdir -p src/capnp/reality/engine/api/request
mkdir -p src/capnp/reality/engine/api/response

bazel build //c8/protolog/api:log-request.capnp-ts && cp -f ../../bazel-bin/c8/protolog/api/log-request.capnp.ts src/capnp/c8/protolog/api

bazel build //c8/stats/api:detail.capnp-ts && cp -f ../../bazel-bin/c8/stats/api/detail.capnp.ts src/capnp/c8/stats/api
bazel build //c8/stats/api:summary.capnp-ts && cp -f ../../bazel-bin/c8/stats/api/summary.capnp.ts src/capnp/c8/stats/api
bazel build //c8/stats/api:histogram-types.capnp-ts && cp -f ../../bazel-bin/c8/stats/api/histogram-types.capnp.ts src/capnp/c8/stats/api


bazel build //reality/engine/api:reality.capnp-ts && cp -f ../../bazel-bin/reality/engine/api/reality.capnp.ts src/capnp/reality/engine/api

bazel build //reality/engine/api/device:info.capnp-ts && cp -f ../../bazel-bin/reality/engine/api/device/info.capnp.ts src/capnp/reality/engine/api/device

bazel build //reality/engine/api/base:camera-intrinsics.capnp-ts && cp -f ../../bazel-bin/reality/engine/api/base/camera-intrinsics.capnp.ts src/capnp/reality/engine/api/base
bazel build //reality/engine/api/base:debug.capnp-ts && cp -f ../../bazel-bin/reality/engine/api/base/debug.capnp.ts src/capnp/reality/engine/api/base
bazel build //reality/engine/api/base:geo-types.capnp-ts && cp -f ../../bazel-bin/reality/engine/api/base/geo-types.capnp.ts src/capnp/reality/engine/api/base
bazel build //reality/engine/api/base:id.capnp-ts && cp -f ../../bazel-bin/reality/engine/api/base/id.capnp.ts src/capnp/reality/engine/api/base
bazel build //reality/engine/api/base:image-types.capnp-ts && cp -f ../../bazel-bin/reality/engine/api/base/image-types.capnp.ts src/capnp/reality/engine/api/base
bazel build //reality/engine/api/base:lighting.capnp-ts && cp -f ../../bazel-bin/reality/engine/api/base/lighting.capnp.ts src/capnp/reality/engine/api/base
bazel build //reality/engine/api/base:feature-types.capnp-ts && cp -f ../../bazel-bin/reality/engine/api/base/feature-types.capnp.ts src/capnp/reality/engine/api/base

bazel build //reality/engine/api/request:app.capnp-ts && cp -f ../../bazel-bin/reality/engine/api/request/app.capnp.ts src/capnp/reality/engine/api/request
bazel build //reality/engine/api/request:flags.capnp-ts && cp -f ../../bazel-bin/reality/engine/api/request/flags.capnp.ts src/capnp/reality/engine/api/request
bazel build //reality/engine/api/request:mask.capnp-ts && cp -f ../../bazel-bin/reality/engine/api/request/mask.capnp.ts src/capnp/reality/engine/api/request
bazel build //reality/engine/api/request:sensor.capnp-ts && cp -f ../../bazel-bin/reality/engine/api/request/sensor.capnp.ts src/capnp/reality/engine/api/request

bazel build //reality/engine/api/response:features.capnp-ts && cp -f ../../bazel-bin/reality/engine/api/response/features.capnp.ts src/capnp/reality/engine/api/response
bazel build //reality/engine/api/response:pose.capnp-ts && cp -f ../../bazel-bin/reality/engine/api/response/pose.capnp.ts src/capnp/reality/engine/api/response
bazel build //reality/engine/api/response:sensor-test.capnp-ts && cp -f ../../bazel-bin/reality/engine/api/response/sensor-test.capnp.ts src/capnp/reality/engine/api/response
bazel build //reality/engine/api/response:status.capnp-ts && cp -f ../../bazel-bin/reality/engine/api/response/status.capnp.ts src/capnp/reality/engine/api/response
