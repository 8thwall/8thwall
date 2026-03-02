# Used to upload semantics-worker.js onto cdn. NOTE: You will need to update WORKER_URL and SIMD_WORKER_URL in 'layers-controller.ts' after.

# default
bazel build --config=wasmrelease //reality/app/xr/js/src/semantics:semantics-worker
node ./apps/client/public/web/cdn/resources/upload-resource.js $HOME/repo/code8/bazel-bin/reality/app/xr/js/src/semantics/semantics-worker.js

# simd
bazel build --config=wasmreleasesimd //reality/app/xr/js/src/semantics:semantics-worker
cp $HOME/repo/code8/bazel-bin/reality/app/xr/js/src/semantics/semantics-worker.js $HOME/repo/code8/bazel-bin/reality/app/xr/js/src/semantics/semantics-worker-simd.js
node ./apps/client/public/web/cdn/resources/upload-resource.js $HOME/repo/code8/bazel-bin/reality/app/xr/js/src/semantics/semantics-worker-simd.js
