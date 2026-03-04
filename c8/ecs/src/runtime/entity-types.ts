// @sublibrary(:world)
import type {Eid, ReadData, Schema} from '../shared/schema'
import type {TransformManager} from './transform-manager-types'
import type {RootAttribute} from './world-attribute'

type FunctionWithoutEid<Fn extends (eid: Eid, ...args: any[]) => any> = (
  Fn extends (eid: Eid, ...args: infer A) => infer R
    ? (...args: A) => R
    : never
)

type EntityTransformManager = {
  [K in keyof TransformManager]: FunctionWithoutEid<TransformManager[K]>
} & {
  lookAt: (other: Eid | Entity) => void
}

type Entity = EntityTransformManager & {
  eid: Eid

  // Component data
  get: <S extends Schema>(component: RootAttribute<S>) => ReadData<S>
  has: <S extends Schema>(component: RootAttribute<S>) => boolean
  set: <S extends Schema>(component: RootAttribute<S>, data: Partial<ReadData<S>>) => void
  remove: <S extends Schema>(component: RootAttribute<S>) => void
  reset: <S extends Schema>(component: RootAttribute<S>) => void

  // States
  hide(): void
  show(): void
  isHidden(): boolean

  disable(): void
  enable(): void
  isDisabled(): boolean

  delete(): void
  isDeleted(): boolean

  // Hierarchy
  setParent(parent: Eid | Entity | undefined | null): void
  getChildren(): Entity[]
  getParent(): Entity | null
  addChild(child: Eid | Entity): void
}

export type {
  Entity,
}
