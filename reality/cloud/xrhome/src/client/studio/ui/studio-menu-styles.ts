import {createThemedStyles} from '../../ui/theme'

const useStyles = createThemedStyles(theme => ({
  studioMenu: {
    'fontSize': '12px',
    'backgroundColor': theme.studioBgOpaque,
    'border': theme.studioSectionBorder,
    'display': 'flex',
    'flexDirection': 'column',
    'borderRadius': '0.5em',
    'width': 'max-content',
    'padding': '0.5rem',
    'gap': '0.25rem',
    'marginBottom': '0.5rem',
    '&::-webkit-scrollbar, & *::-webkit-scrollbar': {
      width: '6px',
    },
    '&::-webkit-scrollbar-track, & *::-webkit-scrollbar-track': {
      backgroundColor: theme.scrollbarTrackBackground,
      borderRadius: '6px',
    },
    '&::-webkit-scrollbar-thumb, & *::-webkit-scrollbar-thumb': {
      'backgroundColor': theme.scrollbarThumbColor,
      '&:hover': {
        backgroundColor: theme.scrollbarThumbHoverColor,
      },
    },
  },
  divider: {
    borderBottom: theme.studioSectionBorder,
    margin: '0.5rem',
  },
  chevronIcon: {
    'transform': 'none',
    '& svg': {
      width: '0.75em',
      height: '0.75em',
    },
  },
  openIcon: {
    transform: 'rotate(-180deg)',
  },
  menuItemContainer: {
    'position': 'relative',
    'borderRadius': '0.25em',
    '&:hover': {
      'color': theme.studioBtnHoverFg,
      'background': theme.studioBtnHoverBg,
      '& $iconButtons': {
        visibility: 'visible',
      },
      '& $checkmark': {
        visibility: 'hidden',
      },
    },
    '&:focus-within': {
      '& $iconButtons': {
        visibility: 'visible',
      },
    },
  },
  menuItem: {
    'fontFamily': 'inherit',
    'lineHeight': '16px',
    'display': 'flex',
    'gap': '0.25em',
    'width': '100%',
    'alignItems': 'center',
    'justifyContent': 'flex-start',
    'padding': '0.25rem 0.5rem',
    'color': theme.fgMain,
    'cursor': 'pointer',
    'textOverflow': 'ellipsis',
    'userSelect': 'none',
    '&:focus-visible': {
      'color': theme.studioBtnHoverFg,
      'background': theme.studioBtnHoverBg,
    },
  },
  iconContainer: {
    position: 'absolute',
    top: 0,
    padding: 0,
    right: 0,
    display: 'flex',
    gap: 0,
    alignItems: 'center',
  },
  checkmark: {
    padding: '0.25rem',
  },
  iconButtons: {
    visibility: 'visible',
  },
  activeIconButton: {
    visibility: 'hidden',
  },
  studioFont: {
    fontSize: '12px !important',
    fontFamily: 'inherit !important',
  },
}))

export {useStyles as useStudioMenuStyles}
