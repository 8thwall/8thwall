import React from 'react'
import {useTranslation} from 'react-i18next'
import {v4 as uuid} from 'uuid'

import {getIncludedSpaces, isPrefabInstance} from '@ecs/shared/object-hierarchy'

import {useSceneContext} from './scene-context'
import {DropTarget} from './ui/drop-target'
import {combine} from '../common/styles'
import {TreeElement} from './tree-element'
import {useStudioStateContext} from './studio-state-context'
import {OptionalFilters, SearchBar} from './ui/search-bar'
import {TreeHierarchySearchResults, TREE_HIERARCHY_FILTERS} from './tree-hierarchy-search-results'
import {Icon} from '../ui/components/icon'
import {NewPrimitiveButton} from './new-primitive-button'
import {useActiveSpace} from './hooks/active-space'
import {TreeHierarchyDivider} from './tree-hierarchy-divider'
import {SpacesMenu} from './spaces-menu'
import {SpaceSectionCollapsible} from './space-section-collapsible'
import {Tooltip} from '../ui/components/tooltip'
import {IsolatedTreeHierarchy} from './isolated-tree-hierarchy'
import {handleDrop} from './tree-drop-targets'
import {useTreeHierarchyStyles} from './ui/tree-hierarchy-styles'
import {ContextMenu, useContextMenuState} from './ui/context-menu'
import {useNewPrimitiveOptions} from './new-primitive-button'
import type {MenuOption} from './ui/option-menu'
import {pasteObjects} from './configuration/copy-object'
import {useActiveRoot} from '../hooks/use-active-root'
import {useDerivedScene} from './derived-scene-context'
import {findPrefabDependencyCycle} from './configuration/prefab'

const TreeHierarchyDropTarget: React.FC<{spaceId?: string}> = ({
  spaceId,
}) => {
  const ctx = useSceneContext()
  const stateCtx = useStudioStateContext()
  const derivedScene = useDerivedScene()
  const classes = useTreeHierarchyStyles()
  const [hovering, setHovering] = React.useState(false)

  const topLevelObjectIds = derivedScene.getTopLevelObjectIds(spaceId)
  const activeSpace = useActiveSpace()
  const showSpacesMenu = !ctx.scene.spaces || spaceId === activeSpace?.id

  return (
    <DropTarget
      as='div'
      className={combine(
        classes.treeHierarchy, hovering && classes.hovered, classes.paddingTop
      )}
      onHoverStart={() => setHovering(true)}
      onHoverStop={() => setHovering(false)}
      onDrop={(e) => {
        handleDrop(e, ctx, stateCtx, derivedScene, setHovering, spaceId)
      }}
    >
      {showSpacesMenu && <SpacesMenu />}
      <div className={classes.paddingTop}>
        {topLevelObjectIds.map(e => (
          <div key={e}>
            <TreeHierarchyDivider parentId={spaceId} siblingId={e} level={0} />
            <TreeElement id={e} />
          </div>
        ))}
      </div>
      <TreeHierarchyDivider parentId={spaceId} siblingId={undefined} level={0} />
      <div
        tabIndex={-1}
        className={
          combine('style-reset', classes.treeHierarchyEmpty)
        }
        onClick={() => stateCtx.setSelection()}
        aria-hidden
      />
    </DropTarget>
  )
}

const MissingSpaceSection: React.FC<{}> = () => {
  const {t} = useTranslation('cloud-studio-pages')
  const derivedScene = useDerivedScene()
  const objectsToShow = derivedScene.getTopLevelObjectIds(undefined)

  if (objectsToShow.length === 0) {
    return null
  }

  return (
    <SpaceSectionCollapsible
      title={(
        <span>
          <Tooltip position='bottom-end' content={t('tree_hierarchy.missing_space.tooltip')}>
            <Icon stroke='warning' color='warning' inline />
          </Tooltip>
          <i>{t('tree_hierarchy.missing_space.title')}</i>
        </span>
      )}
      spaceId=''
    >
      {objectsToShow.map(e => (
        <TreeElement key={e} id={e} />
      ))}
    </SpaceSectionCollapsible>

  )
}

