import type {StudioComponentMetadata} from '@ecs/shared/studio-component'
import type {DeepReadonly} from 'ts-essentials'

import {useStudioComponentsContext} from '../studio-components-context'

const useComponentMetadata = (name: string): DeepReadonly<StudioComponentMetadata> => (
  useStudioComponentsContext().getComponentSchema(name)
)

export {
  useComponentMetadata,
}
