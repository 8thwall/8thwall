// @rule(js_binary)
// @attr(target_compatible_with = WEB_ONLY)

/* eslint-disable import/no-unresolved */
import ECS from 'c8/ecs/ecs-asm'  // @dep(ecs-asm)
/* eslint-enable import/no-unresolved */

/* eslint-disable no-console */

const bigIntPrinter = (value: bigint) => `${value}n`

const jsonReplacer = (_: string, value: unknown) => (
  (typeof value === 'bigint') ? bigIntPrinter(value) : value
)

const log = (...args: unknown[]) => {
  const text = args
    .map(e => ((typeof e === 'string') ? e : JSON.stringify(e, jsonReplacer)))
    .join(' ')

  document.body.textContent += '\n'
  document.body.textContent += text
}

const PREFIX_TO_STRIP = '_c8EmAsm_'
const fixFunctions = (mod: any) => {
  Object.entries(mod).forEach(([key, value]) => {
    if (key.startsWith(PREFIX_TO_STRIP) && typeof value === 'function') {
      mod[key.slice(PREFIX_TO_STRIP.length)] = value
    }
  })
}

const BENCHMARK_REPETITIONS = [
  1000000,
  100000,
  10000,
  1000,
  100,
  10,
  1,
]

const run = async () => {
  log('Creating test ECS.')
  const ecs = await ECS({})
  fixFunctions(ecs)

  const world = ecs.createWorld()
  const entity = ecs.createEntity(world)

  log({world, entity})

  const component1 = ecs.createComponent(world, 0, 0)
  const component2 = ecs.createComponent(world, 5, 0)
  const component3 = ecs.createComponent(world, 10, 0)

  log({component1, component2, component3})

  ecs.entityAddComponent(world, entity, component1)
  ecs.entityAddComponent(world, entity, component2)
  ecs.entityAddComponent(world, entity, component2)

  log({
    has1: ecs.entityHasComponent(world, entity, component1),
    has2: ecs.entityHasComponent(world, entity, component2),
    has3: ecs.entityHasComponent(world, entity, component3),
  })

  const allocateWasmData = (ids: any[]) => {
    const ptr = ecs._malloc(ids.length * 4)
    const view = new Uint32Array(ecs.HEAPU32.buffer, ptr, ids.length)
    view.set(ids)
    return ptr
  }

  const componentIds = allocateWasmData([component1, component2])
  const componentIds2 = allocateWasmData([component1, component3])
  const query1 = ecs.createQuery(world, componentIds, 2)
  const query2 = ecs.createQuery(world, componentIds2, 2)
  ecs._free(componentIds2)

  log({query1, query2})

  const count1 = ecs.countQuery(world, query1)
  const count2 = ecs.countQuery(world, query2)
  log({count1, count2})

  {
    const entity1 = ecs.createEntity(world)
    const entity2 = ecs.createEntity(world)

    ecs.entityAddComponent(world, entity1, component1)
    ecs.entityAddComponent(world, entity1, component2)
    ecs.entityAddComponent(world, entity2, component1)

    const count3 = ecs.countQuery(world, query1)
    const count4 = ecs.countQuery(world, query2)
    log({count3, count4})
    ecs.deleteEntity(world, entity1)
    const count3a = ecs.countQuery(world, query1)
    const count4a = ecs.countQuery(world, query2)
    log({count3a, count4a})
  }

  log('\nEcho counts:')
  BENCHMARK_REPETITIONS.forEach((repetition) => {
    const label = `x${repetition.toLocaleString()}`
    const start = performance.now()
    for (let i = 0; i < repetition; i++) {
      ecs.echo(i)
    }
    log(label)
    const millisPer = (performance.now() - start) / repetition
    const meanNanos = (millisPer * 1000 * 1000).toFixed(0)
    const perFrame = Math.round((1000 / millisPer / 60)).toLocaleString()
    log(`${meanNanos}ns, ${perFrame}/frame`)
  })

  log('\nEntity creation/deletion')
  BENCHMARK_REPETITIONS.forEach((repetition) => {
    const label = `x${repetition.toLocaleString()}`
    const start = performance.now()
    for (let i = 0; i < repetition; i++) {
      const e = ecs.createEntity(world)
      ecs.entityAddComponent(world, e, component1)
      ecs.entityAddComponent(world, e, component2)
      ecs.entityRemoveComponent(world, e, component1)
      ecs.deleteEntity(world, e)
    }
    log(label)
    const millisPer = (performance.now() - start) / repetition
    const meanNanos = (millisPer * 1000 * 1000).toFixed(0)
    const perFrame = Math.round((1000 / millisPer / 60)).toLocaleString()
    log(`${meanNanos}ns, ${perFrame}/frame`)
  })

  {
    console.log('Testing remove')
    const entity3 = ecs.createEntity(world)
    ecs.entityAddComponent(world, entity3, component1)
    ecs.entityAddComponent(world, entity3, component3)
    const componentIds3 = allocateWasmData([component1, component3])
    const allQuery = ecs.createQuery(world, componentIds3, 2)
    ecs._free(componentIds3)
    const count5 = ecs.countQuery(world, allQuery)

    ecs.entityRemoveComponent(world, entity3, component3)
    const count5a = ecs.countQuery(world, allQuery)
    const count6 = ecs.countQuery(world, allQuery)
    log({count5, count5a, count6})
  }

  {
    log('Getting pointer to data.')
    const dataComponent = ecs.createComponent(world, 4, 4)
    const dataEntity = ecs.createEntity(world)
    log({dataComponent, dataEntity})
    ecs.entityAddComponent(world, dataEntity, dataComponent)
    const ptr = ecs.entityGetMutComponent(world, dataEntity, dataComponent)
    log({ptr, world, dataEntity, dataComponent})
    const view = new Uint32Array(ecs.HEAPU32.buffer, ptr, 1)
    view[0] = 42
    log('Data has been set.')

    const ptr2 = ecs.entityGetComponent(world, dataEntity, dataComponent)
    log({ptr2})
    const view2 = new Uint32Array(ecs.HEAPU32.buffer, ptr2, 1)
    log('Data after', Array.from(view2))
    ecs._free(ptr)
  }

  {
    log('Checking observers')
    const addQuery = ecs.createAddObserver(world, componentIds, 2)
    const removeQuery = ecs.createRemoveObserver(world, componentIds, 2)
    ecs._free(componentIds)

    const unneededPtr = ecs._malloc(4)

    const expectAdds = (label: string, expected: number) => {
      const count = ecs.flushObserver(addQuery, unneededPtr)
      if (count === expected) {
        const checkEmoji = '✅'
        log(checkEmoji, label, `(${count})`)
      } else {
        const xEmoji = '❌'
        log(xEmoji, '[adds]', label, `(${count}, expected: ${expected})`)
      }
    }

    const expectRemoves = (label: string, expected: number) => {
      const count = ecs.flushObserver(removeQuery, unneededPtr)
      if (count === expected) {
        const checkEmoji = '✅'
        log(checkEmoji, label, `(${count})`)
      } else {
        const xEmoji = '❌'
        log(xEmoji, '[removes]', label, `(${count}, expected: ${expected})`)
      }
    }

    expectAdds('First thing', 0)
    expectAdds('First thing, again', 0)
    expectRemoves('First thing', 0)
    expectRemoves('First thing, again', 0)
    const e1 = ecs.createEntity(world)
    ecs.entityAddComponent(world, e1, component1)
    ecs.entityAddComponent(world, e1, component2)

    expectAdds('After create', 1)
    expectAdds('Then again', 0)
    expectRemoves('After create', 0)
    expectRemoves('Then again', 0)
    const e2 = ecs.createEntity(world)
    ecs.entityAddComponent(world, e2, component1)
    ecs.entityAddComponent(world, e2, component2)
    expectAdds('After create', 1)
    expectAdds('Then again', 0)
    const e3 = ecs.createEntity(world)
    ecs.entityAddComponent(world, e3, component1)
    ecs.entityAddComponent(world, e3, component2)
    const e4 = ecs.createEntity(world)
    ecs.entityAddComponent(world, e4, component1)
    ecs.entityAddComponent(world, e4, component2)
    expectRemoves('After double creates', 0)

    ecs.entityRemoveComponent(world, e1, component1)
    expectRemoves('After single removes', 1)
    expectRemoves('No change', 0)
    ecs.entityRemoveComponent(world, e3, component1)
    ecs.entityRemoveComponent(world, e4, component1)
    expectRemoves('After double remove', 2)
    expectRemoves('No change', 0)

    expectAdds('After double create', 2)
    expectAdds('Then again', 0)
  }
}

run()
