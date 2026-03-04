/* eslint-disable @typescript-eslint/no-unused-vars */
// @package(npm-ecs)
// @attr(externalize_npm = 1)
// @attr(esnext = 1)

import './test-env'
import {ecs} from './test-runtime-lib'
import type {World} from './world'

declare global {
  interface EcsEventTypes {
    myEvent: {foo: 'bar' | 'baz'}
  }
}

// NOTE(christoph): This test passes or fails based on whether it compiles, we don't need to run
// the code itself.
const run = () => {
  const world: World = null as any
  const entity = world.createEntity()

  world.events.addListener(entity, 'myEvent', (event) => {
    const myThing: 'bar' | 'baz' = event.data.foo

    // @ts-expect-error
    const myOtherThing: 'foo' = event.data.foo
  })

  world.events.addListener(entity, 'myUnknownEvent', (event) => {
    // @ts-expect-error
    const myThing: 'bar' | 'baz' = event.data.foo

    // @ts-expect-error
    const myOtherThing: 'foo' = event.data.foo
  })

  world.events.addListener(entity, 'click', (event) => {
    const v1: number = event.data.x

    // @ts-expect-error
    const v2: number = event.data.z
  })

  world.events.addListener(entity, ecs.input.UI_CLICK, (event) => {
    const v1: number = event.data.x

    // @ts-expect-error
    const v2: number = event.data.z
  })

  world.events.addListener(entity, 'click2', (event) => {
    // @ts-expect-error
    const v1: number = event.data.x

    // @ts-expect-error
    const v2: number = event.data.z
  })

  world.events.addListener(entity, ecs.events.GLTF_ANIMATION_FINISHED, (event) => {
    const v1: string = event.data.name

    // @ts-expect-error
    const v2: any = event.data.somethingElse
  })

  // Can opt out of type checking by using 'any'
  world.events.addListener(entity, ecs.events.FACE_LEFT_EYE_CLOSED, (event: any) => {
    const v1: string = event.data.name
    const v2: any = event.data.somethingElse
  })

  ecs.registerComponent({
    name: 'state-machine',
    stateMachine: ({eid, defineState}) => {
      defineState('initial')
        .listen(eid, 'myEvent', (event) => {
          const myThing: 'bar' | 'baz' = event.data.foo

          // @ts-expect-error
          const myOtherThing: 'foo' = event.data.foo
        })
        // Can opt out of type checking by using 'any'
        .listen(eid, ecs.events.FACE_LEFT_EYE_CLOSED, (event: any) => {
          const v1: string = event.data.name
          const v2: any = event.data.somethingElse
        })
        .onEvent('myEvent', 'initial', {
          where: event => event.data.foo === 'bar',
        })

      ecs.defineState('other')
        .listen(eid, 'myEvent', (event) => {
          const myThing: 'bar' | 'baz' = event.data.foo

          // @ts-expect-error
          const myOtherThing: 'foo' = event.data.foo
        })
        .onEvent('myEvent', 'initial', {
          where: event => event.data.foo === 'bar',
        })
    },
  })
}
