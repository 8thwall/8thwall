// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)
//
// PlayCanvas based renderer

// eslint-disable-next-line @typescript-eslint/no-unused-vars
/* global XR8:readonly, pc:readonly */

// keeps track of whether or not the user has the module running.  This is useful for warning the
// user if they call PlayCanvas.run()/runXr()/runFaceEffects() twice without calling
// stop()/stopXr()/stopFaceEffects in between.
import {create8wCanvas} from './canvas'
import {singleton} from './factory'
import {SUPPORTED_LAYERS} from './types/layers'

const CAMERA_PROPERTIES =
  ['aperture', 'aspectRatio', 'farClip', 'fov', 'nearClip', 'orthoHeight', 'projection',
    'sensitivity', 'shutter', 'cullFaces', 'flipFaces', 'frustumCulling', 'horizontalFov']

let pcIsRunning_ = false
let extraModules_ = []
// [Optional] Use two canvas instead of the new single canvas api.
let useTwoCanvas_ = false
let warned_ = false
let cssStyleReference_ = null

const EngineKind = {
  XR: 0,
  FACE: 1,
}

// Returns the default state but maintains the engineKind
const getDefaultState = engineKind => ({
  firstTick: true,
  doOverlay: true,
  clearColor: null,
  clearColorBuffer: true,
  origin: {x: 0, y: 2, z: 0},
  facing: {w: 1, x: 0, y: 0, z: 0},
  cameraCanvas: null,
  vsize: {},
  orientation: 0,
  engineKind,
  // layerName -> {renderTarget: pc.RenderTarget, camera: pc.Camera}
  layers: {},
})

const updateXrController = (pcCamera, state) => {
  XR8.XrController.updateCameraProjectionMatrix(
    {
      cam: {
        nearClipPlane: pcCamera.camera.nearClip,
        farClipPlane: pcCamera.camera.farClip,
      },
      ...state,
    }
  )
}

const updateController = (controller, pcCamera, state) => {
  controller.configure({
    nearClip: pcCamera.camera.nearClip,
    farClip: pcCamera.camera.farClip,
    coordinates: {
      origin: {
        position: state.origin,
        rotation: state.facing,
      },
    },
  })
}

const logRemovalDeprecationMessage = (name, version, replacement) => {
  // eslint-disable-next-line no-console
  console.warn(`[XR] Deprecation warning: ${name} was deprecated in ${version} and will be` +
  ` removed in the future. Use ${replacement} instead.`)
}

const maybeUpdateCameraProperty = (entityA, entityB, property) => {
  if (entityA.camera[property] !== entityB.camera[property]) {
    entityA.camera[property] = entityB.camera[property]
  }
}

