// Copyright (c) 2019 8th Wall, Inc.
// Original Author: Tony Tomarchio (tony@8thwall.com)
//
// Babylonjs based renderer

// A few issues/comments:
// * doesn't track properly if you rotate to landscape mode
// * scene is skewed when rotating to landscape mode
// * Babylon JS uses a right handed coordinate system

/* global XR8:readonly, BABYLON:readonly */

const EngineKind = {
  XR: 0,
  FACE: 1,
}

function BabylonjsFactory() {
  // Tmp variables to avoid create/destroy objects every frame
  let tmpMatrix; let tmpQuaternion; let
    tmpVector3

  if (window.BABYLON) {
    tmpMatrix = new BABYLON.Matrix()
    tmpQuaternion = new BABYLON.Quaternion()
    tmpVector3 = new BABYLON.Vector3()
  }

  let engine_ = null
  let engineKind_ = null
  let scene_ = null
  let camera_ = null
  let engineConfig_ = null
  let prevIntrinsics_ = null
  let realityEngaged_ = false

  const rawGeoToBabylonVertexData = (geometryData) => {
    const vertexData = new BABYLON.VertexData()
    const positions = geometryData.attributes.find(o => o.name === 'position')
    const colors = geometryData.attributes.find(o => o.name === 'color')

    if (scene_.useRightHandedSystem) {
      let dst = 0
      while (dst < positions.array.length) {
        positions.array[dst] = -positions.array[dst++]  // x = -x
        dst++  // y = y
        positions.array[dst] = -positions.array[dst++]  // z = -z
      }
    }
    vertexData.positions = positions.array

    vertexData.indices = scene_.useRightHandedSystem
      ? geometryData.index.array.reverse()
      : geometryData.index.array

    if (colors) {
      //  Convert rgb to rgba
      const numColors = colors.array.length / 3
      const dstSize = numColors * 4
      vertexData.colors = new Float32Array(dstSize)

      let src = 0
      let dst = 0
      while (dst < dstSize) {
        vertexData.colors[dst++] = colors.array[src++]  // r
        vertexData.colors[dst++] = colors.array[src++]  // g
        vertexData.colors[dst++] = colors.array[src++]  // b
        vertexData.colors[dst++] = 1.0  // a
      }
    }

    const normals = []
    BABYLON.VertexData.ComputeNormals(vertexData.positions, vertexData.indices, normals)
    vertexData.normals = normals

    return vertexData
  }

  const vector3ToEngine = (input, output) => {
    output = output || new BABYLON.Vector3()

    if (scene_.useRightHandedSystem) {
      output.x = -input.x
      output.y = input.y
      output.z = -input.z
    } else {
      output.x = input.x
      output.y = input.y
      output.z = input.z
    }

    return output
  }

  // Coincidence that both directions are the same transformation
  const vector3FromEngine = (input, output) => vector3ToEngine(input, output)

  const quaternionToEngine = (input, output) => {
    output = output || new BABYLON.Quaternion()

    if (scene_.useRightHandedSystem) {
      tmpQuaternion.copyFrom(input)
      tmpQuaternion.toEulerAnglesToRef(tmpVector3)

      tmpVector3.x *= -1
      tmpVector3.z *= -1

      BABYLON.Quaternion.FromEulerVectorToRef(tmpVector3, output)
    } else {
      output.w = input.w
      output.x = input.x
      output.y = input.y
      output.z = input.z
    }

    return output
  }

  // Coincidence that both directions are the same transformation
  const quaternionFromEngine = (input, output) => quaternionToEngine(input, output)

  // Babylon.js has a bug where the camera is not facing forward in a right handed coordinate
  // system.  In a left handed system, it is facing {0, 0, 1}. In a right handed coordinate system,
  // it is also facing {0, 0, 1}.  Therefore, it's facing backwards in a right handed coordinate.
  // If you set the camera's target to {0, 0, -1} in a right handed coordinate system, the rotation
  // will have the Y set to 180 degrees.  If we pass this data to our API, it will return the
  // expected data behind the camera.  To solve all this, in right handed coordinate systems we
  // rotate the camera's Babylon.js rotation by 180 degrees about the y in RHS in between 8th Wall
  // and Bablyon.js logic.
  //
  // Source: https://forum.babylonjs.com/t/solved-coordinates-with-right-hand-rule/3747
  // Example playground: https://www.babylonjs-playground.com/#S84MWJ#8
  //
  // All of the code above (quaternionFromEngine, vector3ToEngine) was under the assumption that
  // the x was flipped, which is what seems to happen due to the Babylon.js bug.  This assumption
  // is incorrect and not used for Face Effects.
  const getCorrectedCamRotation = (camRotation) => {
    const cameraRotation = new BABYLON.Quaternion(
      camRotation.x, camRotation.y, camRotation.z, camRotation.w
    )

    if (scene_.useRightHandedSystem) {
      return cameraRotation.multiply(BABYLON.Quaternion.FromEulerAngles(0, Math.PI, 0))
    }

    return cameraRotation
  }

  const intrinsicsFromEngine = (input, output) => {
    output = output || new BABYLON.Matrix()

    const translatedInput = input.slice(0)

    // TODO(nathan): figure out why we need to invert the clip z values in the intrinsics matrix
    // returned by our API.  Right now the clip values are converted from negative to positive
    // values.  It's surprising our clip z values are negative in a left hand coordinate system
    // considering LHS's forward is positive z.
    if (!scene_.useRightHandedSystem) {
      translatedInput[10] *= -1
      translatedInput[11] *= -1
    }

    BABYLON.Matrix.FromValuesToRef(...translatedInput, output)
    return output
  }

  const realityEventEmitter = (event) => {
    if (!scene_) {
      return
    }

    let observable

    if (engineKind_ === EngineKind.XR) {
      switch (event.name) {
        case 'reality.imageloading':
          observable = scene_.onXrImageLoadingObservable
          break

        case 'reality.imagescanning':
          observable = scene_.onXrImageScanningObservable
          break

        case 'reality.imagefound':
          observable = scene_.onXrImageFoundObservable
          break

        case 'reality.imageupdated':
          observable = scene_.onXrImageUpdatedObservable
          break

        case 'reality.imagelost':
          observable = scene_.onXrImageLostObservable
          break
        case 'reality.meshfound':
          observable = scene_.onXrMeshFoundObservable
          break
        case 'reality.meshupdated':
          observable = scene_.onXrMeshUpdatedObservable
          break
        case 'reality.meshlost':
          observable = scene_.onXrMeshLostObservable
          break
        default:
          break
      }
    } else if (engineKind_ === EngineKind.FACE) {
      switch (event.name) {
        case 'facecontroller.faceloading':
          observable = scene_.onFaceLoadingObservable
          break

        case 'facecontroller.facescanning':
          observable = scene_.onFaceScanningObservable
          break

        case 'facecontroller.facefound':
          observable = scene_.onFaceFoundObservable
          break

        case 'facecontroller.faceupdated':
          observable = scene_.onFaceUpdatedObservable
          break

        case 'facecontroller.facelost':
          observable = scene_.onFaceLostObservable
          break

        case 'facecontroller.mouthopened':
          observable = scene_.onMouthOpened
          break

        case 'facecontroller.mouthclosed':
          observable = scene_.onMouthClosed
          break

        case 'facecontroller.lefteyeopened':
          observable = scene_.onLeftEyeOpened
          break

        case 'facecontroller.lefteyeclosed':
          observable = scene_.onLeftEyeClosed
          break

        case 'facecontroller.righteyeopened':
          observable = scene_.onRightEyeOpened
          break

        case 'facecontroller.righteyeclosed':
          observable = scene_.onRightEyeClosed
          break

        case 'facecontroller.lefteyebrowraised':
          observable = scene_.onLeftEyebrowRaised
          break

        case 'facecontroller.lefteyebrowlowered':
          observable = scene_.onLeftEyebrowLowered
          break

        case 'facecontroller.righteyebrowraised':
          observable = scene_.onRightEyebrowRaised
          break

        case 'facecontroller.righteyebrowlowered':
          observable = scene_.onRightEyebrowLowered
          break

        case 'facecontroller.righteyewinked':
          observable = scene_.onRightEyeWinked
          break

        case 'facecontroller.lefteyewinked':
          observable = scene_.onLeftEyeWinked
          break

        case 'facecontroller.blinked':
          observable = scene_.onBlinked
          break

        case 'facecontroller.interpupillarydistance':
          observable = scene_.onInterpupillaryDistance
          break

        default:
          break
      }
    }

    if (observable && observable.hasObservers()) {
      if (engineKind_ === EngineKind.XR) {
        if (event.detail.position) {
          vector3FromEngine(event.detail.position, event.detail.position)
        }
        if (event.detail.rotation) {
          quaternionFromEngine(event.detail.rotation, event.detail.rotation)
        }

        if (event.name === 'reality.meshfound') {
          event.detail = {
            rotation: event.detail.rotation,
            position: event.detail.rotation,
            vertexData: rawGeoToBabylonVertexData(event.detail.geometry),
            id: event.detail.id,
          }
        }
      }

      observable.notifyObservers(event.detail)
    }
  }

  function pipelineModule() {
    if (!window.BABYLON) {
      throw new Error(
        'window.BABLYON does not exist but is required by the BabylonJS pipeline module'
      )
    }

    return {
      name: 'babylonjsrenderer',
      onAttach: ({canvasWidth, canvasHeight}) => {
        realityEngaged_ = true

        if (engineKind_ === EngineKind.XR) {
          const xrConfig = {
            leftHandedAxes: !scene_.useRightHandedSystem,
            ...engineConfig_,
          }
          XR8.XrController.configure(xrConfig)

          XR8.XrController.updateCameraProjectionMatrix({
            cam: {
              pixelRectWidth: canvasWidth,
              pixelRectHeight: canvasHeight,
              nearClipPlane: 0.01,
              farClipPlane: 1000,
            },
            origin: vector3ToEngine(camera_.position, null, true),
            facing: quaternionToEngine(camera_.rotationQuaternion, null, true),
          })
        }

        if (engineKind_ === EngineKind.FACE) {
          const faceConfig = {
            ...engineConfig_,
            nearClip: camera_.minZ,
            farClip: camera_.maxZ,
            coordinates: {
              origin: {
                position: camera_.position,
                rotation: getCorrectedCamRotation(camera_.rotationQuaternion),
              },
              axes: scene_.useRightHandedSystem ? 'RIGHT_HANDED' : 'LEFT_HANDED',
            },
          }

          XR8.FaceController.configure(faceConfig)
        }
      },
      onDetach: () => {
        realityEngaged_ = false
      },
      onUpdate: ({processCpuResult}) => {
        const {reality, facecontroller} = processCpuResult
        if (!reality && !facecontroller) {
          return
        }

        if (!camera_) {
          return
        }

        const {rotation, position, intrinsics} = reality || facecontroller

        if (intrinsics) {
          const intrinsicsChanged =
            !prevIntrinsics_ || intrinsics.some((e, i) => e !== prevIntrinsics_[i])

          if (intrinsicsChanged) {
            // Make a copy of the intrinsics.
            prevIntrinsics_ = intrinsics.slice(0)
            intrinsicsFromEngine(intrinsics, tmpMatrix)
            camera_.freezeProjectionMatrix(tmpMatrix)
          }
        }

        if (engineKind_ === EngineKind.XR) {
          if (rotation) {
            quaternionFromEngine(rotation, camera_.rotationQuaternion)
          }

          if (position) {
            vector3FromEngine(position, camera_.position)
          }
        } else {
          if (position) {
            camera_.position.set(position.x, position.y, position.z)
          }
          if (rotation) {
            camera_.rotationQuaternion = getCorrectedCamRotation(rotation)
          }
        }
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
        {event: 'facecontroller.faceloading', process: realityEventEmitter},
        {event: 'facecontroller.facescanning', process: realityEventEmitter},
        {event: 'facecontroller.facefound', process: realityEventEmitter},
        {event: 'facecontroller.faceupdated', process: realityEventEmitter},
        {event: 'facecontroller.facelost', process: realityEventEmitter},
      ],
    }
  }

  const xrCameraBehavior = (config, engineConfig, engineKind) => ({
    name: 'xrCameraBehavior',
    attach(cam) {
      engine_ = cam.getEngine()
      scene_ = cam.getScene()

      camera_ = cam
      engineKind_ = engineKind
      engineConfig_ = engineConfig

      camera_.rotationQuaternion = BABYLON.Quaternion.FromEulerVector(camera_.rotation)
      scene_.autoClear = false

      scene_.onBeforeRenderObservable.add(() => {
        if (realityEngaged_) {
          XR8.runPreRender(Date.now())
          XR8.runRender()
        }
      })

      scene_.onAfterRenderObservable.add(() => {
        if (realityEngaged_) {
          XR8.runPostRender()
        }
      })

      XR8.addCameraPipelineModules([
        XR8.GlTextureRenderer.pipelineModule(),
        pipelineModule(),
      ])

      if (engineKind_ === EngineKind.XR) {
        XR8.addCameraPipelineModule(XR8.XrController.pipelineModule())

        Object.assign(scene_, {
          onXrImageLoadingObservable: new BABYLON.Observable(),
          onXrImageScanningObservable: new BABYLON.Observable(),
          onXrImageFoundObservable: new BABYLON.Observable(),
          onXrImageUpdatedObservable: new BABYLON.Observable(),
          onXrImageLostObservable: new BABYLON.Observable(),
          onXrMeshFoundObservable: new BABYLON.Observable(),
          onXrMeshUpdatedObservable: new BABYLON.Observable(),
          onXrMeshLostObservable: new BABYLON.Observable(),
        })
      } else if (engineKind_ === EngineKind.FACE) {
        XR8.addCameraPipelineModule(XR8.FaceController.pipelineModule())

        Object.assign(scene_, {
          onFaceLoadingObservable: new BABYLON.Observable(),
          onFaceScanningObservable: new BABYLON.Observable(),
          onFaceFoundObservable: new BABYLON.Observable(),
          onFaceUpdatedObservable: new BABYLON.Observable(),
          onFaceLostObservable: new BABYLON.Observable(),
          onMouthOpened: new BABYLON.Observable(),
          onMouthClosed: new BABYLON.Observable(),
          onLeftEyeOpened: new BABYLON.Observable(),
          onLeftEyeClosed: new BABYLON.Observable(),
          onRightEyeOpened: new BABYLON.Observable(),
          onRightEyeClosed: new BABYLON.Observable(),
          onLeftEyebrowRaised: new BABYLON.Observable(),
          onLeftEyebrowLowered: new BABYLON.Observable(),
          onRightEyebrowRaised: new BABYLON.Observable(),
          onRightEyebrowLowered: new BABYLON.Observable(),
          onRightEyeWinked: new BABYLON.Observable(),
          onLeftEyeWinked: new BABYLON.Observable(),
          onBlinked: new BABYLON.Observable(),
          onInterpupillaryDistance: new BABYLON.Observable(),
        })
      }

      XR8.run({
        verbose: false,
        canvas: engine_.getRenderingCanvas(),
        ownRunLoop: false,
        ...config,
      })
    },
    init: () => {},
    detach: () => {
      XR8.stop()
      XR8.clearCameraPipelineModules()
    },
  })

  return {
    // Get a behavior that can be attached to a Babylon camera like so:
    // camera.addBehavior(XR8.Babylonjs.xrCameraBehavior())
    //
    // Inputs:
    //    config          [Optional] Configuration parameters to pass to XR8.run:
    //    {
    //      webgl2:       [Optional] If true, use WebGL2 if available, otherwise fallback to WebGL1.
    //                                  If false, always use WebGL1.
    //                                  Default false.
    //      ownRunLoop:   [Optional] If true, XR should use it's own run loop. If false, you will
    //                                  provide your own run loop and be responsible for calling
    //                                  runPreRender and runPostRender yourself
    //                                  [Advanced Users only].  Default true.
    //      cameraConfig: [Optional] Desired camera to use. Supported values for direction are
    //                                  XR8.XrConfig.camera().BACK or XR8.XrConfig.camera().FRONT.
    //                                Default: {direction: XR8.XrConfig.camera().BACK}
    //      glContextConfig: [Optional] The attributes to configure the WebGL canvas context.
    //                                  Default: null.
    //      allowedDevices:  [Optional] Specify the class of devices that the pipeline should run
    //                                  on. If the current device is not in that class, running will
    //                                  fail prior to opening the camera. If allowedDevices is
    //                                  XR8.XrConfig.device().ANY, always open the camera. Note that
    //                                  world tracking should only be used with
    //                                  XR8.XrConfig.device().MOBILE.
    //                                  Default: XR8.XrConfig.device().MOBILE.
    //    }
    //    xrConfig
    //    {
    //      enableWorldPoints:     [Optional] If true, return the map points used for tracking.
    //      enableLighting:        [Optional] If true, return an estimate of lighting information.
    //      disableWorldTracking:  [Optional] If true, turn off SLAM tracking for efficiency.
    //      leftHandedAxes:        [Optional] If true, use left-handed coordinates.
    //      mirroredDisplay:       [Optional] If true, flip left and right in the output.
    //    }
    xrCameraBehavior: (config, xrConfig) => xrCameraBehavior(config, xrConfig, EngineKind.XR),

    // Get a behavior that can be attached to a Babylon camera like so:
    // camera.addBehavior(XR8.Babylonjs.xrCameraBehavior())
    //
    // Inputs:
    //    runConfig         [Optional] Configuration parameters to pass to XR8.run:
    //    {
    //      webgl2:          [Optional] If true, use WebGL2 if available, otherwise fallback to
    //                                   WebGL1.  If false, always use WebGL1.
    //                                  Default false.
    //      ownRunLoop:      [Optional] If true, XR should use it's own run loop. If false, you will
    //                                  provide your own run loop and be responsible for calling
    //                                  runPreRender and runPostRender yourself
    //                                  [Advanced Users only]. Default true.
    //      cameraConfig:    [Optional] Desired camera to use. Supported values for direction are
    //                                  XR8.XrConfig.camera().BACK or XR8.XrConfig.camera().FRONT.
    //                                  Default: {direction: XR8.XrConfig.camera().BACK}
    //      glContextConfig: [Optional] The attributes to configure the WebGL canvas context.
    //                                  Default: null.
    //      allowedDevices:  [Optional] Specify the class of devices that the pipeline should run
    //                                  on.If the current device is not in that class, running will
    //                                  fail prior to opening the camera. If allowedDevices is
    //                                  XR8.XrConfig.device().ANY, always open the camera. Note that
    //                                  world tracking should only be used with
    //                                  XR8.XrConfig.device().MOBILE.
    //                                  Default: XR8.XrConfig.device().MOBILE.
    //    },
    //    faceConfig            [Optional] Face configuration parameters
    //    {
    //      nearClip:           [Optional] The distance from the camera of the near clip plane.  By
    //                                     default it will use the Babylon camera.minZ
    //      farClip:            [Optional] The distance from the camera of the far clip plane.  By
    //                                     default it will use the Babylon camera.maxZ
    //      meshGeometry:       [Optional] List that contains which parts of the head geometry are
    //                                     visible.  Options are:
    //                                     [
    //                                       XR8.FaceController.MeshGeometry.FACE,
    //                                       XR8.FaceController.MeshGeometry.EYES,
    //                                       XR8.FaceController.MeshGeometry.NOSE,
    //                                     ]
    //                                     The default is [XR8.FaceController.MeshGeometry.FACE]
    //
    //      coordinates: {
    //        origin:           [Optional] {position: {x, y, z}, rotation: {w, x, y, z}} of the
    //                                      camera.  Will use the Babylon camera position and
    //                                      rotation by default.
    //        scale:            [Optional] scale of the scene.  Default is one unit equals 1 meter.
    //        axes:             [Optional] 'LEFT_HANDED' or 'RIGHT_HANDED'.  By default it is
    //                                      determined by set by the boolean value for
    //                                      scene.useRightHandedSystem
    //        mirroredDisplay:  [Optional] If true, flip left and right in the output.  Default is
    //                                     false
    //      }
    //      maxDetections:      [Optional] Number of faces that can be tracked at once, default is
    //                                     1. The available choices are 1, 2, or 3.
    //      uvType:             [Optional] Specifies which uvs are returned in the facescanning and
    //                                     faceloading event. Options are:
    //                                     [
    //                                       XR8.FaceController.UvType.PROJECTED,
    //                                       XR8.FaceController.UvType.STANDARD,
    //                                     ]
    //                                     The default is XR8.FaceController.UvType.STANDARD.
    // }
    //    }
    faceCameraBehavior:
      (runConfig, faceConfig) => xrCameraBehavior(runConfig, faceConfig, EngineKind.FACE),
  }
}

export {
  BabylonjsFactory,
}
