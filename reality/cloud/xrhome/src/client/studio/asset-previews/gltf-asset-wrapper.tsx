import React from 'react'
import {createUseStyles} from 'react-jss'
import {AnimationMixer} from 'three'

import StudioModelInfo from '../studio-model-info'
import {getObject3DInfo} from './asset-utils'
import {useProcessedGltf} from '../hooks/gltf'
import type {EditorFileLocation} from '../../editor/editor-file-location'
import {
  StudioModelPreviewContextProvider, useStudioModelPreviewContext,
} from './studio-model-preview-context'

const useStyles = createUseStyles({
  gltfContainer: {
    display: 'flex',
    flexDirection: 'column',
    overflow: 'hidden',
    height: '100%',
  },
})

interface IGltfAssetWrapper {
  selectedAssetLocation: EditorFileLocation
  modelUrl: string
  onGltfLoad: () => void
  defaultFileSize: number
}

const InnerGltfAssetWrapper: React.FC<IGltfAssetWrapper> = ({
  selectedAssetLocation,
  modelUrl,
  onGltfLoad,
  defaultFileSize,
}) => {
  const classes = useStyles()
  const gltf = useProcessedGltf(modelUrl, {cloneMaterials: true})
  const mixer = React.useMemo(() => new AnimationMixer(gltf.model), [gltf.model])
  const {setShowSimplifyMeshWarning, setModelInfo} = useStudioModelPreviewContext()

  React.useEffect(() => {
    if (gltf) {
      onGltfLoad()
    }
  }, [gltf])

  React.useEffect(() => {
    setShowSimplifyMeshWarning(false)
    setModelInfo(getObject3DInfo(gltf.model))
  }, [gltf.model])

  return (
    <div className={classes.gltfContainer}>
      <StudioModelInfo
        previewScene={gltf.model}
        modelUrl={modelUrl}
        defaultFileSize={defaultFileSize}
        selectedAssetLocation={selectedAssetLocation}
        mixer={mixer}
      />
    </div>
  )
}

const GltfAssetWrapper: React.FC<IGltfAssetWrapper> = props => (
  <StudioModelPreviewContextProvider>
    <InnerGltfAssetWrapper {...props} />
  </StudioModelPreviewContextProvider>
)

export {
  GltfAssetWrapper,
}
