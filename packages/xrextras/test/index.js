// Copyright (c) 2018 8th Wall, Inc.

// A pipeline module that throws an error after 300 frames -- illustrates the error handling in
// RuntimeError.pipelineModule().
const throwerrorPipelineModule = () => {
  let frame = 0
  return {
    name: 'throwerror',
    onUpdate: () => { if (++frame > 300) { throw Error('Too many frames!') } },
  }
}

const onxrloaded = () => {
  const {XR8, XRExtras} = window
  XR8.addCameraPipelineModules([  // Add camera pipeline modules.
    // Existing pipeline modules.
    XR8.GlTextureRenderer.pipelineModule(),      // Draws the camera feed.
    XRExtras.AlmostThere.pipelineModule(),       // Detects unsupported browsers and gives hints.
    XRExtras.FullWindowCanvas.pipelineModule(),  // Modifies the canvas to fill the window.
    XRExtras.Loading.pipelineModule(),           // Manages the loading screen on startup.
    XRExtras.RuntimeError.pipelineModule(),      // Shows an error image on runtime error.
    // Custom pipeline modules.
    throwerrorPipelineModule(),  // Throw an error after 300 frames.
  ])

  // Request permissions and run camera.
  XR8.run({
    canvas: document.getElementById('camerafeed'),
    allowedDevices: XR8.XrConfig.device().ANY,
  })
}

// Show loading screen before the full XR library has been loaded.
window.XRExtras.Loading.showLoading({onxrloaded})
