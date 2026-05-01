import {watchForAScenes, unwatchForAScenes} from './xr-hud-aframe'
import {ConsoleManager} from './xr-hud-console-manager'
import {setUpXrHudStats, disposeXrHudStats} from './xr-hud-stats'
import {StatsManager} from './xr-hud-stats-manager'
import {VersionManager} from './xr-hud-version-manager'
import type {XrHudCallback, XrHudBaseSettings, XrHudLogger} from './xr-hud-types'
import XrHudFonts from './xr-hud-fonts.html'

document.head.insertAdjacentHTML('beforeend', XrHudFonts)  // Add Fonts

const XrHudBase = () => {
  const versionManager_ = VersionManager()
  const consoleManager_ = ConsoleManager()
  const statsManager_ = StatsManager()
  const disableCallbacks_: XrHudCallback[] = []
  const enableCallbacks_: XrHudCallback[] = []
  let logger_: XrHudLogger | undefined

  const enable = ({version, console: cs, verbose}: XrHudBaseSettings) => {
    if (version !== undefined) {
      versionManager_.enable({version})
    }
    if (cs !== undefined) {
      logger_ = consoleManager_.enable({console: cs})
    }
    if (verbose !== undefined) {
      statsManager_.enable({verbose})
    }
    enableCallbacks_.forEach(c => c())
  }

  const onDisable = (callback: XrHudCallback) => disableCallbacks_.push(callback)
  const onEnable = (callback: XrHudCallback) => enableCallbacks_.push(callback)

  const disable = () => {
    consoleManager_.disable()
    statsManager_.disable()
    versionManager_.disable()
    disableCallbacks_.forEach(c => c())
    logger_ = undefined
  }

  return {
    notifyLog: (fn: string, args): void => logger_?.[fn]?.(args),
    disable,
    enable,
    onDisable,
    onEnable,
    clearVersions: versionManager_.clearVersions,
    setVersions: versionManager_.setVersions,
    setActions: consoleManager_.setActions,
    clearActions: consoleManager_.clearActions,
    setStats: statsManager_.setStats,
    clearStats: statsManager_.clearStats,
  }
}

const XrHudManager = () => {
  const base = XrHudBase()
  base.onEnable(watchForAScenes(base))
  base.onEnable(setUpXrHudStats(base))
  base.onDisable(unwatchForAScenes)
  base.onDisable(disposeXrHudStats)
  return base
}

export {
  XrHudManager,
}
