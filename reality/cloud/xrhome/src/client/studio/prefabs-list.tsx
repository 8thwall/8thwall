import React from 'react'

import {useTranslation} from 'react-i18next'

import {makePrefabAndReplaceWithInstance} from './configuration/prefab'

import {useSceneContext} from './scene-context'
import {TreeElement} from './tree-element'
import {createThemedStyles} from '../ui/theme'
import {OptionalFilters, SearchBar} from './ui/search-bar'
import {TREE_HIERARCHY_FILTERS, TreeHierarchySearchResults} from './tree-hierarchy-search-results'
import {NewPrimitiveButton} from './new-primitive-button'
import {combine} from '../common/styles'
import {useTreeHierarchyStyles} from './ui/tree-hierarchy-styles'
import {DropTarget} from '../editor/drop-target'
import {createIdSeed} from './id-generation'
import {useDerivedScene} from './derived-scene-context'

const useStyles = createThemedStyles(theme => ({
  prefabContainer: {
    padding: '0rem 0.5rem',
    display: 'flex',
    flexDirection: 'column',
    gap: '0.25rem',
  },
  prefabInner: {
    'border': `${theme.studioSectionBorder}`,
    'borderRadius': '0.5rem',
    '--tree-element-border-radius': 'calc(0.5rem - 1px)',
  },
  searchBarContainer: {
    'padding': '0 0.75rem 0.5rem',
    'display': 'flex',
    'gap': '0.5rem',
    '& > div': {
      minWidth: 0,
    },
  },
}))

const PrefabsList: React.FC = () => {
  const ctx = useSceneContext()
  const derivedScene = useDerivedScene()
  const prefabs = derivedScene.getPrefabs()
  const classes = useStyles()
  const treeHierarchyClasses = useTreeHierarchyStyles()
  const {t} = useTranslation('cloud-studio-pages')

  const [hovering, setHovering] = React.useState(false)

  const [searchText, setSearchText] = React.useState('')
  const [filters, setFilters] = React.useState([])
  const searching = searchText.length > 0 || filters.length > 0

  return (
    <DropTarget
      as='div'
      className={combine(
        treeHierarchyClasses.treeHierarchy, hovering && treeHierarchyClasses.hovered
      )}
      onHoverStart={() => setHovering(true)}
      onHoverStop={() => setHovering(false)}
      onDrop={(e: React.DragEvent) => {
        e.preventDefault()
        setHovering(false)
        const objectId = e.dataTransfer.getData('objectId')

        if (!objectId) {
          return
        }

        if (
          derivedScene.hasInstanceDescendant(objectId) ||
          derivedScene.getParentPrefabId(objectId)
        ) {
          return
        }

        const seed = createIdSeed()
        ctx.updateScene(scene => makePrefabAndReplaceWithInstance(scene, objectId, seed))
      }}
    >
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
          a8='click;studio-bottom-left-pane;prefab-list-click'
        />
        <NewPrimitiveButton makePrefab />
      </div>
      {searching
        ? (
          <TreeHierarchySearchResults
            searchText={searchText}
            filters={filters}
            resetSearch={() => {
              setSearchText('')
              setFilters([])
            }}
            prefabList
          />
        )
        : (
          <div className={classes.prefabContainer}>
            {prefabs.map(prefab => (
              <div key={prefab.id} className={classes.prefabInner}>
                <TreeElement id={prefab.id} startCollapsed />
              </div>
            ))}
          </div>
        )}
    </DropTarget>
  )
}

export {
  PrefabsList,
}
