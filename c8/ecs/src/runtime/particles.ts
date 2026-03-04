import ParticleSystem, {
  Emitter,
  Gravity,
  Life,
  Mass,
  RadialVelocity,
  Rotate,
  Vector3D,
  VectorVelocity,
  Scale as NebulaScale,
  Rate,
  Body,
  MeshRenderer,
  SpriteRenderer,
  Particle,
  Position as NebulaPosition,
  BoxZone,
  SphereZone,
  CrossZone,
  Color as NebulaColor,
  Collision,
  RandomDrift,
} from 'three-nebula'

import type {DeepReadonly} from 'ts-essentials'

import three from './three'

import type {Eid} from '../shared/schema'
import {registerComponent} from './registry'
import type {World} from './world'
import {createInstanced} from '../shared/instanced'
import {assets} from './assets'
import type {Object3D, Vector3, Euler} from './three-types'
import {getGltfLoader} from './loaders'
import {mat4} from './math/mat4'
import {addChild, notifyChanged} from './matrix-refresh'
import {quat, vec3} from './math/math'

interface ParticlesSchema {
  stopped: boolean
  emitterLife: number
  particlesPerShot: number
  emitDelay: number
  minimumLifespan: number
  maximumLifespan: number
  mass: number
  gravity: number
  scale: number
  forceX: number
  forceY: number
  forceZ: number
  spread: number
  radialVelocity: number
  spawnAreaType: string
  spawnAreaWidth: number
  spawnAreaHeight: number
  spawnAreaDepth: number
  spawnAreaRadius: number
  boundingZoneType: string
  boundingZoneWidth: number
  boundingZoneHeight: number
  boundingZoneDepth: number
  boundingZoneRadius: number
  resourceType: string
  resourceUrl: string
  // note(alancastillo): none, normal, add, multiply, subtract
  blendingMode: string
  animateColor: boolean
  colorStart: string
  colorEnd: string
  randomDrift: boolean
  randomDriftRange: number
  collisions: boolean
}

interface ParticleEmitterData {
  initFlag: boolean
  assetReady: boolean
}

interface Component<Schema, Data> {
  eid: DeepReadonly<Eid>
  schema: DeepReadonly<Schema>
  data: Data
}

interface ParticleComponent extends Component<ParticlesSchema, ParticleEmitterData> { }

const EULER_ORDER = 'YXZ'
const tempEuler = new three.Euler(0, 0, 0, EULER_ORDER)
const tempRotation = new three.Quaternion()
const systems = createInstanced<World, Map<Eid, ParticleSystem>>(() => new Map())
const globalVector = new Vector3D(0, 0, 0)
const tempTrs = {
  t: vec3.zero(),
  r: quat.zero(),
  s: vec3.zero(),
}

const worldTransform = mat4.i()
const blendingModes: {[key: string]: typeof three.Blending} = {
  none: three.NoBlending,
  normal: three.NormalBlending,
  add: three.AdditiveBlending,
  multiply: three.MultiplyBlending,
  subtract: three.SubtractiveBlending,
}

const resetParticleSystem = (system: ParticleSystem) => {
  const {emitters, renderers} = system
  emitters.forEach(e => e.removeAllParticles())
  system.update()
  emitters.forEach((e) => {
    system.removeEmitter(e)
    e.destroy()
  })
  renderers.forEach(r => system.removeRenderer(r))
  system.destroy()
}

const createSprite = async (url: string, blendingMode: string) => {
  if (!url) return null
  const asset = await assets.load({url})
  const spriteMap = new three.TextureLoader().load(asset.localUrl)
  const spriteMaterial = new three.SpriteMaterial({
    map: spriteMap,
    blending: blendingModes[blendingMode] ? blendingModes[blendingMode] : blendingModes.normal,
  })
  return new three.Sprite(spriteMaterial)
}

const loadModel = async (componentId: Eid, world: World, url: string) => {
  if (!url) return null
  const asset = await assets.load({url})
  const data = await asset.data.arrayBuffer()
  const gltf = await getGltfLoader().parseAsync(data, url)
  return gltf.scene
}

