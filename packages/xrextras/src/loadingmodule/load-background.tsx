import * as React from 'preact'

import spinnerImage from './load-grad.png'
import poweredByImage from '../almosttheremodule/poweredby-horiz-white.svg'

const LoadBackground = ({fading}: {fading: boolean}) => (
  <div id='loadBackground' className={`absolute-fill${fading ? ' fade-out' : ''}`}>
    <div id='loadImageContainer' className='absolute-fill'>
      <img
        alt='Loading'
        src={spinnerImage}
        id='loadImage'
        className='spin'
      />
      <img
        alt='Powered by 8th Wall'
        className='foreground-image poweredby-img'
        src={poweredByImage}
      />
    </div>
  </div>
)

export {
  LoadBackground,
}
