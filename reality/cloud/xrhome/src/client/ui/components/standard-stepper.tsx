import React from 'react'

import {useTranslation} from 'react-i18next'

import {combine} from '../../common/styles'
import {createThemedStyles} from '../theme'
import {IconButton} from './icon-button'

const useStyles = createThemedStyles(theme => ({
  stepperContainer: {
    display: 'flex',
    alignItems: 'center',
    whiteSpace: 'nowrap',
    backgroundColor: theme.sfcBackgroundDefault,
    boxSizing: 'border-box',
    boxShadow: `0 0 0 1px ${theme.stepperBorder} inset`,
    borderRadius: '0.25rem',
    overflow: 'hidden',
  },
  arrowButtonContainer: {
    'color': theme.fgMuted,
    'backgroundColor': theme.stepperArrowBackground,
    'userSelect': 'none',
  },
  arrowButtonHover: {
    '&:hover': {
      color: theme.fgMain,
      backgroundColor: theme.listItemHoverBg,
    },
  },
  value: {
    padding: '0 0.5rem',
    color: theme.fgMain,
    overflow: 'hidden',
    textOverflow: 'ellipsis',
  },
  disabledStepper: {
    color: theme.fgMuted,
  },
}))

type StepperOption = {
  value: string
  textContent: string
}

interface IStandardStepper {
  value: string
  options: StepperOption[]
  onChange: (value: string) => void
  disabled?: boolean
}

const StandardStepper: React.FC<IStandardStepper> = ({
  value, options, onChange, disabled,
}) => {
  const classes = useStyles()
  const {t} = useTranslation(['common'])
  const initialIndex = options.findIndex(option => option.value === value)

  const handleValueChange = (direction: 'left' | 'right') => {
    let newIndex = initialIndex
    if (direction === 'left') {
      newIndex = initialIndex === 0 ? options.length - 1 : initialIndex - 1
    } else if (direction === 'right') {
      newIndex = initialIndex === options.length - 1 ? 0 : initialIndex + 1
    }
    onChange(options[newIndex].value)
  }

  return (
    <div className={classes.stepperContainer}>
      <div className={combine(
        classes.arrowButtonContainer, !disabled && classes.arrowButtonHover
      )}
      >
        <IconButton
          stroke='arrowLeft'
          onClick={() => handleValueChange('left')}
          text={t('standard_stepper.previous_button.text')}
          disabled={disabled}
        />
      </div>
      <div className={combine(classes.value, disabled && classes.disabledStepper)}>
        {options[initialIndex]?.textContent}
      </div>
      <div className={combine(
        classes.arrowButtonContainer, !disabled && classes.arrowButtonHover
      )}
      >
        <IconButton
          stroke='arrowRight'
          onClick={() => handleValueChange('right')}
          text={t('standard_stepper.next_button.text')}
          disabled={disabled}
        />
      </div>
    </div>
  )
}

export {
  StandardStepper,
}

export type {
  StepperOption,
}
