// @rule(js_binary)
// @package(npm-ecs)
// @attr(esnext = 1)

import type {application} from '../src/runtime/application'
import type {Ecs} from '../src/runtime/index'
import {demoSceneGraph} from './scene'

type FullEcs = Ecs & {application: typeof application}

const {ecs} = window as any as {ecs: FullEcs}

await ecs.application.init(demoSceneGraph)

const world = ecs.application.getWorld()

if (world) {
  const camera = world.createEntity()
  ecs.Camera.set(world, camera, {type: 'perspective'})
  ecs.Position.set(world, camera, {x: 0, y: 5, z: 10})

  ecs.Quaternion.set(
    world,
    camera,
    ecs.math.quat.pitchYawRollDegrees({x: 20, y: 180, z: 0})
  )
  world.camera.setActiveEid(camera)
}
