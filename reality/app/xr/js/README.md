# Engine

## Running
First, serve the engine:

```bash
bazel run --config=wasm //reality/app/xr/js:serve-xr
```

Then use the served `xr.js` file in your project, e.g. `https://192.168.68.65:8888/reality/app/xr/js/xr.js`.

## Building
To build the engine for distribution, run:
```bash
bazel build --config=wasmreleasesimd //reality/app/xr/js:bundle
```

Or, if building for a non-SIMD environment, run:
```bash
bazel build --config=wasmrelease //reality/app/xr/js:bundle
```

## Using the open source engine alongside the distributed engine binary

> [!WARNING]
> This approach is a work in progress, the real end state will be a version which doesn't require you to serve the open source engine alongside your app.

This open source version of the engine doesn't include SLAM. But the [distributed engine binary](https://github.com/8thwall/engine) does. To use the open source engine for the camera pipeline and the distributed engine binary for SLAM, you can do the following:

1. Host the open source engine with:
```bash
cd ~/repo/8thwall
bazel run --config=wasmreleasesimd //reality/app/xr/js:serve-xr
```
2. Take note of the IP address in the logs
3. In your app, serve the distributed engine binary alongside the app. If you downloaded your app from 8thWall.com, it will already do this. An example file structure for your app is:
```
my-app/
  ├── external/
  │   └── xr/
  │       ├── xr.js       # Distributed engine binary entry point - with this approach, we don't use xr.js.
  │       └── xr-slam.js  # The SLAM chunk - this is what we instruct the open source engine to load.
  ├── src/
  │   ├── app.js
  │   ├── index.html
  │   └── ...
  ├── config/
  │   └── webpack.config.js
  └── package.json
```

4. In your app, switch to the open source engine by updating `my-app/src/index.html` from:
```html
<script crossorigin="anonymous" src="./external/xr/xr.js" data-preload-chunks="slam">
```
to:
```html
<script crossorigin="anonymous" src="https://192.168.68.55:8888/reality/app/xr/js/xr.js" async data-preload-chunks="slam: ./external/xr/xr-slam.js"></script>
```
Use the IP address from step 2., or if you have the engine uploaded elsewhere, you can use that domain.