const setupEmitter = (world: World, component: ParticleComponent) => {
  const {schema} = component
  const emitter = new Emitter()

  // NOTE:(alancastillo): Rate needs to be set outside the initializers
  emitter.setRate(new Rate(schema.particlesPerShot, schema.emitDelay))

  emitter.addInitializers([
    new Mass(schema.mass >= 0 ? schema.mass : 0),
    new Life(schema.minimumLifespan, schema.maximumLifespan),
  ])

  const velocity = new Vector3D(schema.forceX, schema.forceY, schema.forceZ)
  if (schema.radialVelocity > 0) {
    emitter.addInitializer(new RadialVelocity(schema.radialVelocity, velocity, schema.spread))
  } else {
    emitter.addInitializer(new VectorVelocity(velocity, schema.spread))
  }

  // note:(alancastillo): spawning zones are set to relative positions
  if (schema.spawnAreaType === 'box') {
    emitter.addInitializer(new NebulaPosition(
      new BoxZone(0, 0, 0, schema.spawnAreaWidth, schema.spawnAreaHeight, schema.spawnAreaDepth)
    ))
  } else if (schema.spawnAreaType === 'sphere') {
    emitter.addInitializer(new NebulaPosition(
      new SphereZone(0, 0, 0, schema.spawnAreaRadius)
    ))
  }

  // note:(alancastillo): bounding zones are set to absolute positions
  world.transform.getWorldPosition(component.eid, tempTrs.t)
  if (schema.boundingZoneType === 'box') {
    emitter.addBehaviour(new CrossZone(
      new BoxZone(
        tempTrs.t.x,
        tempTrs.t.y,
        tempTrs.t.z,
        schema.boundingZoneWidth,
        schema.boundingZoneHeight,
        schema.boundingZoneDepth
      ), 'bound'
    ))
  } else if (schema.boundingZoneType === 'sphere') {
    emitter.addBehaviour(new CrossZone(
      new SphereZone(
        tempTrs.t.x,
        tempTrs.t.y,
        tempTrs.t.z,
        schema.boundingZoneRadius
      ), 'bound'
    ))
  }

  // NOTE:(alancastillo) These are the modifiers that will be applied to the particles
  emitter.addBehaviours([
    new Rotate('random', 'random'),
    new NebulaScale(schema.scale),
    new Gravity(schema.gravity),
  ])

  if (schema.resourceType === 'sprite' && schema.animateColor) {
    emitter.addBehaviour(
      new NebulaColor(new three.Color(schema.colorStart), new three.Color(schema.colorEnd))
    )
  }

  if (schema.randomDrift) {
    emitter.addBehaviour(
      new RandomDrift(schema.randomDriftRange, schema.randomDriftRange, schema.randomDriftRange)
    )
  }

  if (schema.collisions) {
    emitter.addBehaviour(new Collision(emitter))
  }

  return emitter
}

const updateEmitter = (
  world: World,
  component: ParticleComponent,
  particleSystem: ParticleSystem
) => {
  const {schema, data} = component
  if (schema.stopped) {
    particleSystem.emitters.forEach(e => e.setRate(new Rate(0)))
    world.events.dispatch(component.eid, 'particle-emitter: stopped emitting')
    data.initFlag = false
  } else if (!data.initFlag) {
    particleSystem.emitters.forEach((e) => {
      e.emit(
        schema.emitterLife > 0 ? schema.emitterLife : Infinity
      )
      e.setRate(new Rate(schema.particlesPerShot, schema.emitDelay))
    })
    data.initFlag = true
    world.events.dispatch(component.eid, 'particle-emitter: emitting')
  }

  const {eid} = component
  world.getWorldTransform(eid, worldTransform)
  worldTransform.decomposeTrs(tempTrs)
  globalVector.set(tempTrs.t.x, tempTrs.t.y, tempTrs.t.z)
  tempRotation.set(tempTrs.r.x, tempTrs.r.y, tempTrs.r.z, tempTrs.r.w)
  tempEuler.setFromQuaternion(tempRotation)
  particleSystem.emitters.forEach((e) => {
    e.setPosition(globalVector)
    e.setRotation(tempEuler)
  })
}

