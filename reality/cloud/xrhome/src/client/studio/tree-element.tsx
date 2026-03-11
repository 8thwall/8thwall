import React from 'react'
import {useTranslation} from 'react-i18next'
import {IgnoreKeys} from 'react-hotkeys'

import {isPrefab} from '@ecs/shared/object-hierarchy'

import {useSceneContext} from './scene-context'
import {displayNameForObject} from './display'
import {DropTarget} from './ui/drop-target'
import {combine} from '../common/styles'
import {Icon} from '../ui/components/icon'
import '../static/styles/index.scss'
import {ElementTypeIcon} from './ui/element-type-icon'
import {useStudioStateContext} from './studio-state-context'
import InlineTextInput from '../common/inline-text-input'
import {sceneInterfaceContext} from './scene-interface-context'
import {useObjectSelection} from './hooks/selected-objects'
import {TreeElementMenuOptions} from './tree-element-menu-options'
import {TreeHierarchyDivider} from './tree-hierarchy-divider'
import {
  useTreeElementStyles, IndentTreeElementCssProperties,
  getTreeElementA8Tag,
  useTreeElementStatusClass,
} from './ui/tree-element-styles'
import {ContextMenu, useContextMenuState} from './ui/context-menu'
import {FloatingTrayCheckboxField} from '../ui/components/floating-tray-checkbox-field'
import {IconButton} from '../ui/components/icon-button'
import {InvalidImageTargetTooltip} from './invalid-image-target-tooltip'
import {handleDrop} from './tree-drop-targets'
import {enterPrefabEditor} from './prefab-editor'
import {prefabNameExists} from './common/studio-files'
import {isOverriddenInstance} from './configuration/prefab'
import {Popup} from '../ui/components/popup'
import {useDerivedScene} from './derived-scene-context'
import {ObjectChip} from './configuration/diff-chip'

const transparent = document.createElement('img')
transparent.src = 'data:image/gif;base64,R0lGODlhAQABAIAAAAAAAP///yH5BAEAAAAALAAAAAABAAEAAAIBRAA7'

interface ITreeElement {
  id: string
  level?: number
  hideChildren?: boolean
  startCollapsed?: boolean
}

