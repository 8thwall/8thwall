import React from 'react'
import type {DeepReadonly} from 'ts-essentials'
import {useTranslation} from 'react-i18next'

import type {
  Camera, XrConfig, XrFaceConfig, XrWorldConfig, XrCameraType,
} from '@ecs/shared/scene-graph'
import {checkDeviceConfig, getDefaultDeviceSupport} from '@ecs/shared/xr-config'
import {CAMERA_DEFAULTS} from '@ecs/shared/camera-constants'

import {RowBooleanField, RowNumberField, RowSelectField} from './row-fields'
import {createThemedStyles} from '../../ui/theme'
import {Icon} from '../../ui/components/icon'
import {gray3, gray6} from '../../static/styles/settings'
import {StaticBanner} from '../../ui/components/banner'
import {Tooltip} from '../../ui/components/tooltip'
import {CAMERA_COMPONENT} from './direct-property-components'
import {ComponentConfiguratorTray} from './component-configurator-tray'

const useStyles = createThemedStyles(theme => ({
  row: {
    display: 'flex',
    flexDirection: 'row',
    marginBottom: '0.5em',
    alignItems: 'center',
    justifyContent: 'space-between',
    gap: '0.5em',
    paddingTop: '1em',
    paddingBottom: '0.5em',
  },
  flexItem: {
    flex: 1,
  },
  deviceFields: {
    display: 'flex',
    flexDirection: 'column',
    alignItems: 'center',
    gap: '1em',
  },
  select: {
    'padding': '0.5em',
    'paddingLeft': '0.75rem',
    'fontFamily': 'inherit',
    'background': 'transparent',
    'boxSizing': 'border-box',
    'border': 'none',
    'outline': 'none',
    'color': theme.fgMain,
    'width': '100%',
    'borderRight': '0.5rem solid transparent',  // This properly aligns the dropdown arrow
    'appearance': 'none',
    'WebkitAppearance': 'none',
  },
  chevron: {
    'borderRight': `1.5px solid ${theme.fgMuted}`,
    'borderBottom': `1.5px solid ${theme.fgMuted}`,
    'width': '0.5rem',
    'height': '0.5rem',
    'transform': 'translateY(-65%) rotate(45deg)',
    'position': 'absolute',
    'right': '1rem',
    'top': '50%',
    'pointerEvents': 'none',
    'borderRadius': '1px',
    'select:disabled + &': {
      'opacity': 0.5,
    },
  },
  IconContainer: {
    width: '100%',
    height: '20px',
    display: 'flex',
    alignItems: 'center',
    justifyContent: 'center',
    color: gray3,
  },
  disabledIcon: {
    color: gray6,
  },
  info: {
    color: theme.fgMuted,
    paddingBottom: '0.5em',
  },
}))

interface ICameraConfigurator {
  camera: DeepReadonly<Camera>
  onChange: (updater: (newCamera: Camera) => Camera) => void
  resetToPrefab?: (componentIds: string[], nonDirect?: boolean) => void
}

