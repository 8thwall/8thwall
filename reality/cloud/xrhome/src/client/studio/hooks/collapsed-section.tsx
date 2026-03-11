import React from 'react'

import {StudioStateContext, useStudioStateContext} from '../studio-state-context'
import {useSelectedTarget} from './selected-objects'

type SectionCollapsed = [boolean, (collapsed: boolean) => void]

const setSectionCollapsed = (
  stateCtx: StudioStateContext, objectId: string, componentId: string, collapsed: boolean
) => {
  if (collapsed) {
    stateCtx.update(p => ({
      ...p,
      collapsedSections: {
        ...p.collapsedSections,
        [objectId]: [...p.collapsedSections[objectId] || [], componentId],
      },
    }))
  } else {
    stateCtx.update(p => ({
      ...p,
      collapsedSections: {
        ...p.collapsedSections,
        [objectId]: p.collapsedSections[objectId]?.filter(id => id !== componentId),
      },
    }))
  }
}

const useSectionCollapsed = (componentId: string, defaultOpen: boolean): SectionCollapsed => {
  const stateCtx = useStudioStateContext()
  // NOTE(alancastillo): The fallbackCollapsed state is used when we don't have componentId/objectId
  const [fallbackCollapsed, setFallbackCollapsed] = React.useState<boolean>(!defaultOpen)
  const objectId = useSelectedTarget()
  if (!objectId || !componentId) {
    return [fallbackCollapsed, setFallbackCollapsed]
  }
  const isSectionCollapsed = !!stateCtx.state.collapsedSections[objectId]?.includes(componentId)
  return [
    isSectionCollapsed,
    (collapsed: boolean) => setSectionCollapsed(stateCtx, objectId, componentId, collapsed),
  ]
}

export {
  useSectionCollapsed,
  setSectionCollapsed,
}
