/* eslint-disable quote-props */
import React from 'react'
import {createUseStyles} from 'react-jss'
import {Dropdown} from 'semantic-ui-react'

import {gray3, brandPurple, twoPercentCherry, cherry} from '../../static/styles/settings'
import {combine} from '../../common/styles'
import FloatingLabel from './floating-label'
import {useIdFallback} from '../../studio/use-id-fallback'

const useStyles = createUseStyles({
  floatingLabelDropdown: {
    position: 'relative',
    display: 'block',
    zIndex: '1',
    '--input-left-spacing': '1em',
    '&.open': {
      zIndex: '10',
    },
    '& .ui.selection.dropdown': {
      padding: '1.5em 1em 0.5em',
      height: '3em',
      borderRadius: '0.5em',
      '--border-color': gray3,
      '&:hover': {
        boxShadow: 'inset 0 1px 3px #0002',
      },
      '&, & .menu': {
        borderColor: 'var(--border-color) !important',
      },
      '& .menu': {
        borderRadius: '0 0 0.5em 0.5em',
      },
      '&.upward .menu': {
        borderRadius: '0.5em 0.5em 0 0 ',
      },
      '&.active': {
        boxShadow: 'none',
        filter: 'drop-shadow(0 1px 3px #0003)',
        '--border-color': brandPurple,
        '&.menu': {
          boxShadow: 'none',
        },
      },
      '& .dropdown.icon': {
        padding: '1em',
      },
    },
    '&.error .ui.selection.dropdown': {
      backgroundColor: twoPercentCherry,
      '--border-color': cherry,
    },
  },
})

interface IFloatingLabelDropdown {
  label: string
  id?: string
  errorText?: string
  value: string
  options: {key: any, value: string, text: React.ReactNode}[]
  onChange: (event: React.SyntheticEvent<HTMLElement, Event>, target: {value: string}) => void
  onOpen: () => void
  onClose: () => void
}

const FloatingLabelDropdown: React.FunctionComponent<IFloatingLabelDropdown> =
  ({label, id: idOverride, errorText, value, onOpen, onClose, ...rest}) => {
    const {floatingLabelDropdown} = useStyles()
    const id = useIdFallback(idOverride)
    const [isOpen, setIsOpen] = React.useState(false)

    const handleOpen = () => {
      setIsOpen(true)
      onOpen?.()
    }

    const handleClose = () => {
      setIsOpen(false)
      onClose?.()
    }

    const labelsFloated = !!(value || isOpen || errorText)
    const classes = combine(floatingLabelDropdown, isOpen && 'open', errorText && 'error')

    return (
      <label htmlFor={id} className={classes}>
        <FloatingLabel floated={labelsFloated} visible={!errorText}>{label}</FloatingLabel>
        <FloatingLabel
          error
          floated={labelsFloated}
          visible={!!errorText}
        >{errorText}
        </FloatingLabel>
        <Dropdown
          id={id}
          fluid
          selection
          value={value}
          {...rest}
          onOpen={handleOpen}
          onClose={handleClose}
        />
      </label>
    )
  }

export default FloatingLabelDropdown
