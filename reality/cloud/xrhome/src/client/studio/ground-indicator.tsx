import React from 'react'
import {Grid} from '@react-three/drei'
import {useFrame, useThree} from '@react-three/fiber'

import {useUiTheme} from '../ui/theme'
import {darkOrange, persimmon} from '../static/styles/settings'

const GroundIndicator: React.FC<{isIsolated?: boolean}> = ({isIsolated}) => {
  const {camera} = useThree()
  const gridRef = React.useRef(null)
  const [cameraDistance, setCameraDistance] = React.useState(0)

  const theme = useUiTheme()

  useFrame(() => {
    if (gridRef.current) {
      setCameraDistance(camera.position.distanceTo(gridRef.current.position))
      if (gridRef.current.material) {
        gridRef.current.material.polygonOffset = true
        gridRef.current.material.polygonOffsetFactor = -1
        gridRef.current.material.polygonOffsetUnits = -1
      }
    }
  })

  const calculateFadeDistance = () => {
    if (cameraDistance < 10) {
      return 400
    } else {
      return 400 + cameraDistance * 2
    }
  }

  const calculateFadeStrength = () => {
    if (cameraDistance < 40) {
      return 3
    } else {
      return 4
    }
  }

  return (
    <Grid
      ref={gridRef}
      position={[0, 0, 0]}
      cellSize={1}
      cellThickness={1}
      cellColor={isIsolated ? darkOrange : theme.studioGridCell}
      sectionSize={5}
      sectionColor={isIsolated ? persimmon : theme.studioGridSection}
      sectionThickness={1}
      infiniteGrid
      fadeDistance={calculateFadeDistance()}
      fadeStrength={calculateFadeStrength()}
    />
  )
}

export {
  GroundIndicator,
}
