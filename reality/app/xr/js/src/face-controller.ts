// A pipeline for face detection and meshing.

/* global XR8:readonly */

// @dep(//reality/app/xr/js:xr-face-wasm)

// The following imports resolve because of the NODE_PATH variabled set in build.js
/* eslint-disable import/no-unresolved */
import XRFACECC from 'reality/app/xr/js/xr-face-wasm'
import type {XrFaceccModule} from 'reality/app/xr/js/src/types/xrfacecc'

import {Message} from 'capnp-ts'
import {
  AttachmentPointMsg, FaceMsg, FaceOptions, FaceResponse, FrameworkResponse,
} from 'reality/engine/api/face.capnp'
import type {DetectedPointsMsg} from 'reality/engine/api/mesh.capnp'
import type {Position32f} from 'reality/engine/api/base/geo-types.capnp'
import {CameraEnvironment} from 'reality/engine/api/base/camera-environments.capnp'
import {CameraCoordinates} from 'reality/engine/api/base/camera-intrinsics.capnp'

/* eslint-enable import/no-unresolved */
import {loadEarModel, loadFacemeshModels} from './faces/load-models'
import {singleton} from './factory'
import type {Quaternion} from './quaternion'
import type {Coordinates, Position, ImageData2D, Uv, Pose} from './types/common'
import type {EnvironmentUpdate} from './types/environment-update'
import type {
  FrameworkHandle, onAttachInput, onCameraStatusChangeInput, ProcessCpuInput, ProcessGpuInput,
  SessionAttributes,
} from './types/pipeline'

// The following imports resolve because of the NODE_PATH variable set in build.js
/* eslint-disable import/no-unresolved */

// Options for defining which portions of the face have mesh triangles returned.
const AttachmentPoints = {
  FOREHEAD: 'forehead',
  RIGHT_EYEBROW_INNER: 'rightEyebrowInner',
  RIGHT_EYEBROW_MIDDLE: 'rightEyebrowMiddle',
  RIGHT_EYEBROW_OUTER: 'rightEyebrowOuter',
  LEFT_EYEBROW_INNER: 'leftEyebrowInner',
  LEFT_EYEBROW_MIDDLE: 'leftEyebrowMiddle',
  LEFT_EYEBROW_OUTER: 'leftEyebrowOuter',
  LEFT_EAR: 'leftEar',
  RIGHT_EAR: 'rightEar',
  LEFT_CHEEK: 'leftCheek',
  RIGHT_CHEEK: 'rightCheek',
  NOSE_BRIDGE: 'noseBridge',
  NOSE_TIP: 'noseTip',
  LEFT_EYE: 'leftEye',
  RIGHT_EYE: 'rightEye',
  LEFT_EYE_OUTER_CORNER: 'leftEyeOuterCorner',
  RIGHT_EYE_OUTER_CORNER: 'rightEyeOuterCorner',
  UPPER_LIP: 'upperLip',
  LOWER_LIP: 'lowerLip',
  MOUTH: 'mouth',
  MOUTH_RIGHT_CORNER: 'mouthRightCorner',
  MOUTH_LEFT_CORNER: 'mouthLeftCorner',
  CHIN: 'chin',
  LEFT_IRIS: 'leftIris',
  RIGHT_IRIS: 'rightIris',
  LEFT_UPPER_EYELID: 'leftUpperEyelid',
  RIGHT_UPPER_EYELID: 'rightUpperEyelid',
  LEFT_LOWER_EYELID: 'leftLowerEyelid',
  RIGHT_LOWER_EYELID: 'rightLowerEyelid',
  // Only available for ear
  EAR_LEFT_HELIX: 'leftHelix',
  EAR_LEFT_CANAL: 'leftCanal',
  EAR_LEFT_LOBE: 'leftLobe',
  EAR_RIGHT_HELIX: 'rightHelix',
  EAR_RIGHT_CANAL: 'rightCanal',
  EAR_RIGHT_LOBE: 'rightLobe',
} as const

// Map the Capnp Proto enums to AttachmentPoints string constants
const AttachmentProtoEnums = Object.keys(AttachmentPoints).reduce(
  (m, v) => { m[AttachmentPointMsg.AttachmentName[v]] = v; return m }, {}
)

// Ear point keys for emitting messages
const EarPointStateKeys = [
  'leftLobeFound', 'leftCanalFound', 'leftHelixFound',
  'rightLobeFound', 'rightCanalFound', 'rightHelixFound',
]

// Options for defining which portions of the face have mesh triangles returned.
const MeshGeometry = {
  FACE: 'face',
  MOUTH: 'mouth',
  EYES: 'eyes',
  IRIS: 'iris',
} as const
type MeshGeometryChoice = typeof MeshGeometry[keyof typeof MeshGeometry];

// Option for defining which UV set to use.
const UvType = {
  PROJECTED: 'projected',
  STANDARD: 'standard',
} as const
type UvTypeChoice = typeof UvType[keyof typeof UvType]

interface DebugOptions {
  extraOutput?: {
    renderedImg?: boolean
    detections?: boolean
    earRenderedImg?: boolean
    earDetections?: boolean
  }
  onlyDetectFaces?: boolean
}

interface Options {
  debug?: DebugOptions
  nearClip?: number
  farClip?: number
  coordinates?: Coordinates
  meshGeometry?: MeshGeometryChoice[]
  uvType?: UvTypeChoice
  useStandardUVs: boolean
  maxDetections: number
  enableEars?: boolean
}

interface FaceData {
  id: number
  transform: {
    position: Position
    rotation: Quaternion
    scale: number
    scaledWidth: number
    scaledHeight: number
    scaledDepth: number
  }
  vertices: Position[]
  normals: Position[]
  uvsInCameraFrame: Uv[]
  attachmentPoints: Record<string, Pose>
  mouthOpen: boolean
  leftEyeOpen: boolean
  rightEyeOpen: boolean
  leftEyebrowRaised: boolean
  rightEyebrowRaised: boolean
  interpupillaryDistanceInMM: number