const CameraConfigurator: React.FC<ICameraConfigurator> = (
  {camera, onChange, resetToPrefab}
) => {
  const {t} = useTranslation(['cloud-studio-pages'])
  const classes = useStyles()

  const [configState, setConfigState] = React.useState<string>(
    () => checkDeviceConfig({
      camera: camera.xr?.xrCameraType ?? '3dOnly',
      phone: camera.xr?.phone ?? '3D',
      desktop: camera.xr?.desktop ?? '3D',
      headset: camera.xr?.headset ?? '3D',
    })
  )

  const handleFaceConfigChange = (newConfig: XrFaceConfig) => {
    onChange(currentCamera => ({
      ...currentCamera,
      xr: {
        ...currentCamera.xr,
        face: {
          ...camera.xr?.face,
          ...newConfig,
        },
      },
    }))
  }

  const handleXrConfigChange = (newConfig: XrConfig) => {
    onChange(currentCamera => ({
      ...currentCamera,
      xr: {
        ...currentCamera.xr,
        ...newConfig,
      },
    }))
  }

  const handleXrWorldConfigChange = (newConfig: XrWorldConfig) => {
    onChange(currentCamera => ({
      ...currentCamera,
      xr: {
        ...currentCamera.xr,
        world: {
          ...currentCamera.xr.world,
          ...newConfig,
        },
      },
    }))
  }

  React.useEffect(() => {
    const cameraType = camera.xr?.xrCameraType ?? '3dOnly'
    const {phone, desktop, headset} = getDefaultDeviceSupport(cameraType)

    setConfigState(checkDeviceConfig({
      camera: cameraType,
      phone: camera.xr?.phone ?? phone,
      desktop: camera.xr?.desktop ?? desktop,
      headset: camera.xr?.headset ?? headset,
    }))
  },
  [camera])

  const currentCameraType = camera.xr?.xrCameraType ?? '3dOnly'

  const handleCameraTypeChange = (e: XrCameraType) => {
    handleXrConfigChange({
      xrCameraType: e,
      ...getDefaultDeviceSupport(e),
    })
  }

  return (
    <ComponentConfiguratorTray
      title={t('camera_configurator.title')}
      onResetToPrefab={resetToPrefab ? () => resetToPrefab([CAMERA_COMPONENT]) : undefined}
      sectionId='camera-configurator'
      componentData={[CAMERA_COMPONENT]}
    >
      {configState === 'not-ready' &&
        <>
          <StaticBanner type='warning'>
            <p>
              {t('camera_configurator.banner.not_ready')}
            </p>
          </StaticBanner>
          <br />
        </>
      }
      {configState === 'not-supported' &&
        <>
          <StaticBanner type='danger'>
            <p>{t('camera_configurator.banner.not_supported')}</p>
          </StaticBanner>
          <br />
        </>
      }
      <RowSelectField
        id='xr-type'
        label={t('camera_configurator.xr_type.label')}
        value={currentCameraType}
        options={[
          {value: '3dOnly', content: t('camera_configurator.xr_type.option.3d_only')},
          {value: 'world', content: t('camera_configurator.xr_type.option.world')},
          {value: 'face', content: t('camera_configurator.xr_type.option.face')},
          // Note (alancastillo): Only World and face will be enabled at launch 6/18/2024
          // {value: 'hand', content: t('camera_configurator.xr_type.option.hand')},
          // {value: 'layers', content: t('camera_configurator.xr_type.option.layers')},
          // {value: 'worldLayers', content: t('camera_configurator.xr_type.option.world_layers')},
        ]}
        onChange={handleCameraTypeChange}
      />
      <RowNumberField
        id='near-clip'
        label={t('camera_configurator.near_clip.label')}
        value={camera.nearClip ?? CAMERA_DEFAULTS.nearClip}
        onChange={e => onChange(currentCamera => ({
          ...currentCamera,
          nearClip: e,
        }))}
        step={0.1}
      />
      <RowNumberField
        id='far-clip'
        label={t('camera_configurator.far_clip.label')}
        value={camera.farClip ?? CAMERA_DEFAULTS.farClip}
        onChange={e => onChange(currentCamera => ({
          ...currentCamera,
          farClip: e,
        }))}
        step={0.1}
      />
      {(currentCameraType === '3dOnly') &&
        <>
          <RowSelectField
            id='projection'
            label={t('camera_configurator.projection.label')}
            value={camera.type}
            options={[
              {
                value: 'perspective',
                content: t('camera_configurator.projection.option.perspective'),
              },
              {
                value: 'orthographic',
                content: t('camera_configurator.projection.option.orthographic'),
              },
            ]}
            onChange={(value) => {
              onChange(currentCamera => ({
                ...currentCamera,
                type: value,
              }))
            }}
          />
          {camera.type === 'perspective' &&
            <RowNumberField
              id='fov'
              label={t('camera_configurator.fov.label')}
              value={camera.fov ?? 80}
              onChange={e => onChange(currentCamera => ({
                ...currentCamera,
                fov: e,
              }))}
              step={1}
            />
          }
          {camera.type === 'orthographic' &&
            <>
              <RowNumberField
                id='width'
                label={t('camera_configurator.width.label')}
                value={2 * (camera.right ?? 1)}
                onChange={(e) => {
                  onChange(currentCamera => ({
                    ...currentCamera,
                    left: e / -2,
                    right: e / 2,
                  }))
                }}
                step={0.1}
              />
              <RowNumberField
                id='height'
                label={t('camera_configurator.height.label')}
                value={2 * (camera.top ?? 1)}
                onChange={(e) => {
                  onChange(currentCamera => ({
                    ...currentCamera,
                    bottom: e / -2,
                    top: e / 2,
                  }))
                }}
                step={0.1}
              />
            </>
          }
          <RowNumberField
            id='zoom'
            label={t('camera_configurator.zoom.label')}
            value={camera.zoom ?? 1}
            onChange={e => onChange(currentCamera => ({
              ...currentCamera,
              zoom: e,
            }))}
            step={0.1}
          />
        </>
      }
      {(currentCameraType === 'world' || currentCameraType === 'face') &&
        <RowSelectField
          id='direction'
          label={t('camera_configurator.direction.label')}
          value={camera.xr?.direction ?? (currentCameraType === 'face' ? 'front' : 'back')}
          options={[
            {value: 'back', content: t('camera_configurator.direction.option.back_camera')},
            {value: 'front', content: t('camera_configurator.direction.option.front_camera')},
          ]}
          onChange={(e) => {
            handleXrConfigChange({
              direction: e,
            })
          }}
        />
      }
      {currentCameraType === 'face' &&
        <>
          <RowBooleanField
            id='mirrored-display'
            label={t('camera_configurator.mirrored_display.label')}
            checked={camera.xr?.face?.mirroredDisplay ?? false}
            onChange={(e) => {
              handleFaceConfigChange({
                mirroredDisplay: e.target.checked,
              })
            }}
          />
          <RowSelectField
            id='maxDetection'
            label={t('camera_configurator.max_detections.label')}
            value={camera.xr?.face?.maxDetections ? camera.xr?.face?.maxDetections.toString() : '1'}
            options={[
              {value: '1', content: '1'},
              {value: '2', content: '2'},
              {value: '3', content: '3'},
            ]}
            onChange={(value) => {
              handleFaceConfigChange({
                maxDetections: Number(value),
              })
            }}
          />
          <RowBooleanField
            id='face'
            label={t('camera_configurator.face.label')}
            key='face'
            checked={camera.xr?.face?.meshGeometryFace}
            onChange={(e) => {
              handleFaceConfigChange({
                meshGeometryFace: e.target.checked,
              })
            }}
          />
          <RowBooleanField
            id='eyes'
            label={t('camera_configurator.eyes.label')}
            key='eyes'
            checked={camera.xr?.face?.meshGeometryEyes}
            onChange={(e) => {
              handleFaceConfigChange({
                meshGeometryEyes: e.target.checked,
              })
            }}
          />
          <RowBooleanField
            id='iris'
            label={t('camera_configurator.iris.label')}
            key='iris'
            checked={camera.xr?.face?.meshGeometryIris}
            onChange={(e) => {
              handleFaceConfigChange({
                meshGeometryIris: e.target.checked,
              })
            }}
          />
          <RowBooleanField
            id='mouth'
            label={t('camera_configurator.mouth.label')}
            key='mouth'
            checked={camera.xr?.face?.meshGeometryMouth}
            onChange={(e) => {
              handleFaceConfigChange({
                meshGeometryMouth: e.target.checked,
              })
            }}
          />
          <RowBooleanField
            id='enable-ears'
            label={t('camera_configurator.enable_ears.label')}
            checked={camera.xr?.face?.enableEars}
            onChange={(e) => {
              handleFaceConfigChange({
                enableEars: e.target.checked,
              })
            }}
          />
        </>
      }
      {(currentCameraType === 'world' || currentCameraType === 'worldLayers') &&
        <>
          <RowSelectField
            id='scale-mode'
            label={t('camera_configurator.scale_mode.label')}
            value={camera.xr?.world?.scale ?? 'responsive'}
            options={[
              {value: 'absolute', content: t('camera_configurator.scale_mode.option.absolute')},
              {value: 'responsive', content: t('camera_configurator.scale_mode.option.responsive')},
            ]}
            onChange={(e) => {
              handleXrWorldConfigChange({
                scale: e,
              })
            }}
          />
          <RowBooleanField
            id='disable-slam'
            label={t('camera_configurator.slam_disable.label')}
            checked={camera.xr?.world?.disableWorldTracking ?? false}
            onChange={(e) => {
              handleXrWorldConfigChange({
                disableWorldTracking: e.target.checked,
              })
            }}
          />
          <RowBooleanField
            id='enable-vps'
            label={t('camera_configurator.vps.label')}
            checked={camera.xr?.world?.enableVps ?? false}
            onChange={(e) => {
              handleXrWorldConfigChange({
                enableVps: e.target.checked,
              })
            }}
          />
          <p>{t('camera_configurator.heading.image_targets')}</p>
          <div className={classes.info}>
            {t('camera_configurator.image_targets.in_memory.label')}
            <Tooltip
              content={t('camera_configurator.image_targets.in_memory.tooltip')}
              position='top-end'
            >
              <Icon stroke='questionMark12' color='muted' inline />
            </Tooltip>
          </div>
          <div className={classes.info}>
            {t('camera_configurator.image_targets.in_view.label')}
            <Tooltip
              content={t('camera_configurator.image_targets.in_view.tooltip')}
              position='top-end'
            >
              <Icon stroke='questionMark12' color='muted' inline />
            </Tooltip>
          </div>
        </>
      }
    </ComponentConfiguratorTray>
  )
}

export {
  CameraConfigurator,
}
