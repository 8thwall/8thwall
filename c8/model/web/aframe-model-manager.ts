import {ThreejsModelManager} from './threejs-model-manager'

declare let THREE: any

let registered_ = false

const splatComponent = {
  schema: {
    src: {type: 'string'},
  },
  loadSrc(src) {
    // Flush any data from the manager before loading the new source. This will cause the old model
    // to disappear while we load the new one. I anticipate that when we load a new source, we might
    // also want to set the camera position to match the new source, and if the old model were still
    // around, it would jump. If this behavior ends up feeling disruptive, we might need an event to
    // fire after the new model is loaded, so that the camera can be updated by an external
    // component after load. In this case, we should at least keep this behavior of unloading the
    // model when src is empty.
    if (this.manager_) {
      this.manager_.dispose()
    }

    if (!src) {
      return
    }

    // Fetch the new model, and then set it on the manager. If the manager is not yet initialized,
    // store the data for later.
    const filename = src.split('/').pop()
    fetch(src, {mode: 'cors'})
      .then(response => Promise.all([response.arrayBuffer(), response.status]))
      .then(([buffer, status]) => {
        if (status !== 200) {
          console.error(`Failed to load ${src}: ${status}`)  // eslint-disable-line no-console
          return
        }
        const bytes = new Uint8Array(buffer)
        if (this.manager_) {
          this.manager_.setModelBytes(filename, bytes)
          return
        }
        if (!this.manager_) {
          this.loadedData_ = {filename, bytes}
        }
      })
  },
  mergeSrc(src, position, rotation) {
    if (!src) {
      return
    }

    // Fetch the new model, and then set it on the manager. If the manager is not yet initialized,
    // store the data for later.
    const filename = src.split('/').pop()
    fetch(src, {mode: 'cors'})
      .then(response => Promise.all([response.arrayBuffer(), response.status]))
      .then(([buffer, status]) => {
        if (status !== 200) {
          console.error(`Failed to load ${src}: ${status}`)  // eslint-disable-line no-console
          return
        }
        const bytes = new Uint8Array(buffer)
        if (this.manager_) {
          this.manager_.mergeModelBytes(filename, bytes, position, rotation)
          return
        }
        if (!this.manager_) {
          this.mergeData_ = {filename, bytes, position, rotation}
        }
      })
  },
  initManager() {
    let camera = null
    this.el.sceneEl.object3D.traverse((child) => {
      if (child instanceof THREE.PerspectiveCamera) {
        camera = child
      }
    })
    this.manager_ = ThreejsModelManager.create(
      {camera, renderer: this.el.sceneEl.renderer, config: {}}
    )

    // If the model data has already been loaded, set it on the manager.
    if (this.loadedData_) {
      const {filename, bytes} = this.loadedData_
      this.manager_.setModelBytes(filename, bytes)
      this.loadedData_ = null
    }

    if (this.mergeData_) {
      const {filename, bytes, position, rotation} = this.mergeData_
      this.manager_.mergeModelBytes(filename, bytes, position, rotation)
      this.mergeData_ = null
    }

    // Add the manager's model to the scene.
    this.el.object3D.add(this.manager_.model())

    // Set the manager's render width to match the scene's canvas width.
    this.canvas_ = this.el.sceneEl.canvas
    this.lastWidth_ = 0  // This will get updated on the next tick.
  },
  update(oldData) {
    // Fetch the new source if it has changed.
    if ((oldData as any)?.src !== this.data.src) {
      this.loadSrc(this.data.src)
    }
  },
  play() {
    // Initialize the manager with the scene's camera if it is not yet initialized. This is robust
    // to scene entity object declaration / initialization order.
    if (this.manager_) {
      return
    }

    this.initManager()
  },
  tick() {
    // Check if renderWidth needs to be updated.
    if (this.lastWidth_ !== this.canvas_.width) {
      this.lastWidth_ = this.canvas_.width
      this.manager_.setRenderWidthPixels(this.lastWidth_)
    }

    // Tick the manager.
    this.manager_.tick()
  },
  remove() {
    // Dispose the manager when the component is removed.
    if (this.manager_) {
      this.manager_.dispose()
      this.manager_ = null
    }
  },
}

const splatPrimitive = {
  defaultComponents: {
    'splat-model': {},
  },
  mappings: {
    src: 'splat-model.src',
  },
}

const registerAFrameComponents = () => {
  const {AFRAME} = window as any
  if (!AFRAME || registered_) {
    return
  }

  AFRAME.registerComponent('splat-model', splatComponent)
  AFRAME.registerPrimitive('splat-model', splatPrimitive)

  registered_ = true
}

const AFrameModelManager = {
  registerAFrameComponents,
}

export {
  AFrameModelManager,
}
