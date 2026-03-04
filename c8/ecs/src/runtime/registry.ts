/* eslint-disable max-len */
import type {DeepReadonly} from 'ts-essentials'

import type {World} from './world'
import type {Eid, Schema, WriteData, ReadData} from '../shared/schema'
import type {RootAttribute, WorldAttribute} from './world-attribute'
import {defineQuery, lifecycleQueries} from './query'
import {createAttribute, createExtendedAttribute} from './attribute'
import {defineSystem} from './system'
import {
  BaseMachineDefProps, createStateMachine, defineState, defineStateGroup, deleteStateMachine, generateStateMachineDefiner, MachineId,
  tickStateMachine, type StateMachineDefinition,
} from './state-machine'
import {createInstanced} from '../shared/instanced'
import {Collider} from './physics'
import {Disabled} from './disabled'
import type {ComponentCallbackArgs, ComponentStateMachineDefiner, ComponentStateMachineDefinition} from './state-machine-types'
import type {BaseSchema, ExtendedSchema} from '../shared/extended-schema'

type WorldBehavior = (w: World) => void

const behaviors: WorldBehavior[] = []

const attributes: Record<string, RootAttribute<any>> = {
  collider: Collider,
  disabled: Disabled,
}

const registerBehavior = (callback: WorldBehavior) => {
  behaviors.push(callback)
}

const unregisterBehavior = (callback: WorldBehavior) => {
  const index = behaviors.indexOf(callback)
  if (index !== -1) {
    behaviors.splice(index, 1)
  }
}

const getBehaviors = (): DeepReadonly<typeof behaviors> => behaviors

const runBehaviors = (w: World): World => {
  getBehaviors().forEach(c => c(w))
  return w
}

const registerAttribute = <T extends Schema>(
  name: string, schema?: T, customDefaults?: Partial<ReadData<T>>
): RootAttribute<T> => {
  const attribute = createAttribute(schema, customDefaults)
  attributes[name] = attribute
  return attribute
}

type ComponentCursor<S extends Schema, D extends Schema> = {
  eid: Eid
  schema: WriteData<S>
  data: WriteData<D>
  schemaAttribute: WorldAttribute<S>
  dataAttribute: WorldAttribute<D>
}

type RemovedComponentCursor<S extends Schema, D extends Schema> = Omit<
ComponentCursor<S, D>, 'schema' | 'data'
>

type ComponentRegistration<ES extends ExtendedSchema<Schema>, ED extends ExtendedSchema<Schema>> = {
  name: string
  /**
   * Add data that can be configured on the component.
   */
  schema?: ES
  /**
   * Add defaults for the schema fields.
   */
  schemaDefaults?: Partial<ReadData<BaseSchema<ES>>>
  /**
   * Add data that cannot be configured outside of the component.
   */
  data?: ED
  /**
   * Runs when the component is added to an entity.
   */
  add?: (w: World, cursor: ComponentCursor<BaseSchema<ES>, BaseSchema<ED>>) => void
  /**
   * Runs every frame for each entity.
   */
  tick?: (w: World, cursor: ComponentCursor<BaseSchema<ES>, BaseSchema<ED>>) => void
  /**
   * Runs when the component is removed from an entity.
   */
  remove?: (w: World, cursor: RemovedComponentCursor<BaseSchema<ES>, BaseSchema<ED>>) => void
  /**
   * Define stateful behaviors such as event handling and transitions.
   */
  stateMachine?: ComponentStateMachineDefinition<BaseSchema<ES>, BaseSchema<ED>> | ComponentStateMachineDefiner<BaseSchema<ES>, BaseSchema<ED>>
}

const isStateMachineDefiner = <S extends Schema, D extends Schema>(
  machineDef: StateMachineDefinition<any> | ComponentStateMachineDefiner<S, D>
): machineDef is ComponentStateMachineDefiner<S, D> => typeof machineDef === 'function'

const createComponentStateMachine = <S extends Schema, D extends Schema>(
  world: World, eid: Eid, schemaComponent: RootAttribute<S>, dataComponent: RootAttribute<D> | undefined,
  machineDef: ComponentStateMachineDefinition<S, D> | ComponentStateMachineDefiner<S, D>
): MachineId => {
  const definition = isStateMachineDefiner(machineDef)
    ? generateStateMachineDefiner<ComponentCallbackArgs<S, D>>(
      world, eid, (props: BaseMachineDefProps) => machineDef({
        ...props,
        schemaAttribute: schemaComponent.forWorld(props.world),
        dataAttribute: dataComponent?.forWorld(props.world)!,
        defineState,
        defineStateGroup,
      })
    )
    : machineDef

  const prepareCallback = () => ({
    schema: schemaComponent.forWorld(world).cursor(eid),
    data: dataComponent?.forWorld(world).cursor(eid),
  })

  return createStateMachine(world, eid, {...definition, prepareCallback})
}

