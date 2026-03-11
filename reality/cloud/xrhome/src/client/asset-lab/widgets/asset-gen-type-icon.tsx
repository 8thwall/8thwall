import React from 'react'

import type {AssetGeneration} from '../../common/types/db'
import {Icon} from '../../ui/components/icon'
import {createThemedStyles} from '../../ui/theme'

const useStyles = createThemedStyles(() => ({
  iconHolder: {
    background: 'linear-gradient(321deg, rgb(45 46 67 / 20%) 0%, rgba(237, 237, 237, 0.26) 100%)',
    borderRadius: '0.25rem',
  },
}))

interface IAssetGenTypeIcon {
  ag: AssetGeneration
}

const AssetGenTypeIcon: React.FC<IAssetGenTypeIcon> = ({ag}) => {
  // A lot of time these outputs have white background, we want to improve the visibility of these
  // icons a bit
  const classes = useStyles()
  if (ag?.assetType === 'MESH') {
    const iconStroke = ag.metadata?.type === 'MESH_TO_ANIMATION' ? 'guyRunningRight' : 'meshCube'
    return <div className={classes.iconHolder}><Icon inline stroke={iconStroke} /></div>
  }
  return null
}

export {AssetGenTypeIcon}
