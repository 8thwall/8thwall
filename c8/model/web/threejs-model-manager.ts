// We are copying this from c8/pixels/render/shaders
import {
  splatInstanceAttributesVert, splatTextureVert, splatMultiTextureVert,
  splatFrag,
} from 'c8/model/web/splat-shaders'

import {ModelManager} from './model-manager'
import type {ModelManagerConfiguration, ModelSrc} from './model-manager-types'
import {addSplatCubeMap} from './bake-cubemap'

let THREE

interface LoadedData {
  numPoints?: number
}

type BoundingBox = {
  width: number
  height: number
  depth: number
  centerX: number
  centerY: number
  centerZ: number
}

const uint8Array = (
  data: Uint32Array, byteData: Uint8Array, stride, dataOffset: { ptr: number }
): Uint8Array => {
  const numValues = data[dataOffset.ptr++] * stride
  if (numValues % 4 !== 0) {
    throw new Error(`Expected uint8 array count ${numValues} to be multiple of 4`)
  }
  const byteOffset = dataOffset.ptr * 4  // ptr units are 4 byte words.
  const values = byteData.subarray(byteOffset, byteOffset + numValues)
  if (values.length !== numValues) {
    throw new Error('Insufficient data reading Uint8Array')
  }
  dataOffset.ptr += numValues / 4  // ptr units are 4 byte words.
  return values
}

const uint32Array = (data: Uint32Array, stride, dataOffset: { ptr: number }): Uint32Array => {
  const numValues = data[dataOffset.ptr++] * stride
  const values = data.subarray(dataOffset.ptr, dataOffset.ptr + numValues)
  if (values.length !== numValues) {
    throw new Error('Insufficient data reading Uint32Array')
  }
  dataOffset.ptr += numValues
  return values
}

const float32Array = (
  data: Uint32Array, floatData: Float32Array, stride, dataOffset: { ptr: number }
): Float32Array => {
  const numValues = data[dataOffset.ptr++] * stride
  const values = floatData.subarray(dataOffset.ptr, dataOffset.ptr + numValues)
  if (values.length !== numValues) {
    throw new Error('Insufficient data reading Float32Array')
  }
  dataOffset.ptr += numValues
  return values
}

const uint8Pixels = (data: Uint32Array, byteData: Uint8Array, dataOffset: { ptr: number }) => {
  const height = data[dataOffset.ptr++]
  const width = data[dataOffset.ptr++]
  const rowElements = data[dataOffset.ptr++]

  if (rowElements % 4 !== 0) {
    throw new Error('Expected multiple of 4 row elements for byte image')
  }

  const byteOffset = dataOffset.ptr * 4  // ptr units are 4 byte words.
  const pixels = byteData.subarray(byteOffset, byteOffset + height * rowElements)

  if (pixels.length !== height * rowElements) {
    throw new Error('Not enough pixel data')
  }

  dataOffset.ptr += (height * rowElements) / 4  // ptr units are 4 byte words.
  return {width, height, rowElements, data: pixels}
}

const getInstancePositions = (interleavedFloatAttributeData, stride): Number[] => {
  const instancePositions: Number[] = []
  for (let i = 0; i < interleavedFloatAttributeData.length; i += stride) {
    const x = interleavedFloatAttributeData[i]
    const y = interleavedFloatAttributeData[i + 1]
    const z = interleavedFloatAttributeData[i + 2]
    if (Number.isNaN(x) || Number.isNaN(y) || Number.isNaN(z)) {
      throw new Error('Invalid positional data got NaN')
    }
    instancePositions.push(x, y, z)
  }

  return instancePositions
}

const intDataTexture = ({cols, rows, rowElements, data}) => {
  const channels = rowElements / cols
  const tex = new THREE.DataTexture(data, cols, rows)
  tex.format = channels === 4 ? THREE.RGBAIntegerFormat : THREE.RedIntegerFormat
  tex.internalFormat = channels === 4 ? 'RGBA32UI' : 'R32UI'
  tex.type = THREE.UnsignedIntType
  tex.needsUpdate = true
  return tex
}

