import {h} from 'preact'
import type {FunctionComponent} from 'preact'
import {useEffect, useState} from 'preact/hooks'
import * as QRCode from 'qrcode'

import styles from '../styles.module.scss'
import type {StartFlowStrings} from '../../types'

const COPY_ICON_PATH = 'M5 7h9a2 2 0 0 1 2 2v11a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2V9' +
  'a2 2 0 0 1 2-2Zm3 0V4a2 2 0 0 1 2-2h9a2 2 0 0 1 2 2v11a2 2 0 0 1-2 2'
const MESSENGER_ICON_PATH = 'M12 0C5.4 0 0 5 0 11.1c0 3.5 1.7 6.6 4.5 8.7V24l4.1-2.2' +
  'c1.1.3 2.2.4 3.4.4 6.6 0 12-5 12-11.1S18.6 0 12 0Zm1.2 15-3.1-3.3L4.2 15' +
  'l6.5-7 3.2 3.3L19.8 8l-6.6 7Z'

const PromptLink: FunctionComponent<{url: string}> = ({url}) => {
  const [copied, setCopied] = useState(false)
  const text = url.replace(/^https:\/\//, '')
  const copy = async (event: MouseEvent) => {
    event.preventDefault()
    try {
      await navigator.clipboard.writeText(url)
    } catch {
      const input = document.createElement('input') as unknown as HTMLInputElement
      document.body.appendChild(input)
      input.value = url
      input.select()
      document.execCommand('copy')
      input.remove()
    }
    setCopied(true)
  }
  return (
    <a className={styles.promptLink} href={url} onClick={copy}>
      <span className={styles.linkPlaceholder}>{text}</span>
      <span className={styles.linkOverflow} aria-hidden='true'>
        <span className={styles.linkOverflowText}>
          {text}
          <svg className={styles.copyIcon} width='24' height='24' viewBox='0 0 24 24'>
            <path
              d={COPY_ICON_PATH}
              fill='none'
              stroke='currentColor'
              strokeWidth='2'
              strokeLinecap='round'
            />
            {copied && <path
              d='m6 16 2 2 5-5'
              fill='none'
              stroke='#080'
              strokeWidth='2'
              strokeLinecap='round'
              strokeLinejoin='round'
            />}
          </svg>
        </span>
      </span>
    </a>
  )
}

const LandingPrompt: FunctionComponent<{
  strings: StartFlowStrings
  url: string
}> = ({url, strings}) => {
  const [qr, setQr] = useState('')
  const isVr = /Pacific|Quest/.test(window.navigator?.userAgent ?? '')
  useEffect(() => {
    let active = true
    const generateQr = async () => {
      try {
        const value = await QRCode.toString(url, {
          type: 'svg',
          width: 250,
          margin: 2,
          errorCorrectionLevel: 'M',
          color: {dark: '#000000', light: '#FFFFFF'},
        })
        if (active) {
          setQr(`data:image/svg+xml,${encodeURIComponent(value)}`)
        }
      } catch {
        setQr('')
      }
    }
    generateQr()
    return () => { active = false }
  }, [url])
  return (
    <div className={styles.promptSection}>
      {!isVr && <img
        className={styles.qr}
        src={qr}
        alt={strings.landingQrAlt}
      />}
      <div className={styles.promptText}>
        {isVr && (
          <a
            className={styles.messengerButton}
            href={`https://www.facebook.com/dialog/send?${new URLSearchParams({
              link: url,
              redirect_uri: url,
              app_id: 'org.8thwall',
            })}`}
          >
            <svg viewBox='0 0 24 24' aria-hidden='true'>
              <path fill='currentColor' d={MESSENGER_ICON_PATH} />
            </svg>
            {strings.landingMessengerLabel}
          </a>
        )}
        {isVr
          ? strings.landingVrPromptPrefix
          : strings.landingPromptPrefix}{' '}
        <PromptLink url={url} />{' '}{strings.landingPromptSuffix}
      </div>
    </div>
  )
}

export {LandingPrompt}
