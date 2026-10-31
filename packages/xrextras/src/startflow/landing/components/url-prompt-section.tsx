import {h} from 'preact'
import {useState, useEffect} from 'preact/hooks'
import type {FunctionComponent as FC} from 'preact'
import * as QRCode from 'qrcode'

import type {LandingParameters} from '../parameters'
import styles from '../styles.module.scss'

import {UrlPromptLink} from './url-prompt-link'
import {FacebookMessengerButton} from './facebook-messenger-button'

type IUrlPromptSection = Pick<LandingParameters,
  'url' | 'promptPrefix' | 'promptSuffix' | 'vrPromptPrefix' | 'qrAlt' | 'messengerLabel'
>

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

const UrlPromptSection: FC<IUrlPromptSection> = ({
  url, promptPrefix, promptSuffix, vrPromptPrefix, qrAlt, messengerLabel,
}) => {
  const isVr = isOnOculus()
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
          alt={qrAlt}
        />
      }
      <div className={styles.promptText}>
        {isVr && <FacebookMessengerButton label={messengerLabel} url={url} />}
        {isVr ? vrPromptPrefix : promptPrefix }{' '}
        <UrlPromptLink url={url} />
        {' '}{promptSuffix}
      </div>
    </div>
  )
}

export {
  UrlPromptSection,
}