const splatHeader = (splatData: Uint32Array, splatFloatData: Float32Array,
  dataOffset: { ptr: number }) => ({
  numPoints: splatData[dataOffset.ptr++],
  maxNumPoints: splatData[dataOffset.ptr++],
  shDegree: splatData[dataOffset.ptr++],
  antialiased: !!splatData[dataOffset.ptr++],
  hasSkybox: !!splatData[dataOffset.ptr++],
  width: splatFloatData[dataOffset.ptr++],
  height: splatFloatData[dataOffset.ptr++],
  depth: splatFloatData[dataOffset.ptr++],
  centerX: splatFloatData[dataOffset.ptr++],
  centerY: splatFloatData[dataOffset.ptr++],
  centerZ: splatFloatData[dataOffset.ptr++],
})

const intPixels = (splatData: Uint32Array, dataOffset: { ptr: number }) => {
  const rows = splatData[dataOffset.ptr++]
  const cols = splatData[dataOffset.ptr++]
  const rowElements = splatData[dataOffset.ptr++] / 4  // numBytes / 4
  const data = splatData.subarray(
    dataOffset.ptr, dataOffset.ptr + rows * rowElements
  )

  if (data.length !== rows * rowElements) {
    throw new Error('Not enough pixel data')
  }

  dataOffset.ptr += rows * rowElements
  return {cols, rows, rowElements, data}
}

const splatSkybox = (splatData: Uint32Array, splatByteData: Uint8Array,
  dataOffset: { ptr: number }) => {
  const skybox: { resolution: number, faces: any } = {
    resolution: 0,
    faces: [],
  }
  const skyboxByteCount = splatData[dataOffset.ptr++]
  if (skyboxByteCount > 0) {
    skybox.resolution = Math.floor(Math.sqrt(skyboxByteCount / 24))
    const intsPerSkybox = skybox.resolution * skybox.resolution
    const bytesPerSkybox = intsPerSkybox * 4
    if (bytesPerSkybox * 6 !== skyboxByteCount) {
      throw new Error(`Invalid skybox byte count ${skyboxByteCount}`)
    }
    let byteDataOffset = dataOffset.ptr * 4  // Integer index to byte index (4 bytes per int).
    for (let i = 0; i < 6; ++i) {
      const faceData = splatByteData.subarray(byteDataOffset, byteDataOffset + bytesPerSkybox)
      byteDataOffset += bytesPerSkybox
      dataOffset.ptr += intsPerSkybox

      if (faceData.length !== bytesPerSkybox) {
        throw new Error(`Not enough skybox face data for face ${i}`)
      }

      skybox.faces.push({
        data: faceData,
        width: skybox.resolution,
        height: skybox.resolution,
      })
    }
  }
  return skybox
}

const deserializeSplatTexture = (data) => {
  const splatData = new Uint32Array(data.buffer, 4)
  const splatFloatData = new Float32Array(data.buffer, 4)
  const splatByteData = new Uint8Array(data.buffer, 4)
  const dataOffset = {ptr: 0}
  const header = splatHeader(splatData, splatFloatData, dataOffset)
  const numIds = splatData[dataOffset.ptr++]
  const sortedIds = splatData.subarray(dataOffset.ptr, dataOffset.ptr + numIds)
  if (sortedIds.length !== numIds) {
    throw new Error('Not enough sorted IDs')
  }
  dataOffset.ptr += numIds
  const texture = intPixels(splatData, dataOffset)

  // TODO: Get stride from deserialization.
  const stride = 16
  const interleavedAttributeData = texture.data.subarray(0, header.numPoints * stride)
  const attrOffset = 1 +  // model type
    11 +                  // splat header
    1 +                   // numIds
    numIds +              // ids
    3                     // texture header
  const floatInterleavedAttributeData = new Float32Array(data.buffer, 4 * attrOffset)
    .subarray(0, header.numPoints * stride)

  let instancePositions_
  const instancePositions = () => {
    if (!instancePositions_) {
      instancePositions_ = getInstancePositions(floatInterleavedAttributeData, stride)
    }
    return instancePositions_
  }

  const skybox = splatSkybox(splatData, splatByteData, dataOffset)

  return {
    kind: 'splatTexture',
    header,
    sortedIds,
    texture,
    textureData: texture.data,
    interleavedAttributeData,
    instancePositions,
    skybox,
  }
}

