import React from 'react'

import {createThemedStyles} from '../../ui/theme'
import {FloatingPanelButton} from '../../ui/components/floating-panel-button'
import {combine} from '../../common/styles'
import {brandPurple} from '../../static/styles/settings'
import {NumberInput} from '../../studio/configuration/row-fields'
import type {INumberInput} from '../../studio/configuration/row-number-field'
import {CreditPriceChip, ICreditPrice} from '../credit-price-chip'
import {useCreditQuery} from '../../billing/use-credit-query'
import {calcGeneratePrice} from '../../../shared/genai/pricing'

const ASSET_LAB_BUTTON_VERTICAL_MARGIN_IN_REM = '0.5rem'

const useStyles = createThemedStyles(theme => ({
  buttonContainer: {
    '&>button': {
      'display': 'flex',
      'flexDirection': 'row',
      'alignItems': 'center',
      'padding': `${ASSET_LAB_BUTTON_VERTICAL_MARGIN_IN_REM} 0.7rem`,
      'borderRadius': '0.625rem',
      '&>*': {
        height: '1.8rem',
      },
      '&:hover': {
        backgroundColor: theme.studioPanelBtnHoverBg,
      },
      '&:disabled': {
        backgroundColor: theme.buttonBgInactive,
      },
    },
  },
  active: {
    '&>button': {
      'background': theme.primaryBtnBg,
      'color': theme.primaryBtnFg,
      '&:hover:not(:disabled)': {
        backgroundColor: `${theme.primaryBtnBg} !important`,
      },
      '& .standard-chip': {
        'background': brandPurple,
      },
      '&:disabled': {
        'cursor': 'default',
        'color': theme.primaryBtnDisabledFg,
        'background': theme.primaryBtnDisabledBg,
      },
    },
  },
  roundedButtonLink: {
    'background': theme.studioPanelBtnBg,
    'color': theme.fgMain,
    'cursor': 'pointer',
    'user-select': 'none',
    'display': 'flex',
    'flexDirection': 'row',
    'alignItems': 'center',
    'padding': '0.5rem 0.375rem',
    'borderRadius': '0.625rem',
    'height': '2.3rem',
    '&:hover:not(:disabled)': {
      background: theme.studioPanelBtnHoverBg,
      color: theme.fgMain,
    },
    '&:disabled': {
      cursor: 'default',
      color: theme.sfcDisabledColor,
    },
    '&:active:not(:disabled)': {
      background: theme.studioBtnActiveBg,
      color: theme.fgMain,
    },
  },
  numberInput: {
    width: '100px',
  },
}))

interface IAssetLabButton {
  children?: React.ReactNode
  onClick?: () => void
  disabled?: boolean
  loading?: boolean
  active?: boolean
  a8?: string
  className?: string
}

const AssetLabButton: React.FC<IAssetLabButton> = ({
  children, onClick, disabled, loading, active, a8, className = '',
}) => {
  const classes = useStyles()
  return (
    <div className={combine(classes.buttonContainer, active ? classes.active : '', className)}>
      <FloatingPanelButton
        a8={a8}
        height='small'
        onClick={onClick}
        disabled={disabled}
        loading={loading}
      >
        {children}
      </FloatingPanelButton>
    </div>
  )
}

interface IAssetLabButtonLink {
  href: string
  target?: '_blank' | '_self'
  rel?: string
  children?: React.ReactNode
}
const AssetLabButtonLink: React.FC<IAssetLabButtonLink> = ({
  href, target = '_self', rel, children,
}) => {
  const classes = useStyles()
  return (
    <a
      className={combine('roundedButtonLink', classes.roundedButtonLink)}
      href={href}
      target={target}
      rel={rel}
    >
      {children}
    </a>
  )
}

const AssetLabNumberInput: React.FC<INumberInput> = (props) => {
  const classes = useStyles()
  return (
    <div className={combine(classes.buttonContainer, classes.numberInput)}>
      <NumberInput
        {...props}
      />
    </div>
  )
}
interface IAssetLabButtonWithCost extends Omit<IAssetLabButton, 'children'>, ICreditPrice {
  label: string
}

const AssetLabButtonWithCost: React.FC<IAssetLabButtonWithCost> = ({
  a8, active, onClick, disabled, loading, label, type, modelId, multiplier = 1,
}) => {
  const {status, data} = useCreditQuery()
  const price = calcGeneratePrice(type, modelId, multiplier)
  const hasEnoughCredit = status === 'success' && data.creditAmount >= price
  return (
    <AssetLabButton
      a8={a8}
      active={active}
      onClick={onClick}
      disabled={disabled || !hasEnoughCredit}
      loading={loading}
    >
      {label}
      <CreditPriceChip type={type} modelId={modelId} multiplier={multiplier} />
    </AssetLabButton>
  )
}

export {
  AssetLabButton,
  AssetLabButtonLink,
  AssetLabNumberInput,
  AssetLabButtonWithCost,
  ASSET_LAB_BUTTON_VERTICAL_MARGIN_IN_REM,
}
