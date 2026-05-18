import React from 'react'

const BASE_URL = BuildIf.ALL_QA ? 'https://s.8th.io' : 'https://8th.io'

type QrCodeOptions = {
  code?: string
  url?: string
  ecl?: 'l' | 'm' | 'h'
  margin?: number
}

const getQrCodeUrl = ({code, url, margin, ecl}: QrCodeOptions) => {
  const params: Record<string, string> = {
    v: '2',
  }

  if (margin !== undefined) {
    params.margin = `${margin}`
  }

  if (ecl !== undefined) {
    params.ecl = ecl
  }

  let pathPart = ''
  if (url) {
    params.url = url
  } else {
    pathPart = `/${code}`
  }

  return `${BASE_URL}/qr${pathPart}?${new URLSearchParams(params)}`
}

interface IBasicQrCode extends React.ImgHTMLAttributes<HTMLImageElement>, QrCodeOptions {}

const BasicQrCode: React.FC<IBasicQrCode> = ({url, code, ecl, margin, alt, ...rest}) => (
  <img
    {...rest}
    alt={alt !== undefined ? alt : 'QR Code'}
    src={getQrCodeUrl({url, code, ecl, margin})}
  />
)
export {
  BasicQrCode,
}
