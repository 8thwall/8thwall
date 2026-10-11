import {h} from 'preact'
import type {ComponentChildren, FunctionComponent} from 'preact'

import type {StartFlowScreen} from '../../src/startflow/types'
import styles from './debug.module.css'

type ViewMode = 'mobile' | 'full' | 'full-no-sidebar'

interface PreviewItem {
  label: string
  screen: StartFlowScreen
}

interface PreviewWorkspaceProps {
  collection: boolean
  items: PreviewItem[]
  onViewModeChange: (viewMode: ViewMode) => void
  previewStyle: string
  renderScreen: (screen: StartFlowScreen) => ComponentChildren
  viewMode: ViewMode
}

const VIEW_MODES: {value: ViewMode, label: string}[] = [
  {value: 'mobile', label: 'Mobile'},
  {value: 'full', label: 'Full'},
  {value: 'full-no-sidebar', label: 'No sidebar'},
]

const PreviewFrame: FunctionComponent<{
  children: ComponentChildren
  collection: boolean
  previewStyle: string
  viewMode: ViewMode
}> = ({children, collection, previewStyle, viewMode}) => {
  let frameClass = styles.fullPreview
  if (viewMode === 'mobile') {
    frameClass = collection ? `${styles.device} ${styles.gridDevice}` : styles.device
  } else if (collection) {
    frameClass = styles.stackedPreview
  }

  return (
    <div className={frameClass}>
      <div className={styles.preview} style={previewStyle}>
        {children}
      </div>
    </div>
  )
}

const PreviewWorkspace: FunctionComponent<PreviewWorkspaceProps> = ({
  collection,
  items,
  onViewModeChange,
  previewStyle,
  renderScreen,
  viewMode,
}) => {
  let previews: ComponentChildren
  if (collection) {
    const layoutClass = viewMode === 'mobile' ? styles.mobileGrid : styles.fullStack
    previews = (
      <div className={`${styles.allScreens} ${layoutClass}`}>
        {items.map(({label, screen}) => (
          <section className={styles.screenPreview} key={label}>
            <h2>{label}</h2>
            <PreviewFrame collection previewStyle={previewStyle} viewMode={viewMode}>
              {renderScreen(screen)}
            </PreviewFrame>
          </section>
        ))}
      </div>
    )
  } else {
    const [{screen}] = items
    previews = (
      <PreviewFrame collection={false} previewStyle={previewStyle} viewMode={viewMode}>
        {renderScreen(screen)}
      </PreviewFrame>
    )
  }

  return (
    <main className={`${styles.workspace} ${collection ? styles.allWorkspace : ''}`}>
      <div className={styles.viewToggle} role='group' aria-label='Preview size'>
        {VIEW_MODES.map(({value, label}) => (
          <button
            key={value}
            className={viewMode === value ? styles.viewToggleActive : ''}
            type='button'
            aria-pressed={viewMode === value}
            onClick={() => onViewModeChange(value)}
          >
            {label}
          </button>
        ))}
      </div>
      {previews}
    </main>
  )
}

export type {PreviewItem, ViewMode}
export {PreviewWorkspace}
