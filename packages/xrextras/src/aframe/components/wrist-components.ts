import type {ComponentDefinition} from 'aframe'

declare const THREE: any

const buildWristGeometry = (wristGeometry, wristMaterial) => {
  const wrist = new THREE.Object3D()
  wrist.visible = false

  const wristGeom = new THREE.BufferGeometry()
  // Fill geometry with the wrist mesh vertices.
  const wristVerts = new Float32Array(wristGeometry.vertices.length * 3)
  for (let i = 0; i < wristGeometry.vertices.length; ++i) {
    wristVerts[i * 3] = wristGeometry.vertices[i].x
    wristVerts[i * 3 + 1] = wristGeometry.vertices[i].y
    wristVerts[i * 3 + 2] = wristGeometry.vertices[i].z
  }
  wristGeom.setAttribute('position', new THREE.BufferAttribute(wristVerts, 3))

  // Fill the geometry with wrist mesh indices.
  const wristIndices = new Array(wristGeometry.indices.length * 3)
  for (let i = 0; i < wristGeometry.indices.length; ++i) {
    wristIndices[i * 3] = wristGeometry.indices[i].a
    wristIndices[i * 3 + 1] = wristGeometry.indices[i].b
    wristIndices[i * 3 + 2] = wristGeometry.indices[i].c
  }
  wristGeom.setIndex(wristIndices)

  // Fill the geometry with wrist mesh normals.
  const wristNorms = new Float32Array(3 * wristGeometry.normals.length)
  for (let i = 0; i < wristGeometry.normals.length; i++) {
    wristNorms[i * 3] = wristGeometry.normals[i].x
    wristNorms[i * 3 + 1] = wristGeometry.normals[i].y
    wristNorms[i * 3 + 2] = wristGeometry.normals[i].z
  }
  wristGeom.setAttribute('normal', new THREE.BufferAttribute(wristNorms, 3))

  const wristObject = new THREE.Mesh(wristGeom, wristMaterial)
  wrist.add(wristObject)
  return wrist
}

const wristObject = (modelGeometry, material, isWristEnabled) => {
  let handKind = 2
  let wrist

  const leftWrist = buildWristGeometry(
    modelGeometry.leftWristGeometry,
    material
  )
  const rightWrist = buildWristGeometry(
    modelGeometry.rightWristGeometry,
    material
  )
  wrist = rightWrist

  const show = ({detail}) => {
    // Set left or right wrist depending on handKind.
    if (handKind !== detail.handKind) {
      handKind = detail.handKind
      if (handKind === 1) {
        wrist = leftWrist
        rightWrist.visible = false
      } else {
        wrist = rightWrist
        leftWrist.visible = false
      }
    }

    // Update wrist position.
    const {wristTransform} = detail
    wrist.position.copy(wristTransform.position)
    wrist.setRotationFromQuaternion(wristTransform.rotation)
    wrist.scale.set(wristTransform.scale, wristTransform.scale, wristTransform.scale)

    // Make it so frustum doesn't cull mesh when hand is close to camera.
    wrist.frustumCulled = false
    if (isWristEnabled) {
      wrist.visible = true
    }
  }

  const hide = () => {
    wrist.visible = false
  }

  return {
    leftWrist,
    rightWrist,
    show,
    hide,
  }
}

const wristMeshComponent: ComponentDefinition = {
  schema: {
    'material-resource': {type: 'string'},
    'wireframe': {type: 'boolean', default: false},
    'occluder': {type: 'boolean', default: false},
  },
  init() {
    this.isWristEnabled = false

    const beforeRun = ({detail}) => {
      let material
      // Check for wrist enabled to render the wrist.
      this.isWristEnabled = this.el.parentEl.sceneEl.components.xrhand?.data.enableWrists

      if (this.el.getAttribute('material')) {
        material = this.el.components.material.material
      } else if (this.data['material-resource']) {
        material = this.el.sceneEl.querySelector(this.data['material-resource']).material
      } else {
        material = new THREE.MeshBasicMaterial(
          {color: '#7611B6', opacity: 0.5, transparent: true}
        )
      }

      const occluderMaterial = new THREE.MeshStandardMaterial(
        {color: '#F5F5F5', transparent: false, colorWrite: false}
      )

      material.wireframe = this.data.wireframe
      // Use occluder material if specified to do so, this will override the wireframe.
      if (this.data.occluder) {
        material = occluderMaterial
      }

      this.wristMesh = wristObject(
        detail, material,
        this.isWristEnabled
      )

      this.el.setObject3D('leftWrist', this.wristMesh.leftWrist)
      this.el.setObject3D('rightWrist', this.wristMesh.rightWrist)

      this.el.emit('model-loaded')
    }

    const show = (event) => {
      this.wristMesh.show(event)

      this.el.object3D.visible = true
    }

    const hide = () => {
      this.wristMesh.hide()
      this.el.object3D.visible = false
    }

    this.el.sceneEl.addEventListener('xrhandloading', beforeRun)
    this.el.sceneEl.addEventListener('xrhandfound', show)
    this.el.sceneEl.addEventListener('xrhandupdated', show)
    this.el.sceneEl.addEventListener('xrhandlost', hide)
  },
  update() {
    if (!this.wristMesh) {
      return
    }

    let material
    if (this.el.getAttribute('material')) {
      material = this.el.components.material.material
    } else if (this.data['material-resource']) {
      material = this.el.sceneEl.querySelector(this.data['material-resource']).material
    } else {
      material = new THREE.MeshBasicMaterial({color: '#7611B6', opacity: 0.5, transparent: true})
    }
    this.wristMesh.mesh.material = material
  },
}

export {
  wristMeshComponent,
}
