import React from 'react'
import {Canvas} from '@react-three/fiber'

import {createUseStyles} from 'react-jss'

import {StandalonePrefab} from './scene-objects'
import {GroundIndicator} from './ground-indicator'
import {YogaParentContextProvider} from './yoga-parent-context'
import {PreviewCameraComponent} from './preview-camera'

const useStyles = createUseStyles(() => ({
  container: {
    width: '100%',
    aspectRatio: '1 / 1',
    marginBottom: '1em',
  },
}))

interface IStudioScenePreview {
  rootEntityId: string
}

const StudioScenePreview: React.FC<IStudioScenePreview> = ({rootEntityId}) => {
  const classes = useStyles()

  return (
    <div className={classes.container}>
      <Canvas>
        <YogaParentContextProvider>
          <StandalonePrefab rootEntityId={rootEntityId} />
          <PreviewCameraComponent key={rootEntityId} />
          <GroundIndicator />
        </YogaParentContextProvider>
      </Canvas>
    </div>
  )
}

export {StudioScenePreview}