const TreeElement: React.FC<ITreeElement> = ({
  id, level = 0, hideChildren = false, startCollapsed = false,
}) => {
  const classes = useTreeElementStyles()
  const {t} = useTranslation(['cloud-studio-pages'])
  const [hovering, setHovering] = React.useState(false)
  const [collapsed, setCollapsed] = React.useState(startCollapsed)
  const selectedRef = React.useRef<HTMLDivElement>(null)

  const [newName, setNewName] = React.useState('')

  const ctx = useSceneContext()
  const stateCtx = useStudioStateContext()
  const derivedScene = useDerivedScene()
  const object = derivedScene.getObject(id)

  const statusClass = useTreeElementStatusClass(id)

  const selection = useObjectSelection(id)
  const sceneInterfaceCtx = React.useContext(sceneInterfaceContext)

  const childIds = derivedScene.getChildren(id)
  const hasChildren = childIds.length > 0

  React.useEffect(() => {
    if (selection.isSelected && selectedRef.current) {
      selectedRef.current.scrollIntoView({block: 'nearest'})
    }
  }, [selection.isSelected])

  const cancelRename = () => {
    ctx.setIsRenaming(id, false)
  }

  const handleRenameSubmit = () => {
    ctx.setIsRenaming(id, false)
    if (isPrefab(object) && prefabNameExists(derivedScene, newName)) {
      return
    }
    if (newName) {
      ctx.updateObject(object.id, o => ({...o, name: newName}))
    }
  }

  const menuState = useContextMenuState()

  const collapseContextMenu = () => {
    menuState.setContextMenuOpen(false)
  }

  const chevronButton = (
    <button
      type='button'
      onClick={(e) => {
        e.stopPropagation()
        collapseContextMenu()
        setCollapsed(!collapsed)
      }}
      className={combine('style-reset', classes.chevron, collapsed && classes.collapsedChevron)}
    >
      <Icon stroke='chevronDown' />
    </button>
  )

  const text = (
    <div className={combine(
      classes.treeElementButton,
      selection.isSelected && classes.selectedButton,
      menuState.contextMenuOpen && classes.activeButton
    )}
    >
      <div
        className={combine(classes.treeElementNameRow, statusClass)}
        style={{'--indent-level': level} as IndentTreeElementCssProperties}
        ref={menuState.refs.setReference}
        {...menuState.getReferenceProps()}
      >
        <ObjectChip objectId={id} />
        <button
          a8={getTreeElementA8Tag(object)}
          type='button'
          className={combine('style-reset', classes.treeElementBtnText)}
          draggable={!hideChildren}
          onClick={(e) => {
            selection.onClick(e.ctrlKey || e.metaKey, e.shiftKey)
            collapseContextMenu()
          }}
          onDragStart={(e) => {
            e.dataTransfer.setData('objectId', id)
            e.dataTransfer.setDragImage(transparent, 0, 0)
          }}
          onContextMenu={menuState.handleContextMenu}
          onDoubleClick={() => {
            const prefabId = derivedScene.getParentPrefabId(id)
            if (prefabId && stateCtx.state.selectedPrefab !== prefabId) {
              enterPrefabEditor(stateCtx, sceneInterfaceCtx, prefabId)
            } else {
              sceneInterfaceCtx.focusObject(id)
            }
          }}
        >
          <span className={
            // We still want to be able to use level for spacing, but if no level provided, then
            // use default spacing for hideChildren.
            combine(classes.displayName, (level === 0 && hideChildren) && classes.hideChildren)
          }
          >
            <div className={classes.typeIcon}>
              <ElementTypeIcon id={id} />
              {isOverriddenInstance(ctx.scene, id) &&
                <div className={classes.overrideIcon}>
                  <Popup
                    content={t('tree_element.icon.instance_overrides')}
                    position='top'
                    alignment='left'
                    size='tiny'
                    delay={250}
                  >
                    <Icon stroke='filledCircle6' />
                  </Popup>
                </div>
              }
            </div>
            {ctx.isRenamingById[id]
              ? (
                <IgnoreKeys>
                  <InlineTextInput
                    value={newName}
                    onChange={e => setNewName(e.target.value)}
                    onCancel={cancelRename}
                    onSubmit={handleRenameSubmit}
                    inputClassName={combine('style-reset', classes.renaming)}
                    aria-label={t('tree_element.input.set_object_name')}
                  />
                </IgnoreKeys>
              )
              : (displayNameForObject(object))
            }

          </span>
        </button>
        {hasChildren && !hideChildren && chevronButton}
        <div className={classes.treeElementIconRow}>
          <div className={classes.treeElementIcon}>
            <FloatingTrayCheckboxField
              label={t('tree_element.button.enable_object')}
              id={`object-disabled-toggle-${id}`}
              checked={!object.disabled}
              onChange={() => {
                collapseContextMenu()
                ctx.updateObject(object.id, o => ({...o, disabled: o.disabled ? undefined : true}))
              }}
              sr
            />
          </div>
          <div className={combine(classes.treeElementIcon, object.hidden && classes.visibleIcon)}>
            <IconButton
              text={object.hidden
                ? t('tree_element.button.show_element')
                : t('tree_element.button.hide_element')}
              onClick={() => {
                collapseContextMenu()
                ctx.updateObject(object.id, o => ({...o, hidden: !o.hidden}))
              }}
              stroke={object.hidden ? 'eyeClosed' : 'eyeOpen'}
            />
          </div>
          {BuildIf.STUDIO_IMAGE_TARGETS_20260210 && object.imageTarget &&
            <InvalidImageTargetTooltip imageTarget={object.imageTarget}>
              <div className={combine(classes.treeElementIcon, classes.visibleIcon)}>
                <Icon stroke='warning' color='warning' />
              </div>
            </InvalidImageTargetTooltip>
          }
        </div>
      </div>
      <ContextMenu
        menuState={menuState}
        options={collapse => <TreeElementMenuOptions id={id} collapse={collapse} />}
      />
    </div>
  )

  if (hideChildren) {
    return text
  }

  return (
    <DropTarget
      as='div'
      className={combine(classes.treeElement, hovering && classes.hovered)}
      onHoverStart={() => setHovering(true)}
      onHoverStop={() => setHovering(false)}
      onDrop={(e) => {
        handleDrop(e, ctx, stateCtx, derivedScene, setHovering, id)
      }}
    >
      <div
        ref={selectedRef}
      >
        {hasChildren
          ? (
            <div className={combine(
              classes.treeElementGroup,
              selection.isSelected && classes.selectedGroup,
              (object.hidden || object.disabled) && classes.hidden
            )}
            >
              {text}
              {!collapsed && (
                <div className={classes.treeElementChildren}>
                  {childIds.map(e => (
                    <div key={e}>
                      <TreeHierarchyDivider parentId={id} siblingId={e} level={level + 1} />
                      <TreeElement id={e} level={level + 1} />
                    </div>
                  ))}
                  <TreeHierarchyDivider parentId={id} siblingId={undefined} level={level + 1} />
                </div>
              )}
            </div>)
          : text
        }
      </div>
    </DropTarget>
  )
}

export {
  TreeElement,
}
