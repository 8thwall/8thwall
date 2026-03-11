import React from 'react'
import {createUseStyles} from 'react-jss'

import type {IImageTarget} from '../../common/types/models'
import type {ArcCurveFocus} from './image-target-visualizer'
import {useImgPool} from '../../common/resource-pool'
import {useAbandonableEffect} from '../../hooks/abandonable-effect'
import {loadImage} from '../../apps/image-targets/image-helpers'
import {useUiTheme} from '../../ui/theme'
import type {ImageTargetMetadata} from './image-target-geometry-configurator'

const useStyles = createUseStyles({
  container: {
    aspectRatio: '3 / 4',
    display: 'flex',
    flexDirection: 'column',
    justifyContent: 'center',
    overflow: 'hidden',
  },
})

interface IImageTargetArcEditor {
  imageTarget: IImageTarget
  topRadius: number
  bottomRadius: number
  arcCurveFocus: ArcCurveFocus
}

const ImageTargetArcVisualizer: React.FC<IImageTargetArcEditor> = ({
  imageTarget, topRadius, bottomRadius, arcCurveFocus,
}) => {
  const classes = useStyles()
  const theme = useUiTheme()
  const imgPool = useImgPool()

  const metadata = React.useMemo<ImageTargetMetadata>(
    () => JSON.parse(imageTarget.metadata), [imageTarget.metadata]
  )

  const [width, setWidth] = React.useState<number | undefined>(undefined)
  const [height, setHeight] = React.useState<number | undefined>(undefined)

  useAbandonableEffect(async (abandonable) => {
    const imgTag = await abandonable(loadImage(imageTarget?.originalImageSrc, imgPool))
    setWidth(imgTag?.naturalWidth)
    setHeight(imgTag?.naturalHeight)
  }, [imageTarget?.originalImageSrc, imgPool])

  if (!imageTarget || !metadata || !width || !height) {
    return (<div className={classes.container} />)
  }

  const cx = width / 2
  const cy = topRadius >= 0 ? topRadius : height + topRadius
  const rLarge = Math.abs(topRadius)
  const rSmall = bottomRadius
  const strokeWidth = 0.015 * width
  const dashLength = 1 * strokeWidth
  const dashGap = 2.5 * strokeWidth
  const dashArray = `${dashLength} ${dashGap}`

  return (
    <div className={classes.container}>
      <svg xmlns='http://www.w3.org/2000/svg' viewBox={`0 0 ${width} ${height}`}>
        <image href={imageTarget?.originalImageSrc} />
        <circle
          cx={cx}
          cy={cy}
          r={rLarge}
          stroke={theme.fgSuccess}
          fill='none'
          strokeWidth={strokeWidth}
          strokeLinecap='round'
          strokeDasharray={arcCurveFocus !== 'top' ? dashArray : undefined}
          opacity={arcCurveFocus !== 'top' ? 0.5 : 1}
        />
        <circle
          cx={cx}
          cy={cy}
          r={rSmall}
          stroke={theme.fgSuccess}
          fill='none'
          strokeWidth={strokeWidth}
          strokeLinecap='round'
          strokeDasharray={arcCurveFocus !== 'bottom' ? dashArray : undefined}
          opacity={arcCurveFocus !== 'bottom' ? 0.5 : 1}
        />
      </svg>
    </div>
  )
}

export {
  ImageTargetArcVisualizer,
}
