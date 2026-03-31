import * as React from 'preact'
import {useState} from 'preact/hooks'

import poweredByImage from '../almosttheremodule/poweredby-horiz-white.svg'
import androidFallbackImage from './android-fallback.png'

const AndroidCopyLinkView = () => {
  const [previewImage] = useState(() => (
    document.querySelector('meta[name="og:image"]')?.getAttribute('content')
  ))

  const link = window.location.href.replace(/^https:\/\//, '')
  const intent = `intent://${link}#Intent;scheme=https;action=android.intent.action.VIEW;end;`

  return (
    <div id='linkOutViewAndroid' className='absolute-fill'>
      <div className='error-text-outer-container'>
        <div className='error-text-container error-margin-top-5'>
          <img
            alt=''
            id='app_img'
            className={`app-header-img unknown${previewImage ? '' : ' foreground-image'}`}
            src={previewImage || androidFallbackImage}
          />
          <br />
          <a id='open_browser_android' className='start-ar-button' href={intent}>Start AR</a>
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
