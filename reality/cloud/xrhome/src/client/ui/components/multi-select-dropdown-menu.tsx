import React from 'react'
import {createUseStyles} from 'react-jss'
import {Dropdown, Icon} from 'semantic-ui-react'
import type {DeepReadonly as RO} from 'ts-essentials'

import {gray1, gray5} from '../../static/styles/settings'

const useStyles = createUseStyles({
  'dropdownTrigger': {
    '&': {
      marginRight: '2em',
    },
    '& > .options-selected': {
      fontWeight: 'bolder',
      color: gray5,
    },
  },
  'dropdown': {
    '& .dropdownMenu': {
      '& > .item': {
        textAlign: 'left',
      },
      '& > .item:hover': {
        background: `${gray1} !important`,
      },
    },
    '& .dropdownIcon': {
      opacity: '0 !important',
      transition: 'opacity .25s ease-in-out',
    },
    '&:hover .dropdownIcon': {
      opacity: '1 !important',
    },
  },
})

type MultiSelectDropdownMenuOption = {
  value: string
  content: React.ReactNode
  shortContent?: string
}
interface Props {
  label: string
  options: RO<MultiSelectDropdownMenuOption[]>
  value: RO<string[]>
  onChange: (value: string) => void
  placeholder?: string
  allSelectedText?: string
}
interface RawTriggerProps {
  label: string
  subtext: React.ReactNode
}

const RawTrigger: React.FC<RawTriggerProps> = ({label, subtext}) => {
  const classes = useStyles()
  return (
    <div className={classes.dropdownTrigger}>
      <span>{label}</span>{' '}
      <span className='options-selected'>{subtext}</span>
    </div>
  )
}

const Trigger: React.FC<Omit<Props, 'onChange'>> = (
  {label, options, value, allSelectedText = null, placeholder = null}
) => {
  const listText = options.filter(option => value.includes(option.value)).map(
    ({shortContent, content}) => (shortContent || content)
  ).join(', ')
  const enabledText = (value.length === options.length && allSelectedText) || listText
  const subtext = (value.length === 0 && placeholder) || enabledText
  const withFadedTriangle = (
    <span>{subtext}<Icon name='triangle down' className='dropdownIcon' /></span>
  )
  return <RawTrigger label={label} subtext={withFadedTriangle} />
}

const MultiSelectDropdownMenu: React.FC<Props> = (
  {label, options, value, onChange, placeholder, allSelectedText}
) => {
  const classes = useStyles()

  const handleOptionClick = (option: MultiSelectDropdownMenuOption) => {
    onChange(option.value)
  }

  return (
    <Dropdown
      className={classes.dropdown}
      trigger={(
        <Trigger
          label={label}
          options={options}
          value={value}
          allSelectedText={allSelectedText}
          placeholder={placeholder}
        />
      )}
      icon={null}
    >
      <Dropdown.Menu className='dropdownMenu'>
        {options.map(option => (
          <Dropdown.Item
            key={option.value}
            text={option.content}
            onClick={() => handleOptionClick(option)}
            icon={value.includes(option.value) ? 'check' : ''}
          />
        ))}
      </Dropdown.Menu>
    </Dropdown>
  )
}

export {
  MultiSelectDropdownMenu,
  RawTrigger,
}
