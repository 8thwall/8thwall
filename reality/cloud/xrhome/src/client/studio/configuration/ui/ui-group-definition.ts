import type {DeepReadonly} from 'ts-essentials'

import type {UiGraphSettings} from '@ecs/shared/scene-graph'

import type {UiGroup, UiPropertyName} from '../ui-configurator-types'

const UI_PROPERTY_MAPPINGS: DeepReadonly<Record<UiPropertyName, UiGroup | 'none' | 'todo'>> = {
  // Not part of any group
  type: 'none',
  opacity: 'none',
  ignoreRaycast: 'none',
  position: 'none',
  top: 'none',
  left: 'none',
  bottom: 'none',
  right: 'none',
  stackingOrder: 'none',
  width: 'none',
  minWidth: 'none',
  minHeight: 'none',
  height: 'none',

  // Not surfaced yet
  fixedSize: 'todo',
  overflow: 'todo',
  direction: 'todo',
  display: 'todo',
  video: 'todo',

  background: 'background',
  backgroundOpacity: 'background',
  backgroundSize: 'background',
  borderRadius: 'background',
  borderRadiusBottomLeft: 'background',
  borderRadiusBottomRight: 'background',
  borderRadiusTopLeft: 'background',
  borderRadiusTopRight: 'background',
  image: 'background',
  nineSliceBorderBottom: 'background',
  nineSliceBorderLeft: 'background',
  nineSliceBorderRight: 'background',
  nineSliceBorderTop: 'background',
  nineSliceScaleFactor: 'background',

  borderWidth: 'border',
  borderColor: 'border',

  alignContent: 'layout',
  alignItems: 'layout',
  alignSelf: 'layout',
  columnGap: 'layout',
  flex: 'layout',
  flexBasis: 'layout',
  flexDirection: 'layout',
  flexGrow: 'layout',
  flexShrink: 'layout',
  flexWrap: 'layout',
  gap: 'layout',
  justifyContent: 'layout',
  margin: 'layout',
  marginBottom: 'layout',
  marginLeft: 'layout',
  marginRight: 'layout',
  marginTop: 'layout',
  maxHeight: 'layout',
  maxWidth: 'layout',
  padding: 'layout',
  paddingBottom: 'layout',
  paddingLeft: 'layout',
  paddingRight: 'layout',
  paddingTop: 'layout',
  rowGap: 'layout',

  color: 'text',
  font: 'text',
  fontSize: 'text',
  text: 'text',
  textAlign: 'text',
  verticalTextAlign: 'text',
}

const UI_PROPERTIES_BY_GROUP: DeepReadonly<Record<UiGroup, UiPropertyName[]>> = (
  ['layout', 'text', 'border', 'background'].reduce((acc, group) => {
    acc[group] = Object.keys(UI_PROPERTY_MAPPINGS)
      .filter(p => UI_PROPERTY_MAPPINGS[p] === group)
    return acc
  }, {
    layout: [],
    text: [],
    border: [],
    background: [],
  })
)

const INITIAL_UI_PROPERTIES_BY_GROUP: DeepReadonly<Record<UiGroup, UiGraphSettings>> = {
  layout: {
    flexDirection: 'row',
  },
  text: {
    text: 'text',
  },
  border: {
    borderColor: '#ffffff',
    borderWidth: 1,
  },
  background: {
    background: '#ffffff',
    backgroundOpacity: 0.5,
  },
}

const addUiGroup = (group: UiGroup,
  settings: DeepReadonly<UiGraphSettings>): DeepReadonly<UiGraphSettings> => {
  const initial = INITIAL_UI_PROPERTIES_BY_GROUP[group]
  return {...initial, ...settings}
}

const removeUiGroup = (group: UiGroup,
  settings: DeepReadonly<UiGraphSettings>): DeepReadonly<UiGraphSettings> => {
  const res = {...settings}
  UI_PROPERTIES_BY_GROUP[group].forEach((p) => {
    delete res[p]
  })
  return res
}

const isUiGroupPresent = (group: UiGroup, settings: DeepReadonly<UiGraphSettings>): boolean => (
  UI_PROPERTIES_BY_GROUP[group].some(p => settings[p] !== undefined)
)

export {
  addUiGroup,
  removeUiGroup,
  isUiGroupPresent,
}
