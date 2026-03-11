import {brandBlack, persimmon} from '../../static/styles/settings'
import {createThemedStyles} from '../../ui/theme'

const useTreeHierarchyStyles = createThemedStyles(theme => ({
  treeHierarchyContainer: {
    display: 'flex',
    flexDirection: 'column',
    flexGrow: 1,
    height: '100%',
  },
  treeHierarchy: {
    'paddingBottom': '0.5rem',
    'margin': 0,
    'display': 'flex',
    'flexDirection': 'column',
    '&:last-child': {
      flexGrow: 1,
    },
  },
  scrollableContainer: {
    overflowY: 'auto',
    display: 'flex',
    flexDirection: 'column',
    flex: 1,
  },
  hovered: {
    outline: '1px solid #44a',
    outlineOffset: '-1px',
  },
  searchBarContainer: {
    'padding': '0.825rem 0.75rem 0 0.75rem',
    'display': 'flex',
    'gap': '0.5rem',
    '& > div': {
      minWidth: 0,
    },
  },
  treeHierarchyEmpty: {
    minHeight: '0.5rem',
    flexGrow: 1,
  },
  paddingTop: {
    paddingTop: '0.25rem',
  },
  isolationHeader: {
    'fontSize': '0.9rem',
    'fontWeight': 800,
    'padding': '0.6rem',
    'borderBottom': theme.studioSectionBorder,
    'display': 'flex',
    'alignItems': 'center',
    'justifyContent': 'center',
    'background': persimmon,
    'color': brandBlack,
    'position': 'relative',
    'userSelect': 'none',
  },
  isolationHeaderText: {
    textOverflow: 'ellipsis',
    whiteSpace: 'nowrap',
    overflow: 'hidden',
    maxWidth: '15rem',
  },
  backButton: {
    'position': 'absolute',
    'left': '0.5rem',
  },
  prefabName: {
    fontWeight: 600,
  },
}))

export {
  useTreeHierarchyStyles,
}
