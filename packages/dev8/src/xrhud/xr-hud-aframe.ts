import './xr-hud-aframe.css'
import {oom, dataSize} from './xr-hud-core'
import type {XrHudManager} from './xr-hud-manager'

const XrHudComponent = (xrHud: ReturnType<typeof XrHudManager>) => ({
  init() {
    xrHud.setVersions({'AFRAME': window.AFRAME.version})

    const floorObj = document.createElement('a-plane')
    floorObj.id = '8floor'
    floorObj.setAttribute('rotation', '-90 0 0')
    floorObj.setAttribute(
      'material', 'shader: flat; color: #AD50FF; transparent: true; opacity: 0.5'
    )
    floorObj.setAttribute('width', '100')
    floorObj.setAttribute('height', '100')
    floorObj.object3D.visible = false
    floorObj.addEventListener('loaded', () => {
      const grid = new window.THREE.GridHelper(100, 50, 0x000000, 0x000000)
      grid.rotation.set(-Math.PI / 2, 0, 0)
      floorObj.object3D.add(grid)
      floorObj.object3D.children[0].material.polygonOffset = true
      floorObj.object3D.children[0].material.polygonOffsetFactor = -10
      floorObj.object3D.children[0].material.polygonOffsetUnits = 1
    })

    const toggleSurface = () => {
      if (floorObj.object3D.visible === true) {
        floorObj.object3D.visible = false
        xrHud.setActions({
          'Detected Surface': {text: 'Show Detected Surface', action: toggleSurface},
        })
      } else {
        floorObj.object3D.visible = true
        xrHud.setActions({
          'Detected Surface': {text: 'Hide Detected Surface', action: toggleSurface},
        })
      }
    }

    this.el.sceneEl.insertBefore(floorObj, this.el.sceneEl.firstChild)
    xrHud.setActions({
      'Detected Surface': {text: 'Show Detected Surface', action: toggleSurface},
      'Inspector': {
        action: () => {
          window.XR8.pause()
          window.AFRAME.scenes[0].inspect()
          // hide the 'resume inspector' button
          const resumeBtn = document.getElementById('resume-inspect-btn')
          if (resumeBtn) {
            resumeBtn.classList.remove('hidden')
          } else {
            setTimeout(() => {
              Array.from(document.getElementsByClassName('toggle-edit'))
                .forEach((a) => {
                  a.id = 'resume-inspect-btn'
                  a.addEventListener('click', () => {
                    a.classList.add('hidden')
                  })
                })
            }, 500)
          }
        },
      },
    })
  },
  tick() {
    const getEntityCount = () => {
      const elements = this.el.sceneEl.querySelectorAll('*')
      Array.prototype.slice.call(elements).filter(el => el.isEntity)
      return elements.length
    }
    const getMaxTexture = () => {
      const matDimensions: number[] = []
      this.el.sceneEl.object3D.traverse((object) => {
        const objectMaterialImage = object?.material?.map?.image
        if (objectMaterialImage?.width && objectMaterialImage?.height) {
          matDimensions.push(objectMaterialImage.width, objectMaterialImage.height)
        }
      })
      return Math.max(...matDimensions)
    }
    const getModelSize = () => [...document.querySelectorAll('a-asset-item')].reduce(
      (t, {data}) => (data ? t + data.byteLength : t), 0
    )
    const numTris = this.el.sceneEl.renderer.info.render.triangles
    const modelSize = getModelSize()
    const {points} = this.el.sceneEl.renderer.info.render
    const drawCalls = this.el.sceneEl.renderer.info.render.calls
    const {textures} = this.el.sceneEl.renderer.info.memory
    const texMax = getMaxTexture()
    const texMaxValue = texMax === Number.NEGATIVE_INFINITY ? '-' : texMax
    const shaders = this.el.sceneEl.renderer.info.programs.length
    const {geometries} = this.el.sceneEl.renderer.info.memory
    const entities = getEntityCount()
    xrHud.setStats({
      'Tris': {alarm: numTris > 75000, value: oom(numTris)},
      'Draw Calls': {value: drawCalls, alarm: drawCalls > 50},
      'Textures': {value: textures, alarm: textures > 30},
      'Tex(max)': {value: texMaxValue, alarm: texMax > 2048},
      'Shaders': {value: shaders, alarm: shaders > 30},
      'Geometries': {value: geometries, alarm: geometries > 30},
      'Points': {value: oom(points), display: points > 0, alarm: points > 100000},
      'Entities': {value: entities, alarm: entities > 75},
      'Models': {alarm: modelSize > 12 * 1024 * 1024, value: dataSize(modelSize)},
    })
  },
  remove() {
    xrHud.clearVersions(['AFRAME'])
    xrHud.clearActions(['Detected Surface', 'Inspector'])
    xrHud.clearStats([
      'Geometries',
      'Textures',
      'Shaders',
      'Draw Calls',
      'Tris',
      'Points',
      'Entities',
      'Tex(max)',
      'Models',
    ])
  },
})

let registered_ = false
const registerComponent = (xrHud: ReturnType<typeof XrHudManager>) => {
  if (registered_) {
    return
  }
  if (!window.AFRAME) {
    return
  }
  window.AFRAME.registerComponent('xr-hud', XrHudComponent(xrHud))
  registered_ = true
}

const monitorScene = (xrhud: ReturnType<typeof XrHudManager>, scene: Element) => {
  registerComponent(xrhud)
  scene.setAttribute('xr-hud', '')
}

let obs_: MutationObserver | null = null
const watchForAScenes = (xrhud: ReturnType<typeof XrHudManager>) => () => {
  const currScene = document.querySelector('a-scene')
  if (currScene) {
    monitorScene(xrhud, currScene)
  }
  obs_ = new MutationObserver(mutations => mutations.forEach((mutation) => {
    if (!mutation.addedNodes) {
      return
    }
    mutation.addedNodes.forEach((node) => {
      if (node.nodeName.toLowerCase() === 'a-scene' && node instanceof Element) {
        monitorScene(xrhud, node)
      }
    })
  }))
  obs_.observe(document, {childList: true, subtree: true})
}

const unwatchForAScenes = () => {
  if (obs_) {
    obs_.disconnect()
  }
}
export {
  unwatchForAScenes,
  watchForAScenes,
}
