# 8th Wall Engine

> [!WARNING]
> This engine is released as a pre-release build. The API may change before the official 1.0.0 release.

## Usage

See https://8thwall.org/docs/engine/overview for a more detailed guide.

### Option 1: Script tag

```html
<script src="https://cdn.jsdelivr.net/npm/@8thwall/engine@0.1.0/dist/xr.js" async crossorigin="anonymous" data-preload-chunks="slam"></script>
```

> [!NOTE]
> The `XrController` module in this package does not contain SLAM (World Tracking), only Image Targets. However, for backwards compatibility, the chunk name is still `slam`. See "SLAM Integration" below for more details.

### Option 2: npm

```
npm install @8thwall/engine
```

You will need to copy the included artifacts into your dist folder, for example in webpack:

```js
new CopyWebpackPlugin({
  patterns: [
    {
      from: 'node_modules/@8thwall/engine/dist',
      to: 'external/xr',
    }
  ]
})
```

You can then load the SDK by adding the following to index.html:

```html
<script src="./external/xr/xr.js" data-preload-chunks="slam" async></script>
```

When importing the package, you will get a simple helper for accessing XR8 once it is loaded. This promise will only resolve if the script tag is included in the HTML.

```js
import {XR8Promise} from '@8thwall/engine'

XR8Promise.then((XR8) => XR8.XrController.configure({}))
```

## Overview

The 8th Wall open-source engine includes:

- Core engine architecture
- Image Targets
- Face Effects
- Sky Effects

It does not include:

- Niantic Spatial products such as VPS, Lightship Maps, or the Geospatial Browser
- Hand Tracking

### SLAM Integration

The open-source engine does not contain a SLAM module. However, there is an option to load the SLAM module provided by the Distributed Engine Binary into the open-source engine.

> [!NOTE]
> The Distributed Engine Binary is available through a limited-use license which places restrictions on how it can be used. The full license text is [here](https://github.com/8thwall/engine/blob/main/LICENSE). Please see the [Permitted Use FAQ](https://8thwall.org/docs/migration/faq#distributed-engine-binary-license-and-permitted-use) and [Attribution Guidelines](https://8thwall.org/docs/open-source) for more information.

In the below code, the `data-preload-chunks="slam: <url>"` instructs the open-source engine to fetch the `slam` chunk from that given URL. The same XrController API will be available. `XR8.loadChunks({name: 'slam', url: '<url>'})` is also supported.

```html
<script async crossorigin="anonymous"
  src="https://cdn.jsdelivr.net/npm/@8thwall/engine@0.1.0/dist/xr.js"
  data-preload-chunks="slam: https://cdn.jsdelivr.net/npm/@8thwall/engine-binary@1/dist/xr-slam.js"
></script>
```

If using npm, run:

```bash
npm install @8thwall/engine-binary
```

Add the following, or equivalent to your build configuration:

```js
new CopyWebpackPlugin({
  patterns: [
    {
      from: 'node_modules/@8thwall/engine-binary/dist',
      to: 'external/xr-binary', // NOTE: Copied to a different path than @8thwall/engine
    }
  ]
})
```

Then specify the relative URL:

```html
<script async
  src="./external/xr/xr.js"
  data-preload-chunks="slam: ./external/xr-binary/xr-slam.js"
></script>
```

