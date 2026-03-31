import * as React from 'preact'

import computerVoxelImage from '../runtimeerrormodule/computer-voxel.png'

const CameraModeError = ({message}: {message: string}) => (
  <div id='cameraSelectionWorldTrackingError' className='permission-error absolute-fill'>
    <p>
      <img alt='' height='75px' src={computerVoxelImage} className='floater' />
    </p>
    <div className='error-text-header'>Oops, something went wrong!</div>
    <p>{message}</p>
  </div>
)

export {
  CameraModeError,
}
