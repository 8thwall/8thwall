# Engine
This engine packages contains an open source version of the 8th Wall engine with SLAM, VPS, and Hand Tracking removed. The SLAM, VPS, and Hand Tracking algorithms remain proprietary to Niantic Spatial, but other AR features, such as Face Tracking, Image Target tracking, Sky Segmentation are included. Because this core framework is open source, if browser APIs evolve or change, this open source engine code can adjust as needed.

## Usage
Today, the easiest way to add the engine is to use the [Distributed Engine Binary](https://github.com/8thwall/engine), which also supports SLAM. We will also be working on official releases of the open source engine through npm.

## Running
First, serve the engine:

```bash
bazel run --platforms=//bzl:wasm32 //reality/app/xr/js:serve-xr 
```

Then use the served `xr.js` file in your project, e.g. `https://192.168.68.65:8888/reality/app/xr/js/xr.js`.

## Building
To build the engine, run:
```bash
bazel build --config=wasmreleasesimd //reality/app/xr/js:xr-js
```

Or, if building for a non-SIMD environment, run:
```bash
bazel build --config=wasmrelease //reality/app/xr/js:xr-js
```

## Using the open source engine alongside the distributed engine binary
This open source version of the engine doesn't include SLAM. But the [distributed engine binary](https://github.com/8thwall/engine) does. To use the open source engine for the camera pipeline and the distributed engine binary for SLAM, you can do the following:
- 