const deserializeSplatMultiTexture = (data) => {
  const splatData = new Uint32Array(data.buffer, 4)
  const splatFloatData = new Float32Array(data.buffer, 4)
  const splatByteData = new Uint8Array(data.buffer, 4)
  const dataOffset = {ptr: 0}
  return {
    kind: 'splatMultiTexture',
    header: splatHeader(splatData, splatFloatData, dataOffset),
    sortedIdsTexture: intPixels(splatData, dataOffset),
    positionColorTexture: intPixels(splatData, dataOffset),
    rotationScaleTexture: intPixels(splatData, dataOffset),
    shRGTexture: intPixels(splatData, dataOffset),
    shBTexture: intPixels(splatData, dataOffset),
    skybox: splatSkybox(splatData, splatByteData, dataOffset),
  }
}

const deserializeMesh = (data) => {
  const readPtr = {ptr: 1}  // model header offset.
  const uint32Data = new Uint32Array(data.buffer)
  const uint8Data = new Uint8Array(data.buffer)
  const float32Data = new Float32Array(data.buffer)

  return {
    kind: 'mesh',
    geometry: {
      vertices: float32Array(uint32Data, float32Data, 4, readPtr),
      colors: uint8Array(uint32Data, uint8Data, 4, readPtr),
      normals: float32Array(uint32Data, float32Data, 4, readPtr),
      uvs: float32Array(uint32Data, float32Data, 3, readPtr),
      indices: uint32Array(uint32Data, 3, readPtr),
    },
    texture: uint8Pixels(uint32Data, uint8Data, readPtr),
  }
}

const deserializePoints = (data) => {
  const readPtr = {ptr: 1}  // model header offset.
  const uint32Data = new Uint32Array(data.buffer)
  const uint8Data = new Uint8Array(data.buffer)
  const float32Data = new Float32Array(data.buffer)
  return {
    kind: 'points',
    geometry: {
      vertices: float32Array(uint32Data, float32Data, 4, readPtr),
      colors: uint8Array(uint32Data, uint8Data, 4, readPtr),
      normals: float32Array(uint32Data, float32Data, 4, readPtr),
    },
  }
}

const deserialize = (data) => {
  const modelType = new Uint32Array(data.buffer, 0, 1)
  if (modelType[0] === 1) {
    return deserializeMesh(data)
  }
  // Skip modelType[0] == 2: splatAttributes; not implemented yet in three.js (and may not ever)
  if (modelType[0] === 4) {
    return deserializeSplatTexture(data)
  }
  if (modelType[0] === 8) {
    return deserializeSplatMultiTexture(data)
  }
  if (modelType[0] === 16) {
    return deserializePoints(data)
  }
  throw new Error('Unsupported model format')
}

