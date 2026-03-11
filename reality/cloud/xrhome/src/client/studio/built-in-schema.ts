import type {DeepReadonly} from 'ts-essentials'
import type {StudioComponentMetadata} from '@ecs/shared/studio-component'

import {BUILTIN_COMPONENT_SCHEMA} from './generated-schema'

const getBuiltinComponentMetadata = (
  name: string
): DeepReadonly<StudioComponentMetadata> | undefined => (
  BUILTIN_COMPONENT_SCHEMA.find(schema => schema.name === name)
)

export {
  getBuiltinComponentMetadata,
}
