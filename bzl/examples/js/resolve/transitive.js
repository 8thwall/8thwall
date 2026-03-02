// This tests if webpack is resolving transitive dependencies properly.

// is-buffer is a transitive dependency of flat, but under a different version.

// If we only include the top-level is-buffer, that is incorrect behavior.

// We can check this by running:
//   bazel build //bzl/examples/js/resolve:transitive
//   grep "/is-buffer/" bazel-bin/bzl/examples/js/resolve/transitive.js

// We should see both the following included:
//   node_modules/is-buffer
//   node_modules/flat/node_modules/is-buffer

console.log(
  require('flat'),
  require('is-buffer')
)
