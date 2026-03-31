import * as React from 'preact'

import {useState} from 'preact/hooks'

import poweredByImage from '../almosttheremodule/poweredby-horiz-white.svg'

const AndroidCopyLinkView = ({link}: {link: string}) => {
  const [copied, setCopied] = useState(false)

  return (
    <div id='copyLinkViewAndroid' className='absolute-fill hidden'>
      <div className='error-text-outer-container'>
        <div className='error-text-container error-margin-top-5'>
          <span id='error_text_header_unknown' className='open-header-unknown'>
            <h2>Open in Browser<br /> to view AR</h2>
          </span>
          <img alt='' id='app_img' className='app-header-img unknown' />
          <br />
          <span id='app_link' className='desktop-home-link mobile' />
          <button
            type='button'
            id='copy_link_android'
            onClick={() => {
              const dummy = document.createElement('input')
              document.body.appendChild(dummy)
              dummy.value = link
              dummy.select()
              document.execCommand('copy')
              document.body.removeChild(dummy)
              setCopied(true)
            }}
            className={`copy-link-btn${copied ? ' error-copy-link-copied' : ''}`}
          >{copied ? 'Copied!' : 'Copy Link'}
          </button>
          <img
            alt='Powered by 8th Wall'
            className='foreground-image poweredby-img'
            src={poweredByImage}
          />
        </div>
      </div>
    </div>
  )
}

export {
  AndroidCopyLinkView,
}
