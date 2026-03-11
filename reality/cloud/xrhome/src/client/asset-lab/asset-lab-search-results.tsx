import React from 'react'

import {useSelector} from '../hooks'
import {useChangeEffect} from '../hooks/use-change-effect'
import {useAssetLabStateContext} from './asset-lab-context'

const ASSET_LAB_FILTERS = [
  {
    type: 'IMAGE',
    label: 'asset_lab.search_bar.option.image',
  },
  {
    type: 'MESH',
    label: 'asset_lab.search_bar.option.mesh',
  },
  {
    type: 'ANIMATION',
    label: 'asset_lab.search_bar.option.animation',
  },
]

const useUpdateAssetLabSearchResults = (
  searchText: string,
  filters: string[],
  ids: readonly string[]
): {loading: boolean, filterBy: string} => {
  const assetGenerations = useSelector(s => s.assetLab.assetGenerations)
  const assetRequests = useSelector(s => s.assetLab.assetRequests)
  const [loading, setLoading] = React.useState(false)
  const {setLibrarySearchResults} = useAssetLabStateContext()

  useChangeEffect(([prevIds, prevSearchText, prevFilters]) => {
    // Note(Dale): Potentially we could use a deep comparison here, but for now
    // we just check if the length of the ids has changed.
    if (prevIds && ids && prevIds.length === ids.length &&
        prevSearchText === searchText &&
        Array.isArray(prevFilters) &&
        prevFilters.length === filters.length &&
        prevFilters.every((filter, index) => filter === filters[index])
    ) {
      return
    }
    setLoading(true)
    React.startTransition(() => {
      const formattedSearchText = searchText.toLowerCase()
      const filteredIds = ids?.filter((generationId) => {
        const assetGeneration = assetGenerations[generationId]
        const assetRequest = assetRequests[assetGeneration?.RequestUuid]
        const requestType = assetRequest?.type || assetGeneration?.metadata?.type
        // TODO(coco): Add fuzzy search for prompt
        if (searchText && !assetGeneration?.prompt?.toLowerCase().includes(formattedSearchText)) {
          return false
        }
        if (filters.length === 0) {
          return true
        }
        return filters.some((type) => {
          if (type === 'ANIMATION') {
            return requestType === 'MESH_TO_ANIMATION'
          } else if (type === 'MESH') {
            return assetGeneration?.assetType === 'MESH' && requestType !== 'MESH_TO_ANIMATION'
          }
          return assetGeneration?.assetType === type
        })
      }) ?? []
      setLoading(false)
      setLibrarySearchResults(filteredIds)
    })
  }, [ids, searchText, filters])

  return {
    loading,
    filterBy: (ASSET_LAB_FILTERS.find(f => f.type === filters[0])?.label) ||
     'asset_lab.search_bar.filter_by_all',
  }
}

export {
  useUpdateAssetLabSearchResults,
  ASSET_LAB_FILTERS,
}
