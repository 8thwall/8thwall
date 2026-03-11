import type {
  UiGraphSettings, SceneGraph,
} from '@ecs/shared/scene-graph'

import type {DeepReadonly} from 'ts-essentials'

import {EIGHTH_WALL_ICON_URL, WHITE_GRAY_TILES_URL} from '../../shared/studio/cdn-assets'

import {makeEmptyObject} from './make-object'
import type {SceneContext} from './scene-context'
import type {StudioStateContext} from './studio-state-context'
import {makeObjectName} from './common/studio-files'
import type {DerivedScene} from './derive-scene'
import {getNextChildOrder} from './reparent-object'

type UiPresetType = 'frame' | 'button' | 'text' | 'image'

type UiPresetChild = {
  nameKey: string
  ui: UiGraphSettings
  position?: [number, number, number]
}

type UiPreset = {
  type: UiPresetType
  nameKey: string
  ui: UiGraphSettings
  children?: Array<UiPresetChild>
}

const UI_PRESETS: DeepReadonly<Array<UiPreset>> = [
  {
    type: 'frame',
    nameKey: 'new_primitive_button.option.ui.frame',
    ui: {
      width: 100,
      height: 100,
      type: '3d',
      background: '#ffffff',
      backgroundOpacity: 0.5,
    },
  },
  {
    type: 'text',
    nameKey: 'new_primitive_button.option.ui.text',
    ui: {
      width: 70,
      height: 10,
      // eslint-disable-next-line local-rules/hardcoded-copy
      text: 'Sample Text',
      color: '#ffffff',
      fontSize: 12,
      type: '3d',
    },
  },
  {
    type: 'button',
    nameKey: 'new_primitive_button.option.ui.button',
    ui: {
      type: '3d',
      width: 100,
      height: 36,
      background: '#ad50ff',
      borderRadius: 18,
      flexDirection: 'row',
      backgroundOpacity: 1,
      padding: '10',
      gap: '6',
      alignItems: 'center',
      justifyContent: 'center',
    },
    children: [
      {
        nameKey: 'new_primitive_button.option.ui.icon',
        ui: {
          width: 16,
          height: 16,
          image: {
            type: 'url',
            url: EIGHTH_WALL_ICON_URL,
          },
          backgroundOpacity: 1,
        },
        position: [-0.323, 0, 0],
      },
      {
        nameKey: 'new_primitive_button.option.ui.text',
        ui: {
          width: 50,
          height: 14,
          // eslint-disable-next-line local-rules/hardcoded-copy
          text: 'Button',
          color: '#ffffff',
          fontSize: 16,
        },
      },
    ],
  },
  {
    type: 'image',
    nameKey: 'new_primitive_button.option.ui.image',
    ui: {
      type: '3d',
      width: 100,
      height: 100,
      image: {
        type: 'url',
        url: WHITE_GRAY_TILES_URL,
      },
      backgroundOpacity: 1,
    },
  },
  // TODO(chloe): Add video here as preset later
]

const spawnUiPreset = (
  ctx: SceneContext,
  stateCtx: StudioStateContext,
  derivedScene: DerivedScene,
  parentId: string,
  type: UiPresetType,
  i18nTranslator: (key: string) => string
): DeepReadonly<SceneGraph> => {
  const preset = UI_PRESETS.find(p => p.type === type)
  if (!preset) {
    return
  }
  const spaceId = derivedScene.resolveSpaceForObject(parentId)?.id

  const newObject = makeEmptyObject(parentId)
  newObject.ui = preset.ui
  newObject.name = makeObjectName(i18nTranslator(preset.nameKey), ctx.scene, spaceId)

  const newObjects = [newObject]

  if (preset.children) {
    preset.children.forEach((presetChild, i) => {
      const child = makeEmptyObject(newObject.id)
      child.name = makeObjectName(i18nTranslator(presetChild.nameKey), ctx.scene, spaceId)
      child.ui = presetChild.ui
      child.order = i + Math.random() / 2
      if (presetChild.position) {
        child.position = [...presetChild.position]
      }
      newObjects.push(child)
    })
  }

  ctx.updateScene((oldScene) => {
    const newScene = {
      ...oldScene,
      objects: {...oldScene.objects},
    }

    newObjects.forEach((o) => {
      newScene.objects[o.id] = {
        ...o,
        order: o.order ?? getNextChildOrder(newScene, o.parentId),
      }
    })

    return newScene
  })

  stateCtx.setSelection(newObject.id)
}

export {
  spawnUiPreset,
  UI_PRESETS,
}

export type {
  UiPresetType,
  UiPresetChild,
  UiPreset,
}
