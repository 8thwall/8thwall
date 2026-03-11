import {createThemedStyles} from '../ui/theme'

const ASSET_LAB_BUTTON_BAR_MARGIN_REM = '0.5rem'
const GEN_RESULTS_CONTENT_PADDING_REM = '0.6rem'
const GEN_RESULTS_MARGIN_LEFT_REM = '1rem'
const useStyles = createThemedStyles(theme => ({
  assetGenSplit: {
    display: 'flex',
    flexDirection: 'row',
    flexGrow: 1,
    overflowY: 'hidden',
  },
  genForm: {
    display: 'flex',
    flexDirection: 'column',
    // This is related to genResults flex attribute.
    flex: '1 1 0',
    borderRadius: '1rem',
    border: `1px solid ${theme.studioAssetBorder}`,
    padding: '1rem',
    lineHeight: '20px',
    overflowY: 'auto',
    gap: '0.62rem',
  },
  genResults: {
    display: 'flex',
    flex: '1 1 0',
    marginLeft: GEN_RESULTS_MARGIN_LEFT_REM,
    flexDirection: 'column',
    borderRadius: '1rem',
    userSelect: 'none',
    gap: '1rem',
    overflowY: 'auto',
  },
  genResultsContent: {
    flex: '0 0 auto',
    background: theme.mainEditorPane,
    padding: GEN_RESULTS_CONTENT_PADDING_REM,
    borderRadius: '1rem',
  },
  generateRow: {
    display: 'flex',
    flexDirection: 'row',
    alignItems: 'center',
    justifyContent: 'flex-end',
  },
  generateRowLeft: {
    flex: '1 1 auto',
    display: 'flex',
    flexDirection: 'row',
    gap: '0.5rem',
  },
  animationClipControls: {
    'display': 'flex',
    'flexDirection': 'row',
    'flex': ' 1 1 auto',
    '& > button': {
      padding: '6px',
    },
    '& div[role="combobox"]': {
      padding: '0.5rem 1.75rem 0.5rem 0.75rem',
      minHeight: 'auto',
      minWidth: '10rem',
      backgroundColor: theme.sfcBackgroundDefault,
    },
    '& div[role="option"]': {
      'paddingLeft': '1.5rem',
      'whiteSpace': 'nowrap',
      '&[aria-selected=true]:after': {
        right: 'auto',
        left: '0.25rem',
      },
    },
  },
  estimateCreditUsed: {
    flex: '0 0 auto',
  },
  generateButton: {
    'flex': '0 0 auto',
    'display': 'flex',
    'flexDirection': 'row',
    'gap': '0.5rem',
    'alignItems': 'center',
    '& > button': {
      display: 'flex',
      flexDirection: 'row',
    },
  },
  guidanceSliderCombo: {
    display: 'flex',
    flexDirection: 'row',
    gap: '1rem',
    alignItems: 'center',
  },
  modelParameterRow: {
    maxWidth: '30rem',
  },
  buttonBar: {
    'display': 'flex',
    'flexDirection': 'row',
    'justifyContent': 'flex-end',
    'gap': '0.5rem',
    'marginTop': ASSET_LAB_BUTTON_BAR_MARGIN_REM,
    '& button + button': {
      marginLeft: ASSET_LAB_BUTTON_BAR_MARGIN_REM,
    },
  },
  inlineToggleContainer: {
    'display': 'flex',
    'flexDirection': 'row',
    '& > *': {
      flex: '0 1 auto !important',
    },
    '& button': {
      padding: '0.25rem 0.5rem',
    },
  },
  iconToggleLabel: {
    'display': 'flex',
    'alignItems': 'center',
    'flexDirection': 'row',
    'marginLeft': '0.25rem',
    '& > svg': {
      marginRight: '0.5rem',
    },
  },
  strokeIcon: {
    '& > svg': {
      stroke: 'white',  // TODO(dat): need theme
      fill: 'none',
      strokeWidth: '0.75px',
    },
  },
  // For showing a StudioModelPreviewWithLoader on the input left side
  modelContainer: {
    'borderRadius': '0.5rem',
    'backgroundColor': 'transparent',
    'margin': '0.5rem 0',
    'display': 'flex',
    'overflow': 'hidden',
    'justifyContent': 'center',
    'height': '360px',
    'width': '360px',
    'position': 'relative',
  },
  modelContainerOutput: {
    'borderRadius': '0.5rem',
    'backgroundColor': theme.sfcBackgroundDefault,
    'margin': '0.5rem 0',
    'display': 'flex',
    'overflow': 'hidden',
    'justifyContent': 'center',
    'height': '800px',
    'position': 'relative',
  },
  staticBanner: {
    borderRadius: '0.5rem',
  },
  flex4: {
    flex: '4 1 0',
  },
  flex6: {
    flex: '6 1 0',
  },
  tertiaryButton: {
    '& > button': {
      'background': theme.tertiaryBtnFg,
      'border': `1px solid ${theme.secondaryBtnHoverBg}`,
    },
  },
  buttonLink: {
    'borderRadius': '0.5rem',
    'backgroundColor': theme.studioPanelBtnBg,
    'border': 'none',
    'padding': '0.4rem 0.75rem',
    'color': theme.fgMain,
    'cursor': 'pointer',
    'display': 'inline-block',
    'textAlign': 'center',
    '&:hover': {
      color: theme.fgMain,
      background: theme.studioPanelBtnHoverBg,
    },
  },
}))

export {
  useStyles as useAssetLabStyles,
  ASSET_LAB_BUTTON_BAR_MARGIN_REM,
  GEN_RESULTS_CONTENT_PADDING_REM,
  GEN_RESULTS_MARGIN_LEFT_REM,
}
