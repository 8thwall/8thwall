import React from 'react'
import {useTranslation} from 'react-i18next'
import {useDispatch} from 'react-redux'

import useSocket from '../common/use-socket'
import {mint, naeGradient} from '../static/styles/settings'
import {HTML_SHELL_TYPES} from '../../shared/nae/nae-constants'
import type {
  BuildNotificationData,
  HtmlShell,
} from '../../shared/nae/nae-types'
import {platformToLabel} from '../../shared/nae/nae-utils'
import {FloatingTrayButton} from '../ui/components/floating-tray-button'
import {createThemedStyles} from '../ui/theme'
import {RIGHT_PANEL_WIDTH} from './floating-right-panel'
import {NaeBuildStatus, usePublishingStateContext} from '../editor/publishing/publish-context'

const useStyles = createThemedStyles(theme => ({
  naeNotificationPanel: {
    position: 'absolute',
    bottom: '0.75rem',
    right: `calc(${RIGHT_PANEL_WIDTH}px + 11px)`,
    minWidth: '80px',
    maxWidth: '140px',
    pointerEvents: 'auto',
    display: 'flex',
    flexDirection: 'column',
    gap: '0.25rem',
    whiteSpace: 'nowrap',
  },
  notificationButton: {
    width: '100%',
    borderRadius: '0.5em',
    background: `${theme.studioBgMain}`,
  },
}))

const BUILD_STATE_CONFIG = {
  'building': {
    // TODO(akashmahesh): Using warning as a stand-in, color should be a transparent gradient
    // that matches the NAE build progress bar.
    color: 'warning',
    labelKey: 'scene_page.nae_notification.button.status_building',
    progressBarColor: naeGradient,
  },
  'success': {
    color: 'success',
    labelKey: 'scene_page.nae_notification.button.status_complete',
    progressBarColor: mint,
  },
  'failed': {
    color: 'danger',
    labelKey: 'scene_page.nae_notification.button.status_failed',
    progressBarColor: '',
  },
} as const

const NaeNotificationButton = () => {
  const {t} = useTranslation(['cloud-editor-pages', 'cloud-studio-pages'])
  const classes = useStyles()
  const dispatch = useDispatch()
  // TODO(christoph): Clean up
  const specifier = 'fake'
  const {
    htmlShellToNaeBuildData,
    setHtmlShellToNaeBuildData,
    setActivePublishModalOption,
  } = usePublishingStateContext()

  const handler = (msg: {action: string} & BuildNotificationData) => {
    const {action, status, platform} = msg
    switch (action) {
      case 'NAE_NEW_BUILD':
        setHtmlShellToNaeBuildData(platform, {
          naeBuildStatus: (status ? status.toLowerCase() as NaeBuildStatus : 'failed'),
          buildRequest: null,
          buildNotification: {...msg},
        })
        if (status !== 'SUCCESS') {
          // TODO(paris): Would be nicer to:
          // - If AndroidExportForm is open, don't use redux, just show this in the modal.
          // - If AndroidExportForm is closed, use this approach so that it shows up with the main
          //   studio <ErrorMessage />.
          dispatch({
            type: 'ERROR',
            msg: t('editor_page.error.build_failed', {
              platform,
              reason: msg?.errorMessage ? `: "${msg.errorMessage}"` : '',
            }),
          })
        }
        break
      default:
        break
    }
  }

  const onClick = (platform: HtmlShell) => {
    if (!HTML_SHELL_TYPES.includes(platform)) {
      setActivePublishModalOption('html')
      return
    }
    setActivePublishModalOption(platform)
  }

  useSocket(specifier, handler)

  return (
    <div className={classes.naeNotificationPanel}>
      {Object.entries(htmlShellToNaeBuildData).map(([platform, data]) => {
        const {naeBuildStatus} = data
        if (!naeBuildStatus || naeBuildStatus === 'noBuild') {
          return null
        }
        const {color, labelKey, progressBarColor} = BUILD_STATE_CONFIG[naeBuildStatus]
        return (
          <div key={platform} className={classes.notificationButton}>
            <FloatingTrayButton
              a8='click;studio;nae-notification-button'
              onClick={() => onClick(platform as HtmlShell)}
              color={color}
              fauxProgressBar={naeBuildStatus === 'building' || naeBuildStatus === 'success'}
              progressBarColor={progressBarColor}
            >
              {t(labelKey, {
                ns: 'cloud-studio-pages',
                platform: platformToLabel(platform as HtmlShell),
              })}
            </FloatingTrayButton>
          </div>
        )
      })}
    </div>
  )
}
export {NaeNotificationButton}
