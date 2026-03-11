import {v4 as uuid} from 'uuid'
import {useTranslation} from 'react-i18next'
import React from 'react'

import {isPrefab, isPrefabInstance, isScopedObjectId} from '@ecs/shared/object-hierarchy'

import {useSceneContext} from './scene-context'
import {duplicateObjects, canDuplicateObjects} from './configuration/duplicate-object'
import {clearPrefab, deleteObjects} from './configuration/delete-object'
import {copyObjects, pasteObjects} from './configuration/copy-object'
import {useStudioStateContext} from './studio-state-context'
import {MenuOptions, type MenuOption} from './ui/option-menu'
import {
  findPrefabDependencyCycle, makePrefabAndReplaceWithInstance, unlinkPrefabInstance,
} from './configuration/prefab'
import {createIdSeed} from './id-generation'
import {useNewPrimitiveOptions} from './new-primitive-button'
import {sceneInterfaceContext} from './scene-interface-context'
import {enterPrefabEditor} from './prefab-editor'
import {handleDeletePrefab} from './configuration/studio-prefab'
import {selectAllObjects} from './hooks/selected-objects'
import {useDerivedScene} from './derived-scene-context'
import {useActiveRoot} from '../hooks/use-active-root'

interface ITreeElementMenuOptions {
  id: string
  collapse: () => void
  excludeRename?: boolean
}

const TreeElementMenuOptions: React.FC<ITreeElementMenuOptions> = ({
  id, collapse, excludeRename = false,
}) => {
  const {t} = useTranslation(['cloud-studio-pages', 'common'])

  const ctx = useSceneContext()
  const stateCtx = useStudioStateContext()
  const derivedScene = useDerivedScene()
  const sceneInterfaceCtx = React.useContext(sceneInterfaceContext)
  const activeRoot = useActiveRoot()
  const object = ctx.scene.objects[id]

  const copiedObjects = stateCtx.state.clipboard.type === 'objects'
    ? stateCtx.state.clipboard.objects
    : []

  const isInSelection = stateCtx.state.selectedIds.includes(id)
  const objectIds = isInSelection ? stateCtx.state.selectedIds : [id]

  const showError = (message: string) => {
    stateCtx.update(p => ({...p, errorMsg: message}))
  }

  const newObjectOptions = useNewPrimitiveOptions(id)
  const {selectedPrefab} = stateCtx.state

  const parentPrefab = id ? derivedScene.getParentPrefabId(id) : undefined
  const isPartOfPrefab = !!parentPrefab
  const isPartOfInstance = id && isScopedObjectId(id)
  const targetIsPrefab = object && isPrefab(object)

  const canBecomePrefab = object && !targetIsPrefab &&
    !isPartOfPrefab && !isPrefabInstance(object)
  const canDuplicate = object && !isPartOfInstance
  const canCopy = object && !targetIsPrefab

  const pasteAllowed = () => {
    const clipBoardContainsInstances = Object.values(copiedObjects).some(o => isPrefabInstance(o))
    const isNestedPaste = ((targetIsPrefab || isPartOfPrefab) && clipBoardContainsInstances)
    if (!isNestedPaste) {
      return true
    }
    return BuildIf.NESTED_PREFABS_20250625 && copiedObjects.every(o => !findPrefabDependencyCycle(
      ctx.scene, derivedScene, o.id, object.id
    ))
  }

  const canPaste = !!object && pasteAllowed()

  const canOpenPrefab = ((object && targetIsPrefab) || isPartOfPrefab)
  const isMakePrefabDisabled = canBecomePrefab && derivedScene.hasInstanceDescendant(id)

  const canUnlink = object && isPrefabInstance(object)
  const canClearPrefab = object && (parentPrefab === object.id)

  const options: MenuOption[] = [
    {
      type: 'menu',
      content: t('tree_element_context_menu.button.new_object'),
      options: newObjectOptions,
    },
    !excludeRename && {
      content: t('tree_element_context_menu.button.rename'),
      onClick: () => ctx.setIsRenaming(id, true),
    },
    canDuplicate && {
      content: t('tree_element_context_menu.button.duplicate'),
      onClick: () => {
        const seed = createIdSeed()
        const newSelectedIds = objectIds.map(seed.fromId)
        ctx.updateScene(scene => duplicateObjects({
          scene,
          derivedScene,
          objectIds,
          ids: seed,
          onObjectsSkipped: () => showError(t('tree_hierarchy.error.skip_duplicate_location')),
        }))
        stateCtx.setSelection(...newSelectedIds)
      },
      isDisabled: !canDuplicateObjects(derivedScene, objectIds),
    },
    canCopy && {
      content: t('button.copy', {ns: 'common'}),
      onClick: () => {
        copyObjects(stateCtx, ctx.scene, derivedScene, objectIds)
      },
    },
    canPaste && {
      content: t('button.paste', {ns: 'common'}),
      onClick: () => {
        if (stateCtx.state.clipboard.type === 'objects') {
          const {objects} = stateCtx.state.clipboard
          const newObjectIds = objects.map(() => uuid())
          ctx.updateScene(
            scene => pasteObjects(
              scene, objects, activeRoot.id, id, newObjectIds,
              () => showError(t('tree_hierarchy.error.skip_paste_location'))
            )
          )
          stateCtx.setSelection(...newObjectIds)
        }
      },
      isDisabled: stateCtx.state.clipboard.type !== 'objects',
    },
    {
      type: 'item',
      content: t('tree_hierarchy.context_menu.button.select_all'),
      onClick: () => selectAllObjects(derivedScene, stateCtx, [activeRoot.id], parentPrefab),
    },
    canBecomePrefab && {
      content: t('tree_element_context_menu.button.create_prefab'),
      onClick: () => {
        const seed = createIdSeed()
        ctx.updateScene(scene => makePrefabAndReplaceWithInstance(scene, id, seed))
      },
      isDisabled: objectIds.length !== 1 || isMakePrefabDisabled,
    },
    canOpenPrefab && {
      content: t('tree_element_context_menu.button.edit_prefab'),
      onClick: () => enterPrefabEditor(stateCtx, sceneInterfaceCtx, parentPrefab),
    },
    canUnlink && {
      content: t('tree_element_context_menu.button.unlink_instance'),
      onClick: () => {
        ctx.updateScene(scene => unlinkPrefabInstance(scene, id))
        stateCtx.setSelection(id)
      },
    },
    {
      content: t('button.delete', {ns: 'common'}),
      onClick: () => {
        if (object && isPrefab(object)) {
          handleDeletePrefab(ctx, stateCtx, id)
          return
        }
        ctx.updateScene(scene => deleteObjects(scene, objectIds))
      },
      isDisabled: selectedPrefab === id,
    },
    canClearPrefab && {
      content: t('tree_element_context_menu.button.clear_prefab'),
      onClick: () => {
        ctx.updateScene(scene => clearPrefab(scene, parentPrefab))
      },
    },
  ]

  return <MenuOptions options={options} collapse={collapse} />
}

export {
  TreeElementMenuOptions,
}
