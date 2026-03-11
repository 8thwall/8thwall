import React from 'react'
import {useTranslation} from 'react-i18next'

import {fiftyPercentBlack, twentyPercentBlack} from '../../static/styles/settings'
import {createThemedStyles} from '../theme'
import {combine} from '../../common/styles'
import {useIdFallback} from '../../studio/use-id-fallback'

const useStyles = createThemedStyles(theme => ({
  container: {
    'display': 'block',
    '& input': {
      width: 1,
      height: 1,
      overflow: 'hidden',
      position: 'absolute',
      clip: 'rect(1px, 1px, 1px, 1px)',
    },
    '& input:focus + .slider': {
      boxShadow: `0 0 0 1px ${theme.sfcBorderFocus}`,
    },
    '& input:checked + .slider': {
      backgroundColor: theme.toggleBackgroundEnabled,
    },
    '& input:checked + .slider:before': {
      transform: 'translateX(18px)',
      boxShadow: `0 0 0.5px 0.5px ${fiftyPercentBlack}`,
    },
    '& .slider': {
      display: 'block',
      position: 'relative',
      width: '36px',
      height: '18px',
      cursor: 'pointer',
      backgroundColor: theme.toggleBackgroundDisabled,
      borderRadius: '9px',
      marginRight: '1em',
    },
    '& .slider:before': {
      position: 'absolute',
      content: '""',
      height: '14px',
      width: '14px',
      left: '2px',
      bottom: '2px',
      backgroundColor: theme.toggleIndicator,
      transition: 'transform .4s',
      borderRadius: '50%',
      boxShadow: `0 0 0.5px 0.5px ${twentyPercentBlack}`,
    },
  },
  labelText: {
    color: theme.fgMain,
    display: 'block',
  },
  toggleRow: {
    display: 'flex',
    alignItems: 'center',
    flexWrap: 'wrap',
  },
  statusText: {
    color: theme.fgMain,
  },
  bold: {
    fontWeight: 'bold',
  },
}))

interface IStandardToggleField {
  id?: string
  label: React.ReactNode
  checked: boolean
  onChange: (checked: boolean) => void
  showStatus?: boolean
  boldLabel?: boolean
}

const StandardToggleField: React.FC<IStandardToggleField> = ({
  checked,
  onChange,
  id: idOverride,
  label,
  showStatus,
  boldLabel,
}) => {
  const id = useIdFallback(idOverride)
  const {t} = useTranslation('common')
  const classes = useStyles()
  return (
    <label htmlFor={id} className={classes.container}>
      <span className={combine(classes.labelText, boldLabel && classes.bold)}>{label}</span>
      <span className={classes.toggleRow}>
        <input
          id={id}
          type='checkbox'
          checked={checked}
          onChange={e => onChange(e.target.checked)}
        />
        <span className='slider' />
        {showStatus &&
          <span aria-hidden className={classes.statusText}>
            {checked ? t('toggle.on') : t('toggle.off')}
          </span>
        }
      </span>
    </label>
  )
}

export {
  StandardToggleField,
}

export type {
  IStandardToggleField,
}
