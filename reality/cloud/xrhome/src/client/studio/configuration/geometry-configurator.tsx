import React from 'react'
import type {DeepReadonly} from 'ts-essentials'
import type {Faces, Geometry} from '@ecs/shared/scene-graph'
import {useTranslation} from 'react-i18next'

import {makeGeometry} from '../make-object'
import {RowNumberField, RowSelectField} from './row-fields'
import {RowContent} from './row-content'
import {FloatingPanelButton} from '../../ui/components/floating-panel-button'
import {useStudioStateContext} from '../studio-state-context'
import {ScenePathScopeProvider} from '../scene-path-input-context'

const isDimensionsLocked = (geo: DeepReadonly<Geometry | null>) => {
  if (!geo) {
    return false
  }

  switch (geo.type) {
    case 'box':
      return geo.width === geo.height && geo.width === geo.depth
    case 'plane':
      return geo.width === geo.height
    default:
      return false
  }
}

const useDimensionsLocked = (geometry: DeepReadonly<Geometry>) => {
  const stateCtx = useStudioStateContext()
  const [locked, setLocked] = React.useState(() => isDimensionsLocked(geometry))

  React.useLayoutEffect(() => {
    setLocked(isDimensionsLocked(geometry))
  }, [stateCtx.state.selectedIds, geometry?.type])

  return [locked, setLocked] as const
}

interface IGeometryConfigurator {
  geometry: DeepReadonly<Geometry>
  onChange: (updater: (current: DeepReadonly<Geometry>) => DeepReadonly<Geometry>) => void
}

const BoxGeometryConfigurator: React.FC<IGeometryConfigurator> = ({geometry, onChange}) => {
  const [locked, setLocked] = useDimensionsLocked(geometry)
  const {t} = useTranslation(['cloud-studio-pages', 'common'])

  if (geometry?.type !== 'box') {
    return null
  }

  if (locked) {
    return (
      <div>
        <RowNumberField
          id='geometry-size'
          expanseField='width'
          onChange={n => onChange(() => ({...geometry, width: n, height: n, depth: n}))}
          value={geometry.width}
          min={0}
          step={0.1}
          label={t('geometry_configurator.size.label')}
        />
        <RowContent>
          <FloatingPanelButton
            a8='click;studio;unlock-box-geometry-button'
            type='button'
            onClick={() => setLocked(false)}
          >
            {t('geometry_configurator.button.unlock')}
          </FloatingPanelButton>
        </RowContent>
      </div>
    )
  }

  return (
    <div>
      <RowNumberField
        id='geometry-width'
        expanseField='width'
        onChange={n => onChange(() => ({...geometry, width: n}))}
        value={geometry.width}
        min={0}
        step={0.1}
        label={t('geometry_configurator.geometry_width.label')}
      />
      <RowNumberField
        id='geometry-height'
        expanseField='height'
        onChange={n => onChange(() => ({...geometry, height: n}))}
        value={geometry.height}
        min={0}
        step={0.1}
        label={t('geometry_configurator.geometry_height.label')}
      />
      <RowNumberField
        id='geometry-depth'
        expanseField='depth'
        onChange={n => onChange(() => ({...geometry, depth: n}))}
        value={geometry.depth}
        min={0}
        step={0.1}
        label={t('geometry_configurator.geometry_depth.label')}
      />
      <RowContent>
        <FloatingPanelButton
          type='button'
          onClick={() => {
            setLocked(true)
            onChange(() => makeGeometry('box'))
          }}
        >{t('button.reset', {ns: 'common'})}
        </FloatingPanelButton>
      </RowContent>
    </div>
  )
}

