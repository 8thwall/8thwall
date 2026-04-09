import type {RunConfig} from './xr'
import html from '../../xrextras/src/almosttheremodule/almost-there-module.html'

import '../../xrextras/src/fonts/fonts.css'
import '../../xrextras/src/almosttheremodule/almost-there-module.css'

declare const XR8: any

let rootNode = null

const showId = (id) => {
  document.getElementById(id).classList.remove('hidden')
}

const hideAlmostThere = () => {
  if (!rootNode) {
    return
  }
  rootNode.parentNode.removeChild(rootNode)
  rootNode = null
}

// Returns true if it shows a view, false if the fallback views don't apply
const showAlmostThere = (runConfig: RunConfig, url: string): boolean => {
  hideAlmostThere()
  const e = document.createElement('template')
  e.innerHTML = html.trim()
  rootNode = e.content.firstChild
  document.getElementsByTagName('body')[0].appendChild(rootNode)

  const redirectUrl = url || window.location.href
  const redirectLinks = rootNode.querySelectorAll('.desktop-home-link')
  for (let i = 0; i < redirectLinks.length; i++) {
    redirectLinks[i].textContent = redirectUrl
  }

  const reasons = XR8.XrDevice.incompatibleReasons(runConfig)
  const details = XR8.XrDevice.incompatibleReasonDetails(runConfig)
  const device = XR8.XrDevice.deviceEstimate()

  const ogTag = document.querySelector('meta[property="og:image"]')
  const headerImgSrc = ogTag && ogTag.content
  Array.from(document.querySelectorAll('.app-header-img')).forEach((img) => {
    if (headerImgSrc) {
      img.src = headerImgSrc
    } else {
      img.classList.add('foreground-image')
      img.src = './external/xrextras-shared-resources/img/almostthere/v2/safari-fallback.png'
    }
  })

  const cBtn = document.getElementById('error_copy_link_btn')
  cBtn.addEventListener('click', () => {
    const dummy = document.createElement('input')
    document.body.appendChild(dummy)
    dummy.value = redirectUrl
    dummy.select()
    document.execCommand('copy')
    document.body.removeChild(dummy)

    cBtn.innerHTML = 'Copied!'
    cBtn.classList.add('error-copy-link-copied')
  })

  if (reasons.includes(XR8.XrDevice.IncompatibilityReasons.UNSUPPORTED_BROWSER)) {
    if (device.os === 'iOS') {
      if (details.inAppBrowserType === 'Safari') {
        showId('error_msg_open_in_safari')
      } else {
        switch (details.inAppBrowser) {
          case 'Instagram':
          case 'Facebook':
          case 'WeChat':
          case 'LinkedIn':
          case 'QQ':
          case 'Sino Weibo':
          case 'Snapchat':
            showId('error_msg_open_in_safari')
            showId('error_text_header_top')
            showId('top_corner_open_safari')
            if (details.inAppBrowser === 'Instagram') {
              document.body.classList.add('bottombarbump')
            }
            break
          case 'Facebook Messenger':
          case 'Kakao Talk':
          case 'Naver':
            showId('error_msg_open_in_safari')
            showId('error_text_header_bottom')
            showId('bottom_corner_open_safari')
            break
          case 'Line':
          case 'Mozilla Firefox Focus':
            showId('error_msg_open_in_safari')
            showId('error_text_header_top')
            showId('top_close_open_safari')
            break
          default:
            showId('error_unknown_webview')
            break
        }
      }
      return true
    }
  }

  if (reasons.includes(XR8.XrDevice.IncompatibilityReasons.MISSING_WEB_ASSEMBLY)) {
    if (device.os === 'iOS') {
      showId('error_msg_web_assembly_ios')
      return true
    }
    if (device.os === 'Android') {
      showId('error_msg_web_assembly_android')
      return true
    }
  }

  if (device.os === 'iOS') {
    showId('error_unknown_webview')
    showId('error_text_header_unknown')
    return true
  }

  if (device.os === 'Android') {
    showId('error_msg_android_almost_there')
    if (device.manufacturer === 'Huawei') {
      showId('error_msg_detail_huawei_almost_there')
    } else {
      showId('error_msg_detail_android_almost_there')
    }
    return true
  }

  return false
}

let didWarn = false

const showAlmostThereCollisionError = () => {
  if (didWarn) {
    return
  }
  // eslint-disable-next-line no-console
  console.error('[Landing Page] XRExtras Almost There should not be used with Landing Page.')
  didWarn = true
}

export {
  showAlmostThere,
  hideAlmostThere,
  showAlmostThereCollisionError,
}
