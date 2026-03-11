import React from 'react'
import {useTranslation} from 'react-i18next'

import {GridOfSquares} from './grid-of-squares'
import {useCurrentAccountAssetGenerations} from '../hooks'
import {SearchBar, SearchToolbar} from '../../studio/ui/search-bar'
import {useUpdateAssetLabSearchResults} from '../asset-lab-search-results'
import {gray4} from '../../static/styles/settings'
import {combine} from '../../common/styles'
import {SquareAssetWithIcon} from './square-asset'
import {createThemedStyles} from '../../ui/theme'
import {Icon} from '../../ui/components/icon'
import {useAssetLabStateContext} from '../asset-lab-context'

const useStyles = createThemedStyles(theme => ({
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
  backButton: {
    'display': 'flex',
    'alignItems': 'center',
    'gap': '.15rem',
    'color': theme.fgMain,
    '&:hover': {
      color: theme.fgMuted,
    },
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
  picker: {
    height: '360px',
    minHeight: '360px',
    width: '360px',
    borderRadius: '0.5rem',
    border: `1px solid ${theme.studioAssetBorder}`,
    overflowY: 'auto',
    overflowX: 'hidden',
    scrollbarWidth: 'none',
    paddingTop: '0.7rem',
  },
}))

interface IAssetLabLibraryPicker {
  onSelect: (id: string) => void
  onClose: () => void
  filters?: string[]
}

const AssetLabLibraryPicker: React.FC<IAssetLabLibraryPicker> = ({
  onSelect, onClose, filters = [],
}) => {
  const {t} = useTranslation('asset-lab')
  const classes = useStyles()
  const [searchValue, setSearchValue] = React.useState('')
  const searching = searchValue.length > 0
  const {assetGenerationIds} = useCurrentAccountAssetGenerations()

  useUpdateAssetLabSearchResults(searchValue, filters, assetGenerationIds)
  const {librarySearchResults} = useAssetLabStateContext()

  return (
    <div className={classes.picker}>
      <div className={classes.searchFilterBar}>
        <div className={classes.searchBarContainer}>
          <button
            type='button'
            className={combine('style-reset', classes.backButton)}
            onClick={onClose}
          >
            <Icon stroke='chevronLeft' size={0.8} />
            {t('asset_lab.library_picker.back')}
          </button>
          <SearchBar
            searchText={searchValue}
            setSearchText={setSearchValue}
            a8='click;studio-bottom-left-pane;asset-lab-click'
          />
        </div>
      </div>
      {searching &&
        <SearchToolbar
          resultCount={librarySearchResults?.length}
          clearSearch={() => {
            setSearchValue('')
          }}
        />
      }
      <div className={classes.assetsContainer}>
        {
          assetGenerationIds?.length === 0
            ? (
              <div className={classes.emptyStateMessage}>
                {t('asset_lab.library.no_assets')}
              </div>
            )
            : (
              <GridOfSquares numColumns={3}>
                {librarySearchResults?.map(id => (
                  <button
                    key={id}
                    type='button'
                    className={combine('style-reset', classes.squareAssetButton)}
                    onClick={() => {
                      onSelect(id)
                      onClose()
                    }}
                  >
                    <SquareAssetWithIcon
                      key={id}
                      generationId={id}
                      size={82}
                    />
                  </button>
                ))}
              </GridOfSquares>
            )
      }
      </div>
    </div>
  )
}
export default AssetLabLibraryPicker
