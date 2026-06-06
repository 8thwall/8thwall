import React from 'react'
import {createUseStyles} from 'react-jss'

import {FloatingTray} from '../ui/components/floating-tray'
import AssetConfigurator from '../studio/configuration/asset-configurator'
import {FileConfigurator} from '../studio/file-configurator'
import {PanelSelection, useStudioStateContext} from '../studio/studio-state-context'
import {extractFilePath, extractRepoId} from '../editor/editor-file-location'
import {useCurrentRepoId} from '../git/repo-id-context'
import {useScopedGitFile} from '../git/hooks/use-current-git'
import {combine} from '../common/styles'
import {ImageTargetAssetConfigurator} from '../studio/configuration/image-target-asset-configurator'
import {DEFAULT_ROW_INLINE_PADDING, ROW_PADDING_VAR} from '../studio/configuration/row-styles'
import {DISCARD_ROOT_PATH, ScenePathRootProvider} from '../studio/scene-path-input-context'
import {Loader} from '../ui/components/loader'
import {isAssetPath} from '../common/editor-files'

const RIGHT_PANEL_WIDTH = 325

const useStyles = createUseStyles({
  editPanel: {
    overflow: 'auto',
    width: `${RIGHT_PANEL_WIDTH}px`,
    display: 'flex',
    flexDirection: 'column',
    pointerEvents: 'auto',
    fontSize: '12px',
    [ROW_PADDING_VAR]: BuildIf.MIGRATE_PADDING_20250610 ? DEFAULT_ROW_INLINE_PADDING : '0em',
  },
  mainContainer: {
    display: 'flex',
    flexDirection: 'column',
    flex: '1 0 0',
  },
  autoScroll: {
    overflowY: 'auto',
  },
  neverScroll: {
    overflowY: 'hidden',
  },
})

interface IFloatingRightPanel {
  topBar: React.ReactNode
}

const FloatingRightPanel: React.FC<IFloatingRightPanel> = ({
  topBar,
}) => {
  const classes = useStyles()
  const stateCtx = useStudioStateContext()
  const {selectedAsset, selectedImageTarget} = stateCtx.state
  const primaryRepoId = useCurrentRepoId()
  const assetRepoId = extractRepoId(selectedAsset) || primaryRepoId
  const selectedAssetFile = useScopedGitFile(assetRepoId, extractFilePath(selectedAsset))

  const {currentPanelSection} = stateCtx.state

  type InspectorContent = {
    content: React.ReactNode
    containerClass: string
  }

  const renderInspector = (): null | InspectorContent => {
    if (selectedAssetFile) {
      if (isAssetPath(selectedAssetFile.filePath)) {
        return {
          containerClass: classes.autoScroll,
          content: (
            <ScenePathRootProvider path={DISCARD_ROOT_PATH}>
              <AssetConfigurator
                selectedAsset={selectedAsset}
              />
            </ScenePathRootProvider>
          ),
        }
      } else {
        return {
          containerClass: classes.neverScroll,
          content: (
            <ScenePathRootProvider path={DISCARD_ROOT_PATH}>
              <FileConfigurator location={selectedAsset} />
            </ScenePathRootProvider>
          ),
        }
      }
    } else if (selectedImageTarget) {
      return {
        containerClass: classes.neverScroll,
        content: (
          <ScenePathRootProvider path={DISCARD_ROOT_PATH}>
            <ImageTargetAssetConfigurator />
          </ScenePathRootProvider>
        ),
      }
    } else {
      return null
    }
  }

  const inspector = renderInspector()
  const canInspect = !!inspector

  React.useEffect(() => {
    if (!canInspect && currentPanelSection === PanelSelection.INSPECTOR) {
      stateCtx.update(p => ({...p, currentPanelSection: PanelSelection.SETTINGS}))
    }
  }, [canInspect, currentPanelSection])

  return (
    <div className={classes.editPanel}>
      {topBar}
      <FloatingTray
        id='floating-right-panel'
        orientation='vertical'
        fillContainer
      >
        <React.Suspense fallback={<Loader size='small' />}>
          {(currentPanelSection === PanelSelection.INSPECTOR && inspector)
            ? (
              <div className={combine(classes.mainContainer, inspector.containerClass)}>
                {inspector.content}
              </div>
            )
            : (
              <div className={combine(classes.mainContainer, classes.neverScroll)} />
            )
          }
        </React.Suspense>
      </FloatingTray>
    </div>
  )
}

export {
  FloatingRightPanel,
  RIGHT_PANEL_WIDTH,
}
