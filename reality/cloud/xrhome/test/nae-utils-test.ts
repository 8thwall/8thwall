import {expect} from 'chai'

import {getExportDisabled} from '../src/client/editor/modals/nae/utils'
import type {SceneContext} from '../src/client/studio/scene-context'

const DEFAULT_SCENE_CTX: SceneContext = {
  updateScene: () => {},
  updateObject: () => {},
  isDraggingGizmo: false,
  isRenamingById: {},
  setIsRenaming: () => {},
  isDraggingGizmoRef: {current: false},
  setIsDraggingGizmo: () => {},
  canUndo: false,
  undo: () => {},
  canRedo: false,
  redo: () => {},
  scene: {objects: {}},
}

describe('getExportDisabled', () => {
  it('returns false when platform is not in xrNotSupported', () => {
    const sceneCtx: SceneContext = {
      ...DEFAULT_SCENE_CTX,
    }
    const result = getExportDisabled(sceneCtx, 'ios')
    expect(result).to.equal(false)
  })

  it('returns true when platform is supported but all cameras are not 3D', () => {
    const sceneCtx: SceneContext = {
      ...DEFAULT_SCENE_CTX,
      scene: {
        objects: {
          a: {
            camera: {xr: {xrCameraType: 'layers'}},
            id: '',
            components: undefined,
          },
          b: {
            camera: {xr: {xrCameraType: 'worldLayers'}},
            id: '',
            components: undefined,
          },
        },
      },
    }
    const result = getExportDisabled(sceneCtx, 'android')
    expect(result).to.equal(true)
  })

  it('returns false when a 3D-only camera exists', () => {
    const sceneCtx: SceneContext = {
      ...DEFAULT_SCENE_CTX,
      scene: {
        objects: {
          a: {
            camera: {xr: {xrCameraType: '3dOnly'}},
            id: '',
            components: undefined,
          },
          b: {
            camera: {xr: {xrCameraType: 'layers'}},
            id: '',
            components: undefined,
          },
        },
      },
    }
    const result = getExportDisabled(sceneCtx, 'html')
    expect(result).to.equal(false)
  })

  it('returns false when old camera missing xrCameraType exists', () => {
    const sceneCtx: SceneContext = {
      ...DEFAULT_SCENE_CTX,
      scene: {
        objects: {
          a: {
            camera: {xr: {}},
            id: '',
            components: undefined,
          },
          b: {
            camera: {xr: {xrCameraType: 'layers'}},
            id: '',
            components: undefined,
          },
        },
      },
    }
    const result = getExportDisabled(sceneCtx, 'android')
    expect(result).to.equal(false)
  })

  it('returns true when objects exist but none have cameras', () => {
    const sceneCtx: SceneContext = {
      ...DEFAULT_SCENE_CTX,
      scene: {
        objects: {
          a: {id: '', components: undefined},
          b: {id: '', components: undefined},
        },
      },
    }
    const result = getExportDisabled(sceneCtx, 'html')
    expect(result).to.equal(true)
  })

  it('returns true when scene.objects is empty', () => {
    const sceneCtx: SceneContext = {
      ...DEFAULT_SCENE_CTX,
      scene: {objects: {}},
    }
    const result = getExportDisabled(sceneCtx, 'android')
    expect(result).to.equal(true)
  })

  it('returns true when mix of non-camera and non-3D cameras', () => {
    const sceneCtx: SceneContext = {
      ...DEFAULT_SCENE_CTX,
      scene: {
        objects: {
          a: {id: '', components: undefined},
          b: {camera: {xr: {xrCameraType: 'layers'}}, id: '', components: undefined},
        },
      },
    }
    const result = getExportDisabled(sceneCtx, 'html')
    expect(result).to.equal(true)
  })
})