const SphereGeometryConfigurator: React.FC<IGeometryConfigurator> = ({geometry, onChange}) => {
  const {t} = useTranslation(['cloud-studio-pages', 'common'])
  if (geometry?.type !== 'sphere') {
    return null
  }

  return (
    <div>
      <RowNumberField
        id='geometry-radius'
        expanseField='radius'
        onChange={n => onChange(() => ({...geometry, radius: n}))}
        value={geometry.radius}
        min={0}
        step={0.1}
        label={t('geometry_configurator.geometry_radius.label')}
      />
      <RowContent>
        <FloatingPanelButton
          type='button'
          onClick={() => {
            onChange(() => makeGeometry('sphere'))
          }}
        >{t('button.reset', {ns: 'common'})}
        </FloatingPanelButton>
      </RowContent>
    </div>
  )
}

const PlaneGeometryConfigurator: React.FC<IGeometryConfigurator> = ({geometry, onChange}) => {
  const {t} = useTranslation(['cloud-studio-pages', 'common'])
  const [locked, setLocked] = useDimensionsLocked(geometry)

  if (geometry?.type !== 'plane') {
    return null
  }

  if (locked) {
    return (
      <div>
        <RowNumberField
          id='geometry-size'
          expanseField='width'
          onChange={n => onChange(() => ({...geometry, width: n, height: n}))}
          value={geometry.width}
          min={0}
          step={0.1}
          label={t('geometry_configurator.geometry_size.label')}
        />
        <RowContent>
          <FloatingPanelButton
            a8='click;studio;unlock-plane-geometry-button'
            type='button'
            onClick={() => setLocked(false)}
          >
            {t('geometry_configurator.button.unlock')}
          </FloatingPanelButton>
        </RowContent>
      </div>
    )
  }

  return (
    <div>
      <RowNumberField
        id='geometry-width'
        expanseField='width'
        onChange={n => onChange(() => ({...geometry, width: n}))}
        value={geometry.width}
        min={0}
        step={0.1}
        label={t('geometry_configurator.geometry_width.label')}
      />
      <RowNumberField
        id='geometry-height'
        expanseField='height'
        onChange={n => onChange(() => ({...geometry, height: n}))}
        value={geometry.height}
        min={0}
        step={0.1}
        label={t('geometry_configurator.geometry_height.label')}
      />
      <RowContent>
        <FloatingPanelButton
          type='button'
          onClick={() => {
            setLocked(true)
            onChange(() => makeGeometry('plane'))
          }}
        >{t('button.reset', {ns: 'common'})}
        </FloatingPanelButton>
      </RowContent>
    </div>
  )
}

const CapsuleGeometryConfigurator: React.FC<IGeometryConfigurator> = ({geometry, onChange}) => {
  const {t} = useTranslation(['cloud-studio-pages', 'common'])
  if (geometry?.type !== 'capsule') {
    return null
  }

  return (
    <div>
      <RowNumberField
        id='geometry-radius'
        expanseField='radius'
        onChange={n => onChange(() => ({...geometry, radius: n}))}
        value={geometry.radius}
        min={0}
        step={0.1}
        label={t('geometry_configurator.geometry_radius.label')}
      />
      <RowNumberField
        id='geometry-height'
        expanseField='height'
        onChange={n => onChange(() => ({...geometry, height: n}))}
        value={geometry.height}
        min={0}
        step={0.1}
        label={t('geometry_configurator.geometry_height.label')}
      />
      <RowContent>
        <FloatingPanelButton
          type='button'
          onClick={() => {
            onChange(() => makeGeometry('capsule'))
          }}
        >{t('button.reset', {ns: 'common'})}
        </FloatingPanelButton>
      </RowContent>
    </div>
  )
}

const ConeGeometryConfigurator: React.FC<IGeometryConfigurator> = ({geometry, onChange}) => {
  const {t} = useTranslation(['cloud-studio-pages', 'common'])
  if (geometry?.type !== 'cone') {
    return null
  }

  return (
    <div>
      <RowNumberField
        id='geometry-radius'
        expanseField='radius'
        onChange={n => onChange(() => ({...geometry, radius: n}))}
        value={geometry.radius}
        min={0}
        step={0.1}
        label={t('geometry_configurator.geometry_radius.label')}
      />
      <RowNumberField
        id='geometry-height'
        expanseField='height'
        onChange={n => onChange(() => ({...geometry, height: n}))}
        value={geometry.height}
        min={0}
        step={0.1}
        label={t('geometry_configurator.geometry_height.label')}
      />
      <RowContent>
        <FloatingPanelButton
          type='button'
          onClick={() => {
            onChange(() => makeGeometry('cone'))
          }}
        >{t('button.reset', {ns: 'common'})}
        </FloatingPanelButton>
      </RowContent>
    </div>
  )
}

