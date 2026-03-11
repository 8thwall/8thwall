import React from 'react'
import {useTranslation} from 'react-i18next'

import {useAssetLabStateContext} from '../asset-lab-context'
import {Icon, IconStroke} from '../../ui/components/icon'
import {createThemedStyles} from '../../ui/theme'
import {combine} from '../../common/styles'

const useStyles = createThemedStyles(theme => ({
  workflowCrumbsContainer: {
    display: 'flex',
    flexDirection: 'row',
    alignItems: 'center',
    justifyContent: 'center',
    flex: '1 1 0',
  },
  workflowCrumbs: {
    padding: '0.5rem 1rem',
    borderRadius: '1.5rem',
    border: `1px solid ${theme.studioAssetBorder}`,
  },
  clickableCrumb: {
    'marginRight': '0.5rem',
    'fontSize': '14px',
    '&.active': {
      'color': theme.linkBtnFg,
    },
    'cursor': 'pointer',
  },
}))

interface Step {
  active?: boolean
  text: string
  onClick?: () => void
  stroke?: IconStroke
  disabled?: boolean
}

const Chevron: React.FC = () => (
  <span><Icon stroke='chevronRight' inline /></span>
)

const WorkflowCrumbs: React.FC = () => {
  const assetLabCtx = useAssetLabStateContext()
  const {mode, workflow, requestLineage} = assetLabCtx.state
  const {t} = useTranslation('asset-lab')
  const classes = useStyles()

  if (mode !== 'image' && mode !== 'model' && mode !== 'animation') {
    return null
  }

  const BaseStep: React.FC<Step> = ({active, text, onClick, stroke, disabled}) => (
    <button
      type='button'
      className={combine('style-reset', classes.clickableCrumb, active ? 'active' : '')}
      onClick={onClick}
      disabled={disabled}
    >
      <Icon stroke={stroke} inline /> {text}
    </button>
  )
  const ImageStep = () => (
    <BaseStep
      stroke='flatTarget'
      onClick={() => {
        assetLabCtx.setState({
          assetRequestUuid: requestLineage.multiViews?.[0] || requestLineage.root,
          mode: 'image',
        })
      }}
      active={mode === 'image'}
      disabled={mode === 'image'}
      text={t('asset_lab.image')}
    />
  )
  const ModelStep = () => (
    <BaseStep
      stroke='meshCube'
      onClick={() => {
        assetLabCtx.setState({
          assetRequestUuid: requestLineage.model,
          mode: 'model',
        })
      }}
      active={mode === 'model'}
      disabled={mode === 'model'}
      text={t('asset_lab.3d_model')}
    />
  )
  const AnimationStep = () => (
    <BaseStep
      stroke='guyRunningRight'
      onClick={() => {
        assetLabCtx.setState({
          assetRequestUuid: requestLineage.animation,
          mode: 'animation',
        })
      }}
      active={mode === 'animation'}
      disabled={mode === 'animation'}
      text={t('asset_lab.animation')}
    />
  )

  if (workflow === 'gen-image') {
    return (
      <div className={classes.workflowCrumbsContainer}>
        <div className={classes.workflowCrumbs}>
          <ImageStep />
        </div>
      </div>
    )
  }

  if (workflow === 'gen-3d-model') {
    return (
      <div className={classes.workflowCrumbsContainer}>
        <div className={classes.workflowCrumbs}>
          <ImageStep />
          <Chevron />
          <ModelStep />
        </div>
      </div>
    )
  }

  if (workflow === 'gen-animated-char') {
    return (
      <div className={classes.workflowCrumbsContainer}>
        <div className={classes.workflowCrumbs}>
          <ImageStep />
          <Chevron />
          <ModelStep />
          <Chevron />
          <AnimationStep />
        </div>
      </div>
    )
  }

  return null
}

export {
  WorkflowCrumbs,
}
