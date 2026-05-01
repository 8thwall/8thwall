import './xr-hud-core.css'
import {removeDom} from './xr-hud-core'
import type {XrHudBaseSettings} from './xr-hud-types'

type VersionData = Record<string, string>

const VersionManager = () => {
  let disabled_ = true
  const versionDisplay_ = document.createElement('div')
  versionDisplay_.classList.add('xr-tablet-ignore', 'version-display')

  const versionData_: VersionData = {
    'XR8': '',
  }

  const updateVersion = () => {
    while (versionDisplay_.firstChild) {
      versionDisplay_.removeChild(versionDisplay_.firstChild)
    }
    Object.entries(versionData_).forEach(([, v]) => {
      const aVersionDisplay = document.createElement('div')
      aVersionDisplay.classList.add('version-display-text')
      aVersionDisplay.textContent = v
      versionDisplay_.appendChild(aVersionDisplay)
    })
  }

  const setVersions = (data: VersionData) => {
    Object.assign(versionData_, data)
    updateVersion()
  }

  const clearVersions = (tags: string[]) => tags.forEach((tag) => {
    if (tag === 'XR8') {
      return
    }
    delete versionData_[tag]
    updateVersion()
  })

  const disable = () => {
    disabled_ = true
    removeDom(versionDisplay_)
  }

  const enable = ({version}: Pick<XrHudBaseSettings, 'version'>) => {
    if (!version) {
      disable()
      return
    }
    disabled_ = false
    const checkXR8Version = () => {
      if (disabled_) {
        return
      }

      try {
        if (window.XR8 && window.XR8.version()) {
          let xr8Version = window.XR8.version()
          if (window.XR8.featureFlags) {
            xr8Version += window.XR8.featureFlags().join('')
          }
          setVersions({XR8: xr8Version})
          document.body.appendChild(versionDisplay_)
          return
        }
      } catch {
        // XR8.version() throws if version is queried too early.
      }

      requestAnimationFrame(checkXR8Version)
    }

    checkXR8Version()
  }

  return {
    disable,
    enable,
    clearVersions,
    setVersions,
  }
}

export {
  VersionManager,
}