  leftEarFound: boolean
  rightEarFound: boolean

  leftLobeFound: boolean
  leftCanalFound: boolean
  leftHelixFound: boolean
  rightLobeFound: boolean
  rightCanalFound: boolean
  rightHelixFound: boolean
}

type FaceState = Partial<{
  mouthOpen: boolean
  leftEyeOpen: boolean
  rightEyeOpen: boolean
  leftEyebrowRaised: boolean
  rightEyebrowRaised: boolean
  leftEyeWinked: boolean
  rightEyeWinked: boolean
  interpupillaryDistanceInMM: number
  leftEarFound: boolean
  rightEarFound: boolean
  leftLobeFound: boolean
  leftCanalFound: boolean
  leftHelixFound: boolean
  rightLobeFound: boolean
  rightCanalFound: boolean
  rightHelixFound: boolean
}>

const mapResponseDetection = (d: DetectedPointsMsg) => ({
  confidence: d.getConfidence(),
  viewport: {
    offsetX: d.getViewport().getX(),
    offsetY: d.getViewport().getY(),
    width: d.getViewport().getW(),
    height: d.getViewport().getH(),
  },
  roi: {
    corners: {
      upperLeft: {
        x: d.getRoi().getCorners().getUpperLeft().getX(),
        y: d.getRoi().getCorners().getUpperLeft().getY(),
      },
      upperRight: {
        x: d.getRoi().getCorners().getUpperRight().getX(),
        y: d.getRoi().getCorners().getUpperRight().getY(),
      },
      lowerLeft: {
        x: d.getRoi().getCorners().getLowerLeft().getX(),
        y: d.getRoi().getCorners().getLowerLeft().getY(),
      },
      lowerRight: {
        x: d.getRoi().getCorners().getLowerRight().getX(),
        y: d.getRoi().getCorners().getLowerRight().getY(),
      },
    },
    renderTexToImageTexMatrix44f: d.getRoi().getRenderTexToImageTexMatrix44f().toArray(),
  },
  points: d.getPoints().map(p => ({
    x: p.getX(),
    y: p.getY(),
    z: p.getZ(),
  })),
})

const toXYZ = (p: Position32f): Position => ({
  x: p.getX(),
  y: p.getY(),
  z: p.getZ(),
})

// Combine list of attachment points into an object by the attachment point's name
const reduceAttachmentPoint = (m, a): Record<string, Pose> => {
  m[AttachmentPoints[AttachmentProtoEnums[a.getName()]]] = {
    position: toXYZ(a.getPosition()),
    // TODO(nb): Make mesh attachment orientation more stable.
    // rotation: {
    //   w: a.getRotation().getW(),
    //   x: a.getRotation().getX(),
    //   y: a.getRotation().getY(),
    //   z: a.getRotation().getZ(),
    // },
  }
  return m
}

const mapFaces = (f: FaceMsg): FaceData => {
  // Add ears to the end of face data
  const attachmentPoints = f.getAttachmentPoints().reduce(reduceAttachmentPoint, {})
  f.getEarAttachmentPoints().reduce(reduceAttachmentPoint, attachmentPoints)
  const vertices = f.getVertices().map(toXYZ)
  return {
    id: f.getId(),
    // we're not including the status here because the status will be emitted in the framework
    // event, such as facecontroller.facefound.
    transform: {
      position: toXYZ(f.getTransform().getPosition()),
      rotation: {
        w: f.getTransform().getRotation().getW(),
        x: f.getTransform().getRotation().getX(),
        y: f.getTransform().getRotation().getY(),
        z: f.getTransform().getRotation().getZ(),
      },
      scale: f.getTransform().getScale(),
      scaledWidth: f.getTransform().getScaledWidth(),
      scaledHeight: f.getTransform().getScaledHeight(),
      scaledDepth: f.getTransform().getScaledDepth(),
    },
    vertices,
    normals: f.getNormals().map(toXYZ),
    uvsInCameraFrame: f.getUvsInCameraFrame().map(uv => ({
      u: uv.getU(),
      v: uv.getV(),
    })),
    // Convert the list of anchors into a map with the name being the key
    attachmentPoints,
    mouthOpen: f.getMouthOpen(),
    leftEyeOpen: f.getLeftEyeOpen(),
    rightEyeOpen: f.getRightEyeOpen(),
    leftEyebrowRaised: f.getLeftEyebrowRaised(),
    rightEyebrowRaised: f.getRightEyebrowRaised(),
    interpupillaryDistanceInMM: f.getInterpupillaryDistanceInMM(),

    leftEarFound: f.getLeftEarFound(),
    rightEarFound: f.getRightEarFound(),

    leftLobeFound: f.getLeftLobeFound(),
    leftCanalFound: f.getLeftCanalFound(),
    leftHelixFound: f.getLeftHelixFound(),
    rightLobeFound: f.getRightLobeFound(),
    rightCanalFound: f.getRightCanalFound(),
    rightHelixFound: f.getRightHelixFound(),
  }
}

type Detection = ReturnType<typeof mapResponseDetection>
interface DebugOutputs {
  renderedImg?: ImageData2D
  detections?: {
    faces?: Detection[]
    meshes?: Detection[]
    ears?: Detection[]
    earsInFaceRoi?: Position[]
    earsInCameraFeed?: Position[]
    videoWidth?: number
    videoHeight?: number
  }
  earRenderedImg?: ImageData2D
}

type OnProcessCpuOutput = {
  cameraFeedTexture: number
  intrinsics: number[]
  rotation: Quaternion
  position: Position
  debug?: DebugOutputs
}

