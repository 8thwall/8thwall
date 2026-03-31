import * as React from 'preact'

import cameraSvg from './camera.svg'
import dotsSvg from './dots.svg'
import reloadSvg from './reload.svg'

const AndroidCameraPermissionsError = () => (
  <div id='cameraPermissionsErrorAndroid' className='absolute-fill hidden'>
    <div className='permissionIcon'>
      <img alt='' className='foreground-image' src={cameraSvg} />
    </div>
    <div className='loading-error-header'>Let&apos;s enable your camera</div>
    <ol className='loading-error-instructions'>
      <li>Tap the{' '}
        <img alt='3 dot menu' className='foreground-image' src={dotsSvg} />
        {' '}in the top right
      </li>
      <li>Tap Settings</li>
      <li className='chrome-instruction hidden'>
        <span className='highlight'>Site settings</span>
      </li>
      <li className='chrome-instruction hidden'>
        <span className='highlight'>Camera</span>
      </li>
      <li className='chrome-instruction hidden'>
        <span className='highlight'>Blocked</span>
        <br />
        <span className='camera-instruction-block'>{window.location.hostname}</span>
      </li>
      <li className='chrome-instruction hidden'>
        <span className='camera-instruction-button'>CLEAR & RESET</span>
      </li>
      <li className='samsung-instruction hidden'>
        <span className='highlight'>Advanced</span>
      </li>
      <li className='samsung-instruction hidden'>
        <span className='highlight'>Manage website data</span>
      </li>
      <li className='samsung-instruction hidden'>
        Press and hold<br />
        <span className='camera-instruction-block'>{window.location.hostname}</span>
      </li>
      <li className='samsung-instruction hidden'>
        <span className='highlight' style={{color: '#1500ba'}}>DELETE</span>
      </li>
    </ol>
    <div className='loading-error-footer'>
      <img
        alt=''
        className='foreground-image'
        style={{transform: 'rotate(130deg)'}}
        src={reloadSvg}
      />
      Then, reload the page for AR!
    </div>
  </div>
)

export {
  AndroidCameraPermissionsError,
}
