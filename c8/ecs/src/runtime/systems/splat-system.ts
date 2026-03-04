import {loadScript} from '../../shared/load-script'
import {getResourceBase} from '../../shared/resources'
import type {Eid} from '../../shared/schema'
import {assets} from '../assets'
import {Splat} from '../components'
import {events} from '../event-ids'
import {addChild, notifyChanged} from '../matrix-refresh'
import type {Object3D} from '../three-types'
import type {World} from '../world'
import {makeSystemHelper} from './system-helper'

const removeRaycast = (object: Object3D) => {
  object.raycast = () => {}
  object.traverse((child) => {
    child.raycast = () => {}
  })
}

const makeSplatSystem = (world: World) => {
  const {enter, changed, exit} = makeSystemHelper(Splat)
  const managers: Map<Eid, any> = new Map()
  let Model: any

  const getModelPromise = (() => {
    let promise: Promise<void> | null = null
    return () => {
      if (!promise) {
        promise = new Promise<void>((resolve, reject) => {
          loadScript(`${getResourceBase()}splat/splat-loader.js`).then(() => {
            Model = (window as any).Model
            Model.setInternalConfig({workerUrl: `${getResourceBase()}splat/splat-worker.js`})
            resolve()
          }).catch(reject)
        })
      }
      return promise
    }
  })()

  const applySplat = async (eid: Eid) => {
    const object = world.three.entityToObject.get(eid)

    if (!object) {
      return
    }

    const {url, skybox} = Splat.get(world, eid)

    if (!url) {
      return
    }

    assets.load({url}).then(async (asset) => {
      await getModelPromise()
      const manager = Model.ThreejsModelManager.create(
        {camera: world.three.activeCamera, renderer: world.three.renderer}
      )
      manager.configure({pointFrustumLimit: 0, bakeSkyboxMeters: 190})
      const buffer = new Uint8Array(await asset.data.arrayBuffer())
      manager.setModelBytes(asset.remoteUrl, buffer)
      manager.setOnLoaded(() => {
        manager.toggleSplatSkybox(!!skybox)
        const model = manager.model()
        model.userData.isSplat = true
        removeRaycast(model)
        addChild(object, model)
        world.events.dispatch(eid, events.SPLAT_MODEL_LOADED, {model})
        managers.set(eid, manager)
      })
    })
  }

  const updateSplat = () => {
    managers.forEach((manager, eid) => {
      const object = world.three.entityToObject.get(eid)
      if (!object) {
        return
      }

      manager.tick()
      const canvas = world.three.renderer.domElement
      if (canvas && object.userData?.lastWidth !== canvas.width) {
        manager.setRenderWidthPixels(canvas.width)
        object.userData.lastWidth = canvas.width
      }
      notifyChanged(object)
    })
  }

  const removeSplat = (eid: Eid) => {
    const manager = managers.get(eid)
    if (manager) {
      manager.dispose()
      managers.delete(eid)
    }

    const object = world.three.entityToObject.get(eid)
    if (!object) {
      return
    }
    delete object.userData.lastWidth
    object.children.forEach((child) => {
      if (child.userData?.isSplat) {
        object.remove(child)
      }
    })
  }

  return () => {
    exit(world).forEach(removeSplat)
    enter(world).forEach(applySplat)
    changed(world).forEach(applySplat)
    updateSplat()
  }
}

export {
  makeSplatSystem,
}
