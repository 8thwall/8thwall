import {createThemedStyles} from '../../../ui/theme'
import {blueberry, smallMonitorViewOverride} from '../../../static/styles/settings'
import {hexColorWithAlpha} from '../../../../shared/colors'

const useStyles = createThemedStyles(theme => ({
  flexCol: {
    flexDirection: 'column',
  },
  columnsContent: {
    display: 'flex',
    width: '100%',
    gap: '1.5rem',
    height: '100%',
    position: 'relative',
    paddingBottom: '1rem',
    [smallMonitorViewOverride]: {
      flexDirection: 'column',
    },
  },
  dimmer: {
    position: 'absolute',
    top: 0,
    left: 0,
    right: 0,
    bottom: 0,
    backgroundColor: hexColorWithAlpha(theme.publishModalBg, 0.65),
    zIndex: 1,
    pointerEvents: 'auto',
    userSelect: 'none',
  },
  leftCol: {
    display: 'flex',
    flex: '0 0 auto',
    flexDirection: 'column',
    alignItems: 'flex-start',
    gap: '0.5rem',
    alignSelf: 'stretch',
    width: '200px',
    [smallMonitorViewOverride]: {
      display: 'none',
    },
  },
  smallMonitorView: {
    display: 'none',
    [smallMonitorViewOverride]: {
      display: 'flex',
      flexDirection: 'column',
      gap: '1rem',
      alignItems: 'flex-start',
      alignSelf: 'stretch',
    },
  },
  smallMonitorHidden: {
    [smallMonitorViewOverride]: {
      display: 'none',
    },
  },
  smallMonitorVisible: {
    display: 'none',
    [smallMonitorViewOverride]: {
      display: 'inline',
    },
  },
  rightCol: {
    position: 'relative',
    display: 'flex',
    flexDirection: 'column',
    gap: '1.5rem',
    flex: '2 1 auto',
  },
  inputFields: {
    display: 'flex',
    flexDirection: 'column',
    alignItems: 'flex-start',
    gap: '0.75rem',
    alignSelf: 'stretch',
    maxWidth: '100%',
  },
  iconAndTextContainer: {
    display: 'flex',
    gap: '0.5rem',
    alignItems: 'center',
  },
  displayText: {
    fontSize: '12px',
  },
  error: {
    color: theme.fgError,
  },
  upgrade: {
    color: blueberry,
  },
  toggleTooltip: {
    fontSize: '12px',
  },
  exportType: {
    zIndex: 1000,
  },
  exportTypeDropdown: {
    maxWidth: '18.5rem',
    gap: '0.5rem',
    display: 'flex',
    alignItems: 'center',
    justifyContent: 'space-between',
    minWidth: '100%',
    borderRadius: '0.375rem',
  },
  exportTypeContent: {
    display: 'flex',
    flexDirection: 'column',
    flex: 1,
  },
  exportTypeDropdownDescription: {
    color: theme.fgMuted,
  },
  checkmark: {
    marginLeft: '0.5rem',
    flexShrink: 0,
  },
  rowLabel: {
    '& span': {
      color: theme.fgMuted,
    },
  },
  unsupportedExportList: {
    display: 'flex',
    flexDirection: 'column',
    gap: '0.5rem',
    width: '100%',
    maxHeight: '20%',
  },
  unsupportedExportsIntro: {
    fontSize: '12px',
    lineHeight: '16px',
    color: theme.fgMuted,
  },
  exportItemGrid: {
    display: 'grid',
    gridTemplateColumns: '1fr 1fr',
  },
  exportItem: {
    display: 'flex',
    alignItems: 'center',
    gap: '0.5rem',
  },
  exportItemText: {
    fontSize: '12px',
    lineHeight: '20px',
    color: theme.fgMuted,
  },
  unsupportedExportLearnMoreLinkContainer: {
    display: 'flex',
    flexDirection: 'column',
    justifyContent: 'flex-start',
    alignItems: 'flex-start',
    gap: '0.5rem',
    alignSelf: 'stretch',
    height: '1.25rem',
  },
  configurePageHelperText: {
    display: 'flex',
    flexDirection: 'column',
    gap: '0.5rem',
  },
  learnMoreText: {
    display: 'flex',
    alignItems: 'center',
    gap: '0.5rem',
    alignSelf: 'stretch',
    color: theme.fgMuted,
    fontSize: '12px',
    lineHeight: '20px',
  },
  whiteSpaceNoWrap: {
    whiteSpace: 'nowrap',
  },
  downloadPage: {
    flex: '1 0 0',
    alignSelf: 'stretch',
    display: 'flex',
    flexDirection: 'column',
    justifyContent: 'center',
    alignItems: 'center',
  },
  appCompletedPane: {
    display: 'flex',
    width: '23rem',
    padding: '1rem',
    flexDirection: 'column',
    alignItems: 'center',
    gap: '1.5rem',
    borderRadius: '0.5rem',
    border: `1px solid ${theme.subtleBorder}`,
    background: theme.publishModalBg,
    boxShadow:
      '10px 10px 100px -10px rgba(173, 80, 255, 0.20), ' +
      '-10px -10px 85px -5px rgba(87, 191, 255, 0.10)',
  },
  downloadAppContainer: {
    display: 'flex',
    justifyContent: 'center',
    alignItems: 'flex-start',
    alignSelf: 'stretch',
  },
  downloadPageAppIcon: {
    display: 'flex',
    flexDirection: 'column',
    alignItems: 'flex-start',
    justifyContent: 'space-between',
    flex: '1 0 0',
    height: '100%',
    gap: '0.5rem',
  },
  appIconImg: {
    width: '5rem',
    height: '5rem',
    aspectRatio: '1/1',
    border: `1px solid ${theme.subtleBorder}`,
    borderRadius: '0.5rem',
  },
  buildDownloadButton: {
    width: '13.75rem',
  },
  downloadPageApp: {
    display: 'flex',
    flexDirection: 'column',
    alignItems: 'flex-start',
    gap: '0.125rem',
  },
  downloadPageAppDisplayName: {
    fontWeight: 600,
    fontSize: '12px',
    lineHeight: '16px',
    wordBreak: 'break-word',
    overflowWrap: 'break-word',
  },
  downloadPageFileName: {
    fontSize: '12px',
    lineHeight: '18px',
    wordBreak: 'break-word',
    overflowWrap: 'break-word',
  },
  downloadPageMetadata: {
    display: 'flex',
    flexDirection: 'column',
    paddingLeft: '1rem',
    justifyContent: 'center',
    gap: '0.5rem',
    flex: '1 0 0',
    borderLeft: `1px solid ${theme.subtleBorder}`,
  },
  downloadPageMetadataText: {
    'flex': '1 0 0',
    'fontSize': '12px',
    'color': theme.fgMuted,
    'fontStyle': 'italic',
    'lineHeight': '1.25rem',
    '& span': {
      display: 'block',
    },
  },
  downloadPageBanners: {
    width: '100%',
    display: 'flex',
    flexDirection: 'column',
    alignItems: 'flex-start',
    justifyContent: 'center',
    gap: '1rem',
    paddingTop: '3rem',
  },
  continueButton: {
    width: '5.5rem',
  },
  buildButton: {
    'display': 'flex',
    'alignItems': 'center',
    'paddingRight': '4px',
    '& > span': {
      display: 'flex',
      alignItems: 'center',
      gap: '8px',
    },
  },
  justifyBetween: {
    display: 'flex',
    justifyContent: 'space-between',
  },
  alignCenter: {
    display: 'flex',
    alignItems: 'center',
  },
  gap05: {
    gap: '0.5rem',
  },
  gap0375: {
    gap: '0.375rem',
  },
  textMuted: {
    color: theme.fgMuted,
  },
  hoverTextMain: {
    '&:hover': {
      color: theme.fgMain,
    },
  },
}))

export {
  useStyles as useNaeStyles,
}
