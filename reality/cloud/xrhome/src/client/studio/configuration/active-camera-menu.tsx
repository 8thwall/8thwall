import React from 'react'

import {useTranslation} from 'react-i18next'

import {createUseStyles} from 'react-jss'

import {useActiveSpace} from '../hooks/active-space'
import {useSceneContext} from '../scene-context'
import {SelectMenu} from '../ui/select-menu'
import {StandardFieldLabel} from '../../ui/components/standard-field-label'
import {StandardFieldContainer} from '../../ui/components/standard-field-container'
import {combine} from '../../common/styles'
import {useStyles as useRowStyles} from './row-fields'
import {useStudioMenuStyles} from '../ui/studio-menu-styles'
import {ActiveCameraSubmenu} from './active-camera-submenu'
import {FloatingMenuButton} from '../../ui/components/floating-menu-button'
import {Icon} from '../../ui/components/icon'
import {useDerivedScene} from '../derived-scene-context'

const useStyles = createUseStyles({
  categoryChevron: {
    '& svg': {
      width: '0.75em',
      height: '0.75em',
    },
  },
  categoryButton: {
    width: '100%',
    display: 'flex',
    justifyContent: 'space-between',
  },
})

interface IIncludedSpaceOption {
  spaceId: string | undefined
  onSpaceSelect: (spaceId: string) => void
}

const IncludedSpaceOption: React.FC<IIncludedSpaceOption> = ({spaceId, onSpaceSelect}) => {
  const classes = useStyles()
  const ctx = useSceneContext()

  const space = ctx.scene.spaces[spaceId]

  return (
    <FloatingMenuButton
      onClick={() => onSpaceSelect(spaceId)}
    >
      <div className={classes.categoryButton}>
        {space?.name}
        <div className={classes.categoryChevron}>
          <Icon stroke='chevronRight' />
        </div>
      </div>
    </FloatingMenuButton>
  )
}

const ActiveCameraMenu: React.FC = () => {
  const ctx = useSceneContext()
  const derivedScene = useDerivedScene()
  const rowClasses = useRowStyles()
  const menuStyles = useStudioMenuStyles()

  const {t} = useTranslation(['cloud-studio-pages'])

  const activeSpace = useActiveSpace()

  const allCameras = derivedScene.getAllCameras()
  const spacesWithCamera = derivedScene.getAllIncludedSpaces(activeSpace?.id)
    .filter(spaceId => allCameras
      .some(camera => derivedScene.resolveSpaceForObject(camera.id)?.id === spaceId))

  const activeCamera = derivedScene.getActiveCamera(activeSpace?.id)

  const [subMenu, setSubMenu] = React.useState('')

  if (spacesWithCamera && subMenu &&
    (!spacesWithCamera.includes(subMenu) || spacesWithCamera.length < 2)
  ) {
    setSubMenu('')
  }

  const displayText = activeCamera
    ? activeCamera.name
    : t('space_configurator.active_camera.placeholder')

  const updateActiveCamera = (id: string) => {
    if (id === activeCamera?.id) {
      return
    }

    ctx.updateScene((scene) => {
      if (activeSpace) {
        return {
          ...scene,
          spaces: {
            ...scene.spaces,
            [activeSpace.id]: {
              ...scene.spaces[activeSpace.id],
              activeCamera: id,
            },
          },
        }
      }
      return {
        ...scene,
        activeCamera: id,
      }
    })
  }

  if ((spacesWithCamera.length === 0 && activeSpace?.id) ||
    (!activeSpace && derivedScene.getCamerasForSpace(activeSpace?.id).length === 0)) {
    return null
  }

  return (
    <label htmlFor='included-spaces-menu' className={rowClasses.row}>
      <div className={rowClasses.flexItem}>
        <StandardFieldLabel
          label={t('space_configurator.active_camera.title')}
          mutedColor
        />
      </div>
      <div className={rowClasses.flexItem}>
        <StandardFieldContainer>
          <SelectMenu
            id='included-spaces-menu'
            menuWrapperClassName={combine(menuStyles.studioMenu, rowClasses.selectMenuContainer)}
            trigger={(
              <button
                type='button'
                className={combine('style-reset', rowClasses.select, rowClasses.preventOverflow)}
              >
                <div className={rowClasses.selectText}>
                  {displayText}
                </div>
                <div className={rowClasses.chevron} />
              </button>
            )}
            matchTriggerWidth
            margin={4}
            placement='bottom-end'
          >
            {collapse => (
              <>
                {spacesWithCamera.length < 2 &&
                  <ActiveCameraSubmenu
                    spaceId={spacesWithCamera[0]}
                    onCameraSelect={(newCamera) => {
                      updateActiveCamera(newCamera)
                      collapse()
                    }}
                  />
                }
                {subMenu === '' && spacesWithCamera.length > 1 &&
                  spacesWithCamera.map(id => (
                    <IncludedSpaceOption
                      key={id}
                      spaceId={id}
                      onSpaceSelect={(spaceId) => {
                        setSubMenu(spaceId)
                      }}
                    />
                  ))
                }
                {subMenu &&
                  <ActiveCameraSubmenu
                    spaceId={subMenu}
                    onCameraSelect={(newCamera) => {
                      updateActiveCamera(newCamera)
                      setSubMenu('')
                      collapse()
                    }}
                    onBackClick={(isOpen) => {
                      if (!isOpen) {
                        setSubMenu('')
                      }
                    }}
                  />
                }
              </>
            )}
          </SelectMenu>
        </StandardFieldContainer>
      </div>
    </label>
  )
}

export {
  ActiveCameraMenu,
}
