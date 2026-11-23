import SHARE_SRC from './share.svg'
import type {MediaPreviewOptions, PreviewActions} from './media-preview-types'
import type {MediaRecorderStrings} from './strings'
import VIEW_SRC from './view.svg'

const WK_WEBVIEW_IOS_BROWSERS = [
  'Microsoft Edge',
  'Google Chrome',
  'Mozilla Firefox Focus',
  'Firefox',
  'Opera Touch',
  'Pinterest',
  'Snapchat',
  'Instagram',
  'Facebook',
  'Facebook Messenger',
  'Line',
  'LinkedIn',
  'Naver',
  'Baidu',
  'Brave',
]

const resolvePreviewActions = (
  options: MediaPreviewOptions,
  strings: MediaRecorderStrings
): PreviewActions => {
  const estimate = window.XR8.XrDevice.deviceEstimate()
  const browser = estimate.browser.inAppBrowser || estimate.browser.name
  const tmpFile = new File([new Blob()], 'tmp.mp4', {
    type: 'video/mp4',
    lastModified: Date.now(),
  })

  if (navigator.canShare?.({files: [tmpFile]})) {
    return {
      actionButton: {
        icon: SHARE_SRC,
        kind: 'share',
        layout: estimate.os === 'iOS' ? 'alone' : 'with-download',
        text: options.actionButtonShareText || strings.shareCta,
      },
      disabledDownload: false,
      showDownloadButton: estimate.os !== 'iOS',
      webShareAPILevel2: true,
    }
  }
  if (estimate.os === 'iOS' && WK_WEBVIEW_IOS_BROWSERS.includes(browser)) {
    return {
      disabledDownload: true,
      showDownloadButton: false,
      webShareAPILevel2: false,
    }
  }
  if (estimate.os === 'iOS') {
    return {
      actionButton: {
        icon: VIEW_SRC,
        kind: 'view',
        layout: 'after-download',
        text: options.actionButtonViewText || strings.viewCta,
      },
      disabledDownload: false,
      showDownloadButton: true,
      webShareAPILevel2: false,
    }
  }
  return {
    disabledDownload: false,
    showDownloadButton: true,
    webShareAPILevel2: false,
  }
}

export {resolvePreviewActions}
