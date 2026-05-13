import * as React from 'react'

import {LogContainer} from './log-container'
import {useTheme} from '../user/use-theme'

interface IRepoInfoFooter {
  logKey: string
  showLogs: boolean
  toggleShowLogs: () => void
  autoExpand?: boolean
  extraTabContent?: React.ReactNode
}

const RepoInfoFooter: React.FC<IRepoInfoFooter> = ({
  logKey, showLogs, toggleShowLogs, autoExpand = true, extraTabContent,
}) => {
  const themeName = useTheme() || 'dark'
  return (
    <div className={`footer-pane horizontal-flex expand-1 ${themeName}`}>
      <LogContainer
        logKey={logKey}
        expanded={showLogs}
        toggleExpanded={toggleShowLogs}
        autoExpand={autoExpand}
        extraTabContent={extraTabContent}
      />
    </div>
  )
}

export default RepoInfoFooter