const TreeHierarchy: React.FC = () => {
  const {t} = useTranslation(['cloud-studio-pages', 'common'])
  const classes = useTreeHierarchyStyles()
  const [searchText, setSearchText] = React.useState('')
  const [filters, setFilters] = React.useState([])
  const searching = searchText.length > 0 || filters.length > 0
  const activeRoot = useActiveRoot()
  const ctx = useSceneContext()
  const stateCtx = useStudioStateContext()
  const isIsolated = !!activeRoot.prefab
  const includedSpaces = getIncludedSpaces(ctx.scene, activeRoot.space?.id)
  const activeSpace = activeRoot.space
  const currentSpaceId = activeRoot.id
  const primitiveOptions = useNewPrimitiveOptions(currentSpaceId)
  const menuState = useContextMenuState()
  const derivedScene = useDerivedScene()

  const copiedObjects = stateCtx.state.clipboard.type === 'objects'
    ? stateCtx.state.clipboard.objects
    : []

  const pasteAllowed = () => {
    const clipBoardContainsInstances = copiedObjects &&
    Object.values(copiedObjects).some(o => isPrefabInstance(o))
    const isNestedPaste = isIsolated && clipBoardContainsInstances
    if (!isNestedPaste) {
      return true
    }
    return BuildIf.NESTED_PREFABS_20250625 &&
      copiedObjects.every(o => !findPrefabDependencyCycle(
        ctx.scene, derivedScene, o.id, currentSpaceId
      ))
  }

  const canPaste = !!copiedObjects && pasteAllowed()

  const menuOptions: MenuOption[] = [
    {
      type: 'item',
      content: t('tree_hierarchy.context_menu.button.select_all'),
      onClick: () => {
        const topLevelObjectIds = derivedScene.getTopLevelObjectIds(currentSpaceId)
        stateCtx.setSelection(...topLevelObjectIds, currentSpaceId)
      },
    },
    canPaste && {
      content: t('button.paste', {ns: 'common'}),
      onClick: () => {
        if (stateCtx.state.clipboard.type === 'objects') {
          const newObjectIds = copiedObjects.map(() => uuid())
          ctx.updateScene(
            scene => pasteObjects(
              scene, copiedObjects, currentSpaceId, undefined, newObjectIds,
              () => stateCtx.update(p => (
                {...p, errorMsg: t('tree_hierarchy.error.skip_paste_location')}
              ))
            )
          )
          stateCtx.setSelection(...newObjectIds)
        }
      },
      isDisabled: stateCtx.state.clipboard.type !== 'objects',
    },
    {
      type: 'menu',
      content: t('tree_element_context_menu.button.new_object'),
      options: primitiveOptions,
    },
  ]

  const searchResults = (
    <TreeHierarchySearchResults
      searchText={searchText}
      filters={filters}
      resetSearch={() => {
        setSearchText('')
        setFilters([])
      }}
    />
  )

  const treeHierarchy = (
    <>
      <TreeHierarchyDropTarget
        spaceId={activeSpace?.id}
      />
      {activeSpace?.id &&
        <>
          {includedSpaces.map(spaceId => (
            <SpaceSectionCollapsible
              key={spaceId}
              title={ctx.scene.spaces[spaceId].name}
              spaceId={spaceId}
            >
              <TreeHierarchyDropTarget
                spaceId={spaceId}
              />
            </SpaceSectionCollapsible>
          ))}
          <MissingSpaceSection />
        </>
      }
    </>
  )

  return (
    <div className={classes.treeHierarchyContainer}>
      <div className={classes.searchBarContainer}>
        <SearchBar
          searchText={searchText}
          setSearchText={setSearchText}
          filterOptions={collapse => (
            <OptionalFilters
              options={TREE_HIERARCHY_FILTERS}
              noneOption={t('tree_hierarchy_search_results.option.all_objects')}
              filters={filters}
              setFilters={setFilters}
              collapse={collapse}
            />
          )}
          a8='click;studio-top-left-pane;tree-hierarchy-click'
        />
        <NewPrimitiveButton />
      </div>
      <div
        className={classes.scrollableContainer}
        onContextMenu={menuState.handleContextMenu}
        {...menuState.getReferenceProps()}
      >
        {!isIsolated && (searching ? searchResults : treeHierarchy)}
        {isIsolated && (searching ? searchResults : <IsolatedTreeHierarchy />)}
        <ContextMenu menuState={menuState} options={menuOptions} />
      </div>
    </div>
  )
}

export {
  TreeHierarchy,
}
