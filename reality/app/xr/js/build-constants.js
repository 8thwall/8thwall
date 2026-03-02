// NOTE(christoph): These constants are inserted at by the :build-constants bazel rule.

// @inliner-off
// eslint-disable-next-line no-undef
const IS_SIMD = !!XR8_IS_SIMD

const VERSION = 'XR8_VERSION'

export {
  IS_SIMD,
  VERSION,
}
