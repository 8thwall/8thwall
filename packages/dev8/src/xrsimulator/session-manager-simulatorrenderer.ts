import type {
  RunConfig,
  RunOnCameraStatusChangeCb,
} from '@repo/reality/app/xr/js/src/types/pipeline'
import {checkGLError} from '@repo/reality/app/xr/js/src/gl-renderer'

import type {
  LoopBoundaries, SimulatorConfig, SimulatorSessionManager, LocationEventControls,
  ThreeJsonObject, TargetGeometry,
} from './simulator-types'
import {loadFrameData, LoadedFrameData} from './frame-data'
import {
  emitFakeOrientationData,
  emitOrientationData,
} from './motion-orientation-events'
import {broadcastLoadingSequence} from './broadcast-messages'
import type {MotionDetails} from './motion-orientation-events'

declare const THREE: any
declare const XR8: any
type Texture = typeof THREE.Texture
type Object3D = typeof THREE.Object3D

const START_POINT_OFFSET = 250000000
const MODEL_URL = 'https://cdn.8thwall.com/web/resources/model-standalone-lwrzwsvy.js'
const WORKER_URL = 'https://cdn.8thwall.com/web/resources/model-manager-worker-lws0ty8a.js'

// The width and height are consistent with the video sequences
// in session-manager-simulatorsequence.ts.
const RENDER_WIDTH = 768
const RENDER_HEIGHT = 1024

const CAMERA_VELOCITY = 0.05
const MOUSE_SENSITIVITY = 0.005

const GRAVITY = 9.81
const NANOS_TO_MICROS = 1e-3
const MICRO_TO_SECONDS = 1e-6
const MILLIS_TO_NANOS = 1e6

