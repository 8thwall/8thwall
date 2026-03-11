import type {GraphComponent, GraphObject, Space} from '@ecs/shared/scene-graph'
import type {TFunction} from 'i18next'
import React from 'react'
import {useTranslation} from 'react-i18next'
import type {DeepReadonly} from 'ts-essentials'

import {hexColorWithAlpha} from '../../shared/colors'
import {createThemedStyles} from '../ui/theme'
import {ALL_NEW_COMPONENT_OPTIONS} from './configuration/new-component-strings'
import {useActiveSpace} from './hooks/active-space'
import {DIRECT_PROPERTY_COMPONENTS} from './hooks/available-components'
import {useSceneContext} from './scene-context'
import {StudioStateContext, useStudioStateContext} from './studio-state-context'
import {TreeElement} from './tree-element'
import {SearchToolbar} from './ui/search-bar'
import {useDerivedScene} from './derived-scene-context'
import type {DerivedScene} from './derive-scene'

const SPACELESS_PLACEHOLDER_ID = 'spaceless-placeholder-id'

const useStyles = createThemedStyles(theme => ({
  elementIcon: {
    display: 'flex',
    alignItems: 'center',
    color: theme.fgMuted,
  },
  resultsContainer: {
    display: 'flex',
    flexDirection: 'column',
    overflowY: 'auto',
    flexGrow: 1,
  },
  selected: {
    background: theme.studioTreeHoverBg,
  },
  resultEntry: {
    'padding': '0.25rem 0.75rem',
    'display': 'flex',
    'alignItems': 'center',
    'gap': '0.25rem',
    'borderStyle': 'solid',
    'borderColor': 'transparent',
    'borderWidth': '1px 0 1px 0',
    '&:hover': {
      background: theme.studioTreeHoverBg,
      borderStyle: 'solid',
      borderColor: hexColorWithAlpha(theme.sfcBorderFocus, 0.5),
      borderWidth: '1px 0 1px 0',
    },
  },
  componentName: {
    'margin': '0.5rem 1.5rem',
    'color': theme.studioTreeHidden,
    'userSelect': 'none',
  },
  entityTypeHeader: {
    userSelect: 'none',
    padding: '0.25rem 0.5rem',
    color: theme.fgMain,
    borderBottom: theme.studioSectionBorder,
    fontWeight: 'bold',
    fontSize: '14px',
  },
  componentUsersContainer: {
    padding: '0.25rem 0rem',
  },
  spaceHeader: {
    'padding': '0.25rem 0.5rem',
    'display': 'flex',
    'justifyContent': 'space-between',
    'userSelect': 'none',
    'color': theme.fgMain,
    'flex': 1,
    'gap': '0.5em',
    'alignItems': 'center',
    'fontSize': '12px',
    'border': theme.studioSectionBorder,
    'borderRadius': '0.5rem',
  },
  spaceResultsContainer: {
    display: 'flex',
    flexDirection: 'column',
    paddingBottom: '0.5rem',
  },
}))

const TREE_HIERARCHY_FILTERS = [
  {
    type: 'geometry',
    label: 'tree_hierarchy_search_results.option.geometry',
    isType: (object: DeepReadonly<GraphObject>) => object?.geometry || object?.gltfModel,
  },
  {
    type: 'audio',
    label: 'tree_hierarchy_search_results.option.audio',
    isType: (object: DeepReadonly<GraphObject>) => object?.audio,
  },
  {
    type: 'camera',
    label: 'tree_hierarchy_search_results.option.camera',
    isType: (object: DeepReadonly<GraphObject>) => object?.camera,
  },
  {
    type: 'light',
    label: 'tree_hierarchy_search_results.option.light',
    isType: (object: DeepReadonly<GraphObject>) => object?.light,
  },
  {
    type: 'other',
    label: 'tree_hierarchy_search_results.option.other',
    isType: (object: DeepReadonly<GraphObject>) => !(object?.geometry || object?.gltfModel) &&
      !object?.audio && !object?.light,
  },
]

// Checks if an object is valid to appear in search given current state
// E.g. if user is filtering out all mesh objects, mesh objects are not
// searchable.
const isSearchableObject = (
  object: DeepReadonly<GraphObject>,
  filters: string[],
  stateCtx: StudioStateContext,
  derivedScene: DerivedScene,
  activeSpace: DeepReadonly<Space>,
  prefabList: boolean
) => {
  const typeFilters = TREE_HIERARCHY_FILTERS.filter(({type}) => filters.includes(type))
  const allIncludedSpaces = derivedScene.getAllIncludedSpaces(activeSpace?.id)

  const objectSpaceId = derivedScene.resolveSpaceForObject(object.id)?.id
  const parentPrefab = derivedScene.getParentPrefabId(object.id)

  if (prefabList && !parentPrefab) {
    return false
  }

  if (!prefabList && parentPrefab !== stateCtx.state.selectedPrefab) {
    return false
  }

  if (
    !prefabList && activeSpace && objectSpaceId && !allIncludedSpaces!.includes(objectSpaceId)
  ) {
    return false
  }

  if (filters.length === 0) {
    return true
  }

  if (typeFilters.length > 0) {
    return typeFilters.some(({isType}) => isType(object))
  }

  return false
}

const getAllComponentNames = (
  object: DeepReadonly<GraphObject>,
  t: TFunction
) => {
  const customs = Object.values(object.components).map(component => component.name)
  const includedDirects = DIRECT_PROPERTY_COMPONENTS.filter(
    directComponentName => !!object[directComponentName]
  )

  // Swapping over object field names and internal object names to their display names
  // if we don't have a display name, they must be a user defined object, so we
  // can keep the existing name.
  const localized = [...customs, ...includedDirects].map(
    (internalName) => {
      const foundInternal = ALL_NEW_COMPONENT_OPTIONS.find(option => option.value === internalName)
      if (!foundInternal) {
        return internalName
      }
      return foundInternal.ns
        ? t(foundInternal.content, {ns: foundInternal.ns})
        : foundInternal.content
    }
  )

  return localized
}

