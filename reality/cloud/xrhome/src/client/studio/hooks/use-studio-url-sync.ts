import type {Location} from 'history'
import {useLocation} from 'react-router-dom'

import type {SwitchTab} from '../../editor/hooks/use-tab-actions'
import type {EditorTab} from '../../editor/tab-state'
import {
  deriveEditorRouteParams, editorFileLocationEqual, resolveEditorFileLocation,
} from '../../editor/editor-file-location'
import {parseStudioLocation} from '../studio-route'
import {useLayoutChangeEffect} from '../../hooks/use-change-effect'
import {parseHashFragment} from '../../editor/editor-utils'

type StudioUrlSyncLayoutDeps = [EditorTab, Location<unknown>]

const useStudioUrlSync = (
  curFileLocation: EditorTab,
  isTabsFromLocalForageLoaded: boolean,
  switchTab: SwitchTab,
  setFileUrlParam: (file: string | undefined) => void
) => {
  const location = useLocation()
  const pathLocation = resolveEditorFileLocation(parseStudioLocation(location))

  useLayoutChangeEffect<StudioUrlSyncLayoutDeps>(
    ([prevFileLocation, prevLocation]) => {
      if (!isTabsFromLocalForageLoaded || !prevLocation) {
        return
      }

      const prevPathLocation = resolveEditorFileLocation(parseStudioLocation(prevLocation))

      const {line, column} = parseHashFragment(location.hash)

      const hashChanged = location.hash &&
      ((location.hash !== prevLocation.hash) || (location.key !== prevLocation.key))

      const paramsNeedsUpdate = !editorFileLocationEqual(curFileLocation, prevFileLocation)
      const editorFileNeedsUpdate = !editorFileLocationEqual(pathLocation, prevPathLocation)

      if (paramsNeedsUpdate) {
        const params = deriveEditorRouteParams(curFileLocation)
        if (params === '') {
          return
        }
        setFileUrlParam(params)
      }

      if (editorFileNeedsUpdate || hashChanged) {
        switchTab(pathLocation, {line, column})
      }
    }, [curFileLocation, location]
  )
}

export {useStudioUrlSync}
