import {getInitializedApplication} from '../studio-debug'
import {dataSize, oom} from './xr-hud-core'
import type {XrHudManager} from './xr-hud-manager'
import {retrieveCurrentSessionStats} from '../shared/ecs/shared/session-stats'
import {MAX_RECOMMENDED_TEXTURE_SIZE} from '../shared/ecs/shared/material-constants'

const TRIANGLE_LIMIT = 75000
const DRAW_CALLS_LIMIT = 50
const TEXTURE_LIMIT = 30
const SHADERS_LIMIT = 30
const GEOMETRY_LIMIT = 30
const POINTS_LIMIT = 100000
const ENTITY_LIMIT = 75
const MODEL_SIZE_LIMIT = 12 * 1024 * 1024  // 12 MB

let tickFrame: number | null = null
const disposeXrHudStats = () => {
  if (tickFrame) {
    cancelAnimationFrame(tickFrame)
    tickFrame = null
  }
}

const updateStats = (xrHud: ReturnType<typeof XrHudManager>) => {
  const pollStats = () => {
    // ensure we cancel an existing / previous frame before scheduling a new one
    if (tickFrame) {
      cancelAnimationFrame(tickFrame)
    }

    tickFrame = requestAnimationFrame(() => updateStats(xrHud))
  }

  const ecsApp = getInitializedApplication()
  if (!ecsApp) {
    // we keep polling until ecs + the world is initialized
    pollStats()
    return
  }

  const stats = retrieveCurrentSessionStats(ecsApp)
  if (stats) {
    xrHud.setStats({
      'Tris': {alarm: stats.triangles > TRIANGLE_LIMIT, value: oom(stats.triangles)},
      'Draw Calls': {alarm: stats.drawCalls > DRAW_CALLS_LIMIT, value: stats.drawCalls},
      'Textures': {alarm: stats.textures > TEXTURE_LIMIT, value: stats.textures},
      'Tex(max)': {
        alarm: stats.textureMaxSize > MAX_RECOMMENDED_TEXTURE_SIZE,
        value: stats.textureMaxSize,
      },
      'Shaders': {alarm: stats.shaders > SHADERS_LIMIT, value: stats.shaders},
      'Geometries': {alarm: stats.geometries > GEOMETRY_LIMIT, value: stats.geometries},
      'Points': {alarm: stats.points > POINTS_LIMIT, value: oom(stats.points)},
      'Entities': {alarm: stats.entities > ENTITY_LIMIT, value: stats.entities},
      'Models': {alarm: stats.modelSize > MODEL_SIZE_LIMIT, value: dataSize(stats.modelSize)},
    })
  } else {
    xrHud.clearStats([
      'Tris',
      'Draw Calls',
      'Textures',
      'Tex(max)',
      'Shaders',
      'Geometries',
      'Points',
      'Entities',
    ])
  }

  pollStats()
}

const setUpXrHudStats = (xrHud: ReturnType<typeof XrHudManager>) => () => {
  tickFrame = requestAnimationFrame(() => updateStats(xrHud))
}

export {
  setUpXrHudStats,
  disposeXrHudStats,
}
