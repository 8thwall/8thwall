import {
  COLOR_SUFFIX, OPACITY_SUFFIX, LAYERS, MAP_THEME_DEFAULTS, VISIBILITY_SUFFIX, MAX_RADIUS,
  DEFAULT_SCALE,
} from './map-constants'
import {DEFAULT_EMISSIVE_COLOR, MATERIAL_DEFAULTS} from './material-constants'
import type {MapTheme, MapPoint, Map, BaseGraphObject} from './scene-graph'

type LayerGeometry = typeof LAYERS[number]

interface MaterialProperties {
  color: string
  roughness: number
  metalness: number
  opacity: number
  side: 'front' | 'back' | 'double'
  normalScale: number
  emissiveIntensity: number
  emissive: string
  blending: 'no' | 'normal' | 'additive' | 'subtractive' | 'multiply'
  depthTest: boolean
  depthWrite: boolean
  wireframe: boolean
  forceTransparent: boolean
}

interface GeometryStyle extends Partial<MapTheme> {
  layerGeometryRadiusMax: Record<LayerGeometry, number>
}

interface MapStyles<TMaterial> {
  mapGeometryStyle?: GeometryStyle
  geometryMaterial?: Record<string, TMaterial>
}

const NON_ZERO_PROPERTIES = [
  'roadSMeters', 'roadMMeters', 'roadLMeters', 'roadXLMeters', 'transitMeters', 'waterMeters',
] as const

const themeToMapStyles = <TMaterial>(
  theme: MapTheme,
  createMaterial: (props: MaterialProperties) => TMaterial
): MapStyles<TMaterial> => {
  const completeMap: Required<MapTheme> = {...MAP_THEME_DEFAULTS, ...theme}
  NON_ZERO_PROPERTIES.forEach((prop) => {
    completeMap[prop] = Math.max(completeMap[prop], 1e-6)
  })

  return {
    mapGeometryStyle: {
      ...completeMap,
      layerGeometryRadiusMax: Object.fromEntries(
        LAYERS.map((layer) => {
          const key = `${layer}${VISIBILITY_SUFFIX}` as keyof MapTheme
          return [layer, (theme[key] ?? MAP_THEME_DEFAULTS[key]) ? MAX_RADIUS : 0]
        })
      ) as Record<LayerGeometry, number>,
    },
    geometryMaterial: Object.fromEntries(
      LAYERS.map((layer) => {
        const colorProp = `${layer}${COLOR_SUFFIX}` as keyof MapTheme
        const color = (theme[colorProp] ?? MAP_THEME_DEFAULTS[colorProp]) as string
        const opacityProp = `${layer}${OPACITY_SUFFIX}` as keyof MapTheme
        const opacity = (theme[opacityProp] ?? MAP_THEME_DEFAULTS[opacityProp]) as number
        const emissive = DEFAULT_EMISSIVE_COLOR

        // eslint-disable-next-line @typescript-eslint/no-unused-vars
        const {offsetX, offsetY, repeatX, repeatY, ...props} = MATERIAL_DEFAULTS

        return [layer, createMaterial({...props, color, opacity, emissive})]
      })
    ),
  }
}

interface MapPointPose {
  position: {x: number, y: number, z: number}
  rotation: {x: number, y: number, z: number, w: number}
  scale: {x: number, y: number, z: number}
  inRange: boolean
}

// coordinate space is rotated realtive to studio
const convertPosition = (p: MapPointPose['position']): MapPointPose['position'] => ({
  x: p.x * DEFAULT_SCALE,
  y: p.z * DEFAULT_SCALE,
  z: -p.y * DEFAULT_SCALE,
})

const convertScale = (s: MapPointPose['scale']): MapPointPose['scale'] => ({
  x: s.x * DEFAULT_SCALE,
  y: s.z * DEFAULT_SCALE,
  z: s.y * DEFAULT_SCALE,
})

// rotate quaternion -90 degrees around the x axis
// (simplified from quaternion multiplication)
const HALF_SQRT_2 = -0.70710678118
const convertQuaternion = (q: MapPointPose['rotation']): MapPointPose['rotation'] => ({
  x: q.x * HALF_SQRT_2 - q.w * HALF_SQRT_2,
  y: q.y * HALF_SQRT_2 + q.z * HALF_SQRT_2,
  z: q.z * HALF_SQRT_2 - q.y * HALF_SQRT_2,
  w: q.w * HALF_SQRT_2 + q.x * HALF_SQRT_2,
})

const MapPointControlledProperties = ['hidden', 'position', 'rotation', 'scale'] as const
type ControlledType = typeof MapPointControlledProperties[number]
interface MapPointOverrides extends Pick<BaseGraphObject, ControlledType> { }

interface View {
  lat: number
  lng: number
  radius: number
  rotationRadian: number
}

interface MapPointProps {
  lat: number
  lng: number
  meters: number
  min: number
  maxViewableRadius: number
}

type MapPointToPose = (mapView: View, mapPoint: MapPointProps) => MapPointPose

const getMapPointOverrides = (
  map: Pick<Map, 'latitude' | 'longitude' | 'radius'>,
  mapPoint: Pick<MapPoint, 'latitude' | 'longitude' | 'meters' | 'minScale'>,
  mapPointToPose: MapPointToPose
): MapPointOverrides => {
  const pose = mapPointToPose(
    {
      lat: map.latitude,
      lng: map.longitude,
      radius: map.radius,
      rotationRadian: 0,
    },
    {
      lat: mapPoint.latitude,
      lng: mapPoint.longitude,
      meters: mapPoint.meters,
      min: mapPoint.minScale,
      maxViewableRadius: MAX_RADIUS,
    }
  )

  const convertedPos = convertPosition(pose.position)
  const convertedRot = convertQuaternion(pose.rotation)
  const convertedScale = convertScale(pose.scale)
  const position: [number, number, number] = [convertedPos.x, convertedPos.y, convertedPos.z]
  const rotation: [number, number, number, number] = [
    convertedRot.x, convertedRot.y, convertedRot.z, convertedRot.w,
  ]
  const scale: [number, number, number] = [convertedScale.x, convertedScale.y, convertedScale.z]
  return {position, rotation, scale, hidden: !pose.inRange}
}

export type {
  MapPointOverrides,
}

export {
  themeToMapStyles,
  getMapPointOverrides,
  MapPointControlledProperties,
}
