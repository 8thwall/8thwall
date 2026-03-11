import {createThemedStyles} from '../../../ui/theme'
import {ROW_PADDING} from '../row-styles'

const useStyles = createThemedStyles(theme => ({
  groupHeaderRow: {
    'display': 'flex',
    'width': '100%',
    'gap': '1em',
    'alignItems': 'center',
    'justifyContent': 'space-between',
    'overflow': 'hidden',
    'fontSize': '12px',
    'padding': `0.2rem ${ROW_PADDING}`,
    'color': theme.fgMain,
    'cursor': 'pointer',
    'user-select': 'none',
  },
  deleteButton: {
    'padding': '0 0.5em',
    'display': 'flex',
    'color': theme.fgMuted,
    'background': 'transparent',
    'border': 'none',
    'cursor': 'pointer',
    '&:hover': {
      color: theme.fgMain,
    },
  },
  iconButton: {
    'width': '26px',
    'height': '26px',
    'display': 'flex',
    'justifyContent': 'center',
    'alignItems': 'center',
    'cursor': 'pointer',
    'overflow': 'hidden',
    'borderRadius': '0.25em',
    'color': theme.fgMuted,
    'position': 'relative',
    'boxSizing': 'border-box',
    'background': theme.sfcBackgroundDefault,
    '&:focus': {
      outline: 'none',
      boxShadow: '0 0 0px 1px #007BFF',
    },
  },
  columnContainer: {
    display: 'flex',
    flexDirection: 'column',
    gap: '0.5em',
    width: '100%',
  },
  wrapContent: {
    padding: '.4em 0 0 0',
  },
}))

export {
  useStyles as useUiConfiguratorStyles,
}
