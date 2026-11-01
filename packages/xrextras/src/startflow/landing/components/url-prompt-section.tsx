import {h} from 'preact'
import {useState, useEffect} from 'preact/hooks'
import type {FunctionComponent as FC} from 'preact'
import * as QRCode from 'qrcode'

import type {StartFlowContentProps} from '../../types'
import styles from '../styles.module.scss'

import {UrlPromptLink} from './url-prompt-link'
import {FacebookMessengerButton} from './facebook-messenger-button'

const isOnOculus = () => {
  try {
    return window.navigator.userAgent.match(/Pacific/) ||  // Oculus Go
      window.navigator.userAgent.match(/Quest/)  // Oculus Quest
  } catch {
    return false
  }
}

const makeQrCode = async (url: string) => {
  try {
    const dataUrl = await QRCode.toDataURL(url, {
      type: 'svg',
      width: 250,
      margin: 2,
      errorCorrectionLevel: 'M',
      color: {
        dark: '#000000',
        light: '#FFFFFF',
      },
    })
    return dataUrl
  } catch (err) {
    // eslint-disable-next-line no-console
    console.error('Error generating QR code:', err)
    return ''
  }
}

const UrlPromptSection: FC<StartFlowContentProps> = ({options, strings}) => {
  const isVr = isOnOculus()
  const url = options.url ?? ''
  const [qrCodeDataUrl, setQrCodeDataUrl] = useState<string>('')

  useEffect(() => {
    const generateQrCode = async () => {
      const dataUrl = await makeQrCode(url)
      setQrCodeDataUrl(dataUrl)
    }
    generateQrCode()
  }, [url])

  return (
    <div className={styles.promptSection}>
      {!isVr &&
        <img
          className={styles.qr}
          src={qrCodeDataUrl}
          alt={strings.landingQrAlt}
        />
      }
      <div className={styles.promptText}>
        {isVr && <FacebookMessengerButton label={strings.landingMessengerLabel} url={url} />}
        {isVr ? strings.landingVrPromptPrefix : strings.landingPromptPrefix }{' '}
        <UrlPromptLink url={url} />
        {' '}{strings.landingPromptSuffix}
      </div>
    </div>
  )
}

export {
  UrlPromptSection,
}
