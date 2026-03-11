import React from 'react'
import {useTranslation} from 'react-i18next'
import {createUseStyles} from 'react-jss'
import type {DeepReadonly} from 'ts-essentials'
import type {ImageTarget as ImageTargetConfig} from '@ecs/shared/scene-graph'

import {Tooltip} from '../ui/components/tooltip'
import {useImageTarget} from './hooks/use-image-target'

const useStyles = createUseStyles({
  tooltipContent: {
    display: 'flex',
    flexDirection: 'column',
    padding: '0.5em',
    gap: '0.25em',
  },
})

interface IInvalidImageTargetTooltip {
  imageTarget: DeepReadonly<ImageTargetConfig>
  children: React.ReactNode
}

const InvalidImageTargetTooltip: React.FC<IInvalidImageTargetTooltip> = ({
  imageTarget, children,
}) => {
  const classes = useStyles()
  const {t} = useTranslation('cloud-studio-pages')
  const [targetData, loading] = useImageTarget(imageTarget.name)

  const isTargetInvalid = !targetData && !!imageTarget.name
  // NOTE(chloe): Store previous visibility here to prevent flickering: while loading, we keep
  // showing the tooltip as its previous visibility until loading is complete.
  const [prevTooltipVisibility, setPrevTooltipVisibility] = React.useState<boolean>(false)

  React.useEffect(() => {
    if (!loading) {
      setPrevTooltipVisibility(isTargetInvalid)
    }
  }, [loading, isTargetInvalid])

  const showTooltip = loading ? prevTooltipVisibility : isTargetInvalid

  if (!showTooltip) {
    return null
  }

  return (
    <Tooltip
      content={(
        <div className={classes.tooltipContent}>
          {t('tree_element.invalid_image_target.tooltip.content')}
        </div>
      )}
      closeDelay={1000}
      position='right'
    >
      {children}
    </Tooltip>
  )
}

export {
  InvalidImageTargetTooltip,
}
