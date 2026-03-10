// @package(npm-ecs)
// @attr(externalize_npm = 1)
// @attr(esnext = 1)

import {describe, it, before, beforeEach, afterEach, assert} from '@repo/bzl/js/chai-js'

import {initThree} from './test-env'
import {ecs} from './test-runtime-lib'
import type {World} from './world'
import type {SceneGraph, Sky, Fog, Space} from '../shared/scene-graph'

describe('Effects Manager', () => {
  let world: World

  before(async () => {
    await ecs.ready()
  })

  beforeEach(() => {
    world = ecs.createWorld(...initThree())
    world.effects.attach()
  })

  afterEach(() => {
    world.destroy()
  })

  describe('Fog', () => {
    it('loads a scene with linear fog in a space and applies it correctly', async () => {
      const space: Space = {
        id: 'space1',
        name: 'Space 1',
        fog: {
          type: 'linear',
          near: 10,
          far: 100,
          color: '#ffffff',
        },
      }

      const sceneGraph: SceneGraph = {
        objects: {},
        spaces: {space1: space},
        entrySpaceId: 'space1',
      }

      world.loadScene(sceneGraph)

      const effectsFog = world.effects.getFog()
      assert.isOk(effectsFog)
      assert.strictEqual(effectsFog, space.fog)

      assert.isOk(world.three.scene.fog)
      assert.equal(world.three.scene.fog!.color.getHexString(), 'ffffff')
    })

    it('loads a scene with exponential fog in a space and applies it correctly', async () => {
      const space: Space = {
        id: 'space1',
        name: 'Space 1',
        fog: {
          type: 'exponential',
          density: 0.01,
          color: '#cccccc',
        },
      }

      const sceneGraph: SceneGraph = {
        objects: {},
        spaces: {space1: space},
        entrySpaceId: 'space1',
      }

      world.loadScene(sceneGraph)

      const effectsFog = world.effects.getFog()
      assert.isOk(effectsFog)
      assert.strictEqual(effectsFog, space.fog)

      assert.isOk(world.three.scene.fog)
      assert.equal(world.three.scene.fog!.color.getHexString(), 'cccccc')
    })

    it('loads a scene with no fog and removes fog correctly', async () => {
      const fog: Fog = {
        type: 'linear',
        near: 10,
        far: 100,
        color: '#ffffff',
      }

      world.effects.setFog(fog)

      assert.isOk(world.three.scene.fog)

      const space: Space = {
        id: 'space1',
        name: 'Space 1',
        fog: {type: 'none'},
      }

      const sceneGraph: SceneGraph = {
        objects: {},
        spaces: {space1: space},
        entrySpaceId: 'space1',
      }

      world.loadScene(sceneGraph)

      const effectsFog = world.effects.getFog()
      assert.isOk(effectsFog)
      assert.equal(effectsFog!.type, 'none')

      assert.isNull(world.three.scene.fog)
    })

    it('loads spaces with different fog and switches correctly', async () => {
      const space1: Space = {
        id: 'space1',
        name: 'Space 1',
        fog: {type: 'linear', near: 10, far: 100, color: '#ffffff'},
      }

      const space2: Space = {
        id: 'space2',
        name: 'Space 2',
        fog: {type: 'exponential', density: 0.01, color: '#cccccc'},
      }

      const sceneGraph: SceneGraph = {
        objects: {},
        spaces: {
          space1,
          space2,
        },
        entrySpaceId: 'space1',
      }

      const sceneHandle = world.loadScene(sceneGraph)

      let effectsFog = world.effects.getFog()
      assert.isOk(effectsFog)
      assert.equal(effectsFog!.type, 'linear')
      assert.equal(world.three.scene.fog!.color.getHexString(), 'ffffff')

      sceneHandle.loadSpace('space2')

      effectsFog = world.effects.getFog()
      assert.isOk(effectsFog)
      assert.equal(effectsFog!.type, 'exponential')
      assert.equal(world.three.scene.fog!.color.getHexString(), 'cccccc')
    })

    it('sets fog manually and applies it correctly', async () => {
      const fog: Fog = {
        type: 'linear',
        near: 5,
        far: 50,
        color: '#ff0000',
      }

      world.effects.setFog(fog)

      const effectsFog = world.effects.getFog()
      assert.strictEqual(effectsFog, fog)

      assert.isOk(world.three.scene.fog)
      assert.equal(world.three.scene.fog!.color.getHexString(), 'ff0000')
    })

    it('space fog overrides manual fog settings', async () => {
      const manualFog: Fog = {
        type: 'linear',
        near: 10,
        far: 100,
        color: '#ff0000',
      }

      world.effects.setFog(manualFog)

      const space: Space = {
        id: 'space1',
        name: 'Space 1',
        fog: {type: 'exponential', density: 0.02, color: '#00ff00'},
      }

      const sceneGraph: SceneGraph = {
        objects: {},
        spaces: {space1: space},
        entrySpaceId: 'space1',
      }

      world.loadScene(sceneGraph)

      const effectsFog = world.effects.getFog()
      assert.isOk(effectsFog)
      assert.strictEqual(effectsFog, space.fog)
      assert.equal(world.three.scene.fog!.color.getHexString(), '00ff00')
    })
  })

  describe('Sky', () => {
    it('loads a scene with sky and applies it correctly', async () => {
      const sky: Sky = {
        type: 'color',
        color: '#ff0000',
      }

      const sceneGraph: SceneGraph = {
        sky,
        objects: {},
        spaces: {},
      }

      world.loadScene(sceneGraph)

      assert.strictEqual(world.effects.getSky(), sky)
    })

    it('loads a scene with gradient sky and applies it correctly', async () => {
      const sky: Sky<string> = {
        type: 'gradient',
        style: 'linear',
        colors: ['#ff0000', '#00ff00', '#0000ff'],
      }

      const sceneGraph: SceneGraph = {
        sky,
        objects: {},
        spaces: {},
      }

      world.loadScene(sceneGraph)

      assert.strictEqual(world.effects.getSky(), sky)
    })

    it('loads spaces with different skies and switches correctly', async () => {
      const space1Sky: Sky<string> = {
        type: 'color',
        color: '#ff0000',
      }

      const space2Sky: Sky<string> = {
        type: 'gradient',
        style: 'linear',
        colors: ['#ffffff', '#000000'],
      }

      const space1: Space = {
        id: 'space1',
        name: 'Space 1',
        sky: space1Sky,
      }

      const space2: Space = {
        id: 'space2',
        name: 'Space 2',
        sky: space2Sky,
      }

      const sceneGraph: SceneGraph = {
        objects: {},
        spaces: {
          space1,
          space2,
        },
        entrySpaceId: 'space1',
      }

      const sceneHandle = world.loadScene(sceneGraph)

      assert.strictEqual(world.effects.getSky(), space1Sky)

      sceneHandle.loadSpace('space2')

      assert.strictEqual(world.effects.getSky(), space2Sky)
    })

    it('sets sky manually and applies it correctly', async () => {
      const sky: Sky<string> = {
        type: 'gradient',
        style: 'radial',
        colors: ['#ffffff', '#000000'],
      }

      world.effects.setSky(sky)

      assert.strictEqual(world.effects.getSky(), sky)
    })

    it('space sky overrides manual sky settings', async () => {
      const manualSky: Sky<string> = {
        type: 'color',
        color: '#ff0000',
      }

      world.effects.setSky(manualSky)

      const spaceSky: Sky<string> = {type: 'color', color: '#00ff00'}

      const space: Space = {
        id: 'space1',
        name: 'Space 1',
        sky: spaceSky,
      }

      const sceneGraph: SceneGraph = {
        objects: {},
        spaces: {space1: space},
        entrySpaceId: 'space1',
      }

      world.loadScene(sceneGraph)

      assert.strictEqual(world.effects.getSky(), spaceSky)
    })

    it('updating a scene does not override the manual sky settings', async () => {
      const space: Space = {
        id: 'space1',
        name: 'Space 1',
        sky: {type: 'color', color: '#00ff00'},
      }

      const sceneGraph: SceneGraph = {
        objects: {},
        spaces: {space1: space},
        entrySpaceId: 'space1',
      }

      const scene = world.loadScene(sceneGraph)

      const manualSky: Sky<string> = {
        type: 'color',
        color: '#ff0000',
      }

      world.effects.setSky(manualSky)

      scene.updateBaseObjects({})
      scene.updateDebug({...sceneGraph, sky: {type: 'color', color: '#0000ff'}})

      assert.strictEqual(world.effects.getSky(), manualSky)
    })
  })

  describe('Detached Behavior', () => {
    it('does not apply fog changes when detached', async () => {
      world.effects.detach()

      const fog: Fog = {
        type: 'linear',
        near: 10,
        far: 100,
        color: '#ffffff',
      }

      world.effects.setFog(fog)

      assert.isNull(world.three.scene.fog)
    })

    it('does not apply sky changes when detached', async () => {
      world.effects.detach()

      const sky: Sky<string> = {
        type: 'color',
        color: '#ff0000',
      }

      world.effects.setSky(sky)

      assert.equal(world.three.scene.children.length, 0)
    })

    it('clears fog on detach', async () => {
      const fog: Fog = {
        type: 'linear',
        near: 10,
        far: 100,
        color: '#ffffff',
      }

      world.effects.setFog(fog)

      assert.isOk(world.three.scene.fog)

      world.effects.detach()
      assert.isNull(world.three.scene.fog)
    })

    it('clears sky on detach', async () => {
      const sky: Sky<string> = {
        type: 'color',
        color: '#ff0000',
      }

      world.effects.setSky(sky)

      // Wait for async sky creation
      await new Promise(resolve => setTimeout(resolve, 50))

      assert.isTrue(world.three.scene.children.length > 0)

      world.effects.detach()

      assert.equal(world.three.scene.children.length, 0)
    })

    it('reapplies effects when reattached', async () => {
      const fog: Fog = {
        type: 'exponential',
        density: 0.01,
        color: '#cccccc',
      }

      const sky: Sky<string> = {
        type: 'color',
        color: '#ff0000',
      }

      world.effects.setFog(fog)
      world.effects.setSky(sky)

      // Wait for async sky creation
      await new Promise(resolve => setTimeout(resolve, 50))

      assert.isOk(world.three.scene.fog)
      assert.isTrue(world.three.scene.children.length > 0)

      world.effects.detach()
      assert.isNull(world.three.scene.fog)
      assert.equal(world.three.scene.children.length, 0)

      world.effects.attach()

      // Wait for async sky recreation
      await new Promise(resolve => setTimeout(resolve, 50))

      assert.isOk(world.three.scene.fog)
      assert.equal(world.three.scene.fog!.color.getHexString(), 'cccccc')
      assert.isTrue(world.three.scene.children.length > 0)
    })

    it('space changes are ignored when detached', async () => {
      world.effects.detach()

      const space: Space = {
        id: 'space1',
        name: 'Space 1',
        fog: {
          type: 'linear',
          near: 10,
          far: 100,
          color: '#ffffff',
        },
        sky: {type: 'color', color: '#ff0000'},
      }

      const sceneGraph: SceneGraph = {
        objects: {},
        spaces: {space1: space},
        entrySpaceId: 'space1',
      }

      world.loadScene(sceneGraph)

      assert.isNull(world.three.scene.fog)
      assert.equal(world.three.scene.children.length, 0)
    })
  })
})