const FaceControllerFactory = singleton((
  runConfig, xrccPromise, logCallback, errorCallback
): Promise<any> => {
  let modelGeometry_ = null
  let xrcc_ = null
  let framework_ = null

  let computeHandle_: number = 0  // handle for Compute Context

  // Must resolve the returned promise to ensure that this module CC gets loaded before accessing
  // the constructed controller instance.
  let xrFacecc_: XrFaceccModule | null
  const resolveXrFaceCC = async (): Promise<XrFaceccModule> => {
    if (!xrFacecc_) {
      const xrFacecc = await XRFACECC({
        print: logCallback(false),
        printErr: logCallback(true),
        onAbort: errorCallback,
        onError: errorCallback,
      })
      xrFacecc_ = xrFacecc
    }
    return xrFacecc_
  }
  const xrFacePromise_ = resolveXrFaceCC()

  // Maps the faceId to an expression state object to keep track of whether the face expression
  // state has changed for emitting an event.
  let faceIdToFaceState_: Record<number, FaceState> = {}

  const env_: EnvironmentUpdate = {
    videoWidth: 0,
    videoHeight: 0,
    canvasWidth: 0,
    canvasHeight: 0,
    orientation: 0,
    deviceEstimate: null,
  }

  const options_: Options = {
    debug: null,
    nearClip: null,
    farClip: null,
    coordinates: null,
    meshGeometry: [MeshGeometry.FACE],
    useStandardUVs: true,
    maxDetections: 1,
  }
  if (BuildIf.EARS_20230911) {
    options_.enableEars = false
  }

  let configurationChanged_ = false

  const maybeDispatchConfigChange = (framework: FrameworkHandle) => {
    if (!configurationChanged_ || !framework) {
      return
    }

    if (!options_.coordinates || options_.coordinates.mirroredDisplay === undefined) {
      return
    }

    framework.dispatchEvent('cameraconfigured', {
      mirroredDisplay: options_.coordinates.mirroredDisplay,
      origin: options_.coordinates.origin,
    })
    configurationChanged_ = false
  }

  const maybeWinkOrBlinkStarted = (winkKey: string, faceId: number) => {
    // Now that we've closed and eye, if we open it within the next 750 milliseconds then we could
    // either have a blink or wink. The duration of a blink is on average 100–150 milliseconds
    // according to UCL researcher and between 100 and 400 ms according to the Harvard Database of
    // Useful Biological Numbers. In practice, 500 is too short for a wink and 1000 is too long
    // because it misses natural consecutive blinks.
    faceIdToFaceState_[faceId][winkKey] = true
    setTimeout(() => {
      if (faceIdToFaceState_ && faceIdToFaceState_[faceId]) {
        faceIdToFaceState_[faceId][winkKey] = false
      }
    }, 750)
  }

  const maybeWinkOrBlinkEnded = (winkKey: string, faceId: number, framework: FrameworkHandle) => {
    // Triggered when we open an eye, see if we winked or blinked.
    setTimeout(() => {
      if (!faceIdToFaceState_[faceId]) { return }
      const detail = {id: faceId}
      if (faceIdToFaceState_[faceId].leftEyeWinked && faceIdToFaceState_[faceId].rightEyeWinked) {
        framework.dispatchEvent('blinked', detail)
      } else if (faceIdToFaceState_[faceId][winkKey]) {
        // emit as lowered {left/right}eyewinked.
        framework.dispatchEvent(winkKey.toLowerCase(), detail)
      }
      faceIdToFaceState_[faceId].leftEyeWinked = false
      faceIdToFaceState_[faceId].rightEyeWinked = false
    }, 100)  // Timeout to account for asymmetry of blinking
  }

  const maybeEmitFaceStateUpdates = (faceData: FaceData, framework: FrameworkHandle) => {
    if (!faceIdToFaceState_[faceData.id]) {
      faceIdToFaceState_[faceData.id] = {
        // By default, the IPD is 0.0 before being defined.
        interpupillaryDistanceInMM: 0.0,
      }
    }

    const storedFaceData = faceIdToFaceState_[faceData.id]

    const detail = {id: faceData.id}

    const expressionsToEvent = {
      'mouthOpen': ['mouthopened', 'mouthclosed'],
      'leftEyeOpen': ['lefteyeopened', 'lefteyeclosed'],
      'rightEyeOpen': ['righteyeopened', 'righteyeclosed'],
      'leftEyebrowRaised': ['lefteyebrowraised', 'lefteyebrowlowered'],
      'rightEyebrowRaised': ['righteyebrowraised', 'righteyebrowlowered'],
    }

    Object.keys(expressionsToEvent).forEach((key) => {
      if (storedFaceData[key] !== faceData[key]) {
        framework.dispatchEvent(
          faceData[key] ? expressionsToEvent[key][0] : expressionsToEvent[key][1], detail
        )

        // If there was an eye open/close event, determine if a wink or blink has occurred.
        if (key.endsWith('EyeOpen')) {
          const direction = key.split('EyeOpen')[0]

          // either rightEyeWinked or leftEyeWinked.
          const winkKey = `${direction}EyeWinked`
          // The EyeOpen key is either true or false. True means opened, false means closed.
          if (faceData[key]) {
            maybeWinkOrBlinkEnded(winkKey, faceData.id, framework)
          } else {
            maybeWinkOrBlinkStarted(winkKey, faceData.id)
          }
        }

        storedFaceData[key] = faceData[key]
      }
    })

    if (BuildIf.EARS_20230911) {
      const earStates = ['leftEarFound', 'rightEarFound']
      earStates.forEach((key: string) => {
        if (storedFaceData[key] === faceData[key]) {
          return
        }

        const ear = key === 'leftEarFound' ? 'left' : 'right'
        framework.dispatchEvent(
          faceData[key] ? 'earfound' : 'earlost',
          {
            id: faceData.id,
            ear,
          }
        )
        storedFaceData[key] = faceData[key]
      })

      EarPointStateKeys.forEach((key: string) => {
        if (storedFaceData[key] === faceData[key]) {
          return
        }

        // e.g. leftLobeFound event will map to leftLobe point
        const point = key.substring(0, key.length - 5)
        framework.dispatchEvent(
          faceData[key] ? 'earpointfound' : 'earpointlost',
          {
            id: faceData.id,
            point,
          }
        )
        storedFaceData[key] = faceData[key]
      })
    }

    if (storedFaceData.interpupillaryDistanceInMM !== faceData.interpupillaryDistanceInMM) {
      framework.dispatchEvent(
        'interpupillarydistance',
        {interpupillaryDistance: faceData.interpupillaryDistanceInMM, ...detail}
      )
      storedFaceData.interpupillaryDistanceInMM = faceData.interpupillaryDistanceInMM
    }
  }

  const removeExtraEarData = (faceData: FaceData) => {
    EarPointStateKeys.forEach((key: string) => {
      delete faceData[key]
    })
  }

  const updateEnvironment = ({
    videoWidth,
    videoHeight,
    canvasWidth,
    canvasHeight,
    orientation,
    deviceEstimate,
  }: EnvironmentUpdate) => {
    if (videoWidth !== undefined) {
      env_.videoWidth = videoWidth
    }
    if (videoHeight !== undefined) {
      env_.videoHeight = videoHeight
    }
    if (canvasWidth !== undefined) {
      env_.canvasWidth = canvasWidth
    }
    if (canvasHeight !== undefined) {
      env_.canvasHeight = canvasHeight
    }
    if (orientation !== undefined) {
      env_.orientation = orientation
    }
    if (deviceEstimate !== undefined) {
      env_.deviceEstimate = deviceEstimate
    }

    const message = new Message()
    const env = message.initRoot<CameraEnvironment>(CameraEnvironment)
    env.setCameraWidth(env_.videoWidth)
    env.setCameraHeight(env_.videoHeight)
    env.setDisplayWidth(env_.canvasWidth)
    env.setDisplayHeight(env_.canvasHeight)
    env.setOrientation(env_.orientation)
    if (env_.deviceEstimate) {
      env.getDeviceEstimate().setManufacturer(env_.deviceEstimate.manufacturer)
      env.getDeviceEstimate().setModel(env_.deviceEstimate.model)
      env.getDeviceEstimate().setOs(env_.deviceEstimate.os)
      env.getDeviceEstimate().setOsVersion(env_.deviceEstimate.osVersion)
    }

    xrFacePromise_.then((xrFacecc) => {
      if (computeHandle_ > 0) {
        const buffer = message.toArrayBuffer()
        const ptr = xrFacecc._malloc(buffer.byteLength)
        xrFacecc.writeArrayToMemory(new Uint8Array(buffer), ptr)
        xrFacecc._c8EmAsm_updateCameraEnvironment(ptr, buffer.byteLength)
        xrFacecc._free(ptr)
      }
    })
  }

  // Update `options_` state with new partial configuration.
  const optionsReducer = (args?: Options) => {
    if (!args) {
      return
    }

    const {debug, nearClip, farClip, coordinates, meshGeometry, uvType, maxDetections} = args

    if (debug !== undefined) {
      options_.debug = debug
    }
    if (nearClip !== undefined) {
      options_.nearClip = nearClip
    }
    if (farClip !== undefined) {
      options_.farClip = farClip
    }
    if (meshGeometry !== undefined) {
      options_.meshGeometry = meshGeometry
    }
    if (uvType !== undefined) {
      options_.useStandardUVs = uvType === UvType.STANDARD
    }
    if (maxDetections !== undefined) {
      if (!Number.isInteger(maxDetections)) {
        throw new Error('[XR] maxDetections must be an integer')
      }
      if (maxDetections < 1 || maxDetections > 3) {
        throw new Error(
          `[XR] Invalid maxDetections value ${maxDetections}, must be between 1 and 3`
        )
      }
      options_.maxDetections = maxDetections
    }

    if (BuildIf.EARS_20230911) {
      const {enableEars} = args
      if (enableEars !== undefined) {
        options_.enableEars = enableEars
      }
    }

    // Update coordinate configuration piece by piece to allow different components to be set at
    // different times.
    if (coordinates === null) {
      options_.coordinates = null
    } else if (coordinates !== undefined) {
      if (!options_.coordinates) {
        options_.coordinates = {}
      }
      const {origin, scale, axes, mirroredDisplay} = coordinates
      if (origin !== undefined) {
        configurationChanged_ = true
        options_.coordinates.origin = origin
      }
      if (scale !== undefined) {
        options_.coordinates.scale = scale
      }
      if (axes !== undefined) {
        options_.coordinates.axes = axes
      }
      if (mirroredDisplay !== undefined) {
        configurationChanged_ = configurationChanged_ ||
          (!!mirroredDisplay !== options_.coordinates.mirroredDisplay)
        options_.coordinates.mirroredDisplay = !!mirroredDisplay
      }
    }
  }

  const updateOptions = (args?: Options) => {
    optionsReducer(args)

    const message = new Message()
    const opt = message.initRoot<FaceOptions>(FaceOptions)
    if (options_.debug) {
      const {extraOutput, onlyDetectFaces} = options_.debug
      const optDebug = opt.getDebug()
      if (extraOutput) {
        const {renderedImg, detections, earRenderedImg, earDetections} = extraOutput
        optDebug.getExtraOutput().setRenderedImg(!!renderedImg)
        optDebug.getExtraOutput().setDetections(!!detections)
        optDebug.getExtraOutput().setEarRenderedImg(!!earRenderedImg)
        optDebug.getExtraOutput().setEarDetections(!!earDetections)
      }
      optDebug.setOnlyDetectFaces(!!onlyDetectFaces)
    }
    opt.setNearClip(options_.nearClip > 0 ? options_.nearClip : 0.01)
    opt.setFarClip(options_.farClip > 0 ? options_.farClip : 1000)

    const geometryOptions = options_.meshGeometry || []
    const meshGeometry = opt.initMeshGeometry(geometryOptions.length)
    let i = 0
    geometryOptions.forEach((geometryOption) => {
      if (geometryOption === MeshGeometry.FACE) {
        meshGeometry.set(i, FaceOptions.MeshGeometryOptions.FACE)
        i++
      } else if (geometryOption === MeshGeometry.MOUTH) {
        meshGeometry.set(i, FaceOptions.MeshGeometryOptions.MOUTH)
        i++
      } else if (geometryOption === MeshGeometry.EYES) {
        meshGeometry.set(i, FaceOptions.MeshGeometryOptions.EYES)
        i++
      } else if (geometryOption === MeshGeometry.IRIS) {
        meshGeometry.set(i, FaceOptions.MeshGeometryOptions.IRIS)
        i++
      } else {
        // eslint-disable-next-line no-console
        console.warn(`Invalid meshGeometry option: "${geometryOption}"`)
        meshGeometry.set(i, FaceOptions.MeshGeometryOptions.UNSPECIFIED)
        i++
      }
    })

    opt.setUseStandardUvs(options_.useStandardUVs)

    opt.setMaxDetections(options_.maxDetections)

    const {origin, scale, axes, mirroredDisplay} = options_.coordinates || {}
    const o = opt.getCoordinates().getOrigin()
    if (origin) {
      o.getPosition().setX(origin.position.x)
      o.getPosition().setY(origin.position.y)
      o.getPosition().setZ(origin.position.z)
      o.getRotation().setW(origin.rotation.w)
      o.getRotation().setX(origin.rotation.x)
      o.getRotation().setY(origin.rotation.y)
      o.getRotation().setZ(origin.rotation.z)
    } else {
      o.getPosition().setX(0)
      o.getPosition().setY(0)
      o.getPosition().setZ(0)
      o.getRotation().setW(1)
      o.getRotation().setX(0)
      o.getRotation().setY(0)
      o.getRotation().setZ(0)
    }

    // Default mirrored display to false if not specified.
    opt.getCoordinates().setMirroredDisplay(!!mirroredDisplay)

    opt.getCoordinates().setScale(scale > 0 ? scale : 1.0)

    opt.getCoordinates().setAxes(axes === 'LEFT_HANDED'
      ? CameraCoordinates.Axes.LEFT_HANDED
      : CameraCoordinates.Axes.RIGHT_HANDED)

    if (BuildIf.EARS_20230911) {
      opt.setEnableEars(options_.enableEars)
    }

    return xrFacePromise_.then((xrFacecc) => {
      const buffer = message.toArrayBuffer()
      const ptr = xrFacecc._malloc(buffer.byteLength)
      xrFacecc.writeArrayToMemory(new Uint8Array(buffer), ptr)
      xrFacecc._c8EmAsm_updateFaceOptions(ptr, buffer.byteLength)
      xrFacecc._free(ptr)

      // by updating the texture renderer inside the promise, we keep the camera feed in sync with
      // the face controller pipeline output so the user's face effect updates at the same time.
      XR8.GlTextureRenderer.configure({mirroredDisplay: !!(mirroredDisplay)})
    })
  }

  // NOTE(dat): There seems to be a bug here where initializeFacecontroller and initializeEars
  //            will not re-initialize xrcc methods after a call to _c8EmAsm_faceControllerCleanup.
  //            This happens when onDetach()
  // TODO(dat): Add unit test for attach/detach x 2 behavior.
  const initializeFacecontroller = singleton(() => Promise.all([
    loadFacemeshModels(xrFacePromise_),

    xrccPromise.then((xrcc) => {
      xrcc_ = xrcc
    }),
    xrFacePromise_.then((xrFacecc) => {
      xrFacecc_ = xrFacecc
    }),
    updateOptions(),  // we need updateOptions to be called at least once to get default values.
  ]))

  // load ears model
  const initializeEars = singleton((): Promise<void> => loadEarModel(xrFacePromise_))

  // Configures what processing is performed by FaceController.
  //
  // Inputs:
  // {
  //   nearClip:           [Optional] The distance from the camera of the near clip plane.
  //   farClip:            [Optional] The distance from the camera of the far clip plane.
  //   meshGeometry:       [Optional] List that contains which parts of the head geometry are
  //                                  visible.  Options are:
  //                                  [
  //                                    XR8.FaceController.MeshGeometry.FACE,
  //                                    XR8.FaceController.MeshGeometry.EYES,
  //                                    XR8.FaceController.MeshGeometry.NOSE,
  //                                    XR8.FaceController.MeshGeometry.IRIS,
  //                                  ]
  //                                  The default is [XR8.FaceController.MeshGeometry.FACE]
  //
  //   coordinates: {
  //     origin:           [Optional] {position: {x, y, z}, rotation: {w, x, y, z}} of the camera.
  //     scale:            [Optional] scale of the scene.
  //     axes:             [Optional] 'LEFT_HANDED' or 'RIGHT_HANDED', default is 'RIGHT_HANDED'
  //     mirroredDisplay:  [Optional] If true, flip left and right in the output.
  //   }
  //   maxDetections:      [Optional] Number of faces that can be tracked at once, default is 1. The
  //                                  available choices are 1, 2, or 3.
  //   uvType:            [Optional] Specifies which uvs are returned in the facescanning and
  //                                  faceloading event. Options are:
  //                                  [
  //                                    XR8.FaceController.UvType.PROJECTED,
  //                                    XR8.FaceController.UvType.STANDARD,
  //                                  ]
  //                                  The default is XR8.FaceController.UvType.STANDARD.
  // }
  const configure = (args: Options) => {
    initializeFacecontroller()
    updateOptions(args)
    if (args.enableEars) {
      // This only happen once even if we disable and re-enable ears
      initializeEars()
    }
  }

  // Gets a pipeline module for tracking faces with the specified configuration.
  //
  // Inputs:
  // {
  //   nearClip:           [Optional] The distance from the camera of the near clip plane.
  //   farClip:            [Optional] The distance from the camera of the far clip plane.
  //   meshGeometry:       [Optional] List that contains which parts of the head geometry are
  //                                  visible.  Options are:
  //                                  [
  //                                    XR8.FaceController.MeshGeometry.FACE,
  //                                    XR8.FaceController.MeshGeometry.EYES,
  //                                    XR8.FaceController.MeshGeometry.NOSE,
  //                                  ]
  //                                  The default is [XR8.FaceController.MeshGeometry.FACE]
  //
  //   coordinates: {
  //     origin:           [Optional] {position: {x, y, z}, rotation: {w, x, y, z}} of the camera.
  //     scale:            [Optional] scale of the scene.
  //     axes:             [Optional] 'LEFT_HANDED' or 'RIGHT_HANDED', default is 'RIGHT_HANDED'
  //     mirroredDisplay:  [Optional] If true, flip left and right in the output.
  //   }
  // }
  const pipelineModule = (args: Options) => {
    // Start loading tf stuff if it has't been loaded already.
    initializeFacecontroller()

    if (args) {
      configure(args)
    }

    const onAttach = ({
      computeCtx,
      framework,
      canvasWidth,
      canvasHeight,
      videoWidth,
      videoHeight,
      orientation,
    }: onAttachInput) => {
      if (xrFacecc_) {
        computeHandle_ = xrFacecc_.GL.registerContext(computeCtx, computeCtx.getContextAttributes())
        xrFacecc_.GL.makeContextCurrent(computeHandle_)
      }
      if (xrFacecc_ && xrcc_) {
        // Share the previously allocated textures with the global xrcc GL compute context
        if ((xrFacecc_.GL.textures !== xrcc_.GL.textures)) {
          xrFacecc_.GL.textures = xrcc_.GL.textures
        }
      }

      framework_ = framework
      maybeDispatchConfigChange(framework_)
      framework_.dispatchEvent('mirrordisplay', {
        mirroredDisplay: !!(options_.coordinates && options_.coordinates.mirroredDisplay),
      })

      // TODO(nb): get model geometry from c++
      xrFacePromise_
        .then((xrFacecc) => {
          xrFacecc._c8EmAsm_getFrameworkData()
          // Read processed output.
          const byteBuffer =
            xrFacecc_.HEAPU8.subarray(window._c8.frameworkResponse,
              window._c8.frameworkResponse + window._c8.frameworkResponseSize)
          const message = new Message(byteBuffer, false).getRoot<FrameworkResponse>(
            FrameworkResponse
          )
          const mg = message.getModelGeometry()

          const indices = mg.getIndices().map(index => ({
            a: index.getA(),
            b: index.getB(),
            c: index.getC(),
          }))

          const uvs = mg.getUvs().map(uv => ({
            u: uv.getU(),
            v: uv.getV(),
          }))

          // TODO(nb): modelGeometry can be local scope; remove the member variable.
          modelGeometry_ = {
            maxDetections: mg.getMaxDetections(),
            pointsPerDetection: mg.getPointsPerDetection(),
            indices,
            uvs,
          }
        })
        .then(() => framework.dispatchEvent('faceloading', modelGeometry_))
        .then(() => initializeFacecontroller())
        .then(() => framework.dispatchEvent('facescanning', modelGeometry_))

      // Configure GLTextureRenderer to be in sync with face processing.
      XR8.GlTextureRenderer.configure({
        mirroredDisplay: !!(options_.coordinates && options_.coordinates.mirroredDisplay),
      })
      XR8.GlTextureRenderer.setTextureProvider(
        ({processCpuResult: {facecontroller}}) => (
          facecontroller ? facecontroller.cameraFeedTexture : null)
      )
      updateEnvironment({
        canvasWidth,
        canvasHeight,
        videoWidth,
        videoHeight,
        orientation,
        deviceEstimate: XR8.XrDevice.deviceEstimate(),
      })
    }

    const onStart = ({
      canvasWidth,
      canvasHeight,
      videoWidth,
      videoHeight,
      orientation,
    }: EnvironmentUpdate) => {
      maybeDispatchConfigChange(framework_)
      updateEnvironment({
        canvasWidth,
        canvasHeight,
        videoWidth,
        videoHeight,
        orientation,
        deviceEstimate: XR8.XrDevice.deviceEstimate(),
      })
    }

    const onDetach = ({framework}: {framework: FrameworkHandle}) => {
      maybeDispatchConfigChange(framework)
      XR8.GlTextureRenderer.setTextureProvider(null)
      XR8.GlTextureRenderer.configure({mirroredDisplay: false})
      if (xrFacecc_) {
        xrFacecc_._c8EmAsm_faceControllerCleanup()
        xrFacecc_.GL.textures = null
        computeHandle_ = 0
        xrFacecc_.GL.makeContextCurrent(computeHandle_)
      }

      // we have set the values of the FaceControllerData singleton inside facecontroller.cc
      // to nullptr when calling faceControllerCleanup.  We need to unreference those values
      // in JS or else we will be pointing to an already deleted object.
      window._c8.response = null
      window._c8.responseSize = null
      window._c8.responseTexture = null
      framework_ = null
      faceIdToFaceState_ = {}
    }

    const onProcessGpu = ({framework, frameStartResult}: ProcessGpuInput) => {
      const {cameraTexture, repeatFrame} = frameStartResult

      if (repeatFrame) {
        return
      }

      maybeDispatchConfigChange(framework)
      xrFacecc_._c8EmAsm_stageFaceControllerFrame(cameraTexture.name)
    }

    // Returns:
    // {
    //   rotation: {w, x, y, z} The orientation (quaternion) of the camera in the scene.
    //   position: {x, y, z} The position of the camera in the scene.
    //   intrinsics: a 16 dimensional column-major 4x4 projection matrix that gives the scene
    //     camera the same field of view as the rendered camera feed.
    //   cameraFeedTexture: The WebGLTexture containing camera feed data.
    // }
    //
    // Dispatches Events:
    //
    // faceloading: Fires when loading begins for additional face AR resources.
    //    detail: {
    //      maxDetections: the maximum number of faces that can be simultaneously processed.
    //      pointsPerDetection: number of vertices that will be extracted per face.
    //      indices [{a, b, c]: indexes into the vertices array that form the triangles of the
    //        requested mesh, as specified with meshGeometry on configure.
    //      uvs [{u, v]: uv positions into a texture map corresponding to the returned vertex
    //        points.
    //    }
    // facescanning: Fires when all face AR resources have been loaded and scanning has begun.
    //    detail: {
    //      maxDetections: the maximum number of faces that can be simultaneously processed.
    //      pointsPerDetection: number of vertices that will be extracted per face.
    //      indices: [{a, b, c]: indexes into the vertices array that form the triangles of the
    //        requested mesh, as specified with meshGeometry on configure.
    //      uvs: [{u, v]: uv positions into a texture map corresponding to the returned vertex
    //        points.
    //    }
    // facefound: Fires when a face first found.
    //    detail: {
    //      id: A numerical id of the located face.
    //      transform: {
    //        position {x, y, z}: The 3d position of the located face.
    //        rotation {w, x, y, z}: The 3d local orientation of the located face.
    //        scale: A scale factor that should be applied to objects attached to this face.
    //        scaledWidth: Approximate width of the head in the scene when multiplied by scale.
    //        scaledHeight: Approximate height of the head in the scene when multiplied by scale.
    //        scaledDepth: Approximate depth of the head in the scene when multiplied by scale.
    //      }
    //      vertices [{x, y, z}]: Position of face points, relative to transform.
    //      normals [{x, y, z}]: Normal direction of vertices, relative to transform.
    //      attachmentPoints: {
    //        'name': {  // see list of available attachment point names below
    //          position: {x, y, z}: position of attachment point on the face, relative to the
    //            transform
    //        }
    //      }
    //    }
    // faceupdated: Fires when a face is subsequently found.
    //    detail: {
    //      id: A numerical id of the located face.
    //      transform: {
    //        position {x, y, z}: The 3d position of the located face.
    //        rotation {w, x, y, z}: The 3d local orientation of the located face.
    //        scale: A scale factor that should be applied to objects attached to this face.
    //        scaledWidth: Approximate width of the head in the scene when multiplied by scale.
    //        scaledHeight: Approximate height of the head in the scene when multiplied by scale.
    //        scaledDepth: Approximate depth of the head in the scene when multiplied by scale.
    //      }
    //      vertices [{x, y, z}]: Position of face points, relative to transform.
    //      normals [{x, y, z}]: Normal direction of vertices, relative to transform.
    //      attachmentPoints: {
    //        'name': {  // see list of available attachment point names below
    //          position: {x, y, z}: position of attachment point on the face, relative to the
    //            transform
    //        }
    //      }
    //    }
    // facelost: Fires when a face is no longer being tracked.
    //    detail: {
    //      id: A numerical id of the face that was lost.
    //    }
    //
    // The names of the attachment points are:
    // [ forehead, rightEyebrowInner, rightEyebrowMiddle, rightEyebrowOuter, leftEyebrowInner,
    //   leftEyebrowMiddle, leftEyebrowOuter, leftEar, rightEar, leftCheek, rightCheek, noseBridge,
    //   noseTip, leftEye, rightEye, leftEyeOuterCorner, rightEyeOuterCorner, upperLip, lowerLip,
    //   mouth, mouthRightCorner, mouthLeftCorner, chin ]
    //
    // Extra ear tracking attachment points are:
    // [ leftLobe, leftCanal, leftHelix, rightLobe, rightCanal, rightHelix ]
    //
    const onProcessCpu = ({framework, frameStartResult}: ProcessCpuInput): OnProcessCpuOutput => {
      if (runConfig.paused) {
        return null
      }

      maybeDispatchConfigChange(framework)
      const {repeatFrame} = frameStartResult

      if (!repeatFrame) {
        if (!window._c8) {
          window._c8 = {}
        }
        xrFacecc_._c8EmAsm_processStagedFaceControllerFrame()
      }

      if (!window._c8.responseTexture) {
        return null
      }

      // Read processed output.
      const byteBuffer = xrFacecc_.HEAPU8.subarray(
        window._c8.response,
        window._c8.response + window._c8.responseSize
      )
      const message = new Message(byteBuffer, false).getRoot<FaceResponse>(FaceResponse)

      // Dispatch framework events.
      message.getFaces().forEach((f) => {
        if (f.getStatus() === FaceMsg.TrackingStatus.LOST) {
          framework.dispatchEvent('facelost', {id: f.getId()})
        } else if (f.getStatus() === FaceMsg.TrackingStatus.FOUND) {
          const faceData = mapFaces(f)
          framework.dispatchEvent('facefound', faceData)
          maybeEmitFaceStateUpdates(faceData, framework_)
          removeExtraEarData(faceData)
        } else if (f.getStatus() === FaceMsg.TrackingStatus.UPDATED) {
          const faceData = mapFaces(f)
          framework.dispatchEvent('faceupdated', faceData)
          maybeEmitFaceStateUpdates(faceData, framework_)
          removeExtraEarData(faceData)
        }
      })

      const camera = message.getCamera()
      const intrinsics = camera.getIntrinsic().getMatrix44f().toArray()
      const extrinsic = camera.getExtrinsic()
      const xrq = extrinsic.getRotation()
      const xrp = extrinsic.getPosition()
      const rotation = {x: xrq.getX(), y: xrq.getY(), z: xrq.getZ(), w: xrq.getW()}
      const position = {x: xrp.getX(), y: xrp.getY(), z: xrp.getZ()}

      const output: OnProcessCpuOutput = {
        cameraFeedTexture: XR8.drawTexForComputeTex(window._c8.responseTexture),
        // Information about the camera on this frame. Info contained in emitted face events is
        // relative to these values.
        intrinsics,
        rotation,
        position,
      }

      // Extract debug output if requested.
      if (message.hasDebug()) {
        output.debug = {}
        if (message.getDebug().hasRenderedImg()) {
          const rim = message.getDebug().getRenderedImg()
          Object.assign(output.debug, {
            renderedImg: {
              rows: rim.getRows(),
              cols: rim.getCols(),
              rowBytes: rim.getBytesPerRow(),
              pixels: rim.getUInt8PixelData().toUint8Array(),
            },
          })
        }
        if (message.getDebug().hasDetections() && message.getDebug().getDetections().hasFaces()) {
          const dbgFaces = message.getDebug().getDetections().getFaces().map(mapResponseDetection)
          const dbgMeshes = message.getDebug().getDetections().getMeshes().map(mapResponseDetection)
          Object.assign(output.debug, {detections: {faces: dbgFaces, meshes: dbgMeshes}})
        }
        if (BuildIf.EARS_20230911) {
          if (message.getDebug().hasEarRenderedImg()) {
            const rim = message.getDebug().getEarRenderedImg()
            Object.assign(output.debug, {
              earRenderedImg: {
                rows: rim.getRows(),
                cols: rim.getCols(),
                rowBytes: rim.getBytesPerRow(),
                pixels: rim.getUInt8PixelData().toUint8Array(),
              },
            })
          }
          if (message.getDebug().hasDetections() && message.getDebug().getDetections().hasEars()) {
            const dbgEars = message.getDebug().getDetections().getEars().map(mapResponseDetection)
            output.debug.detections = output.debug.detections || {}
            Object.assign(output.debug.detections, {ears: dbgEars})

            const dbgEarsInRoi = message.getDebug().getDetections().getEarsInFaceRoi().map(toXYZ)
            Object.assign(output.debug.detections, {earsInFaceRoi: dbgEarsInRoi})

            const dbgEarsInCamera = message.getDebug().getDetections().getEarsInCameraFeed()
              .map(toXYZ)
            Object.assign(output.debug.detections, {earsInCameraFeed: dbgEarsInCamera})
            Object.assign(output.debug.detections, {videoWidth: env_.videoWidth})
            Object.assign(output.debug.detections, {videoHeight: env_.videoHeight})
          }
        }
      }

      return output
    }

    const onCanvasSizeChange = ({
      canvasWidth,
      canvasHeight,
      videoWidth,
      videoHeight,
    }: EnvironmentUpdate) => {
      maybeDispatchConfigChange(framework_)
      updateEnvironment({canvasWidth, canvasHeight, videoWidth, videoHeight})
    }

    const onVideoSizeChange = ({
      canvasWidth,
      canvasHeight,
      videoWidth,
      videoHeight,
      orientation,
    }: EnvironmentUpdate) => {
      maybeDispatchConfigChange(framework_)
      updateEnvironment({canvasWidth, canvasHeight, videoWidth, videoHeight, orientation})
    }

    const onDeviceOrientationChange = ({
      videoWidth,
      videoHeight,
      orientation,
    }: EnvironmentUpdate) => {
      maybeDispatchConfigChange(framework_)
      updateEnvironment({videoWidth, videoHeight, orientation})
    }

    const onCameraStatusChange = ({
      status,
      video,
      cameraConfig,
    }: onCameraStatusChangeInput) => {
      if (status === 'requesting') {
        const {direction} = cameraConfig
        const {os} = XR8.XrDevice.deviceEstimate()
        const isMobile = os === 'iOS' || os === 'Android'

        if (isMobile && direction === XR8.XrConfig.camera().BACK &&
          options_.coordinates.mirroredDisplay === true) {
          // eslint-disable-next-line
          console.warn('mirroredDisplay is not compatible with the back camera.' +
            'mirroredDisplay has been set to false.')
          options_.coordinates.mirroredDisplay = false
          updateOptions()
        }
      }

      if (status !== 'hasVideo' || !video) {
        return
      }
      maybeDispatchConfigChange(framework_)
      updateEnvironment({
        videoWidth: video.videoWidth,
        videoHeight: video.videoHeight,
      })
    }

    const onBeforeSessionInitialize = ({
      sessionAttributes,
    }: {
      sessionAttributes: SessionAttributes
    }) => {
      if (!sessionAttributes.fillsCameraTexture) {
        throw new Error('[XR] FaceController requires a session that can provide a camera texture')
      }
      if (!XR8.XrDevice.isDeviceBrowserCompatible(runConfig)) {
        throw new Error('[XR] FaceController requires a session that is compatible with the device')
      }
    }

    return {
      name: 'facecontroller',
      onAttach,
      onBeforeSessionInitialize,
      onCameraStatusChange,
      onCanvasSizeChange,
      onDeviceOrientationChange,
      onDetach,
      onProcessCpu,
      onProcessGpu,
      onStart,
      onVideoSizeChange,
    }
  }

  return xrFacePromise_.then(() => ({
    pipelineModule,
    configure,
    MeshGeometry: Object.freeze(MeshGeometry),
    AttachmentPoints: Object.freeze(AttachmentPoints),
    UvType: Object.freeze(UvType),
  }))
})

export {
  FaceControllerFactory,
}
