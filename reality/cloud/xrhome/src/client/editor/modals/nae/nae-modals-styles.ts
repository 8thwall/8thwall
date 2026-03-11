import {createThemedStyles} from '../../../ui/theme'

const useStyles = createThemedStyles(theme => ({
  inputGroup: {
    'width': '100%',
    'display': 'flex',
    'flex-direction': 'column',
    'text-align': 'left',
    'gap': '0.5rem',
    '>*': {
      'margin': '0.5em',
    },
    '& .field > label': {
      color: `${theme.publishModalText} !important`,
    },
    '& label': {
      '& > div:first-child': {
        flex: '0 0 auto',
        minWidth: '8rem',
        maxWidth: '8rem',
        marginRight: '1.5rem',
      },
      '& > div:last-child': {
        flex: '1 1 auto',
      },
    },
    '& button[type="button"]': {
      maxWidth: 'none !important',
      height: '24px',
      fontSize: '12px',
    },
    '& input[type="text"]': {
      height: '24px',
      fontSize: '12px',
    },
    '& .selectText': {
      width: '100% !important',
      maxWidth: 'none !important',
      overflow: 'visible !important',
      textOverflow: 'clip !important',
    },
    '& input': {
      textAlign: 'left !important',
    },
  },
}))

export {
  useStyles,
}
