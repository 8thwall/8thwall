import React from 'react'
import type {DeepReadonly as RO} from 'ts-essentials'

import {combine} from '../../common/styles'
import {createThemedStyles} from '../theme'
import {StandardFieldContainer} from './standard-field-container'
import {StandardFieldLabel} from './standard-field-label'
import {CoreDropdown, DropdownOption, DropdownSection} from './core-dropdown'
import {useIdFallback} from '../../studio/use-id-fallback'

const useStyles = createThemedStyles(theme => ({
  target: {
    'userSelect': 'none',
    'display': 'flex',
    'overflow': 'hidden',
    'alignItems': 'center',
    '&:focus': {
      outline: 'none',
    },
    'cursor': 'pointer',
    '&[aria-disabled=true]': {
      cursor: 'default',
      opacity: 0.5,
    },
  },
  auto: {
    minHeight: '38px',
    padding: '0.5rem 0.75rem',
  },
  medium: {
    height: '38px',
    paddingLeft: '0.75rem',
    paddingRight: '3rem',
  },
  small: {
    height: '32px',
    paddingLeft: '0.75rem',
    paddingRight: '3rem',
  },
  tiny: {
    height: '24px',
    fontSize: '12px',
    paddingLeft: '0.5rem',
    paddingRight: '2rem',
  },
  chevron: {
    borderRight: `1.5px solid ${theme.sfcDropdownChevronColor}`,
    borderBottom: `1.5px solid ${theme.sfcDropdownChevronColor}`,
    width: theme.sfcDropdownChevronSize,
    height: theme.sfcDropdownChevronSize,
    transform: 'translateY(-65%) rotate(45deg)',
    position: 'absolute',
    right: '1rem',
    top: '50%',
    pointerEvents: 'none',
  },
  chevronOpen: {
    transform: 'translateY(-10%) rotate(-135deg)',
  },
  menu: {
    userSelect: 'none',
    position: 'absolute',
    left: 0,
    right: 0,
    maxHeight: '20rem',
    overflow: 'auto',
    background: theme.studioBgMain,
    color: theme.studioBtnHoverFg,
    border: theme.listBoxBorder,
    boxShadow: theme.listBoxShadow,
    backdropFilter: 'blur(50px)',
    zIndex: '10',
    borderRadius: theme.sfcBorderRadius,
    display: 'none',
  },
  menuTop: {
    bottom: 'calc(100% + 0.5rem)',
  },
  menuBottom: {
    top: 'calc(100% + 0.5rem)',
  },
  menuOpen: {
    display: 'block',
  },
  borderBottom: {
    '&:not(:last-child)': {
      borderBottom: theme.listItemBorder,
    },
  },
  menuOption: {
    'cursor': 'pointer',
    'position': 'relative',
    'overflow': 'hidden',
    'padding': '0.5rem 1rem',
    '&:hover': {
      background: theme.listItemHoverBg,
    },
    '&[aria-selected=true]:after': {
      content: '""',
      zIndex: 5,
      position: 'absolute',
      right: '1rem',
      top: '50%',
      background: theme.fgPrimary,
      width: '12px',
      height: '12px',
      // eslint-disable-next-line max-len
      clipPath: 'path("m 0.445 6.111 a 0.5 0.5 0 0 1 1.1082 -1.1337 l 2.8883 2.6636 l 5.8793 -7.1223 a 0.5 0.5 0 0 1 1.2846 1.0599 l -6.4314 7.8021 q -0.561 0.5884 -1.096 0.1504 z")',
      transform: 'translateY(-50%)',
    },
  },
  menuOptionGroup: {
    padding: '0.5rem 0',
    position: 'relative',
  },
  menuOptionGroupLabel: {
    color: theme.fgMuted,
  },
  focusedOption: {
    background: theme.listItemHoverBg,
  },
  maxContent: {
    width: 'max-content',
  },
}))

interface IStandardDropdownField {
  id?: string
  label: React.ReactNode
  value: string
  onChange: (newValue: string) => void
  height?: 'auto' | 'medium' | 'small' | 'tiny'
  placeholder?: React.ReactNode
  options: RO<DropdownOption[]>
  sections?: RO<DropdownSection[]>
  disabled?: boolean
  boldLabel?: boolean
  starredLabel?: boolean
  width?: 'maxContent'
  maxHeight?: number
  maxWidth?: number
  shouldReposition?: boolean
  positionAbove?: boolean
  formatVisibleContent?: (option: DropdownOption) => React.ReactNode
}

const StandardDropdownField: React.FC<IStandardDropdownField> = ({
  id: idOverride, label, height = 'medium', options, sections, value, onChange, placeholder,
  disabled, boldLabel, starredLabel, width, maxHeight, maxWidth, formatVisibleContent,
  shouldReposition, positionAbove,
}) => {
  const id = useIdFallback(idOverride)
  const classes = useStyles()
  const labelId = `${id}-labeltext`

  return (
    <div>
      <label htmlFor={id}>
        <StandardFieldLabel
          id={labelId}
          label={label}
          disabled={disabled}
          bold={boldLabel}
          starred={starredLabel}
        />
        <StandardFieldContainer disabled={disabled}>
          <CoreDropdown
            id={id}
            labelId={labelId}
            value={value}
            options={options}
            placeholder={placeholder}
            disabled={disabled}
            onChange={onChange}
            renderChevron={menuOpen => (
              <span
                className={combine(classes.chevron, menuOpen && classes.chevronOpen)}
                aria-hidden
              />
            )}
            targetClassName={combine(classes.target, classes[height])}
            menuClassName={combine(classes.menu, classes[width])}
            menuTopClassName={classes.menuTop}
            menuBottomClassName={classes.menuBottom}
            menuOpenClassName={classes.menuOpen}
            optionClassName={combine(classes.menuOption, !sections && classes.borderBottom)}
            optionGroupClassName={combine(classes.menuOptionGroup, classes.borderBottom)}
            optionGroupLabelClassName={classes.menuOptionGroupLabel}
            focusedOptionClassName={classes.focusedOption}
            sections={sections}
            formatVisibleContent={formatVisibleContent}
            maxHeight={maxHeight}
            maxWidth={maxWidth}
            shouldReposition={shouldReposition}
            positionAbove={positionAbove}
          />
        </StandardFieldContainer>
      </label>
    </div>
  )
}

export {
  StandardDropdownField,
  useStyles,
}

export type {
  DropdownOption,
}
