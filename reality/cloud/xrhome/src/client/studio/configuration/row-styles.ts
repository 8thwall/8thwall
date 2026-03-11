import {createThemedStyles} from '../../ui/theme'

const DEFAULT_ROW_INLINE_PADDING = '1em'

const ROW_PADDING_VAR = '--row-padding-inline'

const ROW_PADDING = `var(${ROW_PADDING_VAR}, 0em)`

const useStyles = createThemedStyles(theme => ({
  row: {
    display: 'flex',
    flexDirection: 'row',
    marginBottom: '0.5em',
    alignItems: 'center',
    paddingInline: ROW_PADDING,
    position: 'relative',
  },
  reverseRow: {
    display: 'flex',
    flexDirection: 'row-reverse',
  },
  noMargin: {
    margin: '0 !important',
  },
  rowContentAligner: {
    marginBottom: '0.5em',
    paddingInline: ROW_PADDING,
    position: 'relative',
  },
  defaultPadding: {
    [ROW_PADDING_VAR]: DEFAULT_ROW_INLINE_PADDING,
  },
  noPadding: {
    [ROW_PADDING_VAR]: '0',
  },
  flexItem: {
    flex: 1,
  },
  flexItemGroup: {
    display: 'flex',
    alignItems: 'center',
    gap: '0.5em',
  },
  flexItemGroupSpaceBetween: {
    display: 'flex',
    alignItems: 'center',
    gap: '0.5em',
    justifyContent: 'space-between',
  },
  input: {
    'width': '100%',
    'padding': '0.5em',
    'marginLeft': '0.25em',
    'background': 'transparent',
    'border': 'none',
    'color': theme.fgMain,
    'appearance': 'textfield',
    'textOverflow': 'ellipsis',
    'textAlign': 'center',
    '&::placeholder': {
      'color': theme.fgMuted,
    },
    '&:focus-visible': {
      'outline': 'none',
    },
    '&::-webkit-inner-spin-button': {
      marginRight: '0.25em',
    },
    '&::-webkit-outer-spin-button': {
      marginRight: '0.25em',
    },
    '&::selection': {
      background: theme.sfcHighlight,
      color: theme.fgMain,
    },
  },
  select: {
    padding: '0.5em',
    paddingLeft: '0.75rem',
    fontFamily: 'inherit',
    background: 'transparent',
    boxSizing: 'border-box',
    border: 'none',
    outline: 'none',
    color: theme.fgMain,
    width: '100%',
    borderRight: '0.5rem solid transparent',  // This properly aligns the dropdown arrow
    appearance: 'none',
    WebkitAppearance: 'none',
    lineHeight: '1em',
    cursor: 'pointer',
  },
  selectMenuContainer: {
    maxHeight: '20rem',
    overflowY: 'auto',
  },
  disabledSelect: {
    color: theme.fgMuted,
    cursor: 'not-allowed',
  },
  chevron: {
    'borderRight': `1.5px solid ${theme.fgMuted}`,
    'borderBottom': `1.5px solid ${theme.fgMuted}`,
    'width': '0.5rem',
    'height': '0.5rem',
    'transform': 'translateY(-65%) rotate(45deg)',
    'position': 'absolute',
    'right': '1rem',
    'top': '50%',
    'pointerEvents': 'none',
    'borderRadius': '1px',
    'select:disabled + &': {
      'opacity': 0.5,
    },
  },
  checkmark: {
    marginRight: '-0.25rem',
  },
  selectOption: {
    display: 'flex',
    justifyContent: 'space-between',
    width: '100%',
  },
  selectText: {
    whiteSpace: 'nowrap',
    overflow: 'hidden',
    textOverflow: 'ellipsis',
    width: '90%',
  },
  swatch: {
    'cursor': 'pointer',
    'display': 'flex',
    'alignItems': 'center',
    'justifyContent': 'center',
    'height': '1.75rem',
    'width': '1.8rem',
    '&::-webkit-color-swatch': {
      border: 'none',
      borderRadius: '0.25rem',
    },
  },
  colorPickerContainer: {
    'display': 'flex',
    'alignItems': 'center',
    'padding': '0 0.25em',
    '& input': {
      textAlign: 'center',
    },
  },
  checkboxesRow: {
    display: 'flex',
    alignItems: 'center',
    flex: 1,
    gap: '0.5em',
  },
  checkboxLabel: {
    color: theme.fgMain,
  },
  rowGroup: {
    display: 'flex',
    flexDirection: 'row',
    alignItems: 'center',
    gap: '0.5em',
  },
  columnGroup: {
    display: 'flex',
    flexDirection: 'column',
    alignItems: 'center',
    gap: '0.5em',
  },
  indent: {
    '& $flexItem:first-child > span': {
      marginLeft: '10px',
    },
  },
  subInput: {
    'padding': '0.5em 0.25em',
  },
  preventOverflow: {
    maxWidth: '9.5em',
  },
  flexItemShrink: {
    flexShrink: 0,
  },
  textIconInputContainer: {
    display: 'flex',
    alignItems: 'center',
    padding: '0 0.5em',
    userSelect: 'none',
  },
  selectIconInputContainer: {
    display: 'flex',
    alignItems: 'center',
    paddingLeft: '0.5em',
    userSelect: 'none',
  },
  rowAlignTop: {
    alignItems: 'flex-start',
  },
  subMenuOption: {
    'padding': '0 0.5em',
  },
  optionText: {
    'color': theme.fgMain,
  },
  rowJointToggleButton: {
    boxShadow: `0 0 0 1px ${theme.sfcBorderDefault} inset`,
    borderRadius: '4px',
    backgroundColor: theme.mainEditorPane,
  },
  uploadContainer: {
    position: 'relative',
  },
  disabledOverlay: {
    position: 'absolute',
    top: 0,
    left: 0,
    right: 0,
    bottom: 0,
    zIndex: 10,
    cursor: 'not-allowed',
  },
}))

export {
  useStyles,
  DEFAULT_ROW_INLINE_PADDING,
  ROW_PADDING_VAR,
  ROW_PADDING,
}