const registerComponent = <ES extends ExtendedSchema<Schema>, ED extends ExtendedSchema<Schema>>({
  name, schema, schemaDefaults, stateMachine: machineDef,
  data, tick, add, remove,
}: ComponentRegistration<ES, ED>) => {
  if (attributes[name]) {
    // eslint-disable-next-line no-console
    console.warn(`Already registered component with name: ${name}`)
  }

  type S = BaseSchema<ES>
  type D = BaseSchema<ED>

  const schemaComponent = createExtendedAttribute(schema, schemaDefaults)
  attributes[name] = schemaComponent

  const isPlainAttribute = !(tick || add || remove || machineDef)
  if (isPlainAttribute) {
    if (data) {
      throw new Error('Can only specify data when defining component lifecycle.')
    }
    return schemaComponent
  }

  const dataComponent: RootAttribute<D> | undefined = data && createExtendedAttribute(data)
  const schemaQuery = defineQuery([schemaComponent])
  const {enter, exit} = lifecycleQueries(schemaQuery)
  const machineMap = machineDef && createInstanced<World, Map<Eid, MachineId>>(() => new Map())

  const componentCursor: ComponentCursor<S, D> = {
    eid: 0n,
    schema: null!,
    data: null!,
    schemaAttribute: null!,
    dataAttribute: null!,
  }

  const setCursorEid = (world: World, eid: Eid) => {
    componentCursor.schemaAttribute = schemaComponent.forWorld(world)
    componentCursor.eid = eid
    componentCursor.schema = schemaComponent.cursor(world, eid)
    if (dataComponent) {
      componentCursor.data = dataComponent.cursor(world, eid)
      componentCursor.dataAttribute = dataComponent.forWorld(world)
    }
  }

  registerBehavior((world) => {
    for (const eid of exit(world)) {
      if (dataComponent) {
        dataComponent.remove(world, eid)
      }

      if (machineMap) {
        const id = machineMap(world).get(eid)
        if (id) {
          deleteStateMachine(world, id)
          machineMap(world).delete(eid)
        }
      }

      if (remove) {
        componentCursor.eid = eid
        componentCursor.schema = null!
        componentCursor.data = null!
        remove(world, componentCursor)
      }
    }

    for (const eid of enter(world)) {
      if (dataComponent) {
        if (!dataComponent.has(world, eid)) {
          dataComponent.reset(world, eid)
        }
      }
      if (add) {
        setCursorEid(world, eid)
        add(world, componentCursor)
      }
      if (machineMap && machineDef) {
        const machine = createComponentStateMachine(world, eid, schemaComponent, dataComponent, machineDef)
        machineMap(world).set(eid, machine)
      }
    }
  })

  if (tick) {
    type SystemTuple = [RootAttribute<S>, RootAttribute<D>]

    const systemTerms: SystemTuple = dataComponent
      ? [schemaComponent, dataComponent]
      : [schemaComponent] as any as SystemTuple

    const tickSystem = defineSystem<SystemTuple>(
      systemTerms,
      (world, eid, [schemaCursor, dataCursor]) => {
        componentCursor.eid = eid
        componentCursor.schema = schemaCursor
        componentCursor.data = dataCursor
        tick(world, componentCursor)
      }
    )

    registerBehavior(tickSystem)
  }

  if (machineMap) {
    const tickStateMachineSystem = defineSystem<[RootAttribute<S>]>(
      [schemaComponent],
      (world, eid) => {
        const machineId = machineMap(world).get(eid)
        if (machineId) {
          tickStateMachine(world, machineId)
        }
      }
    )
    registerBehavior(tickStateMachineSystem)
  }

  return schemaComponent
}

const getAttribute = (name: string): RootAttribute<{}> => {
  if (!attributes[name]) {
    throw new Error(`No attribute registered with name: ${name}`)
  }

  return attributes[name]
}

const listAttributes = (): string[] => Object.keys(attributes)

export {
  registerBehavior,
  unregisterBehavior,
  registerComponent,
  getBehaviors,
  runBehaviors,
  registerAttribute,
  getAttribute,
  listAttributes,
}
