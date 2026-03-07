// @sublibrary(three-types)

/* eslint-disable no-restricted-imports */
import type {Object3D as OriginalObject3D} from 'three'

import type {UserData} from './user-data'

type Overrides = {
  userData: UserData
  parent: Object3D | null
  children: Object3D[]
}

type Object3D = Omit<OriginalObject3D, keyof Overrides> & Overrides

// This doesn't have a strict of a type because things like Camera and Scene don't extend our
// custom Object3D
type RelaxedObject3D = OriginalObject3D | Object3D

export type {
  Object3D,
  RelaxedObject3D,
}