const setUpMeshRenderer = (world: World, renderer: MeshRenderer, mesh: any) => {
  renderer.onParticleCreated = (p: Particle) => {
    p.target = renderer._targetPool.get(mesh)
    const obj = p.target as Object3D
    obj.position.copy(p.position as unknown as Vector3)
    addChild(world.three.scene, obj)
  }
  renderer.onParticleUpdate = (p: Particle) => {
    const obj = p.target as Object3D
    obj.position.copy(p.position as unknown as Vector3)
    const rotation = p.rotation as unknown as Euler
    obj.rotation.set(rotation.x, rotation.y, rotation.z)
    obj.scale.set(p.scale, p.scale, p.scale)
    notifyChanged(obj)
  }
  renderer.onParticleDead = (p: Particle) => {
    const obj = p.target as Object3D
    renderer._targetPool.expire(obj)
    world.three.scene.remove(obj)
  }
}

const ParticleEmitter = registerComponent({
  name: 'particle-emitter',
  schema: {
    stopped: 'boolean',
    emitterLife: 'f32',
    particlesPerShot: 'ui32',
    emitDelay: 'f32',
    minimumLifespan: 'f32',
    maximumLifespan: 'f32',
    mass: 'f32',
    gravity: 'f32',
    scale: 'f32',
    forceX: 'f32',
    forceY: 'f32',
    forceZ: 'f32',
    spread: 'f32',
    radialVelocity: 'f32',
    // @enum point, box, sphere
    spawnAreaType: 'string',
    // @condition spawnAreaType=box
    spawnAreaWidth: 'f32',
    // @condition spawnAreaType=box
    spawnAreaHeight: 'f32',
    // @condition spawnAreaType=box
    spawnAreaDepth: 'f32',
    // @condition spawnAreaType=sphere
    spawnAreaRadius: 'f32',
    // @enum none, box, sphere
    boundingZoneType: 'string',
    // @condition boundingZoneType=box
    boundingZoneWidth: 'f32',
    // @condition boundingZoneType=box
    boundingZoneHeight: 'f32',
    // @condition boundingZoneType=box
    boundingZoneDepth: 'f32',
    // @condition boundingZoneType=sphere
    boundingZoneRadius: 'f32',
    // @enum none, sprite, model
    resourceType: 'string',
    // @condition resourceType=model|resourceType=sprite
    // @asset
    resourceUrl: 'string',
    // @condition resourceType=sprite
    // @enum none, normal, add, multiply, subtract
    blendingMode: 'string',
    // @condition resourceType=sprite
    animateColor: 'boolean',
    // @condition animateColor=true&resourceType=sprite
    colorStart: 'string',
    // @condition animateColor=true&resourceType=sprite
    colorEnd: 'string',
    randomDrift: 'boolean',
    // @condition randomDrift=true
    randomDriftRange: 'f32',
    collisions: 'boolean',
  },
  schemaDefaults: {
    emitterLife: 1,
    particlesPerShot: 1,
    emitDelay: 1,
    minimumLifespan: 1,
    maximumLifespan: 10,
    mass: 1,
    scale: 1,
    spawnAreaType: 'point',
    resourceType: 'none',
  },
  data: {
    initFlag: 'boolean',
    assetReady: 'boolean',
  },
  add: (world, component) => {
    const {schema, data, eid} = component

    data.initFlag = false
    data.assetReady = false

    const renderer = schema.resourceType === 'sprite'
      ? new SpriteRenderer(world.three.scene, three as any)
      : new MeshRenderer(world.three.scene, three as any)

    const emitter = setupEmitter(world, component)

    if (schema.resourceType === 'sprite') {
      createSprite(schema.resourceUrl, schema.blendingMode).then((loadedSprite) => {
        if (loadedSprite) {
          emitter.addInitializer(new Body(loadedSprite))
          component.dataAttribute.cursor(eid).assetReady = true
        }
      })
    } else if (schema.resourceType === 'model') {
      loadModel(component.eid, world, schema.resourceUrl).then((loadedModel) => {
        setUpMeshRenderer(world, renderer, loadedModel)
        component.dataAttribute.cursor(eid).assetReady = true
      })
    }

    const system = new ParticleSystem()
    systems(world).set(component.eid, system)
    system.addRenderer(renderer)
    system.addEmitter(emitter)
  },
  tick: (world, component) => {
    const system = systems(world).get(component.eid)
    if (!system || !component.data.assetReady) return
    updateEmitter(world, component, system)
    system.update(world.time.delta / 1000)
  },
  remove: (world, component) => {
    const system = systems(world).get(component.eid)
    if (!system) return
    resetParticleSystem(system)
    systems(world).delete(component.eid)
  },
})

export {
  ParticleEmitter,
}

export type {
  ParticlesSchema,
}