const SessionManagerSimulatorRenderer = (
  simulatorConfig: SimulatorConfig
): {sessionManager: SimulatorSessionManager, locationEventControls: LocationEventControls} => {
  const defaultConfig = {
    version: -1,
    simulatorId: simulatorConfig.simulatorId,
    cameraUrl: '',
  }
  const mouseControls = {
    button: 2,
  }
  const keyboardControls = {
    moveForwardKey: 'w',
    moveBackwardKey: 's',
    moveLeftKey: 'a',
    moveRightKey: 'd',
    moveUpKey: 'e',
    moveDownKey: 'q',
  }
  const keyboardInput = {
    moveForward: 0,
    moveBackward: 0,
    moveLeft: 0,
    moveRight: 0,
    moveUp: 0,
    moveDown: 0,
  }
  const mouseInput = {
    mouseDown: 0,
    movementX: 0,
    movementY: 0,
  }

  let movementSpeed = 1

  let config_: RunConfig
  let frameData_: LoadedFrameData | null = null

  // Keeping track of where we are in the loop and whether we need to seek back to the start
  let currentTimeInNanos_ = 0
  let loopStartInNanos_ = 0
  let loopEndInNanos_ = 0
  let loopRefTimeInNanos_ = 0

  // Used to normalize the time of motion events, relative to a base event time
  // During recording, the event time nanos is from the start of the page load. The media / capnp
  // recorded is after the payload (by 10+ seconds). We need to compute this offset so the time of
  // motion events is relative to these recordings.
  // let baseTimeOffset_ = 0

  // For seeking motion data within frameData_
  // let motionDataCursor_: MotionDataCursor | null = null

  const scrubbing_ = false

  let simulatorConfig_: SimulatorConfig = defaultConfig

  let runOnCameraStatusChange_: RunOnCameraStatusChangeCb | null = null

  const objectLoader_ = new THREE.ObjectLoader()
  const bufferGeometryLoader_ = new THREE.BufferGeometryLoader()

  const parseBufferGeometry = (threeObject: ThreeJsonObject) => {
    if (threeObject.metadata.type === 'BufferGeometry') {
      return bufferGeometryLoader_.parse(threeObject)
    }
    if (threeObject.metadata.type === 'Object' && threeObject.object.type === 'Group') {
      return objectLoader_.parse(threeObject)?.children?.[0]?.geometry
    }
    return null
  }

  const scene_ = new THREE.Scene()
  const renderer_ = new THREE.WebGLRenderer()
  let camera_ = new THREE.PerspectiveCamera()
  let loadedMesh_ = null
  let detectedMesh_: ReturnType<typeof parseBufferGeometry> = null
  let splatManager_: any = null

  // Camera movement.
  const cameraVelocity_ = new THREE.Vector3(CAMERA_VELOCITY, CAMERA_VELOCITY, CAMERA_VELOCITY)
  const verticalSensitivity_ = MOUSE_SENSITIVITY
  const horizontalSensitivity_ = MOUSE_SENSITIVITY
  const camEuler_ = new THREE.Euler(0, 0, 0, 'YXZ')
  const prevEuler_ = new THREE.Euler(0, 0, 0, 'YXZ')

  // Scratch variables for calculating acceleration.
  let prevTimeNanos_ = new Date().getTime()
  const prevPosition_ = new THREE.Vector3()
  const prevVelocity_ = new THREE.Vector3()
  const currVelocity_ = new THREE.Vector3()

  const loadScript = (url: string) => new Promise<void>((resolve, reject) => {
    const script = document.createElement('script')
    script.async = true
    script.crossOrigin = 'anonymous'
    script.onload = () => resolve()
    script.onerror = reject
    script.src = url
    document.head.appendChild(script)
  })

  const dispatchLocationScanning = () => {
    const locationScanningEvent = new CustomEvent('locationscanning8w', {detail: {}})
    window.dispatchEvent(locationScanningEvent)
  }

  const dispatchLocationFound = () => {
    // Leave location transform as identity for now.
    const locationMsg = {
      position: {
        x: 0,
        y: 0,
        z: 0,
      },
      rotation: {
        x: 0,
        y: 0,
        z: 0,
        w: 1,
      },
      name: simulatorConfig_.locationName,
    }

    const locationEvent = new CustomEvent('locationfound8w', {detail: locationMsg})
    window.dispatchEvent(locationEvent)
  }

  // Fires location lost and scanning.
  const dispatchLocationLost = () => {
    const locationMsg = {
      position: {
        x: 0,
        y: 0,
        z: 0,
      },
      rotation: {
        x: 0,
        y: 0,
        z: 0,
        w: 1,
      },
      name: simulatorConfig_.locationName,
    }
    const locationEvent = new CustomEvent('locationlost8w', {detail: locationMsg})
    window.dispatchEvent(locationEvent)
  }

  const dispatchMeshFound = () => {
    if (!simulatorConfig_.loadedMesh) {
      return
    }

    if (detectedMesh_ !== simulatorConfig_.detectedMesh && simulatorConfig_.detectedMesh) {
      detectedMesh_ = parseBufferGeometry(simulatorConfig_.detectedMesh)
    }

    const meshMsg = {
      position: {
        x: 0,
        y: 0,
        z: 0,
      },
      rotation: {
        w: 1,
        x: 0,
        y: 0,
        z: 0,
      },
      // Note(Riyaan): Unlike the engine, we populate bufferGeometry instead of
      // geometry for the simulated meshfound event.
      bufferGeometry: detectedMesh_,
      id: simulatorConfig_.detectedMeshSessionId,
    }
    const meshFoundEvent = new CustomEvent('meshfound8w', {detail: meshMsg})
    window.dispatchEvent(meshFoundEvent)
  }

  const dispatchMeshLost = () => {
    const meshLostEvent = new CustomEvent('meshlost8w', {
      detail: {id: simulatorConfig_.detectedMeshSessionId},
    })
    window.dispatchEvent(meshLostEvent)
  }

  const makeSplatSystem = async (splatUrl: string) => {
    // Load splat data as an asset to send to model manager.
    const response = await fetch(splatUrl)
    const data = await response.blob()
    // Given that the splat data fetched from the VPS server is in SPZ format, here we set filename
    // to a dummy filename with .spz extension so that ModelManager can decode the data correctly.
    const asset = {data, filename: 'gaussian_splat.spz'}
    let Model: any

    // Get splat model manager to load the splat.
    const getModelPromise = (() => {
      let promise: Promise<void> | null = null
      return () => {
        if (!promise) {
          promise = new Promise<void>((resolve, reject) => {
            loadScript(MODEL_URL).then(() => {
              Model = (window as any).Model
              Model.setInternalConfig({workerUrl: WORKER_URL})
              resolve()
            }).catch(reject)
          })
        }
        return promise
      }
    })()

    await getModelPromise()
    splatManager_ = Model.ThreejsModelManager.create({camera: camera_})
    const buffer = new Uint8Array(await asset.data.arrayBuffer())
    splatManager_.setModelBytes(asset.filename, buffer)
    splatManager_.setRenderWidthPixels(RENDER_WIDTH)

    return splatManager_.model()
  }

  const loadTexture = (url: string) => new Promise((resolve, reject) => {
    const loader = new THREE.TextureLoader()
    loader.load(url, resolve, undefined, reject)
  })

  const processCylinderMetadata = (metadata: TargetGeometry) => {
    const {
      cylinderCircumferenceTop,
      cylinderCircumferenceBottom,
      cylinderSideLength,
      arcAngle,
    } = metadata

    let radiusTop = (cylinderCircumferenceTop / (2 * Math.PI))
    let radiusBottom = (cylinderCircumferenceBottom / (2 * Math.PI))
    const arcStartRadians = 0
    const arcLengthRadians = (arcAngle / 180) * Math.PI
    let cylinderHeight = Math.sqrt((cylinderSideLength ** 2.0) - (radiusTop - radiusBottom) ** 2.0)

    const uniformScale = (1 / cylinderHeight)

    // Apply scaling to parameters
    radiusTop *= uniformScale
    radiusBottom *= uniformScale
    cylinderHeight *= uniformScale

    return {
      radiusTop,
      radiusBottom,
      cylinderHeight,
      arcStartRadians,
      arcLengthRadians,
    }
  }

  const addWireframeMesh = (parent: Object3D, radiusTop: number, radiusBottom: number,
    height: number, arcStartRadians: number, arcLengthRadians: number) => {
    // Render the wireframe mesh for the cylinder
    const wireframeMaterial = new THREE.MeshBasicMaterial({
      color: 0xFFFFFF,
      wireframe: true,
    })
    const complementArcStart = arcStartRadians + arcLengthRadians
    const complementArcLength = 2 * Math.PI - arcLengthRadians
    const wireframeGeometry = new THREE.CylinderGeometry(
      radiusTop,
      radiusBottom,
      height,
      64,
      1,
      true,
      complementArcStart,
      complementArcLength
    )
    const wireframe = new THREE.Mesh(wireframeGeometry, wireframeMaterial)
    wireframe.rotation.y = (2 * Math.PI) - (arcLengthRadians / 2)

    // Render the top and bottom wireframe circles
    const circleMatieral = new THREE.LineBasicMaterial({color: 0xFFFFFF})
    const topWireframeGeometry = new THREE.CircleGeometry(radiusTop, 64)
    const topWireframe = new THREE.LineLoop(topWireframeGeometry, circleMatieral)
    topWireframe.position.y = height / 2
    topWireframe.rotation.x = Math.PI / 2

    const bottomWireframeGeometry = new THREE.CircleGeometry(radiusBottom, 64)
    const bottomWireframe = new THREE.LineLoop(bottomWireframeGeometry, circleMatieral)
    bottomWireframe.position.y = -height / 2
    bottomWireframe.rotation.x = Math.PI / 2

    parent.add(wireframe)
    parent.add(topWireframe)
    parent.add(bottomWireframe)
  }

  const offsetMatrix = new THREE.Matrix4()
  const startRenderingMeshScene = () => {
    const ambientLight = new THREE.AmbientLight(0xcccccc, 0.5)
    scene_.add(ambientLight)

    // If the splat URL is provided, load the splat if visualization is set to 'splat', and
    // attempt to default to splat if visualization is 'none'. Otherwise, load the mesh. If
    // no splat or mesh is provided, load the image target if provided.
    if (simulatorConfig_.splatUrl && simulatorConfig_.visualization !== 'mesh') {
      makeSplatSystem(simulatorConfig_.splatUrl).then((model) => {
        const splatMesh = new THREE.Mesh()
        splatMesh.add(model)
        if (simulatorConfig_.splatOffset) {
          offsetMatrix.fromArray(simulatorConfig_.splatOffset)
          splatMesh.applyMatrix4(offsetMatrix)
        }
        scene_.add(splatMesh)

        // TODO (lynn): Fix hardcoded camera position.
        camera_.position.set(0, 0, 2)
        camera_.quaternion.set(0, 0, 0, 1)
      })
    } else if (simulatorConfig_.loadedMesh) {
      loadedMesh_ = objectLoader_.parse(simulatorConfig_.loadedMesh)
      scene_.add(loadedMesh_)

      // Calculate how far the camera should be from the mesh to best render it.
      const bbox = new THREE.Box3().setFromObject(loadedMesh_)
      const bboxSize = new THREE.Vector3()
      bbox.getSize(bboxSize)
      const bboxCenter = new THREE.Vector3()
      bbox.getCenter(bboxCenter)
      camera_.position.set(bboxCenter.x, bboxCenter.y, bboxCenter.z + bboxSize.z / 4)
      camera_.quaternion.set(0, 0, 0, 1)
    } else if (simulatorConfig_.imageTargetName) {
      // TODO(akashmahesh): Make the background color configurable.
      scene_.background = new THREE.Color(0x8083A2)
      const {
        imageTargetOriginalUrl, imageTargetType, imageTargetMetadata, imageTargetQuaternion,
      } = simulatorConfig_

      loadTexture(imageTargetOriginalUrl!).then((texture: Texture) => {
        let material
        let geometry
        let targetMesh

        const parsedMetadata = JSON.parse(imageTargetMetadata)
        let {originalWidth, originalHeight} = parsedMetadata

        texture.anisotropy = 16
        if (parsedMetadata.isRotated) {
          texture.center = new THREE.Vector2(0.5, 0.5)
          texture.rotation = Math.PI / 2
          texture.updateMatrix()

          // Swap width and height for rotated images
          originalWidth = parsedMetadata.originalHeight
          originalHeight = parsedMetadata.originalWidth
        }

        if (imageTargetType === 'PLANAR') {
          const scaleFactor = Math.min(1 / originalWidth, 1 / originalHeight)
          material = new THREE.MeshBasicMaterial({map: texture, side: THREE.DoubleSide})
          geometry = new THREE.PlaneGeometry(originalWidth, originalHeight)
          geometry.scale(scaleFactor, scaleFactor, scaleFactor)
          targetMesh = new THREE.Mesh(geometry, material)
        } else if (imageTargetType === 'CYLINDER' || imageTargetType === 'CONICAL') {
          const {
            radiusTop,
            radiusBottom,
            cylinderHeight,
            arcStartRadians,
            arcLengthRadians,
          } = processCylinderMetadata(parsedMetadata)

          material = new THREE.MeshBasicMaterial({map: texture, side: THREE.DoubleSide})
          geometry = new THREE.CylinderGeometry(
            radiusTop,
            radiusBottom,
            cylinderHeight,
            64,
            1,
            true,
            arcStartRadians,
            arcLengthRadians
          )

          targetMesh = new THREE.Group()
          addWireframeMesh(
            targetMesh,
            radiusTop,
            radiusBottom,
            cylinderHeight,
            arcStartRadians,
            arcLengthRadians
          )

          const imageMesh = new THREE.Mesh(geometry, material)

          // NOTE(akashmahesh): Flip cylinder to face the camera
          imageMesh.rotation.y = (2 * Math.PI) - (arcLengthRadians / 2)
          targetMesh.add(imageMesh)
        } else {
          throw new Error('(Simulator) Unsupported image target type')
        }

        if (imageTargetQuaternion) {
          targetMesh.quaternion.premultiply(
            new THREE.Quaternion().fromArray(imageTargetQuaternion)
          )
        }

        scene_.add(targetMesh)

        // TODO (akashmahesh): Fix hardcoded camera position.
        camera_.position.set(0, 0.25, 2)
        camera_.quaternion.set(0, 0, 0, 1)
      })
    }
    if (simulatorConfig_.detectedMesh) {
      detectedMesh_ = parseBufferGeometry(simulatorConfig_.detectedMesh)
    }
  }

  // Release all frames in the memory. Clear all config settings and state.
  const clear = () => {
    config_ = {}
    frameData_ = null
    // motionDataCursor_ = null
    currentTimeInNanos_ = 0
    loopStartInNanos_ = 0
    loopEndInNanos_ = 0
    // baseTimeOffset_ = 0

    simulatorConfig_ = defaultConfig
    scene_.remove(...scene_.children)
  }

  // Given the frameData_, set the start and end frame number,
  // then move the current frame to start
  // if no frameData_, durationInNanos default to 1s.
  const setLoopFrames = () => {
    let durationInNanos = 1e9
    if (frameData_?.motionOrientation?.length) {
      const lastFrame = frameData_.motionOrientation.length - 1
      durationInNanos = frameData_.motionOrientation[lastFrame].timeNanos -
        frameData_.motionOrientation[0].timeNanos
    } else if (frameData_?.gpsData?.length) {
      const lastFrame = frameData_.gpsData.length - 1
      durationInNanos = frameData_.gpsData[lastFrame].timeNanos - frameData_.gpsData[0].timeNanos
    }
    loopStartInNanos_ = 0
    loopEndInNanos_ = durationInNanos
    currentTimeInNanos_ = loopStartInNanos_
    loopRefTimeInNanos_ = Date.now() * MILLIS_TO_NANOS
  }

  const updateLoopEndpoints = (loopBoundaries: LoopBoundaries) => {
    const {start, end} = loopBoundaries

    simulatorConfig_.start = start ?? simulatorConfig_.start
    simulatorConfig_.end = end ?? simulatorConfig_.end

    setLoopFrames()
  }

  const updateSequenceSrc = async (sequenceUrl: string) => {
    if (sequenceUrl && sequenceUrl !== simulatorConfig_.sequenceUrl) {
      simulatorConfig_.sequenceUrl = sequenceUrl
      broadcastLoadingSequence(true)
      frameData_ = await loadFrameData('', sequenceUrl)
      broadcastLoadingSequence(false)
      // TODO(lynn): Temporarily disabling motion data.
      // if (frameData_?.motionOrientation?.length) {
      //   motionDataCursor_ = makeMotionDataCursor(frameData_.motionOrientation)
      //   // baseTimeOffset_ = getBaseTimeOffset(frameData_.motionOrientation[0])
      // } else {
      //   motionDataCursor_ = null
      //   // baseTimeOffset_ = 0
      // }

      setLoopFrames()
    }
  }

  const updateLoadedMeshSrc = async (loadedMesh?: object) => {
    scene_.remove(...scene_.children)

    simulatorConfig_.loadedMesh = loadedMesh

    startRenderingMeshScene()
    setLoopFrames()
  }

  const updateSplatUrlSrc = async (splatUrl?: string, splatOffset?: number[]) => {
    scene_.remove(...scene_.children)

    simulatorConfig_.splatUrl = splatUrl
    simulatorConfig_.splatOffset = splatOffset

    startRenderingMeshScene()
    setLoopFrames()
  }

  const updateImageTargetInfo = async (imageTargetName?: string, imageTargetType?: string,
    imageTargetOriginalUrl?: string, imageTargetMetadata?: string,
    imageTargetQuaterion?: [number, number, number, number]) => {
    scene_.remove(...scene_.children)

    simulatorConfig_.imageTargetName = imageTargetName
    simulatorConfig_.imageTargetType = imageTargetType
    simulatorConfig_.imageTargetOriginalUrl = imageTargetOriginalUrl
    simulatorConfig_.imageTargetMetadata = imageTargetMetadata
    simulatorConfig_.imageTargetQuaternion = imageTargetQuaterion

    startRenderingMeshScene()
    setLoopFrames()
  }

  const updateConfig = async (config: SimulatorConfig) => {
    const {
      version,
      splatUrl,
      splatOffset,
      loadedMesh,
      sequenceUrl,
      isPaused,
      start,
      end,
      studioPause,
      locationName,
      detectedMesh,
      detectedMeshSessionId,
      visualization,
      imageTargetName,
      imageTargetType,
      imageTargetOriginalUrl,
      imageTargetMetadata,
      imageTargetQuaternion,
    } = config

    // Make sure we don't apply a stale/redundant update.
    if (version <= simulatorConfig_.version) {
      return
    }
    simulatorConfig_.version = version
    simulatorConfig_.visualization = visualization

    simulatorConfig_.locationName = locationName
    simulatorConfig_.detectedMeshSessionId = detectedMeshSessionId

    const engineIntrinsic = XR8.XrController.getIntrinsic()
    if (engineIntrinsic) {
      camera_.fov = (2.0 * Math.atan(1.0 / engineIntrinsic[5]) * 180.0) / Math.PI
      camera_.updateProjectionMatrix()
    }

    if (sequenceUrl) {
      await updateSequenceSrc(sequenceUrl)
    }

    if (splatUrl !== simulatorConfig_.splatUrl || splatOffset !== simulatorConfig_.splatOffset) {
      await updateSplatUrlSrc(splatUrl, splatOffset)
    }
    if (loadedMesh !== simulatorConfig_.loadedMesh) {
      await updateLoadedMeshSrc(loadedMesh)
    }
    if (detectedMesh !== simulatorConfig_.detectedMesh) {
      simulatorConfig_.detectedMesh = detectedMesh
    }
    if (imageTargetName !== simulatorConfig_.imageTargetName) {
      await updateImageTargetInfo(
        imageTargetName,
        imageTargetType,
        imageTargetOriginalUrl,
        imageTargetMetadata,
        imageTargetQuaternion
      )
    }

    if (typeof start !== 'undefined' || typeof end !== 'undefined') {
      updateLoopEndpoints({start, end})
    }

    simulatorConfig_.studioPause = !!studioPause
    simulatorConfig_.isPaused = !!isPaused || simulatorConfig_.studioPause
  }

  const getForwardDirection = () => {
    const direction = new THREE.Vector3(0, 0, -1)
    if (!camera_) return direction
    camera_.updateMatrixWorld(true)
    direction.applyMatrix4(camera_.matrixWorld)
    direction.normalize()
    return camera_.getWorldDirection(direction)
  }

  const getSideDirection = () => {
    const right = new THREE.Vector3()
    if (!camera_) return right
    camera_.updateMatrixWorld()
    const matrix = camera_.matrixWorld
    right
      .set(matrix.elements[0], matrix.elements[1], matrix.elements[2])
      .normalize()
    return right
  }

  const updateRenderCamera = () => {
    // TODO (lynn): Implement other camera movement options.
    // Using mouse movement to rotate the camera
    camEuler_.x -= mouseInput.movementY * verticalSensitivity_
    camEuler_.y -= mouseInput.movementX * horizontalSensitivity_

    mouseInput.movementX = 0
    mouseInput.movementY = 0

    // Limit x-axis rotation to prevent camera from flipping over and to prevent gimbal lock
    const upDownAngleLimitDeg = 80
    camEuler_.x = Math.max(
      -THREE.MathUtils.degToRad(upDownAngleLimitDeg),
      Math.min(THREE.MathUtils.degToRad(upDownAngleLimitDeg), camEuler_.x)
    )
    camera_.quaternion.setFromEuler(camEuler_)

    // Using keyboard input to move the camera
    const tempForward = getForwardDirection().multiplyScalar((
      keyboardInput.moveForward - keyboardInput.moveBackward) * cameraVelocity_.z)
    const tempSide = getSideDirection().multiplyScalar((
      keyboardInput.moveRight - keyboardInput.moveLeft) * cameraVelocity_.x)

    const tempMoveDelta_ = tempForward.add(tempSide)
    tempMoveDelta_.y = (keyboardInput.moveUp - keyboardInput.moveDown) * cameraVelocity_.y
    tempMoveDelta_.multiplyScalar(movementSpeed)

    camera_.position.add(tempMoveDelta_)

    camera_.updateProjectionMatrix()
    splatManager_?.tick()

    requestAnimationFrame(updateRenderCamera)
  }

  const initialize = async ({config, runOnCameraStatusChange}) => {
    config_ = config
    runOnCameraStatusChange_ = runOnCameraStatusChange

    // Reset camera positioning.
    camera_ = new THREE.PerspectiveCamera(75, RENDER_WIDTH / RENDER_HEIGHT, 0.1, 1000)

    renderer_.setSize(RENDER_WIDTH, RENDER_HEIGHT)
    renderer_.outputColorSpace = THREE.LinearSRGBColorSpace

    return Promise.resolve()
  }

  const obtainPermissions = () => Promise.resolve()

  const stop = () => {}

  const startSession = async () => {
    if (!simulatorConfig.poiId && !simulatorConfig.imageTargetName) {
      runOnCameraStatusChange_!({status: 'failed', reason: 'NO_LOCATION_SRC', config: config_})
      throw new Error('No POI or Image Targets selected, cannot run simulator')
    }

    await updateConfig(simulatorConfig)

    if (scene_.children.length === 0) {
      startRenderingMeshScene()
    }

    requestAnimationFrame(updateRenderCamera)

    return false
  }

  // (re)set recording to start of loop
  const seekToLoopEndpoints = () => {
    currentTimeInNanos_ = loopStartInNanos_
    loopRefTimeInNanos_ = Date.now() * MILLIS_TO_NANOS
  }

  const pause = () => {}

  const resume = () => Promise.resolve(false)

  const hasNewFrame = () => ({
    repeatFrame: false,
    videoSize: {
      width: RENDER_WIDTH,
      height: RENDER_HEIGHT,
    },
  })

  const handleKeyDown = (e) => {
    movementSpeed = e.shiftKey ? 10 : 1
    switch (e.key.toLowerCase()) {
      case keyboardControls.moveForwardKey:
        keyboardInput.moveForward = 0.1
        break
      case keyboardControls.moveBackwardKey:
        keyboardInput.moveBackward = 0.1
        break
      case keyboardControls.moveLeftKey:
        keyboardInput.moveLeft = 0.1
        break
      case keyboardControls.moveRightKey:
        keyboardInput.moveRight = 0.1
        break
      case keyboardControls.moveUpKey:
        keyboardInput.moveUp = 0.1
        break
      case keyboardControls.moveDownKey:
        keyboardInput.moveDown = 0.1
        break
      default:
        break
    }
  }

  const handleKeyUp = (e) => {
    movementSpeed = e.shiftKey ? 10 : 1
    switch (e.key.toLowerCase()) {
      case keyboardControls.moveForwardKey:
        keyboardInput.moveForward = 0
        break
      case keyboardControls.moveBackwardKey:
        keyboardInput.moveBackward = 0
        break
      case keyboardControls.moveLeftKey:
        keyboardInput.moveLeft = 0
        break
      case keyboardControls.moveRightKey:
        keyboardInput.moveRight = 0
        break
      case keyboardControls.moveUpKey:
        keyboardInput.moveUp = 0
        break
      case keyboardControls.moveDownKey:
        keyboardInput.moveDown = 0
        break
      default:
        break
    }
  }

  const handleMouseMovement = (e) => {
    mouseInput.movementX += mouseInput.mouseDown * e.movementX
    mouseInput.movementY += mouseInput.mouseDown * e.movementY
  }

  const handleMouseDown = (e) => {
    if (e.button === mouseControls.button) {
      mouseInput.mouseDown = 1
      if (document.pointerLockElement !== document.body) {
        document.body.requestPointerLock()
      }
    }
  }

  const handleMouseUp = (e) => {
    if (e.button === mouseControls.button) {
      document.exitPointerLock()
      mouseInput.mouseDown = 0
      mouseInput.movementX = 0
      mouseInput.movementY = 0
    }
  }

  const handleUnfocus = () => {
    keyboardInput.moveForward = 0
    keyboardInput.moveBackward = 0
    keyboardInput.moveLeft = 0
    keyboardInput.moveRight = 0
    keyboardInput.moveUp = 0
    keyboardInput.moveDown = 0
    mouseInput.mouseDown = 0
    mouseInput.movementX = 0
    mouseInput.movementY = 0
    movementSpeed = 1
  }

  window.addEventListener('keydown', handleKeyDown)
  window.addEventListener('keyup', handleKeyUp)
  window.addEventListener('mousemove', handleMouseMovement)
  window.addEventListener('mousedown', handleMouseDown)
  window.addEventListener('mouseup', handleMouseUp)
  window.addEventListener('blur', handleUnfocus)
  document.addEventListener('contextmenu', event => event.preventDefault())

  const threeJsToDeviceOrientation = (rotation) => {
    // Note(Riyaan): Testing did not check for correct yaw or roll
    const conversionMatrix = new THREE.Matrix4(1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1)
    const threeMatrix = new THREE.Matrix4().makeRotationFromEuler(rotation)
    const finalEuler = new THREE.Euler()
    finalEuler.setFromRotationMatrix(conversionMatrix.multiply(threeMatrix), 'ZXY')
    return {
      alpha: THREE.MathUtils.radToDeg(finalEuler.z),
      beta: THREE.MathUtils.radToDeg(finalEuler.x),
      gamma: THREE.MathUtils.radToDeg(finalEuler.y),
    }
  }
  const threeJsToDeviceTranslation = (position) => {
    const conversionMatrix = new THREE.Matrix4(1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1)
    const threeMatrix = new THREE.Matrix4().makeTranslation(position.x, position.y, position.z)
    const finalPosition = new THREE.Vector3()
    finalPosition.setFromMatrixPosition(conversionMatrix.multiply(threeMatrix))
    return finalPosition
  }

  const emitFakeMotionData = (detail: MotionDetails) => {
    const deviceMotionEvent = new CustomEvent('devicemotion8w', {detail})

    window.dispatchEvent(deviceMotionEvent)
    if (camera_) {
      emitOrientationData(threeJsToDeviceOrientation(camera_.rotation), currentTimeInNanos_)
    } else {
      emitFakeOrientationData(currentTimeInNanos_)
    }
  }

  const emitExtrinsicData = (camera) => {
    const position = {
      x: camera.position.x,
      y: camera.position.y,
      z: camera.position.z,
    }
    const rotation = {
      w: camera.quaternion.w,
      x: camera.quaternion.x,
      y: camera.quaternion.y,
      z: camera.quaternion.z,
    }
    const detail = {
      position,
      rotation,
    }
    window.dispatchEvent(new CustomEvent('extrinsic8w', {detail}))
  }

  const calculateMotionData = () => {
    const timeIntervalMicros = (currentTimeInNanos_ - prevTimeNanos_) * NANOS_TO_MICROS
    const timeIntervalSeconds = timeIntervalMicros * MICRO_TO_SECONDS

    // Calculate current velocity in meters per second
    currVelocity_.subVectors(camera_.position, prevPosition_)
      .multiplyScalar(1 / timeIntervalSeconds)

    // Calculate current acceleration in meters per second squared
    const currAcc = new THREE.Vector3()
      .subVectors(currVelocity_, prevVelocity_)
      .multiplyScalar(1 / timeIntervalSeconds)

    // Calculate the acceleration in device coordinates in meters per second squared
    // https://developer.mozilla.org/en-US/docs/Web/API/DeviceMotionEvent/acceleration
    const calculatedAcc = threeJsToDeviceTranslation(currAcc)

    // Calculate current and previous orientation in device coordinates
    const currentDevice = threeJsToDeviceOrientation(camEuler_)
    const prevDevice = threeJsToDeviceOrientation(prevEuler_)

    // Calculate the rotation rate in device coordinates in degrees per second
    // https://developer.mozilla.org/en-US/docs/Web/API/DeviceMotionEvent/rotationRate
    // Compute the difference in angle accounting for the wrap around at 180 degrees
    const normalizeDegrees = angle => THREE.MathUtils.euclideanModulo(angle + 180, 360) - 180
    const rotationRateInDevice = {
      alpha: normalizeDegrees(currentDevice.alpha - prevDevice.alpha) / timeIntervalSeconds,
      beta: normalizeDegrees(currentDevice.beta - prevDevice.beta) / timeIntervalSeconds,
      gamma: normalizeDegrees(currentDevice.gamma - prevDevice.gamma) / timeIntervalSeconds,
    }

    prevTimeNanos_ = currentTimeInNanos_
    prevVelocity_.copy(currVelocity_)
    prevPosition_.copy(camera_.position)
    prevEuler_.copy(camEuler_)

    const linAccelerationDetail: MotionDetails = {
      acceleration: {x: calculatedAcc.x, y: calculatedAcc.y, z: calculatedAcc.z},
      timeStamp: currentTimeInNanos_ / MILLIS_TO_NANOS,
      interval: timeIntervalMicros,
    }
    emitFakeMotionData(linAccelerationDetail)

    const accelerationIncludingGravityDetail: MotionDetails = {
      accelerationIncludingGravity: {
        x: calculatedAcc.x,
        y: calculatedAcc.y - GRAVITY,
        z: calculatedAcc.z,
      },
      timeStamp: currentTimeInNanos_ / MILLIS_TO_NANOS,
      interval: timeIntervalMicros,
    }
    emitFakeMotionData(accelerationIncludingGravityDetail)

    const rotationRateDetail: MotionDetails = {
      rotationRate: rotationRateInDevice,
      timeStamp: currentTimeInNanos_ / MILLIS_TO_NANOS,
      interval: timeIntervalMicros,
    }
    emitFakeMotionData(rotationRateDetail)
  }

  const frameStartResult = ({repeatFrame, drawCtx, computeCtx, drawTexture, computeTexture}) => {
    if (repeatFrame) {
      return {
        width: 1,
        height: 1,
        videoTime_: currentTimeInNanos_,
      }
    }

    const timeDiffNanos = Date.now() * MILLIS_TO_NANOS - loopRefTimeInNanos_
    currentTimeInNanos_ = timeDiffNanos + loopStartInNanos_

    // Note(Dale): videoEl.currentTime rounds our current time so we add an offset
    if (
      currentTimeInNanos_ >= loopEndInNanos_ ||
      currentTimeInNanos_ + START_POINT_OFFSET < loopStartInNanos_) {
      if (!scrubbing_) {
        seekToLoopEndpoints()
      }
    }

    calculateMotionData()
    if (camera_) {
      emitOrientationData(threeJsToDeviceOrientation(camera_.rotation), currentTimeInNanos_)
    } else {
      emitFakeOrientationData(currentTimeInNanos_)
    }

    // Emit fake extrinsic
    emitExtrinsicData(camera_)

    renderer_.render(scene_, camera_)

    // The video can be paused or have 0 dimensions if the user does something like navigate away
    // from the page and then come back on Safari, which would resume the page instead of
    // reloading. In that case, just return the previous frame by keeping repeatFrame true.
    computeCtx.bindTexture(computeCtx.TEXTURE_2D, computeTexture)

    computeCtx.texImage2D(
      computeCtx.TEXTURE_2D,
      0,
      computeCtx.RGBA,
      computeCtx.RGBA,
      computeCtx.UNSIGNED_BYTE,
      renderer_.domElement
    )

    // also load the camera feed into the draw context.  The computeTexture has a name, so we get
    // the corresponding drawCtx buffer from the drawTexMap cache.  Then we re-use the drawCtx
    // buffer and populate it with the camera feed frame.
    // Cache current bindings.
    const restoreTex = drawCtx.getParameter(drawCtx.TEXTURE_BINDING_2D)
    const restoreUnpackFlipY = drawCtx.getParameter(drawCtx.UNPACK_FLIP_Y_WEBGL)

    // Bind texture and configure pixelStorei.
    drawCtx.bindTexture(drawCtx.TEXTURE_2D, drawTexture)
    // We only need to change UNPACK_FLIP_Y_WEBGL if its value was 'true' before.
    if (restoreUnpackFlipY) {
      drawCtx.pixelStorei(drawCtx.UNPACK_FLIP_Y_WEBGL, false)
    }

    // Read the texture from the video.
    drawCtx.texImage2D(
      drawCtx.TEXTURE_2D,
      0,
      drawCtx.RGBA,
      drawCtx.RGBA,
      drawCtx.UNSIGNED_BYTE,
      renderer_.domElement
    )

    if (config_ && config_.verbose) {
      checkGLError({GLctx: drawCtx, msg: 'CameraLoop.readCameraToTexture'})
    }

    // Restore bindings.
    drawCtx.bindTexture(drawCtx.TEXTURE_2D, restoreTex)

    // We only need to restore UNPACK_FLIP_Y_WEBGL if its value was 'true' before.
    if (restoreUnpackFlipY) {
      drawCtx.pixelStorei(drawCtx.UNPACK_FLIP_Y_WEBGL, restoreUnpackFlipY)
    }

    const result = {
      textureWidth: RENDER_WIDTH,
      textureHeight: RENDER_HEIGHT,
    }

    return result
  }

  const resumeIfAutoPaused = () => {}

  const getSessionAttributes = () => ({
    cameraLinkedToViewer: false,
    controlsCamera: false,
    fillsCameraTexture: true,
    providesRunLoop: false,
    supportsHtmlEmbedded: false,
    supportsHtmlOverlay: true,
    usesMediaDevices: true,
    usesWebXr: false,
    isSimulator: true,
  })

  return {
    sessionManager: {
      initialize,
      obtainPermissions,
      stop,
      start: startSession,
      pause,
      resume,
      resumeIfAutoPaused,
      hasNewFrame,
      frameStartResult,
      getSessionAttributes,
      updateConfig,
      clear,
    },
    locationEventControls: {
      dispatchLocationScanning,
      dispatchLocationFound,
      dispatchLocationLost,
      dispatchMeshFound,
      dispatchMeshLost,
    },
  }
}

export {
  SessionManagerSimulatorRenderer,
}

export type {
  LocationEventControls,
}
