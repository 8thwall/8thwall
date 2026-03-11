import React from 'react'
import {createUseStyles} from 'react-jss'

import SplitPane from 'react-split-pane'

import Measure from 'react-measure'

import {FloatingTray} from '../ui/components/floating-tray'
import {StaticBanner} from '../ui/components/banner'
import {TreeHierarchy} from './tree-hierarchy'
import {combine} from '../common/styles'
import {useStudioStateContext} from './studio-state-context'
import {useSceneContext} from './scene-context'
import {useSelector} from '../hooks'
import editorActions from '../editor/editor-actions'
import useActions from '../common/use-actions'
import {useEnclosedApp} from '../apps/enclosed-app-context'
import {DEFAULT_HEIGHT_PERCENT} from '../editor/editor-reducer'
import {MARGIN_SIZE} from './interface-constants'
import ErrorMessage from '../home/error-message'
import {RecommendationBox} from './recommendation-box'

const LEFT_PANEL_WIDTH = '20rem'
const MIN_FILE_BROWSER_HEIGHT = 35
const MAX_FILE_BROWSER_HEIGHT = -75

const useStyles = createUseStyles({
  editPanelLeft: {
    padding: `0 0 ${MARGIN_SIZE} ${MARGIN_SIZE}`,
    display: 'flex',
    flexDirection: 'column',
    pointerEvents: 'auto',
  },
  open: {
    minWidth: LEFT_PANEL_WIDTH,
    maxWidth: LEFT_PANEL_WIDTH,
  },
  splitPaneContainer: {
    'flex': '1 0 0',
    'position': 'relative !important',
    '--log-container-resizer-color': 'transparent',
  },
  pane1: {
    height: '100%',
  },
  pane2: {
    overflow: 'hidden',
    flexGrow: 1,
  },
})

interface IFloatingLeftPanel {
  fileBrowser?: React.ReactNode
  errorMessage?: React.ReactNode
  fixedHelpButtonTray?: React.ReactNode
  topBar: React.ReactNode
}

const FloatingLeftPanel = React.forwardRef<HTMLDivElement, IFloatingLeftPanel>(({
  fileBrowser, errorMessage, fixedHelpButtonTray, topBar,
}, ref) => {
  const classes = useStyles()
  const ctx = useSceneContext()
  const stateCtx = useStudioStateContext()
  const appKey = useEnclosedApp()?.appKey || ''
  const {state: {errorMsg}} = stateCtx

  const fileBrowserHeightPercent = useSelector(
    s => s.editor.byKey[appKey]?.fileBrowserHeightPercent ?? DEFAULT_HEIGHT_PERCENT
  )
  const isFileBrowserCollapsed = fileBrowserHeightPercent === 0

  const {setFileBrowserHeightPercent, saveFileBrowserHeightPercent} = useActions(editorActions)

  const [isDragging, setIsDragging] = React.useState(false)

  const handleSizeChange = (rawSize: number, containerHeight: number) => {
    if (containerHeight <= 0) {
      return
    }
    setFileBrowserHeightPercent(appKey, rawSize / containerHeight)
  }

  const handleResizeFinished = (rawSize: number, containerHeight: number) => {
    if (containerHeight <= 0) {
      return
    }
    setIsDragging(false)
    if (rawSize <= MIN_FILE_BROWSER_HEIGHT) {
      saveFileBrowserHeightPercent(appKey, 0)
    } else {
      saveFileBrowserHeightPercent(appKey, rawSize / containerHeight)
    }
  }

  return (
    <div className={combine(classes.editPanelLeft, classes.open)} ref={ref}>
      {topBar}
      <FloatingTray
        orientation='vertical'
        isScrollable
        fillContainer
        nonInteractive={ctx.isDraggingGizmo}
      >
        {errorMsg &&
          <StaticBanner
            type='danger'
            onClose={() => stateCtx.update(p => ({...p, errorMsg: ''}))}
          >
            {errorMsg}
          </StaticBanner>
        }
        {errorMessage}
        <ErrorMessage />
        <RecommendationBox />
        {fileBrowser
          ? (
            <Measure bounds>
              {({measureRef, contentRect}) => {
                const height = contentRect.bounds.height || 0
                const size = !isDragging && isFileBrowserCollapsed
                  ? MIN_FILE_BROWSER_HEIGHT
                  : fileBrowserHeightPercent * height

                return (
                  <div ref={measureRef} className={classes.splitPaneContainer}>
                    <SplitPane
                      split='horizontal'
                      size={size}
                      minSize={MIN_FILE_BROWSER_HEIGHT}
                      maxSize={MAX_FILE_BROWSER_HEIGHT}
                      onDragStarted={() => {
                        setIsDragging(true)
                        setFileBrowserHeightPercent(appKey, size / height)
                      }}
                      onChange={newSize => handleSizeChange(newSize, height)}
                      onDragFinished={newSize => handleResizeFinished(newSize, height)}
                      primary='second'
                      pane1Style={{overflowY: 'hidden', flex: '1 0 0px'}}
                    >
                      <div className={classes.pane1}>
                        <TreeHierarchy />
                      </div>
                      <div className={classes.pane2}>
                        {fileBrowser}
                      </div>
                    </SplitPane>
                  </div>
                )
              }}
            </Measure>
          )
          : <TreeHierarchy />}
        {fixedHelpButtonTray}
      </FloatingTray>
    </div>
  )
})

export {
  FloatingLeftPanel,
  LEFT_PANEL_WIDTH,
}
