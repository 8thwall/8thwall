import type {UiGraphSettings} from '@ecs/shared/scene-graph'

type UiGroup = 'layout' | 'text' | 'border' | 'background'

type UiPropertyName = keyof UiGraphSettings

export type {
  UiGroup,
  UiPropertyName,
}
