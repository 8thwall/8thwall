import {createUseStyles} from 'react-jss'

import {brandBlack, brandWhite, gray2} from '../../static/styles/settings'

const useStyles = createUseStyles({
  optionsDropdown: {
    cursor: 'pointer',
    outline: 0,
    top: '100%',
    minWidth: 'max-content',
    margin: 0,
    padding: '0 0',
    background: brandWhite,
    fontSize: '1em',
    textShadow: 'none',
    textAlign: 'left',
    boxShadow: '0 2px 3px 0 rgba(34,36,38,.15)',
    border: '1px solid rgba(34,36,38,.15)',
    borderRadius: '0.28571429rem',
    transition: 'opacity .1s ease',
    zIndex: '11',
  },
  optionItem: {
    'cursor': 'pointer',
    'borderRadius': '4px',
    'fontSize': '12px',
    'padding': '0.5rem',
    'userSelect': 'none',
    'display': 'block',
    'width': '100%',
    'color': brandBlack,
    '&:hover': {
      background: gray2,
    },
  },
  listItem: {
    'listStyle': 'none',
  },
})

export {useStyles as useOptionsDropdownStyles}
