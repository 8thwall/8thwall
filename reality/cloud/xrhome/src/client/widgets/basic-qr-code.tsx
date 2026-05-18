import React from 'react'
import * as QRCode from 'qrcode'
import {useQuery} from '@tanstack/react-query'

type QrCodeOptions = {
  url: string
  ecl?: 'l' | 'm' | 'h'
  margin?: number
}

interface IBasicQrCode extends React.ImgHTMLAttributes<HTMLImageElement>, QrCodeOptions {}

const BasicQrCode: React.FC<IBasicQrCode> = ({url, ecl, margin, alt, ...rest}) => {
  const src = useQuery({
    queryKey: ['qr', url, ecl, margin, 2],
    queryFn: async () => `data:image/svg+xml,${encodeURIComponent(await QRCode.toString(url, {
      type: 'svg',
      width: 250,
      margin,
      errorCorrectionLevel: ecl,
    }))}`,
  })?.data || ''

  return (
    <img
      {...rest}
      // eslint-disable-next-line local-rules/hardcoded-copy
      alt={alt !== undefined ? alt : 'QR Code'}
      src={src}
    />
  )
}

export {
  BasicQrCode,
}
