/* eslint-disable local-rules/hardcoded-copy */
// TODO(dat): Remove this once we are ready to ship this.
import React from 'react'
import {useTranslation} from 'react-i18next'

import {AssetLabMode, AssetLabWorkflow, useAssetLabStateContext} from './asset-lab-context'
import {ModeButton, MODE_BUTTON_HEIGHT_REM} from './widgets/mode-button'
import AssetLabLibrary from './asset-lab-library'
import {AssetLabImageGen} from './asset-lab-image-gen'
import {AssetLabModelGen} from './asset-lab-model-gen'
import {AssetLabAnimationGen} from './asset-lab-animation-gen'
import type {UiTheme} from '../ui/theme'
import {Icon, IconStroke} from '../ui/components/icon'
import {bool, combine} from '../common/styles'
import {IconButton} from '../ui/components/icon-button'
import {AssetLabCreditTracker} from './asset-lab-credit-tracker'
import {WorkflowCrumbs} from './widgets/workflow-crumbs'
import {StandardDropdownField} from '../ui/components/standard-dropdown-field'
import {GENERATE_MODES, translateContent, WorkflowDropdownOptions} from './constants'
import {AssetLabDetailView} from './asset-lab-detail-view'
import {Studio3dModelOptimizer} from './studio-3d-model-optimizer'
import {useSelector} from '../hooks'
import {
  ASSET_LAB_BUTTON_BAR_MARGIN_REM, GEN_RESULTS_CONTENT_PADDING_REM,
  GEN_RESULTS_MARGIN_LEFT_REM,
} from './asset-lab-styles'
import {createCustomUseStyles} from '../common/create-custom-use-styles'
import {ROUNDED_BUTTON_SMALL_HEIGHT_IN_PX} from '../ui/hooks/use-rounded-button-styling'

const ASSET_LAB_CONTAINER_PADDING_PX = '20px'
const ASSET_LAB_CONTAINER_HEIGHT_VH = '100vh'
const ASSET_LAB_CONTAINER_MODAL_HEIGHT_VH = '95vh'
const ASSET_LAB_TITLE_HEIGHT_REM = '2rem'
const ASSET_LAB_MODE_BAR_VERTICAL_MARGIN_REM = '1rem'

// NOTE(coco): Calculating the width of the asset lab container based on the image gen view
const calcAssetLabContainerWidth = (isModal: boolean) => {
  const containerHeight = isModal
    ? ASSET_LAB_CONTAINER_MODAL_HEIGHT_VH
    : ASSET_LAB_CONTAINER_HEIGHT_VH
  const genContentResultHeight = `calc(${containerHeight} -
    ${ASSET_LAB_TITLE_HEIGHT_REM} - ${MODE_BUTTON_HEIGHT_REM} -
    ${ASSET_LAB_MODE_BAR_VERTICAL_MARGIN_REM} * 2 -
    ${ASSET_LAB_CONTAINER_PADDING_PX} * 2)`
  const squareHeight = `calc(${genContentResultHeight} - 2 * ${GEN_RESULTS_CONTENT_PADDING_REM} -
    ${ROUNDED_BUTTON_SMALL_HEIGHT_IN_PX} - ${ASSET_LAB_BUTTON_BAR_MARGIN_REM})`
  const maxWidth = `calc((${squareHeight} + ${GEN_RESULTS_CONTENT_PADDING_REM} * 2 ) * 2 +
    ${ASSET_LAB_CONTAINER_PADDING_PX} * 2 + ${GEN_RESULTS_MARGIN_LEFT_REM})`

  return maxWidth
}

const useStyles = createCustomUseStyles<{isModal: boolean}>()((theme: UiTheme) => ({
  '@global': {
    '@import': [
      'url(https://fonts.googleapis.com/css2' +
      '?family=Geist+Mono:wght@100..900&display=swap)',
    ],
  },
  'titleBar': {
    'display': 'flex',
    'flexDirection': 'row',
    '& > h1': {
      fontSize: '24px',
      flex: '1 0 auto',
    },
  },
  'title': {
    color: theme.linkBtnFg,
    fontFamily: 'Geist Mono, monospace',
    fontSize: '1.125rem',
    fontWeight: 700,
    userSelect: 'none',
    height: ASSET_LAB_TITLE_HEIGHT_REM,
  },
  'closeButton': {
    flex: '0 0 auto',
  },
  'assetLabContainer': {
    color: theme.linkBtnDisableFg,
    backgroundColor: theme.modalBg,
    padding: ASSET_LAB_CONTAINER_PADDING_PX,
    flex: '1 1 auto',
    display: 'flex',
    flexDirection: 'column',
    height: ASSET_LAB_CONTAINER_HEIGHT_VH,
    maxWidth: ({isModal}) => calcAssetLabContainerWidth(isModal),
    minWidth: '51rem',
  },
  'modeActive': {
    color: theme.linkBtnFg,
  },
  'modeBar': {
    display: 'flex',
    flexDirection: 'row',
    margin: `${ASSET_LAB_MODE_BAR_VERTICAL_MARGIN_REM} 0`,
  },
  'modeButtons': {
    'display': 'flex',
    'flexDirection': 'row',
    'flex': '1 1 0',
    'alignItems': 'center',
  },
  'libraryButton': {
    borderRight: `1px solid ${theme.linkBtnFg} !important`,
    paddingLeft: '0 !important',
    width: '7.5rem',
  },
  'assetLabModal': {
    width: '95vw !important',
    height: '95vh',
  },
  'noWorkflow': {
    width: '100%',
    height: '75%',
    display: 'flex',
    textAlign: 'center',
    alignItems: 'center',
    justifyContent: 'center',
    fontFamily: 'Geist Mono, monospace',
    color: theme.fgMain,
  },
  'generateButton': {
    width: '9rem',
  },
  'assetLabBackground': {
    backgroundColor: theme.modalBg,
    height: '100vh',
    width: '100vw',
    display: 'flex',
    justifyContent: 'center',
  },
}))

