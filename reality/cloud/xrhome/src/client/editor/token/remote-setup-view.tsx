import React from 'react'
import {createUseStyles} from 'react-jss'
import {Trans, useTranslation} from 'react-i18next'

import {SpaceBetween} from '../../ui/layout/space-between'
import CopyableLine from '../../widgets/copyable-line'
import {useLocalBuildUrl} from '../../studio/local-sync-context'
import {Loader} from '../../ui/components/loader'
import {gray5} from '../../static/styles/settings'
import {BasicQrCode} from '../../widgets/basic-qr-code'
import {hexColorWithAlpha} from '../../../shared/colors'
import {combine} from '../../common/styles'
import {Icon} from '../../ui/components/icon'
import {StandardRadioButton} from '../../ui/components/standard-radio-group'
import {StandardTextField} from '../../ui/components/standard-text-field'
import {Tooltip} from '../../ui/components/tooltip'
import {StandardLink} from '../../ui/components/standard-link'

const useStyles = createUseStyles({
  remoteSetupView: {
    'position': 'relative',
    'display': 'grid',
    'gridTemplateColumns': '20rem 20rem',
    'gap': '1rem',
  },
  leftColumn: {
    'marginLeft': '1rem',
    'marginTop': '1rem',
    'marginBottom': '1rem',
    'display': 'flex',
    'flexDirection': 'column',
    'gap': '1rem',
    'justifyContent': 'flex-end',
  },
  rightColumn: {
    display: 'flex',
    flexDirection: 'column',
    justifyContent: 'space-between',
  },
  qrImg: {
    display: 'block',
    margin: '2rem auto',
    width: '163px',
    height: '163px',
  },
  qrPlaceholder: {
    opacity: 0.5,
    filter: 'blur(6px)',
  },
  qrOverlayLink: {
    'cursor': 'pointer',
    'backgroundColor': hexColorWithAlpha(gray5, 0.3),
    'display': 'flex',
    'justifyContent': 'center',
    'alignItems': 'center',
    'transition': 'opacity 0.3s, background 0.3s',
    'padding': '0.25rem 0',
    '&:hover, &:focus-visible': {
      'backgroundColor': hexColorWithAlpha(gray5, 0.5),
      'opacity': 1,
    },
  },
})

const extractValidUrl = (url: string) => {
  if (!url) {
    return null
  }

  try {
    return new URL(url).href
  } catch {
    try {
      return new URL(`https://${url}`).href
    } catch {
      // Ignore
    }
  }

  return null
}

const useRemoteProxyUrl = () => {
  const [remoteProxyUrl, _setRemoteProxyUrl] = React.useState(() => (
    localStorage.getItem('remote-proxy-url') || ''
  ))

  const setRemoteProxyUrl = (newValue: string) => {
    localStorage.setItem('remote-proxy-url', newValue)
    _setRemoteProxyUrl(newValue)
  }

  return [remoteProxyUrl, setRemoteProxyUrl] as const
}

const useQrCodeMode = () => {
  const [qrCodeMode, _setQrCodeMode] = React.useState(() => (
    localStorage.getItem('qr-code-mode') || 'proxy'
  ))

  const setQrCodeMode = (newValue: string) => {
    localStorage.setItem('qr-code-mode', newValue)
    _setQrCodeMode(newValue)
  }

  return [qrCodeMode, setQrCodeMode] as const
}

const RemoteSetupView: React.FC = () => {
  const classes = useStyles()
  const {t} = useTranslation('cloud-editor-pages')

  const remoteDeviceUrl = useLocalBuildUrl('remote-device')
  const port = remoteDeviceUrl && new URL(remoteDeviceUrl).port

  const [remoteProxyUrl, setRemoteProxyUrl] = useRemoteProxyUrl()
  const [qrCodeMode, setQrCodeMode] = useQrCodeMode()

  const currentUrl = qrCodeMode === 'ip'
    ? remoteDeviceUrl
    : extractValidUrl(remoteProxyUrl)

  if (!port) {
    return <Loader>{t('remote_setup_view.initializing_server')}</Loader>
  }

  return (
    <div className={classes.remoteSetupView}>
      <div className={classes.leftColumn}>

        {qrCodeMode === 'proxy' &&
          <>
            <div>
              <Trans
                ns='cloud-editor-pages'
                i18nKey='remote_setup_view.recommend_ngrok'
                components={{
                  ngrokLink: <StandardLink newTab href='https://ngrok.com/download/' />,
                }}
              />
            </div>
            <CopyableLine
              // eslint-disable-next-line local-rules/hardcoded-copy
              text={`ngrok http ${port}`}
            />
            <div>
              {t('remote_setup_view.enter_url')}
            </div>
            {qrCodeMode === 'proxy' && <StandardTextField
              label={t('remote_setup_view.label.proxy_url')}
              value={remoteProxyUrl}
              height='small'
              placeholder='proxy-url.ngrok-example.dev'
              onChange={e => setRemoteProxyUrl(e.target.value)}
            />}
          </>}

        <StandardRadioButton
          label={t('remote_setup_view.option.proxy_server')}
          checked={qrCodeMode === 'proxy'}
          onChange={() => setQrCodeMode('proxy')}
        />

        <StandardRadioButton
          label={(
            <SpaceBetween narrow>
              {t('remote_setup_view.option.direct_ip')}
              <Tooltip
                zIndex={2000}
                content={t('remote_setup_view.warning.direct_ip')}
              >
                <Icon stroke='warning' color='darkMango' />
              </Tooltip>
            </SpaceBetween>
          )}
          checked={qrCodeMode === 'ip'}
          onChange={() => setQrCodeMode('ip')}
        />
      </div>
      <div className={classes.rightColumn}>

        {currentUrl
          ? (
            <a
              href={currentUrl}
              className={combine('style-reset', classes.qrOverlayLink)}
              target='_blank'
              rel='noreferrer'
            >
              <Trans
                ns='cloud-editor-pages'
                i18nKey='editor_page.dev_qr_code_popup.open_in_new_tab_2'
                components={{
                  icon: <Icon stroke='external' inline />,
                }}
              />
            </a>)
          : <span />}

        {currentUrl
          ? <BasicQrCode
              url={currentUrl}
              ecl='l'
              margin={0}
              className={classes.qrImg}
          />
          : <BasicQrCode
              url='not-a-real-qr-code'
              ecl='l'
              margin={0}
              className={combine(classes.qrImg, classes.qrPlaceholder)}
          />
          }

      </div>
    </div>
  )
}

export {
  RemoteSetupView,
}
