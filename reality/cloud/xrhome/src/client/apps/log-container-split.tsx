import React from 'react'
import SplitPane from 'react-split-pane'
import localForage from 'localforage'

import {useAbandonableEffect} from '../hooks/abandonable-effect'
import useCurrentApp from '../common/use-current-app'
import {isCloudStudioApp} from '../../shared/app-utils'
import RepoInfoFooter from '../editor/repo-info-footer'

const COLLAPSED_SIZE = 23
const EXPANDED_SIZE = 250
const MIN_EXPANDED_SIZE = 100

const getMaxSize = () => window.innerHeight * 0.8
const limitSize = size => Math.min(getMaxSize(), size)

interface IFileEditorMainContent {
  extraTabContent?: React.ReactNode
  children: React.ReactNode
}

const LogContainerSplit: React.FC<IFileEditorMainContent> = ({
  extraTabContent, children,
}) => {
  const [splitSize, setSplitSize] = React.useState(COLLAPSED_SIZE)
  const [preferredExpandedSize, setPreferredExpandedSize] = React.useState(EXPANDED_SIZE)
  const showLogs = splitSize >= MIN_EXPANDED_SIZE

  const app = useCurrentApp()

  useAbandonableEffect(async (maybeAbandon) => {
    const size = await maybeAbandon(localForage.getItem<number>('editor-log-container-size'))
    if (size) {
      setPreferredExpandedSize(size)
    }
  }, [app.appKey, app.repoId])

  const toggleShowLogs = () => {
    setSplitSize(
      splitSize < MIN_EXPANDED_SIZE
        ? limitSize(preferredExpandedSize)
        : COLLAPSED_SIZE
    )
  }

  const handleResize = (size: number) => {
    setSplitSize(limitSize(size))
  }

  const handleResizeFinished = (rawSize: number) => {
    const size = limitSize(rawSize)
    const collapse = size < MIN_EXPANDED_SIZE

    setSplitSize(collapse ? COLLAPSED_SIZE : size)

    if (!collapse) {
      setPreferredExpandedSize(size)
      localForage.setItem('editor-log-container-size', size)
    }
  }

  return (
    <SplitPane
      split='horizontal'
      primary='second'
      minSize={COLLAPSED_SIZE}
      size={splitSize}
      maxSize={getMaxSize()}
      onChange={handleResize}
      onDragFinished={handleResizeFinished}
    >
      {children}
      <RepoInfoFooter
        logKey={app.appKey}
        showLogs={showLogs}
        toggleShowLogs={toggleShowLogs}
        clientSpecifier={null}
        autoExpand={!isCloudStudioApp(app)}
        extraTabContent={extraTabContent}
      />
    </SplitPane>
  )
}

export {LogContainerSplit}
