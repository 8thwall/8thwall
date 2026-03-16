# Engine
This engine packages contains an open source version of the 8th Wall engine with SLAM, VPS, and Hand Tracking removed. The SLAM, VPS, and Hand Tracking algorithms remain proprietary to Niantic Spatial, but other AR features, such as Face Tracking, Image Target tracking, Sky Segmentation are included. Because this core framework is open source, if browser APIs evolve or change, this open source engine code can adjust as needed.

## Usage
Today, the easiest way to add the engine is to use the [Distributed Engine Binary](https://github.com/8thwall/engine), which also supports SLAM. We will also be working on official releases of the open source engine through npm.

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
This open source version of the engine doesn't include SLAM. But the [distributed engine binary](https://github.com/8thwall/engine) does. To use the open source engine for the camera pipeline and the distributed engine binary for SLAM, you can do the following:

1. In `8thwall/reality/app/xr/js/src/chunk-loader.ts`, update from:
```cpp
const slamChunk = await import(/* webpackIgnore: true */ chunkBaseUrl + 'xr-tracking.js')
```
to:
```cpp
// @ts-ignore
// eslint-disable-next-line
const slamChunk = await import(/* webpackIgnore: true */ slamUrl)
```
This will update the engine to load the SLAM chunk not from the open source engine, but from the distributed engine binary which we will serve alongside your app.

2. Host the open source engine. Right now, we have only tested with a locally served open source engine. But you can host it anywhere you want. To serve the open source engine, run:
```bash
~/repo/8thwall bazel run --config=wasmreleasesimd //reality/app/xr/js:serve-xr
```

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

4. In your app, update to load the open source engine. Update `my-app/src/index.html` from:
```html
<script crossorigin="anonymous" src="./external/xr/xr.js" data-preload-chunks="slam">
```
to:
```html
<script crossorigin="anonymous" src="https://192.168.68.55:8888/reality/app/xr/js/xr.js" data-preload-chunks="slam" data-chunk-base-url="/external/xr/"></script>
```
Note that if you host the open source engine elsewhere, you should update from `https://192.168.68.55:8888` to your host.
