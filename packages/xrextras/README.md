# XR Extras

This library provides modules that extend the
[Camera Pipeline Module framework](https://docs.8thwall.com/web/#camerapipelinemodule) in
[8th Wall XR](https://8thwall.com/products-web.html) to handle common application needs.

## Usage

### Option 1: Script tag

```html
<script src="https://cdn.jsdelivr.net/npm/@8thwall/xrextras@1/dist/xrextras.js" crossorigin="anonymous"></script>
```

### Option 2: npm

```
npm install @8thwall/xrextras
```

You will need to copy the included artifacts into your dist folder, for example in webpack:

```js
new CopyWebpackPlugin({
  patterns: [
    {
      from: 'node_modules/@8thwall/xrextras/dist',
      to: 'external/xrextras',
    }
  ]
})
```

You can then load the library by adding the following to index.html:

```html
<script src="./external/xrextras/xrextras.js"></script>
```

When you import the package, a simple helper for accessing window.XRExtras is provided. The expectation is still that the script tag is added to the HTML. 

```js
import * as XRExtras from '@8thwall/xrextras'
XRExtras.DebugWebViews.enableLogToScreen()
```

## Development

Start the development server with:

```bash
npm install
npm run dev
```

Test pages will be available at https://localhost:9000/test/index.html and https://localhost:9000/test/index-aframe.html, and xrextras itself can be tested on other projects by replacing the existing script tag with:

```html
<script src="https://localhost:9000/xrextras.js"></script>
```

See [RELEASING.md](./RELEASING.md) for release instructions.

## Hello World

### Native JS

index.html:

```html
<html>
  <head>
    <title>XR Extras: Camera Pipeline</title>
    <meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">

    <!-- XR Extras - provides utilities like load screen, error handling, and gesture control helpers.
         See https://github.com/8thwall/web/tree/master/xrextras/ -->
    <script src="./external/xrextras/xrextras.js"></script>

    <!-- Landing Pages - see https://8thwall.org/docs/engine/guides/landing-pages -->
    <script src='./external/landing-page/landing-page.js'></script>

    <!-- 8th Wall Engine -->
    <script async src="./external/xr/xr.js"></script>

    <script src="index.js"></script>
  </head>
  <body>
    <canvas id="camerafeed"></canvas>
  </body>
</html>
```

index.js:

```javascript
const onxrloaded = () => {
  XR8.addCameraPipelineModules([  // Add camera pipeline modules.
    // Existing pipeline modules.
    XR8.GlTextureRenderer.pipelineModule(),       // Draws the camera feed.
    window.LandingPage.pipelineModule(),          // Detects unsupported browsers and gives hints.
    XRExtras.FullWindowCanvas.pipelineModule(),   // Modifies the canvas to fill the window.
    XRExtras.Loading.pipelineModule(),            // Manages the loading screen on startup.
    XRExtras.RuntimeError.pipelineModule(),       // Shows an error image on runtime error.
  ])

  XR8.run({canvas: document.getElementById('camerafeed')})   // Request permissions and run camera.
}

window.XR8 ? onxrloaded() : window.addEventListener('xrloaded', onxrloaded)
```

### AFrame

index.html:

```html
<html>
  <head>
    <title>XRExtras: A-FRAME</title>

    <!-- We've included a slightly modified version of A-Frame, which fixes some polish concerns -->
    <script src="./external/scripts/8frame-1.5.0.min.js"></script>

    <!-- XR Extras - provides utilities like load screen, error handling, and gesture control helpers.
         See https://github.com/8thwall/web/tree/master/xrextras/ -->
    <script src="./external/xrextras/xrextras.js"></script>

    <!-- Landing Pages - see https://8thwall.org/docs/engine/guides/landing-pages -->
    <script src='./external/landing-page/landing-page.js'></script>

    <!-- 8th Wall Engine -->
      <script async src="./external/xr/xr.js"></script>
  </head>

  <body>
    <!-- Add the 'xrweb' attribute to your scene to make it an 8th Wall Web A-FRAME scene. -->
    <a-scene
      xrweb
      landing-pages
      xrextras-loading
      xrextras-runtime-error
      xrextras-tap-recenter>

      <a-camera position="0 8 8"></a-camera>

      <a-entity
        light="type: directional; castShadow: true; intensity: 0.8; shadowCameraTop: 7;
               shadowMapHeight: 1024; shadowMapWidth: 1024;"
        position="1 4.3 2.5">
      </a-entity>

      <a-entity
        light="type: directional; castShadow: false; intensity: 0.5;" position="-0.8 3 1.85">
      </a-entity>

      <a-light type="ambient" intensity="1"></a-light>

      <a-box
        position="-1.7 0.5 -2" rotation="0 45 0" shadow
        material="roughness: 0.8; metalness: 0.2; color: #00EDAF;">
      </a-box>

      <a-sphere
        position="-1.175 1.25 -5.2" radius="1.25" shadow
        material="roughness: 0.8; metalness: 0.2; color: #DD0065;">
      </a-sphere>

      <a-cylinder
        position="2 0.75 -1.85" radius="0.5" height="1.5" shadow
        material="roughness: 0.8; metalness: 0.2; color: #FCEE21;">
      </a-cylinder>

      <a-circle position="0 0 -4" rotation="-90 0 0" radius="4" shadow
        material="roughness: 0.8; metalness: 0.5; color: #AD50FF">
      </a-circle>
    </a-scene>
  </body>
</html>
```

## API

### Pipeline Modules

Quick Reference:

```javascript
  XR8.addCameraPipelineModules([  // Add camera pipeline modules.
    // Existing pipeline modules.
    XR8.GlTextureRenderer.pipelineModule(),       // Draws the camera feed.
    window.LandingPage.pipelineModule(),          // Detects unsupported browsers and gives hints.
    XRExtras.FullWindowCanvas.pipelineModule(),   // Modifies the canvas to fill the window.
    XRExtras.Loading.pipelineModule(),            // Manages the loading screen on startup.
    XRExtras.RuntimeError.pipelineModule(),       // Shows an error image on runtime error.
    XRExtras.PwaInstaller.pipelineModule(),       // Displays a prompt to add to home screen.
  ])
```

Pipeline Modules:

* LandingPage.pipelineModule(): Detects if the user is not on a supported device or browser, and
provides helpful information for how to view the XR experience.
* FullWindowCanvas.pipelineModule(): Makes sure that the camera display canvas fills the full
browser window across device orientation changes, etc.
* Loading.pipelineModule(): Displays a loading overlay and camera permissions prompt while
libraries are loading, and while the camera is starting up.
* RuntimeError.pipelineModule(): Shows an error image when an error occurs at runtime.
* PwaInstaller.pipelineModule(): Displays a prompt to add to home screen.

### AFrame Components

Quick Reference:

```html
    <a-scene
      xrweb
      landing-pages
      xrextras-loading
      xrextras-runtime-error
      xrextras-tap-recenter>
```

Javascript Functions:

* XRExtras.AFrame.registerXrExtrasComponents(): Registers the XR Extras AFrame components if they
aren't already registered. If AFrame is loaded befor XR Extras, then there is no need to call this
method. In the case that AFrame is loaded late and you need to explicitly register the components,
you can call this method.

AFrame Components:

* landing-pages: Detects if the user is not on a supported device or browser, and provides
helpful information for how to view the XR experience.
* xrextras-loading: Displays a loading overlay and camera permissions prompt while the scene and
libraries are loading, and while the camera is starting up.
* xrextras-runtime-error: Hides the scene and shows an error image when an error occurs at runtime.
* xrextras-tap-recenter: Calls 'XR.recenter()' when the AFrame scene is tapped.

### Other

Fonts: Provides common .css for fonts used by various modules.

DebugWebViews: Provides utilities for debugging javascript while it runs in browsers embedded in
applications.

Quick Reference:

```html
    <script src="./external/xrextras/xrextras.js"></script>
    <script>
      const screenlog = () => {
        window.XRExtras.DebugWebViews.enableLogToScreen()
        console.log('screenlog enabled')
      }
      window.XRExtras ? screenlog() : window.addEventListener('xrextrasloaded', screenlog)
    </script>
```