interface IAssetLabContainer {
  isModal?: boolean
}

// The asset lab can be accessed via your workspace at /accountSn/asset-lab or inside of studio at
// /accountSn/projectSn/studio?assetLab=1.
// This container is designed to be used inside of a modal (asset-lab-modal) or as a full page
// (asset-lab-page).
const AssetLabContainer: React.FC<IAssetLabContainer> = ({isModal}) => {
  const {t} = useTranslation('asset-lab')
  const classes = useStyles({isModal})
  const assetLabCtx = useAssetLabStateContext()
  const {mode, workflow, assetGenerationUuid} = assetLabCtx.state
  // TODO(dat): Replace these structure with components that reflects the design.

  const renderModeButton = (
    targetMode: AssetLabMode, label: string, icon: IconStroke, className?: string
  ) => {
    const onClick = () => {
      if (mode === targetMode) {
        return
      }
      assetLabCtx.setState({mode: targetMode})
    }
    return (
      <ModeButton className={className} active={mode === targetMode} onClick={onClick}>
        <Icon stroke={icon} inline /> {t(label)}
      </ModeButton>
    )
  }

  const changeWorkflow = (newWorkflow: AssetLabWorkflow) => {
    let newMode = mode
    // If your new workflow does not support the current mode, switch to image
    if (mode === 'library') {
      newMode = 'image'
    }
    if (mode === 'model' && newWorkflow === 'gen-image') {
      newMode = 'image'
    }
    if (mode === 'animation' && newWorkflow !== 'gen-animated-char') {
      newMode = 'image'
    }

    assetLabCtx.setState({
      workflow: newWorkflow,
      mode: newMode,
      assetGenerationUuid: undefined,
      assetRequestUuid: undefined,
    })
  }

  const extraClass = isModal ? classes.assetLabModal : ''

  // Used to NOT show things that are in a bad state
  const assetGeneration = useSelector(s => s.assetLab.assetGenerations[assetGenerationUuid])
  return (
    <div className={bool(!isModal, classes.assetLabBackground)}>
      <div className={combine(classes.assetLabContainer, extraClass)}>
        <div className={classes.titleBar}>
          <h1 className={classes.title}>{t('asset_lab.title')}</h1>
          <div>
            <IconButton
              stroke='cancelLarge'
              onClick={() => {
                assetLabCtx.setState({open: false, assetRequestUuid: undefined})
                if (!isModal) {
                  // TODO(christoph): Clean up
                }
              }}
              text={t('asset_lab.close')}
            />
          </div>
        </div>
        {mode !== 'detailView' && mode !== 'optimizer' && (
          <div className={classes.modeBar}>
            <div className={classes.modeButtons}>
              {renderModeButton('library', 'asset_lab.library', 'assetLibrary',
                classes.libraryButton)}
              <ModeButton
                active={GENERATE_MODES.includes(mode)}
                onClick={() => assetLabCtx.setState({mode: 'image'})}
                className={classes.generateButton}
              >
                <Icon stroke='beaker' inline /> {t('asset_lab.create')}
              </ModeButton>
              <StandardDropdownField
                id='workflow-dropdown'
                label=''
                options={translateContent(WorkflowDropdownOptions, t)}
                value={workflow}
                onChange={value => changeWorkflow(value as AssetLabWorkflow)}
                placeholder={t('asset_lab.workflow.none')}
              />
            </div>
            <WorkflowCrumbs />
            <AssetLabCreditTracker />
          </div>
        )}
        {mode === 'library' && <AssetLabLibrary />}
        {mode === 'image' && !!workflow && <AssetLabImageGen />}
        {mode === 'image' && !workflow &&
          <div className={classes.noWorkflow}>
            {t('asset_lab.generate.no_workflow')}
          </div>
      }
        {mode === 'model' && <AssetLabModelGen />}
        {mode === 'animation' && <AssetLabAnimationGen />}
        {mode === 'detailView' && <AssetLabDetailView />}
        {mode === 'optimizer' && !!assetGeneration && <Studio3dModelOptimizer />}
      </div>
    </div>
  )
}
export default AssetLabContainer