const CylinderGeometryConfigurator: React.FC<IGeometryConfigurator> = ({geometry, onChange}) => {
  const {t} = useTranslation(['cloud-studio-pages', 'common'])
  if (geometry?.type !== 'cylinder') {
    return null
  }

  return (
    <div>
      <RowNumberField
        id='geometry-radius'
        expanseField='radius'
        onChange={n => onChange(() => ({...geometry, radius: n}))}
        value={geometry.radius}
        min={0}
        step={0.1}
        label={t('geometry_configurator.geometry_radius.label')}
      />
      <RowNumberField
        id='geometry-height'
        expanseField='height'
        onChange={n => onChange(() => ({...geometry, height: n}))}
        value={geometry.height}
        min={0}
        step={0.1}
        label={t('geometry_configurator.geometry_height.label')}
      />
      <RowContent>
        <FloatingPanelButton
          type='button'
          onClick={() => {
            onChange(() => makeGeometry('cylinder'))
          }}
        >{t('button.reset', {ns: 'common'})}
        </FloatingPanelButton>
      </RowContent>
    </div>
  )
}

const TetrahedronGeometryConfigurator: React.FC<IGeometryConfigurator> = ({geometry, onChange}) => {
  const {t} = useTranslation(['cloud-studio-pages', 'common'])
  if (geometry?.type !== 'tetrahedron') {
    return null
  }

  return (
    <div>
      <RowNumberField
        id='geometry-radius'
        expanseField='radius'
        onChange={n => onChange(() => ({...geometry, radius: n}))}
        value={geometry.radius}
        min={0}
        step={0.1}
        label={t('geometry_configurator.geometry_radius.label')}
      />
      <RowContent>
        <FloatingPanelButton
          type='button'
          onClick={() => {
            onChange(() => makeGeometry('tetrahedron'))
          }}
        >{t('button.reset', {ns: 'common'})}
        </FloatingPanelButton>
      </RowContent>
    </div>
  )
}

const PolyhedronGeometryConfigurator: React.FC<IGeometryConfigurator> = ({geometry, onChange}) => {
  const {t} = useTranslation(['cloud-studio-pages', 'common'])
  if (geometry?.type !== 'polyhedron') {
    return null
  }

  return (
    <div>
      <RowNumberField
        id='geometry-radius'
        expanseField='radius'
        onChange={n => onChange(() => ({...geometry, radius: n}))}
        value={geometry.radius}
        min={0}
        step={0.1}
        label={t('geometry_configurator.geometry_radius.label')}
      />
      <RowSelectField
        id='geometry-faces'
        expanseField='faces'
        onChange={n => onChange(() => ({...geometry, faces: parseInt(n, 10) as Faces}))}
        value={geometry.faces.toString()}
        label={t('geometry_configurator.geometry_faces.label')}
        options={[
          {content: '4', value: '4'},
          {content: '8', value: '8'},
          {content: '12', value: '12'},
          {content: '20', value: '20'},
        ]}
      />
      <RowContent>
        <FloatingPanelButton
          type='button'
          onClick={() => {
            onChange(() => makeGeometry('polyhedron'))
          }}
        >{t('button.reset', {ns: 'common'})}
        </FloatingPanelButton>
      </RowContent>
    </div>
  )
}