const PlayCanvasFactory = singleton(() => {
  // PlayCanvas XR Events
  // These are the events you can listen for in your web application if you call runXr()
  //
  //
  // Event: xr:realityready
  // Description: This event is emitted when 8th Wall Web has initialized and at least one frame has
  //   been successfully processed.
  // Example:
  //    this.app.on('xr:realityready', () => {
  //      // Hide loading UI
  //    }, this)
  //
  //
  // Event: xr:realityerror
  // Description: This event is emitted when an error has occured when initializing 8th Wall Web.
  //   This is the recommended time at which any error messages should be displayed. The XrDevice
  //   API can help with determining what type of error messaging should be displayed.
  // Example:
  //    this.app.on('xr:realityerror', ({error, isDeviceBrowserSupported, compatibility}) => {
  //      if (detail.isDeviceBrowserSupported) {
  //        // Browser is compatible. Print the exception for more information.
  //        console.log(error)
  //        return
  //      }
  //
  //      // Browser is not compatible. Check the reasons why it may not be in `compatibility`
  //      cosnole.log(compatibility)
  //    }, this)
  //
  //
  // Event: xr:camerastatuschange
  // Description: This event is emitted when the status of the camera changes. See
  //    `onCameraStatusChange` from XR8.addCameraPipelineModule for more information on the possible
  //    status.
  // Example:
  //   const handleCameraStatusChange = function handleCameraStatusChange(detail) {
  //     console.log('status change', detail.status);
  //
  //     switch (detail.status) {
  //       case 'requesting':
  //         // Do something
  //         break;
  //
  //       case 'hasStream':
  //         // Do something
  //         break;
  //
  //       case 'failed':
  //         this.app.fire('xr:realityerror');
  //         break;
  //     }
  //   }
  //   this.app.on('xr:camerastatuschange', handleCameraStatusChange, this)
  //
  //
  // Event: xr:trackingstatus
  // Description: Fires when the tracking status or tracking reason is updated.
  //
  //
  // Event: xr:screenshotready
  // Description: This event is emitted in response to the `screenshotrequest` event being
  //     being completed. The JPEG compressed image of the AFrame canvas will be provided.
  // Example:
  //    this.app.on('xr:screenshotready', (event) => {
  //      // screenshotPreview is an <img> HTML element
  //      const image = document.getElementById('screenshotPreview')
  //      image.src = 'data:image/jpeg;base64,' + event.detail
  //    }, this)
  //
  //
  // Event: xr:screenshoterror
  // Description: This event is emitted in response to the `screenshotrequest` event resulting
  // in an error.
  // Example:
  //    this.app.on('xr:screenshoterror', (detail) => {
  //       console.log(detail)
  //       // Handle screenshot error.
  //    }, this)
  //
  //
  // Event: xr:projectwayspotscanning
  // Description: Fires when all Project Wayspots have been loaded for scanning.
  //   detail: {
  //     wayspots: [{
  //       id: An id for this Wayspot this is stable within a session.
  //       name: The Project Wayspot name.
  //       imageUrl: URL to a representative image for this Project Wayspot.
  //       title: The Wayspot title.
  //       lat: Latitude of this Project Wayspot.
  //       lng: Longitude of this Project Wayspot.
  //     }]
  //   }
  //
  // Event: xr:projectwayspotfound
  // Description: Fires when a Project Wayspot is first found.
  //   detail: {
  //     name: The Project Wayspot name.
  //     position {x, y, z}: The 3d position of the Project Wayspot.
  //     rotation {w, x, y, z}: The 3d local orientation of the Project Wayspot.
  //   }
  //
  // Event: xr:projectwayspotupdated
  // Description: Fires when a Project Wayspot changes position or rotation.
  //   detail: {
  //     name: The Project Wayspot name.
  //     position {x, y, z}: The 3d position of the Project Wayspot.
  //     rotation {w, x, y, z}: The 3d local orientation of the Project Wayspot.
  //   }
  //
  // Event: xr:projectwayspotlost
  // Description: Fires when a Project Wayspot is no longer being tracked.
  //   detail: {
  //     name: The Project Wayspot name.
  //     position {x, y, z}: The 3d position of the Project Wayspot.
  //     rotation {w, x, y, z}: The 3d local orientation of the Project Wayspot.
  //   }
  //
  //
  // Event: xr:meshfound
  // Description: Fires when a mesh is first found either after start or after a recenter().
  //    detail: {
  //      position {x, y, z}: The 3d position of the located mesh.
  //      rotation {w, x, y, z}: The 3d local orientation of the located mesh.
  //      mesh: pc.Mesh(), a PlayCanvas mesh with index, position, and color attributes
  //      id: Unique id for the mesh that lasts for the length current session
  //    }
  //
  //
  // Event: xr:meshupdated
  // Description: Fires when a the **first** mesh we found changes position or rotation.
  //    detail: {
  //      position {x, y, z}: The 3d position of the located mesh.
  //      rotation {w, x, y, z}: The 3d local orientation of the located mesh.
  //      id: Unique id for the mesh that lasts for the length current session
  //    }
  //
  // Event: xr:meshlost
  // Description: Fires when recenter is called.
  //    detail: {
  //      id: Unique id for the mesh that lasts for the length current session
  //    }
  //
  // PlayCanvas Face Effects Events
  // These are the events you can listen for in your web application if you call runFaceEffects()
  //
  // Event: xr:faceloading
  // Description: Fires when loading begins for additional face AR resources.
  // Example:
  //    this.app.on('xr:faceloading', ({maxDetections, pointsPerDetection, indices, uvs}) => {
  //    }, {})
  //
  //
  // Event: xr:facescanning
  // Description: Fires when all face AR resources have been loaded and scanning has begun.
  // Example:
  //    this.app.on('xr:facescanning', ({maxDetections, pointsPerDetection, indices, uvs}) => {
  //    }, {})
  //
  //
  // Event: xr:facefound
  // Description: Fires when a face first found.
  // Example:
  //    this.app.on('xr:facefound', ({id, transform, attachmentPoints, vertices, normals}) => {
  //    }, {})
  //
  //
  // Event: xr:faceupdated
  // Description: Fires when a face is subsequently found.
  // Example:
  //    this.app.on('xr:faceupdated', ({id, transform, attachmentPoints, vertices, normals}) => {
  //    }, {})
  //
  // Event: xr:facelost
  // Description: Fires when a face is no longer being tracked.
  // Example:
  //    this.app.on('xr:facelost', ({id}) => {
  //    }, {})
  //
  //
  // Event: xr:earfound
  // Description: Fires when an ear is found.
  // Example:
  //    this.app.on('xr:earfound', ({id, ear}) => {
  //    }, {})
  //
  // Event: xr:earlost
  // Description: Fires when an ear lost.
  // Example:
  //    this.app.on('xr:earlost', ({id, ear}) => {
  //    }, {})
  //
  //
  // Event: xr:earpointfound
  // Description: Fires when an ear attachment point is found.
  // Example:
  //    this.app.on('xr:earpointfound', ({id, point}) => {
  //    }, {})
  //
  // Event: xr:earpointlost
  // Description: Fires when an ear attachment point is lost.
  // Example:
  //    this.app.on('xr:earpointlost', ({id, point}) => {
  //    }, {})
  //
  //
  // PlayCanvas Hand Tracking Events
  // These are the events you can listen for in your web application if you call run()
  //
  // Event: xr:handloading
  // Description: Fires when loading begins for additional hand AR resources.
  // Example:
  //    this.app.on('xr:handloading',
  //                 ({maxDetections, pointsPerDetection, rightIndices, leftIndices}) => {
  //    }, {})
  //
  //
  // Event: xr:handscanning
  // Description: Fires when all hand AR resources have been loaded and scanning has begun.
  // Example:
  //    this.app.on('xr:handscanning',
  //                 ({maxDetections, pointsPerDetection, rightIndices, leftIndices}) => {
  //    }, {})
  //
  //
  // Event: xr:handfound
  // Description: Fires when a hand first found.
  // Example:
  //    this.app.on('xr:handfound', ({id, transform, attachmentPoints, vertices, normals}) => {
  //    }, {})
  //
  //
  // Event: xr:handupdated
  // Description: Fires when a hand is subsequently found.
  // Example:
  //    this.app.on('xr:handupdated', ({id, transform, attachmentPoints, vertices, normals}) => {
  //    }, {})
  //
  // Event: xr:handlost
  // Description: Fires when a hand is no longer being tracked.
  // Example:
  //    this.app.on('xr:handlost', ({id}) => {
  //    }, {})
  //
  //
  // PlayCanvas Image Target Events:
  //
  // Image target events can be listened to as this.app.on(event, handler, this). They mirror the
  //    events API in XR8.XrController. See that documentation on possible values.
  //
  //  'xr:imageloading'
  //  'xr:imagescanning'
  //  'xr:imagefound'
  //  'xr:imageupdated'
  //  'xr:imagelost'
  //
  //
  //
  // PlayCanvas LayersController Events:
  //
  // LayersController events can be listened to as this.app.on(event, handler, this). They mirror
  //    the events API in XR8.LayersController. See that documentation on possible values.
  //
  //  'xr:layerscanning'
  //  'xr:layerloading'
  //  'xr:layerfound'
  //
  //
  //
  // PlayCanvas Event Listeners
  // These are the events you can emit from your web application.
  //
  // Event: xr:hidecamerafeed
  // Description: Hides the camera feed. Tracking does not stop.
  // Example:
  //    this.app.fire('xr:hidecamerafeed')
  //
  //
  // Event: xr:showcamerafeed
  // Description: Shows the camera feed.
  // Example:
  //    this.app.fire('xr:showcamerafeed')
  //
  //
  // Event: xr:screenshotrequest
  // Description: Emits a request to the engine to capture a screenshot of the PlayCanvas canvas.
  //   The engine will emit a `xr:screenshotready` event with the JPEG compressed image,
  //   or `xr:screenshoterror` if an error has occured.
  // Example:
  //    this.app.fire('xr:screenshotrequest')
  //
  //
  // Event: xr:recenter
  // Description: Recenters the camera feed to its origin. If a new origin is provided as an
  //   argument, the camera's origin will be reset to that, then it will recenter.
  // Arguments (Optional):
  //  {
  //    // The location of the new origin.
  //    origin: {x, y, z},
  //    // A quaternion representing direction the camera should face at the origin.
  //    facing: {w, x, y, z}
  //  }
  // Examples:
  //    this.app.fire('xr:recenter')
  //  OR
  //    this.app.fire('xr:recenter', {
  //       origin: {x: 1, y: 4, z: 0},
  //       facing: {w: 0.9856, x:0, y:0.169, z:0}
  //    })
  //
  //
  // Event: xr:stopxr
  // Description: Stop the current XR session. While stopped, the camera feed is stopped and
  //   device motion is not tracked.
  // Example:
  //    this.app.fire('xr:stopxr')

  const pipelineModule = ({pcCamera, pcApp, runConfig, engineKind}) => {
    let state = getDefaultState(engineKind)

    const canvasStyle_ = {
      width: '100%',
      height: '100%',
      margin: '0px',
      padding: '0px',
      border: '0px',
      display: 'block',
    }

    const xrUpdate = () => {
      XR8.runPreRender(Date.now())
      if (state.doOverlay) {
        XR8.runRender()
      }
    }

    const rawGeoToPlayCanvasMesh = (geometryData) => {
      const index = geometryData.index.array
      const positions = geometryData.attributes.find(o => o.name === 'position')
      const colors = geometryData.attributes.find(o => o.name === 'color')
      const normals = pc.calculateNormals(positions, index)

      const mesh = new pc.Mesh(pcApp.graphicsDevice)
      mesh.setPositions(positions.array)
      mesh.setIndices(index)
      mesh.setNormals(normals)

      if (colors) {
        mesh.setColors(colors.array, 3 /** 3 channel color */)
      }

      mesh.update()
      return mesh
    }

    const realityEventEmitter = (event) => {
      if (event.name === 'reality.meshfound') {
        const {position, rotation, geometry, id} = event.detail
        pcApp.fire('xr:meshfound',
          {position, rotation, mesh: rawGeoToPlayCanvasMesh(geometry), id})
      } else if (event.name.startsWith('reality.')) {
        pcApp.fire(`xr:${event.name.substring('reality.'.length)}`, event.detail)
      }
    }

    const faceEffectsEventEmitter = event => event.name.startsWith('facecontroller.') &&
      pcApp.fire(`xr:${event.name.substring('facecontroller.'.length)}`, event.detail)

    const layersEventEmitter = event => event.name.startsWith('layerscontroller.') &&
      pcApp.fire(`xr:${event.name.substring('layerscontroller.'.length)}`, event.detail)

    const updateCameraSettings = ({origin, facing}) => {
      if (!origin || !facing) { return }

      const {x, y, z} = origin
      const {w: qw, x: qx, y: qy, z: qz} = facing
      state.origin = {x, y, z}
      state.facing = {w: qw, x: qx, y: qy, z: qz}

      if (state.engineKind === EngineKind.XR) {
        updateXrController(pcCamera, state)
      } else if (state.engineKind === EngineKind.FACE) {
        updateController(XR8.FaceController, pcCamera, state)
      } else if (state.engineKind === null) {
        // In this case we are using the run() API. Use extraModules_ to check if we have any
        // pipeline modules to configure.
        extraModules_.forEach((m) => {
          switch (m.name) {
            case 'reality':
              updateXrController(pcCamera, state)
              break
            case 'facecontroller':
              updateController(XR8.FaceController, pcCamera, state)
              break
            case 'layerscontroller':
              updateController(XR8.LayersController, pcCamera, state)
              break
            default:
              break
          }
        })
      }
    }

    const fillScreenWithCanvas = () => {
      if (!state.cameraCanvas) { return }

      // Get the pixels of the browser window.
      const uww = window.innerWidth
      const uwh = window.innerHeight
      const ww = uww * devicePixelRatio
      const wh = uwh * devicePixelRatio

      // Wait for orientation change to take effect before handline resize.
      if (((state.orientation === 0 || state.orientation === 180) && ww > wh) ||
        ((state.orientation === 90 || state.orientation === -90) && wh > ww)) {
        window.requestAnimationFrame(fillScreenWithCanvas)
        return
      }

      // Compute the portrait-orientation aspect ratio of the browser window.
      const ph = Math.max(ww, wh)
      const pw = Math.min(ww, wh)
      const pa = ph / pw

      // Compute the portrait-orientation dimensions of the video.
      const pvh = Math.max(state.vsize.w, state.vsize.h)
      const pvw = Math.min(state.vsize.w, state.vsize.h)

      // Compute the cropped dimensions of a video that fills the screen, assuming that width is
      // cropped.
      let ch = pvh
      let cw = Math.round(pvh / pa)

      // Figure out if we should have cropped from the top, and if so, compute a new cropped video
      // dimension.
      if (cw > pvw) {
        cw = pvw
        ch = Math.round(pvw * pa)
      }

      // If the video has more pixels than the screen, set the canvas size to the screen pixel
      // resolution.
      if (cw > pw || ch > ph) {
        cw = pw
        ch = ph
      }

      // Switch back to a landscape aspect ratio if required.
      if (ww > wh) {
        const tmp = cw
        cw = ch
        ch = tmp
      }

      // Set the canvas geometry to the new window size.
      Object.assign(state.cameraCanvas.style, canvasStyle_)
      state.cameraCanvas.width = cw
      state.cameraCanvas.height = ch

      // on iOS, rotating from portrait to landscape back to portrait can lead to a situation where
      // address bar is hidden and the content doesn't fill the screen. Scroll back up to the top in
      // this case. In chrome this has no effect. We need to scroll to something that's not our
      // scroll position, so scroll to 0 or 1 depending on the current position.
      setTimeout(() => window.scrollTo(0, (window.scrollY + 1) % 2), 300)
    }

    const updateVideoSize = ({videoWidth, videoHeight}) => {
      state.vsize.w = videoWidth
      state.vsize.h = videoHeight
    }

    const maybeSetAppCameraTransparency = () => {
      // store the old clear color values
      state.clearColor = pcCamera.camera.clearColor
      state.clearColorBuffer = pcCamera.camera.clearColorBuffer

      // PlayCanvas has their own canvas element.  In order to show the XR8 camera feed's canvas
      // behind the PlayCanvas's canvas element, we need to set the clear color for the camera to be
      // transparent
      if (useTwoCanvas_) {
        // set clear value to transparent
        pcCamera.camera.clearColor = new pc.Color(0, 0, 0, 0)
        pcCamera.camera.clearColorBuffer = true
      } else {
        pcCamera.camera.clearColorBuffer = false
      }
    }

    const xrHideCameraFeed = () => {
      if (!state.doOverlay) { return }
      state.doOverlay = false

      // reinstate the previous clear color values
      if (state.clearColor) {
        pcCamera.camera.clearColor = state.clearColor
        pcCamera.camera.clearColorBuffer = state.clearColorBuffer
      }
    }

    const xrShowCameraFeed = () => {
      if (state.doOverlay) { return }
      state.doOverlay = true
      maybeSetAppCameraTransparency()
    }

    const xrScreenshotRequest = () => {
      XR8.CanvasScreenshot.takeScreenshot().then(
        data => pcApp.fire('xr:screenshotready', data),
        error => pcApp.fire('xr:screenshoterror', error)
      )
    }

    const xrRecenter = (detail) => {
      updateCameraSettings(detail || {})
      if (state.engineKind === EngineKind.XR) {
        XR8.XrController.recenter()
      } else if (state.engineKind === null) {
        extraModules_.forEach((m) => {
          switch (m.name) {
            case 'reality':
              XR8.XrController.recenter()
              break
            case 'layerscontroller':
              XR8.LayersController.recenter()
              break
            default:
              break
          }
        })
      }
    }

    // A method which GlTextureRenderer will call and use in rendering. Returns a list where each
    // element contains two WebGLTexture's:
    // 1) foregroundTexture: The render target texture which we've rendered the LayerScene to.
    // 2) foregroundMaskTexture: The LayersController results. Used for alpha blending of
    //                           foregroundTexture.
    const layerTextures = ({processCpuResult}) => {
      if (Object.keys(state.layers).length === 0 || !processCpuResult.layerscontroller?.layers) {
        return []
      }

      return Object.entries(state.layers).map(([layerName8w, layer]) => {
        // Update PlayCanvas' camera (of each layer) properties in case they have changed.
        CAMERA_PROPERTIES.forEach((property) => {
          maybeUpdateCameraProperty(layer.camera, pcCamera, property)
        })

        // Update camera extrinsic.
        const {rotation, position} = processCpuResult.reality || processCpuResult.layerscontroller
        layer.camera.setRotation(rotation.x, rotation.y, rotation.z, rotation.w)
        layer.camera.setPosition(position.x, position.y, position.z)

        // Returns the RenderTarget texture alongside the sky layer alpha mask.
        return {
          foregroundTexture: layer.renderTarget.colorBuffer.impl._glTexture,
          foregroundMaskTexture: processCpuResult.layerscontroller.layers[layerName8w].texture,
        }
      })
    }

    const createRenderTarget = (name, graphicsDevice) => {
      const colorBuffer = new pc.Texture(graphicsDevice, {
        width: graphicsDevice.width,
        height: graphicsDevice.height,
        format: pc.PIXELFORMAT_R8_G8_B8_A8,
        autoMipmap: true,
      })
      colorBuffer.minFilter = pc.FILTER_LINEAR
      colorBuffer.magFilter = pc.FILTER_LINEAR
      return new pc.RenderTarget({
        colorBuffer,
        flipY: true,
        depth: true,
        name,
      })
    }

    return {
      name: 'playcanvas',
      onAttach: ({canvas, orientation}) => {
        if (useTwoCanvas_) {
          XR8.CanvasScreenshot.setForegroundCanvas(document.getElementById('application-canvas'))
        }
        updateCameraSettings({origin: pcCamera.getPosition(), facing: pcCamera.getRotation()})

        // XR event listeners
        pcApp.on('update', xrUpdate, {})
        pcApp.on('postrender', XR8.runPostRender, {})
        pcApp.on('xr:hidecamerafeed', xrHideCameraFeed, {})
        pcApp.on('xr:showcamerafeed', xrShowCameraFeed, {})
        pcApp.on('xr:screenshotrequest', xrScreenshotRequest, {})
        pcApp.on('xr:recenter', xrRecenter, {})
        pcApp.on('xr:stopxr', XR8.stop, {})

        state.cameraCanvas = canvas
        state.orientation = orientation
        if (state.doOverlay) {
          maybeSetAppCameraTransparency()
        }
        fillScreenWithCanvas()

        // Optionally connect an 8th Wall layer to PlayCanvas layers.
        if (runConfig.layers !== undefined) {
          Object.entries(runConfig.layers).forEach(([layerName8w]) => {
            if (!SUPPORTED_LAYERS.includes(layerName8w)) {
              throw new Error(
                `[XR] Invalid 8th Wall layer '${layerName8w}'. Supported layers are: [${
                  SUPPORTED_LAYERS.join(', ')}].`
              )
            }
          })

          Object.entries(runConfig.layers).forEach(([layerName8w, layerNamesPlayCanvas]) => {
            // Get the PlayCanvas layers + layer ids to render for each 8th Wall layer.
            const layers = layerNamesPlayCanvas.map(
              (layerNamePlayCanvas) => {
                const layer = pcApp.scene.layers.getLayerByName(layerNamePlayCanvas)
                if (!layer) {
                  throw new Error(
                    `[XR] PlayCanvas Layer '${layerNamePlayCanvas}' not found. Possible ` +
                    `PlayCanvas Layers: [${pcApp.scene.layers.layerList.map(l => l.name)}].`
                  )
                }
                return layer
              }
            )
            const layerIds = layers.map(layer => layer.id)

            // First create a new camera which we will use with a render target.
            const skyCamera = new pc.Entity({name: `${layerName8w}-Camera`})
            skyCamera.addComponent('camera', {
              // The camera should only render the layers the user specified.
              layers: layerIds,
              // Set a lower priority than the main scene camera so that the main scene camera draws
              // over the sky camera.
              priority: 0,
            })
            pcApp.root.addChild(skyCamera)

            // Then create a RenderTarget to render the PlayCanvas layers to a texture.
            const renderTarget =
              createRenderTarget(`${layerName8w}-RenderTarget`, pcApp.graphicsDevice)

            // NOTE(paris): This logic is actually buggy and will need a refactor to work with other
            // 8th Wall layers. Right now this is our pattern:
            // - Each 8th Wall Layer creates a Camera and a RenderTarget.
            // - Each Camera gets N PlayCanvas Layers set on it (their IDs).
            // - Each PlayCanvas Layer gets 1 RenderTarget set on it.
            //
            // Example:
            // - Input: `layers: {"sky": ["Foo", "Bar"]}`
            // - 8th Wall "sky" layer creates a Camera named "sky-Camera".
            // - Camera "sky-Camera" gets "FooLayer" and "BarLayer" PlayCanvas layers set on it.
            // - RenderTarget named "sky-RenderTarget" is created.
            // - PlayCanvas "FooLayer" has "sky-RenderTarget" set on it.
            // - PlayCanvas "BarLayer" has "sky-RenderTarget" set on it.
            //
            // Issue: if we also have a "ground" layer that uses PlayCanvas "Foo" then things break:
            // - Input: `layers: {"sky": ["Foo", "Bar"], "ground": ["Foo"]}`
            // - First everything from the example above happens. Then:
            // - 8th Wall "ground" layer creates a Camera named "ground-Camera".
            // - Camera "ground-Camera" gets "FooLayer" PlayCanvas layer set on it.
            // - RenderTarget named "ground-RenderTarget" is created.
            // - PlayCanvas "FooLayer" has "ground-RenderTarget" set on it.
            //  - PROBLEM: We just overwrote the "FooLayer" RenderTarget property from
            //             "sky-RenderTarget" to "ground-RenderTarget"
            // Update the PlayCanvas layers to use the RenderTarget.
            layers.forEach((layer) => {
              layer.renderTarget = renderTarget
            })

            // Save the layer information for use in layerTextures().
            state.layers[layerName8w] = {
              camera: skyCamera,
              renderTarget,
              layerNamesPlayCanvas,
            }

            window.XR8.GlTextureRenderer.setForegroundTextureProvider(layerTextures)
          })
        }
      },
      onUpdate: ({processCpuResult}) => {
        const {reality, facecontroller, layerscontroller} = processCpuResult
        if ((!reality && !facecontroller && !layerscontroller) || !pcCamera) {
          return
        }
        if (state.firstTick) {
          state.firstTick = false
          pcApp.fire('xr:realityready', {})
        }
        const {rotation, position, intrinsics} = reality ||
                                                 facecontroller ||
                                                 layerscontroller
        if (rotation == null || position == null || intrinsics == null) {
          return
        }

        if (state.doOverlay) {
          pcCamera.camera.horizontalFov = false
          pcCamera.camera.fov = (2.0 * Math.atan(1.0 / intrinsics[5]) * 180.0) / Math.PI
          pcCamera.camera.aspectRatio = intrinsics[5] / intrinsics[0]
          pcCamera.camera.farClip = intrinsics[14] / (intrinsics[10] + 1)
          pcCamera.camera.nearClip = intrinsics[14] / (intrinsics[10] - 1)
        }
        pcCamera.setRotation(rotation.x, rotation.y, rotation.z, rotation.w)
        pcCamera.setPosition(position.x, position.y, position.z)
      },
      onCameraStatusChange: (data) => {
        pcApp.fire('xr:camerastatuschange', data, {})
        if (data.status !== 'hasVideo') {
          return
        }
        updateVideoSize(data.video)
      },
      onDeviceOrientationChange: ({orientation}) => {
        state.orientation = orientation
        fillScreenWithCanvas()
      },
      onException: (e) => {
        // eslint-disable-next-line no-console
        console.error('[XR] Exception: ', e)
        // eslint-disable-next-line no-console
        if (e && e.stack) { console.error(e.stack) }
        pcApp.fire('xr:realityerror', {
          compatibility: XR8.getCompatibility(),
          error: e,
          isDeviceBrowserSupported: XR8.XrDevice.isDeviceBrowserCompatible(runConfig),
        })
      },
      onVideoSizeChange: ({videoWidth, videoHeight}) => {
        updateVideoSize({videoWidth, videoHeight})
        fillScreenWithCanvas()
      },
      onCanvasSizeChange: () => {
        Object.values(state.layers).forEach((layer) => {
          // Create a new RenderTarget.
          const renderTarget = createRenderTarget(layer.renderTarget.name, pcApp.graphicsDevice)

          // Delete the RenderTarget and free its resources:
          // https://forum.playcanvas.com/t/change-resolution-of-render-target/30061/6
          layer.renderTarget.destroy()
          layer.renderTarget.destroyTextureBuffers()
          layer.renderTarget = renderTarget

          // Update the PlayCanvas Layers with the new RenderTarget.
          layer.layerNamesPlayCanvas.forEach((layerNamePlayCanvas) => {
            pcApp.scene.layers.getLayerByName(layerNamePlayCanvas).renderTarget = renderTarget
          })
        })
      },
      onDetach: () => {
        // we need to remove the PlayCanvas events in case the user re-attaches this module.
        // Otherwise, the events would be called twice.
        pcApp.off('xr:hidecamerafeed', xrHideCameraFeed)
        pcApp.off('xr:showcamerafeed', xrShowCameraFeed)
        pcApp.off('xr:screenshotrequest', xrScreenshotRequest)
        pcApp.off('xr:recenter', xrRecenter)
        pcApp.off('xr:stopxr', XR8.stop)
        pcApp.off('update', xrUpdate)
        pcApp.off('postrender', XR8.runPostRender)

        // revert state to default but leave EngineKind
        state = getDefaultState(state.engineKind)

        window.XR8.GlTextureRenderer.setForegroundTextureProvider(null)
      },
      listeners: [
        {event: 'reality.imageloading', process: realityEventEmitter},
        {event: 'reality.imagescanning', process: realityEventEmitter},
        {event: 'reality.imagefound', process: realityEventEmitter},
        {event: 'reality.imageupdated', process: realityEventEmitter},
        {event: 'reality.imagelost', process: realityEventEmitter},
        {event: 'reality.meshfound', process: realityEventEmitter},
        {event: 'reality.meshupdated', process: realityEventEmitter},
        {event: 'reality.meshlost', process: realityEventEmitter},
        {event: 'reality.trackingstatus', process: realityEventEmitter},
        {event: 'reality.projectwayspotscanning', process: realityEventEmitter},
        {event: 'reality.projectwayspotfound', process: realityEventEmitter},
        {event: 'reality.projectwayspotupdated', process: realityEventEmitter},
        {event: 'reality.projectwayspotlost', process: realityEventEmitter},

        {event: 'facecontroller.faceloading', process: faceEffectsEventEmitter},
        {event: 'facecontroller.facescanning', process: faceEffectsEventEmitter},
        {event: 'facecontroller.facefound', process: faceEffectsEventEmitter},
        {event: 'facecontroller.faceupdated', process: faceEffectsEventEmitter},
        {event: 'facecontroller.facelost', process: faceEffectsEventEmitter},
        {event: 'facecontroller.mouthopened', process: faceEffectsEventEmitter},
        {event: 'facecontroller.mouthclosed', process: faceEffectsEventEmitter},
        {event: 'facecontroller.lefteyeopened', process: faceEffectsEventEmitter},
        {event: 'facecontroller.lefteyeclosed', process: faceEffectsEventEmitter},
        {event: 'facecontroller.righteyeopened', process: faceEffectsEventEmitter},
        {event: 'facecontroller.righteyeclosed', process: faceEffectsEventEmitter},
        {event: 'facecontroller.lefteyebrowraised', process: faceEffectsEventEmitter},
        {event: 'facecontroller.lefteyebrowlowered', process: faceEffectsEventEmitter},
        {event: 'facecontroller.righteyebrowraised', process: faceEffectsEventEmitter},
        {event: 'facecontroller.righteyebrowlowered', process: faceEffectsEventEmitter},
        {event: 'facecontroller.righteyewinked', process: faceEffectsEventEmitter},
        {event: 'facecontroller.lefteyewinked', process: faceEffectsEventEmitter},
        {event: 'facecontroller.blinked', process: faceEffectsEventEmitter},
        {event: 'facecontroller.interpupillarydistance', process: faceEffectsEventEmitter},

        {event: 'facecontroller.earfound', process: faceEffectsEventEmitter},
        {event: 'facecontroller.earlost', process: faceEffectsEventEmitter},
        {event: 'facecontroller.earpointfound', process: faceEffectsEventEmitter},
        {event: 'facecontroller.earpointlost', process: faceEffectsEventEmitter},

        {event: 'layerscontroller.layerscanning', process: layersEventEmitter},
        {event: 'layerscontroller.layerloading', process: layersEventEmitter},
        {event: 'layerscontroller.layerfound', process: layersEventEmitter},
      ],
    }
  }

  const addXRDom = () => {
    let cssSrc = `
      html {
        height: 100%;
        background-color: #1d292c00;
      }
      body {
        margin: 0;
        max-height: 100%;
        height: 100%;
        overflow: hidden;
        background-color: #1d292c00;
        font-family: Helvetica, arial, sans-serif;
        position: relative;
        width: 100%;
        -webkit-tap-highlight-color: transparent;
      }`
    if (useTwoCanvas_) {
      cssSrc += `
      #application-canvas {
        display: block;
        position: absolute;
        top: 0;
        left: 0;
        right: 0;
        bottom: 0;
        z-index: 15;
      }`

      // if the canvas has already been created, don't recreate it.  The can happen if they call
      // run or runXr or runFaceEffects more than once.
      let canv = document.getElementById('camerafeed')
      if (canv) {
        return
      }
      canv = create8wCanvas('camerafeed')
      // Add a Canvas element for our cameraFeed to draw on (sits under the playCanvas canvas)
      canv.id = 'camerafeed'
      canv.width = window.innerWidth
      canv.height = window.innerHeight
      window.document.body.appendChild(canv)  // adds the canvas to the body element
    }

    // Style the application canvas to be in front of the camera feed, but behind loading UX.
    if (!cssStyleReference_) {
      cssStyleReference_ = window.document.createElement('style')
      window.document.head.appendChild(cssStyleReference_)
    }
    cssStyleReference_.innerHTML = cssSrc
  }

  const checkAndSetIfUseTwoCanvasApi = (config) => {
    if (!config || !config.canvas) {
      useTwoCanvas_ = true
      if (!warned_) {
        // eslint-disable-next-line no-console
        console.warn('[XR] Deprecation warning: The new behavior is to pass the application ' +
          'canvas to XR8.PlayCanvas.runXr() ' +
          '(or XR8.PlayCanvas.runXr() / XR8.PlayCanvas.runFaceEffects()) in config, ' +
          'Ex. config = {..., canvas: document.getElementById(\'application-canvas\')}.')
        warned_ = true
      }
    }
  }

  // Opens the camera and starts running XR in a playcanvas scene.
  // Inputs:
  //    {
  //      pcCamera: the playcanvas scene camera to drive with AR.
  //      pcApp:    the playcanvas `app`, typically `this.app`.
  //    }
  //    [extraModules]: An optional array of extra pipeline modules to install.
  //    config            [Optional] Configuration parameters to pass to XR8.run:
  //    {
  //     webgl2:          [Optional] If true, use WebGL2 if available, otherwise fallback to WebGL1.
  //                                 If false, always use WebGL1.
  //                                 Default false.
  //     ownRunLoop:      [Optional] If true, XR should use it's own run loop. If false, you will
  //                                 provide your own run loop and be responsible for calling
  //                                 runPreRender and runPostRender yourself [Advanced Users only].
  //                                 Default false for PlayCanvas.
  //     cameraConfig:    [Optional] Desired camera to use. Supported values for direction are
  //                                 XR8.XrConfig.camera().BACK or XR8.XrConfig.camera().FRONT.
  //                               Default: {direction: XR8.XrConfig.camera().BACK}
  //     glContextConfig: [Optional] The attributes to configure the WebGL canvas context.
  //                                 Default: null.
  //     allowedDevices:  [Optional] Specify the class of devices that the pipeline should run on.
  //                                 If the current device is not in that class, running will fail
  //                                 prior to opening the camera. If allowedDevices is
  //                                 XR8.XrConfig.device().ANY, always open the camera. Note that
  //                                 world tracking should only be used with
  //                                 XR8.XrConfig.device().MOBILE.
  //                                 Default: XR8.XrConfig.device().MOBILE.
  //     layers:          [Optional] Specify the list of layers to draw using GlTextureRenderer. The
  //                                 key is the name of the layer in 8th Wall, and the value is
  //                                 a list of PlayCanvas layer names which we should render to a
  //                                 texture and mask using the 8th Wall layer. Example value:
  //                                 {"sky": ["FirstSkyLayer", "SecondSkyLayer"]}.
  //                                 Default: [].
  //    }
  const run = ({pcCamera, pcApp}, extraModules, config, engineKind) => {
    checkAndSetIfUseTwoCanvasApi(config)
    if (pcIsRunning_) {
      // eslint-disable-next-line no-console
      console.warn(
        '[XR] XR8.PlayCanvas run method was called multiple times. It is recommended to call ' +
        'stop() or stopXr() or stopFaceEffects() in between.'
      )
    }

    pcIsRunning_ = true

    const runConfig = {ownRunLoop: false, ...config}

    XR8.addCameraPipelineModules([
      // Core modules. These should almost always be added.
      XR8.GlTextureRenderer.pipelineModule(),  // Draws the camera feed.
      XR8.CanvasScreenshot.pipelineModule(),  // Generates jpg images from canvas.
      pipelineModule({pcCamera, pcApp, runConfig, engineKind}),  // Drives the playcanvas camera.
    ])

    if (engineKind === EngineKind.XR) {
      XR8.addCameraPipelineModule(XR8.XrController.pipelineModule())  // Enables SLAM tracking.
    } else if (engineKind === EngineKind.FACE) {
      XR8.addCameraPipelineModule(XR8.FaceController.pipelineModule())  // Enables Face Effects
    } else if (engineKind === null && extraModules && extraModules.length) {
      // With the run() API engineKind will be null. This is the only case where we want to set
      // extraModules_; runXr() and runFaceEffects() do not populate it b/c they historically didn't
      // remove their extraModules and we didn't want to change behavior when adding run().
      extraModules_ = extraModules
    }

    // Add extra pipeline modules.
    if (extraModules && extraModules.length) {
      XR8.addCameraPipelineModules(extraModules)
    }

    addXRDom()
    // Open the camera and start running the camera; run inside of playcanvas's run loop.
    XR8.run({
      canvas: useTwoCanvas_ ? document.getElementById('camerafeed') : config.canvas,
      ...runConfig,
    })
  }

  // Removes the modules created by run/runXr/runFaceEffects and stops the camera
  const stop = (engineKind) => {
    pcIsRunning_ = false
    XR8.removeCameraPipelineModules(['playcanvas', 'gltexturerenderer', 'canvasscreenshot'])
    if (engineKind === EngineKind.XR) {
      XR8.removeCameraPipelineModule('reality')
    } else if (engineKind === EngineKind.FACE) {
      XR8.removeCameraPipelineModule('facecontroller')
    }

    // Remove extra pipeline modules. Note that runXr() and runFaceEffects() do not populate this
    // b/c historically they never removed their extraModules and we don't want to change behavior.
    extraModules_.forEach((m) => {
      XR8.removeCameraPipelineModule(m.name)
    })
    extraModules_.length = 0

    useTwoCanvas_ = false
    XR8.stop()
  }

  return {
    // Configures and runs an 8th Wall session. extraModules will be removed on stop().
    run: ({pcCamera, pcApp}, extraModules, config) => run(
      {pcCamera, pcApp}, extraModules, config, null
    ),
    // Removes all modules added in run() and stops the session.
    stop,

    // APIs deprecated in R22.4
    runXr: ({pcCamera, pcApp}, extraModules, config) => {
      logRemovalDeprecationMessage('runXr()', 'R22.4', 'run()')
      run({pcCamera, pcApp}, extraModules, config, EngineKind.XR)
    },
    runFaceEffects: ({pcCamera, pcApp}, extraModules, config) => {
      logRemovalDeprecationMessage('runFaceEffects()', 'R22.4', 'run()')
      run({pcCamera, pcApp}, extraModules, config, EngineKind.FACE)
    },
    stopXr: () => {
      logRemovalDeprecationMessage('stopXr()', 'R22.4', 'stop()')
      stop(EngineKind.XR)
    },
    stopFaceEffects: () => {
      logRemovalDeprecationMessage('stopFaceEffects()', 'R22.4', 'stop()')
      stop(EngineKind.FACE)
    },
  }
})

export {
  PlayCanvasFactory,
}
