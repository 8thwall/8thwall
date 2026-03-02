// @rule(js_cli)
// @package(npm-examples-js-resolve)

// This tests if webpack is resolving transitive dependencies properly.

// is-buffer is a transitive dependency of flat, but under a different version.

// If we only include the top-level is-buffer, that is incorrect behavior.

// We can check this by running:
//   bazel build //bzl/examples/js/resolve:transitive-ts
//   grep "/is-buffer/" bazel-bin/bzl/examples/js/resolve/transitive-ts.js

// We should see both the following included:
//   node_modules/is-buffer
//   node_modules/flat/node_modules/is-buffer

import flat from 'flat'
import isBuffer from 'is-buffer'

console.log(
  flat,
  isBuffer
)