const useComponentSearchResults = (
  searchText: string,
  filters: string[],
  t: TFunction,
  prefabList: boolean
  // Components may be used in different spaces but parent objects are partitioned between them.
): Record<Space['id'], Record<GraphComponent['name'], GraphObject['id'][]>> => {
  const stateCtx = useStudioStateContext()
  const activeSpace = useActiveSpace()
  const derivedScene = useDerivedScene()

  const spaceResults: Record<Space['id'], Record<GraphComponent['name'], GraphObject['id'][]>> = {}

  derivedScene.getAllSceneObjects().forEach((object) => {
    const componentNames = getAllComponentNames(object, t)
    const searchFitNames = componentNames.filter(
      name => name.toLowerCase().includes(searchText.toLowerCase())
    )

    // Ignore filtered out objects
    if (isSearchableObject(object, filters, stateCtx, derivedScene, activeSpace, prefabList)) {
      const spaceId = derivedScene.resolveSpaceForObject(object.id)?.id || SPACELESS_PLACEHOLDER_ID
      searchFitNames.forEach((componentName) => {
        if (!spaceResults[spaceId]) {
          spaceResults[spaceId] = {}
        }

        if (!spaceResults[spaceId][componentName]) {
          spaceResults[spaceId][componentName] = []
        }

        spaceResults[spaceId][componentName].push(object.id)
      })
    }
  })

  return spaceResults
}

const useObjectSearchResults = (searchText: string, filters: string[], prefabList: boolean) => {
  // common section
  const stateCtx = useStudioStateContext()
  const activeSpace = useActiveSpace()
  const derivedScene = useDerivedScene()

  return derivedScene.getAllSceneObjects().filter((object) => {
    if (isSearchableObject(object, filters, stateCtx, derivedScene, activeSpace, prefabList)) {
      return object?.name.toLowerCase().includes(searchText.toLowerCase())
    }
    return false
  }).sort((aObject, bObject) => {
    if (aObject?.name < bObject?.name) {
      return -1
    }
    if (aObject?.name > bObject?.name) {
      return 1
    }
    return 0
  }).reduce((acc, object) => {
    const e = object.id
    const key = derivedScene.resolveSpaceForObject(e)?.id || SPACELESS_PLACEHOLDER_ID
    if (!acc[key]) {
      acc[key] = []
    }
    acc[key].push(e)
    return acc
  }, {} as Record<string, string[]>)
}

interface ITreeHierarchySearchResults {
  searchText: string
  filters: string[]
  resetSearch: () => void
  // When true, searching only in the list of prefabs
  prefabList?: boolean
}

const TreeHierarchySearchResults: React.FC<ITreeHierarchySearchResults> = ({
  searchText, filters, resetSearch, prefabList,
}) => {
  const classes = useStyles()
  const ctx = useSceneContext()
  const activeSpace = useActiveSpace()
  const derivedScene = useDerivedScene()
  const includedSpacesCount = derivedScene.getAllIncludedSpaces(activeSpace?.id).length
  const {t} = useTranslation('cloud-studio-pages')

  const objectResults = useObjectSearchResults(searchText, filters, prefabList)
  const componentResults = useComponentSearchResults(searchText, filters, t, prefabList)

  const componentResultCount = Object.values(componentResults).reduce(
    (sum, obj) => sum + Object.values(obj).flat().length, 0
  )
  const objectResultCount = Object.values(objectResults).flat().length
  const resultCount = objectResultCount + componentResultCount

  return (
    <>
      <SearchToolbar
        resultCount={resultCount}
        clearSearch={resetSearch}
      />
      <div className={classes.resultsContainer}>
        {Object.keys(objectResults).length > 0 &&
          <span className={classes.entityTypeHeader}>
            {t('tree_hierarchy_search_results.entity_type.objects')}
          </span>
        }
        {Object.entries(objectResults).map(([spaceId, objectIds]) => (
          <div key={spaceId} className={classes.spaceResultsContainer}>
            {spaceId !== SPACELESS_PLACEHOLDER_ID && includedSpacesCount > 1 && (
              <span className={classes.spaceHeader}>{ctx.scene.spaces[spaceId].name}</span>
            )}
            {objectIds.map(result => (
              <TreeElement key={result} id={result} hideChildren />
            ))}
          </div>
        ))}
        {Object.keys(componentResults).length > 0 &&
          <span className={classes.entityTypeHeader}>
            {t('tree_hierarchy_search_results.entity_type.components')}
          </span>
        }
        {Object.entries(componentResults).map(([spaceId, componentToObject]) => (
          <div key={spaceId} className={classes.spaceResultsContainer}>
            {spaceId !== SPACELESS_PLACEHOLDER_ID && includedSpacesCount > 1 && (
              <span className={classes.spaceHeader}>{ctx.scene.spaces[spaceId].name}</span>
            )}
            {
              Object.entries(componentToObject).map(([componentName, oIds]) => (
                <div key={componentName} className={classes.componentUsersContainer}>
                  <span key={componentName} className={classes.componentName}>
                    {componentName}
                  </span>
                  {oIds.map(oId => (
                    // Level provides indentation. 1 fits mockup indentation,
                    // since elements here are direct "children" of the component node.
                    <TreeElement key={oId} id={oId} level={1} hideChildren />
                  ))}
                </div>
              ))
            }
          </div>
        ))}
      </div>
    </>
  )
}

export {
  TREE_HIERARCHY_FILTERS,
  TreeHierarchySearchResults,
}