const create = ({camera, renderer, config}) => {
  THREE = (window as any).THREE
  if (!THREE) {
    throw new Error('THREE not found')
  }

  let modelSource_: ModelSrc[] | undefined = []
  let splat_
  let mesh_
  let points_
  let rayToPix_: number
  let splatMaterial_
  let orderedSplatIdsTextureBuffer_
  let geometry_
  let instanceAttributes_
  let boundingBox_: BoundingBox | undefined
  let instanceCount_ = 0
  let splatCubeMap_
  let numSplatsPerInstance_ = 128

  // cb when we have loaded the model
  // eslint-disable-next-line @typescript-eslint/no-unused-vars
  let onLoaded_ = (data?: LoadedData) => { }

  // If true, resort the data in a texture, and upload it as an instance attribute buffer instead of
  // as a texture. This means that we push 16x the amount of data to GPU on a resort, but the data
  // should have better caching properties in the vertex shader. Anecdotally (on a single test scene
  // with non-rigorous measurement) this decreased frame time by about 4ms on Meta Quest 3 and did
  // not impact performance on Apple Vision Pro.
  const sortTexture = false

  const multiTexture = false
  const camera_ = camera
  const renderer_ = renderer

  // In three.js r153, the XR camera API was changed, and it was reverted again in r154. This
  // handles the r153 case. See https://github.com/mrdoob/three.js/pull/26350
  renderer_.xr.setUserCamera?.(camera_)

  const modelRoot_ = new THREE.Group()

  const DEFAULT_CONFIG: Partial<ModelManagerConfiguration> = {
    coordinateSpace: ModelManager.CoordinateSystem.RUB,
    splatResortRadians: 0.18,  // 10 degrees
    splatResortMeters: 0.2,    // 7.5 inches
    bakeSkyboxMeters: 95,      // 95 meters
    sortTexture,
    multiTexture,
    preferTexture: true,
    pointPruneDistance: 0,   // No additional pruning
    pointFrustumLimit: 1.7,  // 120 degree FOV.
    pointSizeLimit: 1e-4,    // 0.3mm @ 1m.
    useOctree: false,
    outputLinearColorSpace: false,
  }

  const currentConfig = {...DEFAULT_CONFIG, ...config}
  const modelManager = ModelManager.create()
  modelManager.configure(currentConfig)

  const disposeSplatCubeMap = () => {
    if (splatCubeMap_) {
      splatCubeMap_.removeFromParent()
      splatCubeMap_.geometry.dispose()
      splatCubeMap_.material.uniforms.envMap.value.dispose()
      splatCubeMap_.material.dispose()
      splatCubeMap_ = null
    }
  }

  const disposeModels = () => {
    if (splat_) {
      splat_.removeFromParent()
      geometry_.dispose()
      splatMaterial_.uniforms.splatDataTexture?.value.dispose()
      splatMaterial_.uniforms.orderedSplatIdsTexture?.value.dispose()
      splatMaterial_.dispose()
      disposeSplatCubeMap()
      geometry_ = null
      orderedSplatIdsTextureBuffer_ = null
      splatMaterial_ = null
      splat_ = null
    }
    if (mesh_) {
      mesh_.removeFromParent()
      mesh_.geometry.dispose()
      mesh_.material.map?.dispose()
      mesh_.material.dispose()
      mesh_ = null
    }
    if (points_) {
      points_.removeFromParent()
      points_.geometry.dispose()
      points_.material.dispose()
      points_ = null
    }
  }

  const dispose = () => {
    disposeModels()
    modelManager.dispose()
  }

  const loadSplat = (splat) => {
    // Make instanced buffer
    geometry_ = new THREE.InstancedBufferGeometry()
    if (sortTexture) {
      numSplatsPerInstance_ = 1
    }
    const meshPositions = new Float32Array(12 * numSplatsPerInstance_)
    const meshIndices = new Uint32Array(6 * numSplatsPerInstance_)
    for (let i = 0; i < numSplatsPerInstance_; ++i) {
      meshPositions.set([
        -1, -1, i,
        1, -1, i,
        1, 1, i,
        -1, 1, i,
      ], i * 12)

      const b = i * 4
      meshIndices.set([
        0 + b, 1 + b, 2 + b, 0 + b, 2 + b, 3 + b,
      ], i * 6)
    }
    geometry_.setAttribute('position', new THREE.BufferAttribute(meshPositions, 3))
    geometry_.setIndex(new THREE.BufferAttribute(meshIndices, 1))

    let uniforms = {}
    let splatVertNoVersion
    let splatFragNoVersion

    // Create the base geometry
    if (sortTexture) {
      // Fake texture, just use a 1x1 texture with zeros, since this isn't needed by the shader.
      const fakeTexture = new THREE.DataTexture(new Uint32Array(4), 1, 1)
      fakeTexture.internalFormat = 'RGBA32UI'
      fakeTexture.format = THREE.RGBAIntegerFormat
      fakeTexture.type = THREE.UnsignedIntType
      fakeTexture.needsUpdate = true

      // Real data for the sortTexture case. For the first initialization, we need to set the
      // buffer to have the maximum number of elements.
      const interleavedAttributeData = new Uint32Array(16 * splat.header.maxNumPoints)
      interleavedAttributeData.set(splat.interleavedAttributeData)
      instanceAttributes_ =
        new THREE.InstancedInterleavedBuffer(interleavedAttributeData, 4, 1)
      instanceAttributes_.stride = 16  // TODO: Get from deserialization.
      geometry_.setAttribute(
        'positionColor', new THREE.InterleavedBufferAttribute(instanceAttributes_, 4, 0)
      )
      geometry_.setAttribute(
        'scaleRotation', new THREE.InterleavedBufferAttribute(instanceAttributes_, 4, 4)
      )
      geometry_.setAttribute('shRG', new
      THREE.InterleavedBufferAttribute(instanceAttributes_,
        4, 8))
      geometry_.setAttribute(
        'shB', new THREE.InterleavedBufferAttribute(instanceAttributes_, 4, 12)
      )
      uniforms = {
        'splatDataTexture': {value: fakeTexture},  // TODO remove need for passing this
        'raysToPix': {value: rayToPix_},
        'maxInstanceIdForFragment': {value: 0},
        'antialiased': {value: Number(splat.header.antialiased)},
      }
      splatVertNoVersion = splatInstanceAttributesVert
      splatFragNoVersion = splatFrag
    } else if (multiTexture) {
      const count = splat.header.maxNumPoints
      const cols = Math.min(Math.ceil(Math.sqrt(count)), 4096)
      const rows = Math.ceil(count / cols)

      orderedSplatIdsTextureBuffer_ = new Uint32Array(rows * cols)
      orderedSplatIdsTextureBuffer_.fill(0xFFFFFFFF)
      orderedSplatIdsTextureBuffer_.set(splat.sortedIdsTexture.data, 0)

      const orderedSplatIdsTexture =
        new THREE.DataTexture(orderedSplatIdsTextureBuffer_, cols, rows)
      orderedSplatIdsTexture.format = THREE.RedIntegerFormat
      orderedSplatIdsTexture.internalFormat = 'R32UI'
      orderedSplatIdsTexture.type = THREE.UnsignedIntType
      orderedSplatIdsTexture.needsUpdate = true

      uniforms = {
        'orderedSplatIdsTexture': {value: orderedSplatIdsTexture},
        'positionColorTexture': {value: intDataTexture(splat.positionColorTexture)},
        'rotationScaleTexture': {value: intDataTexture(splat.rotationScaleTexture)},
        'shRGTexture': {value: intDataTexture(splat.shRGTexture)},
        'shBTexture': {value: intDataTexture(splat.shBTexture)},
        'raysToPix': {value: rayToPix_},
        'maxInstanceIdForFragment': {value: 0},
        'antialiased': {value: Number(splat.header.antialiased)},
        'numSplatsPerInstance': {value: numSplatsPerInstance_},
      }
      splatVertNoVersion = splatMultiTextureVert
      splatFragNoVersion = splatFrag
    } else {
      const splatTexture =
        new THREE.DataTexture(splat.textureData, splat.texture.cols, splat.texture.rows)
      splatTexture.internalFormat = 'RGBA32UI'
      splatTexture.format = THREE.RGBAIntegerFormat
      splatTexture.type = THREE.UnsignedIntType
      splatTexture.needsUpdate = true

      const count = splat.header.maxNumPoints
      const cols = Math.min(Math.ceil(Math.sqrt(count)), 4096)
      const rows = Math.ceil(count / cols)

      orderedSplatIdsTextureBuffer_ = new Uint32Array(rows * cols)
      orderedSplatIdsTextureBuffer_.fill(0xFFFFFFFF)
      orderedSplatIdsTextureBuffer_.set(splat.sortedIds, 0)

      const orderedSplatIdsTexture =
        new THREE.DataTexture(orderedSplatIdsTextureBuffer_, cols, rows)
      orderedSplatIdsTexture.format = THREE.RedIntegerFormat
      orderedSplatIdsTexture.internalFormat = 'R32UI'
      orderedSplatIdsTexture.type = THREE.UnsignedIntType
      orderedSplatIdsTexture.needsUpdate = true

      uniforms = {
        'orderedSplatIdsTexture': {value: orderedSplatIdsTexture},
        'splatDataTexture': {value: splatTexture},
        'raysToPix': {value: rayToPix_},
        'maxInstanceIdForFragment': {value: 0},
        'antialiased': {value: Number(splat.header.antialiased)},
        'numSplatsPerInstance': {value: numSplatsPerInstance_},
      }
      splatVertNoVersion = splatTextureVert
      splatFragNoVersion = splatFrag
    }

    const versionPrefix = '#version 300 es'
    let isGlsl3 = false
    if (THREE.GLSL3 && splatVertNoVersion.startsWith(versionPrefix)) {
      splatVertNoVersion = splatVertNoVersion.substring(versionPrefix.length).trim()
      isGlsl3 = true
    }
    if (THREE.GLSL3 && splatFragNoVersion.startsWith(versionPrefix)) {
      splatFragNoVersion = splatFragNoVersion.substring(versionPrefix.length).trim()
      isGlsl3 = true
    }
    uniforms = {...uniforms, outputLinear: {value: currentConfig.outputLinearColorSpace}}
    splatMaterial_ = new THREE.RawShaderMaterial({
      uniforms,
      side: THREE.BackSide,
      transparent: true,
      depthWrite: false,
      vertexShader: splatVertNoVersion,
      fragmentShader: splatFragNoVersion,
      glslVersion: isGlsl3 ? THREE.GLSL3 : null,
    })

    instanceCount_ = Math.ceil(splat.header.maxNumPoints / numSplatsPerInstance_)
    splat_ = new THREE.InstancedMesh(
      geometry_,
      splatMaterial_,
      instanceCount_
    )
    splat_.frustumCulled = false

    modelRoot_.add(splat_)

    if (splat.skybox.resolution > 0) {
      splatCubeMap_ = addSplatCubeMap(modelRoot_, splat.skybox.faces)
    }

    onLoaded_({numPoints: splat.header.maxNumPoints})
  }

  const loadMesh = (mesh) => {
    const geometry = new THREE.BufferGeometry()
    geometry.setAttribute('position', new THREE.BufferAttribute(mesh.geometry.vertices, 4))
    geometry.setAttribute('normal', new THREE.BufferAttribute(mesh.geometry.normals, 4))
    geometry.setAttribute('uv', new THREE.BufferAttribute(mesh.geometry.uvs, 3))
    geometry.setAttribute('color', new THREE.BufferAttribute(mesh.geometry.colors, 4))
    geometry.setIndex(Array.from(mesh.geometry.indices))

    const texture = new THREE.DataTexture(
      mesh.texture.data, mesh.texture.width, mesh.texture.height
    )
    // TODO: We need to set the encoding to match the renderer. When the renderer output encoding
    // is sRGBEncoding, we need to set:
    // texture.encoding = THREE.sRGBEncoding
    texture.needsUpdate = true
    const material = new THREE.MeshBasicMaterial({
      map: texture,
    })

    mesh_ = new THREE.Mesh(geometry, material)
    modelRoot_.add(mesh_)
  }

  const loadPoints = (pc) => {
    const geometry = new THREE.BufferGeometry()
    geometry.setAttribute('position', new THREE.BufferAttribute(pc.geometry.vertices, 4))
    geometry.setAttribute('color', new THREE.BufferAttribute(pc.geometry.colors, 4, true))

    // TODO: We need to set the encoding to match the renderer.
    const material = new THREE.PointsMaterial({size: 0.005, vertexColors: true, transparent: true})

    points_ = new THREE.Points(geometry, material)
    modelRoot_.add(points_)
  }

  const onloaded = ({data}) => {
    disposeModels()
    const model = deserialize(data)

    if (model.kind === 'mesh') {
      loadMesh(model)
      return
    }
    if (model.kind === 'splatTexture' || model.kind === 'splatMultiTexture') {
      // TODO: (type deserialized data)
      const splat = model as any
      boundingBox_ = {
        width: splat.header.width,
        height: splat.header.height,
        depth: splat.header.depth,
        centerX: splat.header.centerX,
        centerY: splat.header.centerY,
        centerZ: splat.header.centerZ,
      }
      loadSplat(model)
      return
    }

    if (model.kind === 'points') {
      loadPoints(model)
      return
    }

    console.error('Unsupported model type', model.kind)  // eslint-disable-line no-console
  }

  /**
   * This abstracts away the updateRange deprecation in recent threejs versions
   * https://github.com/mrdoob/three.js/pull/27103
   * */
  const updateBufferAttributeRange = (bufferAttribute, offset, count) => {
    if (bufferAttribute.addUpdateRange) {
      bufferAttribute.addUpdateRange(offset, count)
    } else {  // Fallback to old singular updateRange
      bufferAttribute.updateRange.offset = offset
      bufferAttribute.updateRange.count = count
    }
    bufferAttribute.needsUpdate = true
  }

  const updateSplat = (splat) => {
    instanceCount_ = Math.ceil(splat.header.numPoints / numSplatsPerInstance_)
    splat_.count = instanceCount_
    if (sortTexture) {
      instanceAttributes_.array.set(splat.interleavedAttributeData)
      updateBufferAttributeRange(instanceAttributes_, 0, 16 * instanceCount_)
    } else if (multiTexture) {
      orderedSplatIdsTextureBuffer_.set(splat.sortedIdsTexture.data)
      orderedSplatIdsTextureBuffer_.fill(0xFFFFFFFF, splat.sortedIdsTexture.data.length)
      splatMaterial_.uniforms.orderedSplatIdsTexture.value.needsUpdate = true
    } else {
      orderedSplatIdsTextureBuffer_.set(splat.sortedIds)
      orderedSplatIdsTextureBuffer_.fill(0xFFFFFFFF, splat.sortedIds.length)
      splatMaterial_.uniforms.orderedSplatIdsTexture.value.needsUpdate = true
    }
  }

  const updateMesh = (mesh) => {
    // eslint-disable-next-line no-console
    console.error(`No updates expected for model of type '${mesh.kind}.`)
  }

  const updatePoints = (points) => {
    // eslint-disable-next-line no-console
    console.error(`No updates expected for model of type '${points.kind}.`)
  }

  const onupdated = ({data}) => {
    const model = deserialize(data)
    if (model.kind === 'mesh') {
      updateMesh(model)
      return
    }

    if (model.kind === 'splatTexture' || model.kind === 'splatMultiTexture') {
      updateSplat(model)
      return
    }

    if (model.kind === 'points') {
      updatePoints(model)
      return
    }

    // eslint-disable-next-line no-console
    console.error(`No updates expected for model of type '${model.kind}.`)
  }

  modelManager.onloaded(onloaded)
  modelManager.onupdated(onupdated)

  const setModelBytes = (filename, bytes) => {
    modelSource_ = undefined
    modelManager.loadBytes(filename, bytes)
  }

  const mergeModelBytes = (filename, bytes, position, rotation) => {
    modelSource_ = undefined
    modelManager.mergeBytes(
      filename,
      bytes,
      {x: position.x, y: position.y, z: position.z},
      {w: rotation.w, x: rotation.x, y: rotation.y, z: rotation.z}
    )
  }

  const setModelFiles = (files: FileList) => {
    modelSource_ = undefined
    modelManager.loadFiles(files)
  }

  const setModelSrcs = (srcs: ModelSrc[]) => {
    modelSource_ = Array.from(srcs)
    modelManager.loadModel(modelSource_)
  }

  const currentCamera = () => (
    // Get the camera to use for the model manager. If we're in XR, use the XR camera, otherwise use
    // the normal camera. In three.js r153, the XR camera API was changed to have `getCamera()`
    // return undefined, and the camera was set with `setUserCamera()`. This was reverted in r154.
    (renderer_.xr.isPresenting && renderer_.xr.getCamera()) || camera_
  )

  const setRenderWidthPixels = (width) => {
    const invCam = currentCamera().projectionMatrixInverse
    const left = new THREE.Vector3(-1, 0, 0).applyMatrix4(invCam)
    const right = new THREE.Vector3(1, 0, 0).applyMatrix4(invCam)
    const raysLeft = left.x / -left.z
    const raysRight = right.x / -right.z
    rayToPix_ = width / (raysRight - raysLeft)
    if (splatMaterial_) {
      // Note that uniforms are refreshed on every frame, so updating the value of the uniform will
      // immediately update the value available to the GLSL code.
      splatMaterial_.uniforms.raysToPix.value = rayToPix_
    }
  }

  const tick = () => {
    // update the render state
    const camMat = currentCamera().matrixWorld
    const camPos = new THREE.Vector3()
    const camRot = new THREE.Quaternion()
    camMat.decompose(camPos, camRot, new THREE.Vector3())

    const modelMat = modelRoot_.matrixWorld
    const modelPos = new THREE.Vector3()
    const modelRot = new THREE.Quaternion()
    modelMat.decompose(modelPos, modelRot, new THREE.Vector3())

    // Pass threejs camera location to model manager
    modelManager.updateView({
      cameraPos: {
        x: camPos.x,
        y: camPos.y,
        z: camPos.z,
      },
      cameraRot: {
        x: camRot.x,
        y: camRot.y,
        z: camRot.z,
        w: camRot.w,
      },
      modelPos: {
        x: modelPos.x,
        y: modelPos.y,
        z: modelPos.z,
      },
      modelRot: {
        x: modelRot.x,
        y: modelRot.y,
        z: modelRot.z,
        w: modelRot.w,
      },
    })
    if (splat_) {
      // We need to update this on every frame or three.js will revert back to the max instance
      // count.
      splat_.count = instanceCount_
    }
  }

  // Callbacks when user change configs
  const numCounts = (ev) => {
    if (splat_) {
      splat_.count = ev.value
    }
  }
  const maxInstanceIdForFragment = (ev) => {
    if (splatMaterial_) {
      splatMaterial_.uniforms.maxInstanceIdForFragment = {value: ev.value}
      splatMaterial_.uniformsNeedUpdate = true
    }
  }

  const onChange = {
    numCounts,
    maxInstanceIdForFragment,
  }

  const setOnLoaded = (cb) => {
    onLoaded_ = cb
  }

  const configure = (newConfig: Partial<ModelManagerConfiguration>) => {
    // Override default config with user values
    Object.assign(currentConfig, newConfig)
    modelManager.configure(currentConfig)
  }

  const toggleSplatSkybox = (visible) => {
    if (splatCubeMap_) {
      splatCubeMap_.visible = visible
    }
  }

  return {
    configure,
    dispose,
    model: () => modelRoot_,
    boundingBox: () => boundingBox_,
    mergeModelBytes,
    setModelBytes,
    setModelFiles,
    setModelSrcs,
    setRenderWidthPixels,
    setOnLoaded,
    tick,
    toggleSplatSkybox,
    onChange,
  }
}

const ThreejsModelManager = {
  create,
}

export {
  ThreejsModelManager,
}
