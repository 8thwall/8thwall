import React from 'react'
import {useTranslation} from 'react-i18next'

import {FloatingTray} from '../ui/components/floating-tray'
import {ObjectConfigurator} from './configuration/object-configurator'
import {DefaultConfigurator} from './configuration/default-configurator'
import {useSelectedObjects} from './hooks/selected-objects'
import {useSceneContext} from './scene-context'
import AssetConfigurator from './configuration/asset-configurator'
import {FileConfigurator} from './file-configurator'
import {PanelSelection, useStudioStateContext} from './studio-state-context'
import {extractFilePath, extractRepoId} from '../editor/editor-file-location'
import {useCurrentRepoId} from '../git/repo-id-context'
import {useScopedGitFile} from '../git/hooks/use-current-git'
import {useFileBrowserStyles} from './file-browser'
import {combine} from '../common/styles'
import {createThemedStyles} from '../ui/theme'
import {ImageTargetAssetConfigurator} from './configuration/image-target-asset-configurator'
import {MARGIN_SIZE} from './interface-constants'
import {DEFAULT_ROW_INLINE_PADDING, ROW_PADDING_VAR} from './configuration/row-styles'
import {DISCARD_ROOT_PATH, ScenePathRootProvider} from './scene-path-input-context'
import {Loader} from '../ui/components/loader'
import {isAssetPath} from '../common/editor-files'

const RIGHT_PANEL_WIDTH = 325

const useStyles = createThemedStyles(theme => ({
  editPanel: {
    overflow: 'auto',
    width: `${RIGHT_PANEL_WIDTH}px`,
    display: 'flex',
    flexDirection: 'column',
    pointerEvents: 'auto',
    padding: `0 ${MARGIN_SIZE} ${MARGIN_SIZE} 0`,
    fontSize: '12px',
    [ROW_PADDING_VAR]: BuildIf.MIGRATE_PADDING_20250610 ? DEFAULT_ROW_INLINE_PADDING : '0em',
  },
  borderBottom: {
    borderBottom: theme.studioSectionBorder,
    borderTop: 'none',
  },
  disabled: {
    color: `${theme.fgMuted}32`,
    cursor: 'not-allowed',
  },
  mainContainer: {
    display: 'flex',
    flexDirection: 'column',
    flex: '1 0 0',
  },
  autoScroll: {
    overflowY: 'auto',
  },
  reserveScroll: {
    overflowY: 'scroll',
  },
  neverScroll: {
    overflowY: 'hidden',
  },
}))

interface IFloatingRightPanel {
  topBar: React.ReactNode
}

const FloatingRightPanel: React.FC<IFloatingRightPanel> = ({
  topBar,
}) => {
  const classes = useStyles()
  const fileBrowserClasses = useFileBrowserStyles()
  const ctx = useSceneContext()
  const selectedObjects = useSelectedObjects()
  const stateCtx = useStudioStateContext()
  const {selectedAsset, selectedImageTarget} = stateCtx.state
  const primaryRepoId = useCurrentRepoId()
  const assetRepoId = extractRepoId(selectedAsset) || primaryRepoId
  const selectedAssetFile = useScopedGitFile(assetRepoId, extractFilePath(selectedAsset))
  const {t} = useTranslation(['cloud-studio-pages'])

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
    } else if (BuildIf.STUDIO_IMAGE_TARGETS_20260210 && selectedImageTarget) {
      return {
        containerClass: classes.neverScroll,
        content: (
          <ScenePathRootProvider path={DISCARD_ROOT_PATH}>
            <ImageTargetAssetConfigurator />
          </ScenePathRootProvider>
        ),
      }
    } else if (selectedObjects.length) {
      return {
        containerClass: classes.reserveScroll,
        content: (
          <ObjectConfigurator />
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
        nonInteractive={ctx.isDraggingGizmo}
      >
        <React.Suspense fallback={<Loader size='small' />}>
          <div className={combine(classes.borderBottom, fileBrowserClasses.sectionTitleContainer)}>
            <button
              type='button'
              onClick={() => {
                stateCtx.update(p => ({...p, currentPanelSection: PanelSelection.SETTINGS}))
              }}
              className={combine(
                'style-reset', fileBrowserClasses.sectionTitle,
                currentPanelSection === PanelSelection.SETTINGS && fileBrowserClasses.sectionActive
              )}
            >
              {t('right_panel.button.settings')}
            </button>
            <button
              type='button'
              onClick={() => {
                stateCtx.update(p => ({...p, currentPanelSection: PanelSelection.INSPECTOR}))
              }}
              className={combine(
                'style-reset', fileBrowserClasses.sectionTitle,
                currentPanelSection === PanelSelection.INSPECTOR &&
                  fileBrowserClasses.sectionActive,
                !canInspect && classes.disabled
              )}
              disabled={!canInspect}
            >
              {t('right_panel.button.inspector')}
            </button>
          </div>
          {(currentPanelSection === PanelSelection.INSPECTOR && inspector)
            ? (
              <div className={combine(classes.mainContainer, inspector.containerClass)}>
                {inspector.content}
              </div>
            )
            : (
              <div className={combine(classes.mainContainer, classes.reserveScroll)}>
                <DefaultConfigurator />
              </div>
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
