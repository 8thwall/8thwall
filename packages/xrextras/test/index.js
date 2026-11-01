// Copyright (c) 2018 8th Wall, Inc.

// A pipeline module that throws an error after 300 frames -- illustrates StartFlow error handling.
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
    XRExtras.FullWindowCanvas.pipelineModule(),  // Modifies the canvas to fill the window.
    XRExtras.StartFlow.pipelineModule(),         // Manages compatibility, loading, and errors.
    // Custom pipeline modules.
    throwerrorPipelineModule(),  // Throw an error after 300 frames.
  ])

  // Request permissions and run camera.
  XR8.run({
    canvas: document.getElementById('camerafeed'),
    allowedDevices: XR8.XrConfig.device().ANY,
  })
}

if (window.XR8) {
  onxrloaded()
} else {
  window.addEventListener('xrloaded', onxrloaded)
}
