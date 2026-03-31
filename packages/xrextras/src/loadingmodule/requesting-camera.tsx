import * as React from 'preact'

const RequestingCamera = () => (
  <div id='requestingCameraPermissions'>
    <img alt='' id='requestingCameraIcon' src='./camera.svg' />
    Tap &lsquo;Allow&rsquo; to access AR
  </div>
)

export {
  RequestingCamera,
}