const CircleGeometryConfigurator: React.FC<IGeometryConfigurator> = ({geometry, onChange}) => {
  const {t} = useTranslation(['cloud-studio-pages', 'common'])
  if (geometry?.type !== 'circle') {
    return null
  }

  return (
    <div>
      <RowNumberField
        id='geometry-radius'
        expanseField='radius'
        onChange={n => onChange(() => ({...geometry, radius: n}))}
        value={geometry.radius}
        min={0}
        step={0.1}
        label={t('geometry_configurator.geometry_radius.label')}
      />
      <RowContent>
        <FloatingPanelButton
          type='button'
          onClick={() => {
            onChange(() => makeGeometry('circle'))
          }}
        >{t('button.reset', {ns: 'common'})}
        </FloatingPanelButton>
      </RowContent>
    </div>
  )
}

const RingGeometryConfigurator: React.FC<IGeometryConfigurator> = ({geometry, onChange}) => {
  const {t} = useTranslation(['cloud-studio-pages', 'common'])
  if (geometry?.type !== 'ring') {
    return null
  }

  return (
    <div>
      <RowNumberField
        id='geometry-inner-radius'
        expanseField='innerRadius'
        onChange={n => onChange(() => ({...geometry, innerRadius: n}))}
        value={geometry.innerRadius}
        min={0}
        step={0.1}
        label={t('geometry_configurator.geometry_inner_radius.label')}
      />
      <RowNumberField
        id='geometry-outer-radius'
        expanseField='outerRadius'
        onChange={n => onChange(() => ({...geometry, outerRadius: n}))}
        value={geometry.outerRadius}
        min={0}
        step={0.1}
        label={t('geometry_configurator.geometry_outer_radius.label')}
      />
      <RowContent>
        <FloatingPanelButton
          type='button'
          onClick={() => {
            onChange(() => makeGeometry('ring'))
          }}
        >{t('button.reset', {ns: 'common'})}
        </FloatingPanelButton>
      </RowContent>
    </div>
  )
}

const TorusGeometryConfigurator: React.FC<IGeometryConfigurator> = ({geometry, onChange}) => {
  const {t} = useTranslation(['cloud-studio-pages', 'common'])
  if (geometry?.type !== 'torus') {
    return null
  }

  return (
    <div>
      <RowNumberField
        id='geometry-radius'
        expanseField='radius'
        onChange={n => onChange(() => ({...geometry, radius: n}))}
        value={geometry.radius}
        min={0}
        step={0.1}
        label={t('geometry_configurator.geometry_radius.label')}
      />
      <RowNumberField
        id='geometry-tube-radius'
        expanseField='tubeRadius'
        onChange={n => onChange(() => ({...geometry, tubeRadius: n}))}
        value={geometry.tubeRadius}
        min={0}
        step={0.1}
        label={t('geometry_configurator.geometry_tube_radius.label')}
      />
      <RowContent>
        <FloatingPanelButton
          type='button'
          onClick={() => {
            onChange(() => makeGeometry('torus'))
          }}
        >{t('button.reset', {ns: 'common'})}
        </FloatingPanelButton>
      </RowContent>
    </div>
  )
}

const GeometryConfigurator: React.FC<IGeometryConfigurator> = ({geometry, onChange}) => {
  if (!geometry) {
    return null
  }

  return (
    <ScenePathScopeProvider path={['geometry']}>
      <BoxGeometryConfigurator geometry={geometry} onChange={onChange} />
      <SphereGeometryConfigurator geometry={geometry} onChange={onChange} />
      <PlaneGeometryConfigurator geometry={geometry} onChange={onChange} />
      <CapsuleGeometryConfigurator geometry={geometry} onChange={onChange} />
      <ConeGeometryConfigurator geometry={geometry} onChange={onChange} />
      <CylinderGeometryConfigurator geometry={geometry} onChange={onChange} />
      <TetrahedronGeometryConfigurator geometry={geometry} onChange={onChange} />
      <PolyhedronGeometryConfigurator geometry={geometry} onChange={onChange} />
      <CircleGeometryConfigurator geometry={geometry} onChange={onChange} />
      <RingGeometryConfigurator geometry={geometry} onChange={onChange} />
      <TorusGeometryConfigurator geometry={geometry} onChange={onChange} />
    </ScenePathScopeProvider>
  )
}

export {
  GeometryConfigurator,
}
