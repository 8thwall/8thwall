import React from 'react'
import {useTranslation} from 'react-i18next'

import {GridOfSquares} from './widgets/grid-of-squares'
import {OptionalFilters, SearchBar, SearchToolbar} from '../studio/ui/search-bar'
import {createThemedStyles} from '../ui/theme'
import {useCurrentAccountAssetGenerations} from './hooks'
import {ASSET_LAB_FILTERS, useUpdateAssetLabSearchResults} from './asset-lab-search-results'
import {SelectMenu} from '../studio/ui/select-menu'
import {useStudioMenuStyles} from '../studio/ui/studio-menu-styles'
import {combine} from '../common/styles'
import {Loader} from '../ui/components/loader'
import {MenuOptions} from '../studio/ui/option-menu'
import {AssetLabLibraryElement} from './asset-lab-library-element'
import {useAssetLabStateContext} from './asset-lab-context'

const useStyles = createThemedStyles(theme => ({
  searchFilterBar: {
    display: 'flex',
    flexDirection: 'row',
    marginBottom: '0.5rem',
  },
  viewTypeButtons: {
    display: 'flex',
    flexDirection: 'row',
    marginLeft: 'auto',
  },
  searchFilterContainer: {
    'flex': '1 0 auto',
    'display': 'flex',
    'flexDirection': 'row',
    'alignItems': 'center',
    '& > :not(:first-child)': {
      margin: '0 1rem',
    },
  },
  searchBarContainer: {
    maxWidth: '320px',
    flex: '1 1 auto',
  },
  filterMenu: {
    zIndex: 200,
  },
  assetContextMenu: {
    position: 'absolute',
    top: '1px',
    right: '1px',
  },
  button: {
    width: '25px',
    height: '25px',
    backgroundColor: theme.tertiaryBtnFg,
    borderRadius: '4px',
    border: `1px solid ${theme.secondaryBtnHoverBg}`,
    display: 'flex',
    alignItems: 'center',
    justifyContent: 'center',
  },
  libraryList: {
    overflowY: 'auto',
  },
  currentSelection: {
    color: theme.fgMain,
    fontWeight: 'bold',
  },
}))

const AssetLabLibrary = () => {
  const [searchValue, setSearchValue] = React.useState('')
  // TODO(dat): These will be used immediately as the search filter container is worked on.
  const [filters, setFilters] = React.useState<string[]>([])
  const [view] = React.useState<string>('grid')
  // eslint-disable-next-line @typescript-eslint/no-unused-vars, local-rules/hardcoded-copy
  const [sortBy, setSortBy] = React.useState<string>('Latest')
  const [hoveredAssetId, setHoveredAssetId] = React.useState<string | null>(null)
  const [contextMenuActive, setContextMenuActive] = React.useState(false)
  const {librarySearchResults} = useAssetLabStateContext()

  const {t} = useTranslation('asset-lab')
  const [filterBy, setFilterBy] = React.useState<string>(t('asset_lab.search_bar.filter_by_all'))
  const classes = useStyles()
  const {assetGenerationIds} = useCurrentAccountAssetGenerations()
  const menuStyles = useStudioMenuStyles()
  const searching = searchValue.length > 0 || filters.length > 0
  const {loading: searchLoading, filterBy: newFilterBy} =
    useUpdateAssetLabSearchResults(
      searchValue,
      filters,
      assetGenerationIds
    )

  const sortOptions = [
    {value: 'LATEST', label: t('asset_lab.search_bar.sort_by_latest')},
    {value: 'OLDEST', label: t('asset_lab.search_bar.sort_by_oldest')},
  ]

  React.useEffect(() => {
    setFilterBy(t(newFilterBy))
  }, [newFilterBy])

  const searchResults = sortBy === t('asset_lab.search_bar.sort_by_latest')
    ? librarySearchResults
    : librarySearchResults?.slice().reverse()

  const {librarySquareRefScroller} = useAssetLabStateContext()

  return (
    <>
      <div className={classes.searchFilterBar}>
        <div className={classes.searchFilterContainer}>
          <div className={classes.searchBarContainer}>
            <SearchBar
              searchText={searchValue}
              setSearchText={setSearchValue}
              a8='click;studio-bottom-left-pane;asset-lab-click'
            />
          </div>
          <SelectMenu
            id='asset-lab-filter-dropdown'
            trigger={(
              <div>
                {t('asset_lab.library.sort_by')}
                <span className={classes.currentSelection}>&nbsp;{sortBy}</span>
              </div>
            )}
            menuWrapperClassName={combine(menuStyles.studioMenu, classes.filterMenu)}
            placement='right-start'
            margin={16}
            minTriggerWidth
          >
            {collapse => (
              <MenuOptions
                options={sortOptions.map(option => ({
                  content: option.label,
                  onClick: () => {
                    setSortBy(option.label)
                    collapse()
                  },
                }))}
                collapse={collapse}
              />
            )}
          </SelectMenu>
          <SelectMenu
            id='asset-lab-filter-dropdown'
            trigger={(
              <div>
                {t('asset_lab.library.filter')}
                <span className={classes.currentSelection}>&nbsp;{filterBy}</span>
              </div>
            )}
            menuWrapperClassName={combine(menuStyles.studioMenu, classes.filterMenu)}
            placement='right-start'
            margin={16}
            minTriggerWidth
          >
            {collapse => (
              <OptionalFilters
                options={ASSET_LAB_FILTERS}
                noneOption={t('file_browser.search_bar.option.all', {ns: 'cloud-studio-pages'})}
                filters={filters}
                setFilters={setFilters}
                collapse={collapse}
                translationNamespace='asset-lab'
              />
            )}
          </SelectMenu>
        </div>
      </div>
      {searching &&
        <SearchToolbar
          resultCount={searchResults?.length}
          clearSearch={() => {
            setSearchValue('')
            setFilters([])
          }}
        />
      }
      <div className={classes.libraryList}>
        {searchLoading
          ? (
            <Loader animateSpinnerColor />
          )
          : view === 'grid' && (
            <GridOfSquares autoForCellSize='180px'>
              {searchResults?.map(id => (
                <AssetLabLibraryElement
                  key={id}
                  id={id}
                  hoveredAssetId={hoveredAssetId}
                  setHoveredAssetId={setHoveredAssetId}
                  contextMenuActive={contextMenuActive}
                  setContextMenuActive={setContextMenuActive}
                  size={180}
                  librarySquareRefScroller={librarySquareRefScroller}
                />
              ))}
            </GridOfSquares>
          )}
        {/* {view === 'list' &&
        <div>
          {searchResults?.map(id => <SquareAsset key={id} generationId={id} size={180} />)}
        </div>
      } */}
      </div>

    </>
  )
}

export default AssetLabLibrary
