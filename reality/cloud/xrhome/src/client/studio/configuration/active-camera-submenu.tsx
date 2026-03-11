import React from 'react'

import {
  getActiveCameraIdFromSceneGraph,
} from '@ecs/shared/get-camera-from-scene-graph'

import {createUseStyles} from 'react-jss'

import {useActiveSpace} from '../hooks/active-space'
import {useSceneContext} from '../scene-context'
import {SubMenuHeading} from '../ui/submenu-heading'
import {combine} from '../../common/styles'
import {Icon} from '../../ui/components/icon'
import {useStudioMenuStyles} from '../ui/studio-menu-styles'
import {useDerivedScene} from '../derived-scene-context'

const useStyles = createUseStyles({
  subMenuContainer: {
    maxHeight: '200px',
    overflow: 'auto',
  },
  categoryChevron: {
    '& svg': {
      width: '0.75em',
      height: '0.75em',
    },
  },
  subMenuHeading: {
    'paddingBottom': '0.25em',
  },
})

interface ICameraOption {
  objectId: string
  activeId: string
  onClick: () => void
}

const CameraOption: React.FC<ICameraOption> = ({
  objectId, activeId, onClick,
}) => {
  const classes = useStudioMenuStyles()
  const derivedScene = useDerivedScene()

  const cameraObject = derivedScene.getObject(objectId)

  return (
    <div className={classes.menuItemContainer}>
      <button
        type='button'
        className={combine('style-reset', classes.menuItem)}
        onClick={onClick}
      >
        {cameraObject.name}
      </button>
      {activeId === objectId &&
        <div className={combine(classes.iconContainer, classes.checkmark)}>
          <Icon stroke='checkmark' color='highlight' block />
        </div>
        }
    </div>
  )
}

interface IActiveCameraSubmenu {
  spaceId: string
  onCameraSelect?: (id: string) => void
  onBackClick?: (isOpen: boolean) => void
}

const ActiveCameraSubmenu: React.FC<IActiveCameraSubmenu> = ({
  spaceId, onCameraSelect, onBackClick,
}) => {
  const ctx = useSceneContext()
  const classes = useStyles()
  const derivedScene = useDerivedScene()

  const {spaces} = ctx.scene

  const cameraObjects = derivedScene.getCamerasForSpace(spaceId)

  const activeSpace = useActiveSpace()
  const activeId = getActiveCameraIdFromSceneGraph(ctx.scene, activeSpace?.id)

  return (

    <div className={classes.subMenuContainer}>
      {onBackClick &&
        <div className={classes.subMenuHeading}>
          <SubMenuHeading
            title={spaces[spaceId].name}
            onBackClick={() => onBackClick(false)}
            compact
          />
        </div>
      }
      {cameraObjects.map(o => (
        <CameraOption
          key={o.id}
          objectId={o.id}
          activeId={activeId}
          onClick={() => onCameraSelect(o.id)}
        />
      ))}
    </div>
  )
}

export {
  ActiveCameraSubmenu,
}
