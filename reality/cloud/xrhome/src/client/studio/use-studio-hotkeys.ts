import {useHotkeys} from 'react-hotkeys-hook'
import {v4 as uuid} from 'uuid'

import {useTranslation} from 'react-i18next'

import {isPrefab} from '@ecs/shared/object-hierarchy'

import {useSceneContext} from './scene-context'
import {TransformToolTypes, useStudioStateContext} from './studio-state-context'
import {deleteObjects} from './configuration/delete-object'
import {duplicateObjects} from './configuration/duplicate-object'
import {copyObjects, pasteObjects} from './configuration/copy-object'
import {selectAllObjects, useSelectedTarget} from './hooks/selected-objects'
import {canPasteComponent, pasteComponent} from './configuration/copy-component'
import {getActiveSpaceFromCtx} from './hooks/active-space'
import {createIdSeed} from './id-generation'
import {handleDeletePrefab} from './configuration/studio-prefab'
import {useDerivedScene} from './derived-scene-context'
import {useActiveRoot} from '../hooks/use-active-root'
import {unlinkPrefabInstance} from './configuration/prefab'

const useSceneHotKeys = (
  handleFocusObject: (id: string) => void
) => {
  const stateCtx = useStudioStateContext()
  const ctx = useSceneContext()
  const derivedScene = useDerivedScene()
  const {t} = useTranslation(['cloud-studio-pages'])
  const spaceId = getActiveSpaceFromCtx(ctx, stateCtx)?.id

  const targetId = useSelectedTarget()
  const activeRoot = useActiveRoot()

  const object = ctx.scene.objects[targetId]

  const showError = (message: string) => {
    stateCtx.update(p => ({...p, errorMsg: message}))
  }

  useHotkeys('f', () => handleFocusObject(targetId))
  useHotkeys('w',
    () => stateCtx.update(p => ({...p, transformMode: TransformToolTypes.TRANSLATE_TOOL})))
  useHotkeys('e',
    () => stateCtx.update(p => ({...p, transformMode: TransformToolTypes.ROTATE_TOOL})))
  useHotkeys('r',
    () => stateCtx.update(p => ({...p, transformMode: TransformToolTypes.SCALE_TOOL})))
  useHotkeys('mod+z', (e: KeyboardEvent) => {
    e.preventDefault()
    ctx.undo()
  })
  useHotkeys('mod+y, mod+shift+z', (e: KeyboardEvent) => {
    e.preventDefault()
    ctx.redo()
  })
  useHotkeys('mod+backslash', () => {
    stateCtx.update(p => ({...p, showUILayer: !p.showUILayer}))
  })
  useHotkeys('backspace, delete', () => {
    if (object && isPrefab(object)) {
      handleDeletePrefab(ctx, stateCtx, targetId)
      return
    }
    ctx.updateScene(scene => deleteObjects(scene, stateCtx.state.selectedIds))
  })
  useHotkeys('mod+d', (e: KeyboardEvent) => {
    e.preventDefault()
    const seed = createIdSeed()
    const newObjectIds = stateCtx.state.selectedIds.map(seed.fromId)
    ctx.updateScene(scene => duplicateObjects({
      scene,
      derivedScene,
      objectIds: stateCtx.state.selectedIds,
      ids: seed,
      onObjectsSkipped: () => showError(t('tree_hierarchy.error.skip_duplicate_location')),
    }))
    stateCtx.setSelection(...newObjectIds)
  })
  useHotkeys('mod+c', (e: KeyboardEvent) => {
    if (document.getSelection()?.toString()) {
      return
    }
    e.preventDefault()
    if (stateCtx.state.selectedIds.length > 0) {
      copyObjects(stateCtx, ctx.scene, derivedScene, stateCtx.state.selectedIds)
    }
  })
  useHotkeys('mod+v', (e: KeyboardEvent) => {
    e.preventDefault()
    if (stateCtx.state.clipboard.type === 'objects') {
      const {objects} = stateCtx.state.clipboard
      const newObjectIds = objects.map(() => uuid())
      ctx.updateScene(
        scene => pasteObjects(
          scene, objects, activeRoot?.id, targetId, newObjectIds,
          () => showError(t('tree_hierarchy.error.skip_paste_location'))
        )
      )
      stateCtx.setSelection(...newObjectIds)
    } else if (canPasteComponent(stateCtx, derivedScene, stateCtx.state.selectedIds)) {
      pasteComponent(stateCtx, ctx, stateCtx.state.selectedIds)
    }
  })
  useHotkeys('esc', (e: KeyboardEvent) => {
    e.preventDefault()
    stateCtx.setSelection()
    stateCtx.update(p => ({
      ...p,
      selectedAsset: null,
      selectedImageTarget: undefined,
      selectedFiles: [],
      lastSelectedFileIndex: null,
    }))
    if (stateCtx.state.selectedPrefab) {
      stateCtx.update(p => ({...p, selectedPrefab: undefined}))
    }
  })
  useHotkeys('mod+a', (e: KeyboardEvent) => {
    e.preventDefault()
    const parentPrefab = derivedScene.getParentPrefabId(object?.id)
    let spaces: string[]

    if (!stateCtx.state.selectedIds.length) {
      spaces = derivedScene.getAllIncludedSpaces(spaceId)
    } else {
      const spaceSet = new Set<string>()
      stateCtx.state.selectedIds.forEach((id) => {
        const space = derivedScene.resolveSpaceForObject(id)
        spaceSet.add(space?.id || '')
      })
      spaces = Array.from(spaceSet)
    }

    selectAllObjects(derivedScene, stateCtx, spaces, parentPrefab)
  })
  useHotkeys('mod+u', (e: KeyboardEvent) => {
    e.preventDefault()

    ctx.updateScene((oldScene) => {
      let newScene = {...oldScene}

      stateCtx.state.selectedIds.forEach((id) => {
        newScene = unlinkPrefabInstance(newScene, id)
      })

      return newScene
    })
  })
}

export {
  useSceneHotKeys,
}
