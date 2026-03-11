import type {
  StudioComponentData, StudioComponentError, StudioComponentMetadata,

} from '@ecs/shared/studio-component'
import type {DeepReadonly} from 'ts-essentials'

type StudioComponentRecord = {
  locationKey: string
  metadata: StudioComponentMetadata
}

type State = DeepReadonly<{
  studioComponents: Record<string, StudioComponentRecord>
  parsingFileCount: number
  errors: Record<string, StudioComponentError[]>
}>

const addComponents = (
  locationKey: string,
  componentData: DeepReadonly<StudioComponentData[]>,
  errors: DeepReadonly<StudioComponentError[]>
) => (
  {type: 'ADD_COMPONENTS' as const, locationKey, componentData, errors}
)

const incrementParsingFileCount = () => ({type: 'INCREMENT_PARSING_FILE_COUNT' as const})

const decrementParsingFileCount = () => ({type: 'DECREMENT_PARSING_FILE_COUNT' as const})

type AddComponentAction = ReturnType<typeof addComponents>

type IncrementParsingFileCountAction = ReturnType<typeof incrementParsingFileCount>

type DecrementParsingFileCountAction = ReturnType<typeof decrementParsingFileCount>

type Action = AddComponentAction | IncrementParsingFileCountAction | DecrementParsingFileCountAction

const reducer = (state: State, action: Action): State => {
  switch (action.type) {
    case 'ADD_COMPONENTS': {
      const {locationKey, componentData, errors} = action
      const newComponents = {...state.studioComponents}
      Object.keys(newComponents).forEach((name) => {
        if (newComponents[name].locationKey === locationKey) {
          delete newComponents[name]
        }
      })
      componentData.forEach(({name, ...metadata}) => {
        newComponents[name] = {
          locationKey,
          metadata,
        }
      })
      const newErrors = {...state.errors}
      if (!errors.length) {
        delete newErrors[locationKey]
      } else {
        newErrors[locationKey] = errors
      }
      return {...state, studioComponents: newComponents, errors: newErrors}
    }
    case 'INCREMENT_PARSING_FILE_COUNT': {
      return {...state, parsingFileCount: state.parsingFileCount + 1}
    }
    case 'DECREMENT_PARSING_FILE_COUNT': {
      return {...state, parsingFileCount: state.parsingFileCount - 1}
    }
    default:
      throw new Error(
        `Unexpected action type in studio components reducer: ${(action as any).type}`
      )
  }
}

export {
  addComponents,
  incrementParsingFileCount,
  decrementParsingFileCount,
  reducer,
}
