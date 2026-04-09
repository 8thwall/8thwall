import * as React from 'preact'
import type {FunctionComponent as FC} from 'preact'

// eslint-disable-next-line max-len
const ICON_PATH = 'M12 0c-6.627 0-12 4.975-12 11.111 0 3.497 1.745 6.616 4.472 8.652v4.237l4.086-2.242c1.09.301 2.246.464 3.442.464 6.627 0 12-4.974 12-11.111 0-6.136-5.373-11.111-12-11.111zm1.193 14.963l-3.056-3.259-5.963 3.259 6.559-6.963 3.13 3.259 5.889-3.259-6.559 6.963z'

const makeShareIntent = (url: string) => {
  const params = new URLSearchParams({
    link: url,
    redirect_uri: url,
    // This app ID belongs to jenkins@8thwall.com
    // https://developers.facebook.com/apps/<REMOVED_BEFORE_OPEN_SOURCING>/dashboard/
    app_id: '<REMOVED_BEFORE_OPEN_SOURCING>',
  })

  return `https://www.facebook.com/dialog/send?${params.toString()}`
}

interface IFacebookMessengerButton {
  url: string
}

const FacebookMessengerButton: FC<IFacebookMessengerButton> = ({url}) => (
  <a className='landing8-facebook-messenger-button' href={makeShareIntent(url)}>
    <svg
      width='24'
      height='24'
      xmlns='http://www.w3.org/2000/svg'
      fill-rule='evenodd'  // eslint-disable-line react/no-unknown-property
      clip-rule='evenodd'  // eslint-disable-line react/no-unknown-property
      aria-hidden='true'
      className='landing8-facebook-messenger-icon'
    >
      <path fill='currentColor' d={ICON_PATH} />
    </svg>
    Send to Messenger
  </a>
)

export {
  FacebookMessengerButton,
}
