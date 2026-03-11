import React from 'react'
import {useTranslation} from 'react-i18next'
import {createUseStyles} from 'react-jss'

import {GridOfSquares} from './widgets/grid-of-squares'
import {OptionalFilters, SearchBar, SearchToolbar} from '../studio/ui/search-bar'
import {useCurrentAccountAssetGenerations} from './hooks'
import {ASSET_LAB_FILTERS, useUpdateAssetLabSearchResults} from './asset-lab-search-results'
import {FloatingPanelButton} from '../ui/components/floating-panel-button'
import {Icon} from '../ui/components/icon'
import {backToLibraryClear, useAssetLabStateContext} from './asset-lab-context'
import {AssetLabGenerateButton} from './widgets/asset-lab-generate-button'
import {gray4} from '../static/styles/settings'
import {AssetLabLibraryElement} from './asset-lab-library-element'

const useStyles = createUseStyles({
  searchFilterBar: {
    display: 'flex',
    flexDirection: 'row',
  },
  searchBarContainer: {
    'width': '100%',
    'display': 'flex',
    'gap': '0.5rem',
    '& > div': {
      minWidth: 0,
    },
    'padding': '0 0.75rem 0.5rem',
  },
  viewTypeButton: {
    flex: '0 0 auto',
    marginRight: '0.5rem',
  },
  buttonRow: {
    display: 'flex',
    gap: '0.5rem',
    padding: '0 0.5rem 0.5rem 0.5rem',
  },
  assetsContainer: {
    overflowY: 'auto',
    display: 'flex',
    flexDirection: 'column',
    padding: '0 0.75rem',
    flex: '1 1 auto',
  },
  emptyStateMessage: {
    display: 'flex',
    color: gray4,
    justifyContent: 'center',
    alignItems: 'center',
    height: '100%',
    width: '100%',
  },
  squareAssetButton: {
    'width': '100%',
    'cursor': 'pointer',
    '&:hover': {
      'boxShadow': `2px 2px 0 ${gray4}`,
      'borderRadius': '0.25rem',
    },
  },
})

const AssetLabMiniLibrary = () => {
  const {t} = useTranslation('asset-lab')
  const classes = useStyles()
  const [searchValue, setSearchValue] = React.useState('')
  const [filters, setFilters] = React.useState<string[]>([])
  const searching = searchValue.length > 0 || filters.length > 0
  const [view] = React.useState<string>('grid')
  const {assetGenerationIds} = useCurrentAccountAssetGenerations()
  const assetLabCtx = useAssetLabStateContext()
  const {librarySearchResults} = assetLabCtx

  const [hoveredAssetId, setHoveredAssetId] = React.useState<string | null>(null)
  const [contextMenuActive, setContextMenuActive] = React.useState(false)

  useUpdateAssetLabSearchResults(searchValue, filters, assetGenerationIds)

  return (
    <>
      <div className={classes.searchFilterBar}>
        <div className={classes.searchBarContainer}>
          <SearchBar
            searchText={searchValue}
            setSearchText={setSearchValue}
            filterOptions={collapse => (
              <OptionalFilters
                options={ASSET_LAB_FILTERS}
                noneOption={t('file_browser.search_bar.option.all', {ns: 'cloud-studio-pages'})}
                filters={filters}
                setFilters={setFilters}
                collapse={collapse}
                translationNamespace='asset-lab'
              />
            )}
            a8='click;studio-bottom-left-pane;asset-lab-click'
          />
        </div>
      </div>
      {searching &&
        <SearchToolbar
          resultCount={librarySearchResults?.length}
          clearSearch={() => {
            setSearchValue('')
            setFilters([])
          }}
        />
      }
      <div className={classes.buttonRow}>
        <FloatingPanelButton
          onClick={() => backToLibraryClear(assetLabCtx, {partialState: {open: true}})}
          spacing='full'
          height='small'
        >
          <Icon stroke='assetLibrary' inline />
          {t('asset_lab.browser.library')}
        </FloatingPanelButton>
        <AssetLabGenerateButton />
      </div>
      <div className={classes.assetsContainer}>
        {
          assetGenerationIds?.length === 0
            ? (
              <div className={classes.emptyStateMessage}>
                {t('asset_lab.library.no_assets')}
              </div>
            )
            : view === 'grid' && (
              <GridOfSquares numColumns={3}>
                {librarySearchResults?.map(id => (
                  <AssetLabLibraryElement
                    key={id}
                    id={id}
                    hoveredAssetId={hoveredAssetId}
                    setHoveredAssetId={setHoveredAssetId}
                    contextMenuActive={contextMenuActive}
                    setContextMenuActive={setContextMenuActive}
                    size={82}
                  />
                ))}
              </GridOfSquares>
            )
      }
        {/* {view === 'list' &&
          <div>
            {searchResults?.map(id => <SquareAssetWithIcon key={id} generationId={id} size={82} />)}
          </div>
      } */}
      </div>
    </>
  )
}
export default AssetLabMiniLibrary
