import React from 'react'

import {hexColorWithAlpha} from '../../shared/colors'
import {createThemedStyles} from '../ui/theme'
import {MANIFEST_FILE_PATH, isHiddenPath} from '../common/editor-files'
import {FileBrowserListFile} from './file-browser-list-file'
import {useCurrentRepoId} from '../git/repo-id-context'
import {stripPrimaryRepoId} from '../editor/editor-file-location'
import {SearchToolbar} from './ui/search-bar'
import {useCurrentGit} from '../git/hooks/use-current-git'

const useStyles = createThemedStyles(theme => ({
  searchTreeHierarchyContainer: {
    display: 'flex',
    flexDirection: 'column',
  },
  searchBarContainer: {
    padding: '0.825rem 0.75rem',
  },
  filterButton: {
    position: 'relative',
    display: 'flex',
    alignItems: 'center',
    justifyContent: 'space-between',
    width: '100%',
    minWidth: '6rem',
  },
  checkmark: {
    marginRight: '-0.25rem',
  },
  elementIcon: {
    display: 'flex',
    alignItems: 'center',
    color: theme.fgMuted,
  },
  resultsContainer: {
    display: 'flex',
    flexDirection: 'column',
    paddingBottom: '0.5rem',
    overflowY: 'auto',
    flexGrow: 1,
  },
  selected: {
    background: theme.studioTreeHoverBg,
  },
  resultEntry: {
    'padding': '0.25rem 1rem',
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
}))

const FILE_BROWSER_FILTERS = [
  {
    type: 'image',
    label: 'file_browser.search_bar.option.image',
    fileTypes: ['png', 'jpg', 'jpeg', 'gif', 'bmp', 'svg'],
  },
  {
    type: 'video',
    label: 'file_browser.search_bar.option.video',
    fileTypes: ['mp4', 'webm', 'ogg'],
  },
  {
    type: 'audio',
    label: 'file_browser.search_bar.option.audio',
    fileTypes: ['mp3', 'm4a', 'wav', 'ogg', 'aac'],
  },
  {
    type: 'model',
    label: 'file_browser.search_bar.option.model',
    fileTypes: ['obj', 'fbx', 'gltf', 'glb'],
  },
]

const getFileExtension = (filePath: string) => {
  const parts = filePath.split('.')
  return parts[parts.length - 1]
}

interface IFileBrowserSearchResults {
  searchValue: string
  filters: string[]
  clearSearch: () => void
}

const FileBrowserSearchResults: React.FC<IFileBrowserSearchResults> = ({
  searchValue, filters, clearSearch,
}) => {
  const classes = useStyles()
  const typeFilters = FILE_BROWSER_FILTERS.filter(({type}) => filters.includes(type))

  const currentRepoId = useCurrentRepoId()
  const gitFiles = useCurrentGit(git => git.files).filter(file => !isHiddenPath(file.filePath), [])

  const fileList = gitFiles.filter(file => !file.isDirectory)
    .filter(file => !isHiddenPath(file.filePath))
    .filter(file => !(file.filePath.includes(MANIFEST_FILE_PATH) && file.repoId !== currentRepoId))
    .filter((file) => {
      if (searchValue && !file.filePath.toLowerCase().includes(searchValue.toLowerCase())) {
        return false
      }
      if (filters.length === 0) {
        return true
      }
      if (typeFilters.length > 0) {
        return typeFilters
          .some(({fileTypes}) => fileTypes.includes(getFileExtension(file.filePath)))
      }
      return false
    })

  return (
    <>
      <SearchToolbar
        resultCount={fileList.length}
        clearSearch={clearSearch}
      />
      <div className={classes.resultsContainer}>
        {fileList.map(file => (
          <FileBrowserListFile
            key={file.repositoryName + file.filePath}
            fileLocation={stripPrimaryRepoId(file, currentRepoId)}
          />
        ))}
      </div>
    </>
  )
}

export {
  FileBrowserSearchResults,
  FILE_BROWSER_FILTERS,
}
