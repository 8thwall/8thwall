import type {DeepReadonly} from 'ts-essentials'

import {getDeviceTitle} from './device-models'

const createSessionId = () => (
  Array.from({length: 8})
    .map(() => Math.floor(Math.random() * 36).toString(36))
    .join('')
)

const getSessionDisplayTitle = (
  title: string, id?: string, sessionList?: DeepReadonly<Array<{title: string}>>
) => {
  if (!title && !id) {
    return ''
  }
  if (title === id || sessionList?.filter(s => s.title === title).length === 1) {
    return title
  }
  return `${title} [${id.substring(0, 4)}]`
}

const getSessionTitle = (ua: string, screenHeight: number, screenWidth: number, pageId: string) => (
  getDeviceTitle(ua, screenHeight, screenWidth) || pageId
)

export {
  getSessionTitle,
  createSessionId,
  getSessionDisplayTitle,
}
