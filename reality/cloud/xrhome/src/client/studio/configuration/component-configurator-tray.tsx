import React from 'react'

import {useTranslation} from 'react-i18next'

import {isPrefabInstance, isScopedObjectId} from '@ecs/shared/object-hierarchy'

import {ComponentTooltip} from '../ui/component-tooltip'
import {CornerDropdownMenu} from '../ui/corner-dropdown-menu'
import {FloatingTrayCollapsible} from '../../ui/components/floating-tray-collapsible'
import {useSceneContext} from '../scene-context'
import {useStudioStateContext} from '../studio-state-context'
import {MenuOptions, type MenuOption} from '../ui/option-menu'
import {canPasteComponent, pasteComponent} from './copy-component'
import {useSelectedObjects} from '../hooks/selected-objects'
import {areComponentsOverridden, ComponentData} from './prefab'
import {useDerivedScene} from '../derived-scene-context'
import type {DefaultableConfigDiffInfo} from './diff-chip-types'
import type {ExpanseField} from './expanse-field-types'

interface IComponentConfiguratorTopBar {
  a8?: string
  children: React.ReactNode
  title: string
  description?: React.ReactNode
  sectionId: string
  componentData: ComponentData
  topContent?: React.ReactNode
  onEdit?: () => void
  onRemove?: () => void
  onCopy?: () => void
  onReset?: () => void
  onResetToPrefab?: () => void
  onApplyOverridesToPrefab?: () => void
  expanseField?: ExpanseField<DefaultableConfigDiffInfo<unknown, string[][]>>
}

const ComponentConfiguratorTray: React.FC<IComponentConfiguratorTopBar> = ({
  a8, children, title, description, sectionId, componentData, topContent, onEdit, onRemove, onCopy,
  onReset, onResetToPrefab, onApplyOverridesToPrefab, expanseField,
}) => {
  const {t} = useTranslation(['cloud-studio-pages'])

  const ctx = useSceneContext()
  const derivedScene = useDerivedScene()
  const stateCtx = useStudioStateContext()
  const selectedObjects = useSelectedObjects()

  const canPaste = canPasteComponent(stateCtx, derivedScene, stateCtx.state.selectedIds)
  const canReset = selectedObjects.every(obj => !obj.mapPoint)
  const canResetInstance = selectedObjects.every(
    obj => (ctx.scene.objects[obj.id] && isPrefabInstance(ctx.scene.objects[obj.id])) ||
    isScopedObjectId(obj.id)
  )

  const isOverridden = areComponentsOverridden(ctx.scene, stateCtx.state.selectedIds, componentData)

  const options: MenuOption[] = [
    onEdit && {
      content: t('component_context_menu.button.edit'),
      onClick: onEdit,
    },
    onEdit && {
      type: 'divider' as const,
    },
    onRemove && {
      content: t('component_context_menu.button.remove'),
      onClick: onRemove,
    },
    onCopy && {
      content: t('component_context_menu.button.copy'),
      onClick: onCopy,
    },
    {
      content: t('component_context_menu.button.paste'),
      onClick: () => pasteComponent(stateCtx, ctx, stateCtx.state.selectedIds),
      isDisabled: !canPaste,
    },
    onReset && {
      content: t('component_context_menu.button.reset'),
      onClick: onReset,
      isDisabled: !canReset,
    },
    onResetToPrefab && canResetInstance && {
      content: t('component_context_menu.button.reset_to_prefab'),
      onClick: onResetToPrefab,
    },
    onApplyOverridesToPrefab && canResetInstance && isOverridden && {
      content: t('component_context_menu.button.apply_overrides_to_prefab'),
      onClick: onApplyOverridesToPrefab,
    },
  ].filter(Boolean)

  const allTopContent = (
    <>
      {topContent}
      {description && <ComponentTooltip content={description} />}
      <CornerDropdownMenu>
        {collapse => (
          <MenuOptions options={options} collapse={collapse} />
        )}
      </CornerDropdownMenu>
    </>
  )

  return (
    <FloatingTrayCollapsible
      a8={a8}
      title={title}
      topContent={allTopContent}
      sectionId={sectionId}
      contextMenuOptions={options}
      overridden={isOverridden}
      expanseField={expanseField}
    >
      {children}
    </FloatingTrayCollapsible>
  )
}

export {
  ComponentConfiguratorTray,
}
