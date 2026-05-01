import './xr-hud-core.css'
import {removeDom} from './xr-hud-core'
import type {XrHudBaseSettings} from './xr-hud-types'

type StatValues = {
  alarm?: boolean
  display?: boolean
  text?: string
  value: string | number
}

type Stats = {
  node: HTMLDivElement
  textNode: HTMLSpanElement
  valueNode: HTMLSpanElement
} & StatValues

const STATS_MODULE_NAME = 'xrhudstatsmodule'

const FPS_NAME = 'Fps'

const StatsManager = () => {
  const stats_: Record<string, Stats> = {}
  let addedPipelineModule_ = false
  let enabled_ = false
  let verbose_ = true
  let animationFrameUpdateTiming_: number | null = null
  const frameTimes_: number[] = []

  const statsContainer_ = document.createElement('div')
  statsContainer_.classList.add('xr-tablet-ignore', 'stats-container', 'stats-base', 'verbose')

  const stat = (name: string) => {
    if (name in stats_) {
      return stats_[name]
    }
    stats_[name] = {
      node: document.createElement('div'),
      textNode: document.createElement('span'),
      valueNode: document.createElement('span'),
      text: '',
      value: '',
      alarm: false,
    }
    const {node, textNode, valueNode} = stats_[name]
    node.classList.add('stats-counter')
    node.appendChild(textNode)
    node.appendChild(valueNode)

    textNode.classList.add('stats-counter-id')

    valueNode.classList.add('stats-counter-value')

    if (verbose_ || name === FPS_NAME) {
      statsContainer_.appendChild(node)
    }

    return stats_[name]
  }

  const updateStat = (name, {text, value, alarm, display}: StatValues) => {
    const useText = text || name
    const prev = stat(name)
    if (prev.display !== display) {
      if (display === false) {
        prev.node.classList.add('hidden')
      } else {
        prev.node.classList.remove('hidden')
      }
      prev.display = display
    }
    if (prev.text !== useText) {
      prev.textNode.textContent = useText
      prev.text = useText
    }
    if (prev.alarm !== alarm) {
      if (alarm) {
        prev.node.classList.add('alarm')
      } else {
        prev.node.classList.remove('alarm')
      }
      prev.alarm = alarm
    }
    if (prev.value !== value) {
      prev.valueNode.textContent = String(value)
      prev.value = value
    }
  }

  const setStats = (stats: Record<string, StatValues>) => (
    Object.entries(stats).forEach(([k, s]) => updateStat(k, s))
  )

  const clearStats = (names: string[]) => names.forEach((name) => {
    removeDom(stat(name).node)
    delete stats_[name]
  })

  const disable = () => {
    removeDom(statsContainer_)
    enabled_ = false
    if (addedPipelineModule_) {
      window.XR8.removeCameraPipelineModules([STATS_MODULE_NAME])
      addedPipelineModule_ = false
    }
    if (animationFrameUpdateTiming_) {
      cancelAnimationFrame(animationFrameUpdateTiming_)
      animationFrameUpdateTiming_ = null
    }
  }

  const setVerbose = (verbose: boolean) => {
    verbose_ = verbose
    if (verbose) {
      statsContainer_.classList.add('verbose')
    } else {
      statsContainer_.classList.remove('verbose')
    }
    Object.keys(stats_).forEach((name) => {
      if (name === FPS_NAME) {
        return
      }
      const s = stat(name)
      if (verbose_) {
        statsContainer_.appendChild(s.node)
      } else {
        statsContainer_.removeChild(s.node)
      }
    })
  }

  const addStatsPipelineModule = () => {
    if (!enabled_) {
      return
    }

    if (!window.XR8) {
      requestAnimationFrame(addStatsPipelineModule)
      return
    }

    const statsModule = {
      name: STATS_MODULE_NAME,
      listeners: [
        {
          event: 'reality.imageloading',
          process: ({detail: {imageTargets}}) => {
            setStats({'ImgTargets': {value: imageTargets.length}})
          },
        },
        {
          event: 'reality.projectwayspotscanning',
          process: ({detail: {wayspots}}) => {
            setStats({'Wayspots': {value: wayspots.length}})
          },
        },
      ],
    }

    window.XR8.addCameraPipelineModules([statsModule])
    addedPipelineModule_ = true
  }

  const updateTiming = () => {
    frameTimes_.push(performance.now())
    while (frameTimes_.length > 100) {
      frameTimes_.shift()
    }
    const lastEl = frameTimes_.length - 1
    const fps = (1000 * lastEl) / (frameTimes_[lastEl] - frameTimes_[0])
    setStats({
      [FPS_NAME]: {alarm: fps < 20, value: fps.toFixed(2)},
    })
    animationFrameUpdateTiming_ = requestAnimationFrame(updateTiming)
  }

  const enable = ({verbose}: Pick<XrHudBaseSettings, 'verbose'>) => {
    if (!enabled_) {
      document.body.appendChild(statsContainer_)
      enabled_ = true
      updateTiming()
      frameTimes_.length = 0
      frameTimes_.push(performance.now())
      requestAnimationFrame(addStatsPipelineModule)
    }
    setVerbose(verbose)
  }

  statsContainer_.addEventListener('click', () => setVerbose(!verbose_))

  return {
    setStats,  // [{name, text, alarm, value}]
    clearStats,
    enable,
    disable,
  }
}

export {
  StatsManager,
}
