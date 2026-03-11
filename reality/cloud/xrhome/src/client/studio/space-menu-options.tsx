import {useTranslation} from 'react-i18next'
import type {DeepReadonly} from 'ts-essentials'
import type {Space} from '@ecs/shared/scene-graph'

import type {MenuOption} from './ui/option-menu'
import {useSceneContext} from './scene-context'
import {createIdSeed} from './id-generation'
import {duplicateSpace} from './configuration/duplicate-space'
import {deleteSpace} from './configuration/delete-object'
import {useStudioStateContext} from './studio-state-context'

const useSpaceMenuOptions = (space: DeepReadonly<Space>, onRename: () => void): MenuOption[] => {
  const {t} = useTranslation(['cloud-studio-pages', 'common'])
  const ctx = useSceneContext()
  const stateCtx = useStudioStateContext()
  const spaces = ctx.scene.spaces ? Object.values(ctx.scene.spaces) : []

  if (!space) {
    return []
  }

  const handleDuplicateSpace = () => {
    const seed = createIdSeed()
    ctx.updateScene(oldScene => duplicateSpace(oldScene, space.id, seed))
  }

  const handleEntrySpace = () => {
    ctx.updateScene(s => ({...s, entrySpaceId: space.id}))
  }

  const isCurrentEntrySpace = ctx.scene.entrySpaceId === space.id
  const spacesMenuOptions: MenuOption[] = [
    {
      content: t('tree_element_context_menu.button.rename'),
      onClick: onRename,
    },
    {
      content: t('button.duplicate', {ns: 'common'}),
      onClick: handleDuplicateSpace,
    },
    {
      content: isCurrentEntrySpace
        ? t('tree_element_context_menu.button.current_entry_space')
        : t('tree_element_context_menu.button.set_entry_space'),
      onClick: handleEntrySpace,
      isDisabled: isCurrentEntrySpace,
    },
    {
      content: t('button.delete', {ns: 'common'}),
      onClick: spaces.length > 1
        ? () => {
          ctx.updateScene(oldScene => deleteSpace(oldScene, space.id))
        }
        : null,
      isDisabled: spaces.length <= 1,
    },
  ]

  if (BuildIf.SCENE_DIFF_20250730) {
    spacesMenuOptions.push(
      {
        content: t('scene_diff.view_diff'),
        onClick: () => stateCtx.update(p => ({...p, sceneDiffOpen: true})),
      }
    )
  }

  return spacesMenuOptions
}

export {
  useSpaceMenuOptions,
}
