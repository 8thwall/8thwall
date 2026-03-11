import React from 'react'
import type {ContentRect} from 'react-measure'
import {useTranslation} from 'react-i18next'

import {FloatingTray} from '../ui/components/floating-tray'
import {FloatingIconButton} from '../ui/components/floating-icon-button'
import {useStudioStateContext} from './studio-state-context'
import {useSceneContext} from './scene-context'

const GIZMO_SPHERE_RADIUS = 50
const GIZMO_UI_MARGIN = 10
const GIZMO_SPHERE_SCALE = [GIZMO_SPHERE_RADIUS, GIZMO_SPHERE_RADIUS, GIZMO_SPHERE_RADIUS]

const getBottomPadding = (contentRect: ContentRect) => (
  contentRect.bounds.height - contentRect.entry.height
)

const getViewportControlLeftPosition = (contentRect: ContentRect) => (
  // previous calculation was:
  // `${contentRect.bounds.right + GIZMO_SPHERE_RADIUS + GIZMO_UI_MARGIN}px`,
  // but this is more accurate in cases where canvas is offset to the left,
  // since it will be nearest positioned ancestor element.
  `${
    contentRect.bounds.right -
    contentRect.bounds.left +
    (GIZMO_SPHERE_RADIUS * 2) +
    (GIZMO_UI_MARGIN * 2)
  }px`
)

const getViewportControlBottomPosition = (contentRect: ContentRect) => (
  `${getBottomPadding(contentRect)}px`
)

const getGizmoLeftMargin = (contentRect: ContentRect) => (
  contentRect.bounds.right - contentRect.bounds.left + GIZMO_SPHERE_RADIUS + GIZMO_UI_MARGIN
)

const getGizmoBottomMargin = (contentRect: ContentRect) => (
  getBottomPadding(contentRect) + GIZMO_SPHERE_RADIUS
)

interface IViewportControlTray {
  onResetCamera: () => void
  onCameraViewChange: () => void
}

const ViewportControlTray: React.FC<IViewportControlTray> = ({
  onResetCamera, onCameraViewChange,
}) => {
  const {t} = useTranslation(['cloud-studio-pages'])
  const stateCtx = useStudioStateContext()
  const {shadingMode, cameraView} = stateCtx.state
  const ctx = useSceneContext()

  const handleShadingMode = () => {
    stateCtx.update(p => ({...p, shadingMode: p.shadingMode === 'unlit' ? 'shaded' : 'unlit'}))
  }

  const handleCameraView = () => {
    onCameraViewChange()
    stateCtx.update(p => ({
      ...p,
      cameraView: p.cameraView === 'orthographic' ? 'perspective' : 'orthographic',
    }))
  }

  return (
    <FloatingTray orientation='vertical' nonInteractive={ctx.isDraggingGizmo}>
      <FloatingIconButton
        stroke='house'
        onClick={onResetCamera}
        text={t('viewport_control_tray.button.zoom_to_fit_scene')}
      />
      <FloatingIconButton
        stroke={shadingMode === 'unlit' ? 'filledCircle' : 'halfFilledCircle'}
        onClick={handleShadingMode}
        text={shadingMode === 'unlit'
          ? t('viewport_control_tray.button.unlit_draw_mode')
          : t('viewport_control_tray.button.shaded_draw_mode')}
      />
      <FloatingIconButton
        stroke={cameraView === 'perspective' ? 'projectionGrid' : 'grid'}
        onClick={handleCameraView}
        text={cameraView === 'perspective'
          ? t('viewport_control_tray.button.perspective_camera_view')
          : t('viewport_control_tray.button.orthographic_camera_view')}
      />
    </FloatingTray>
  )
}

export {
  ViewportControlTray,
  getViewportControlBottomPosition,
  getViewportControlLeftPosition,
  getGizmoLeftMargin,
  getGizmoBottomMargin,
  GIZMO_SPHERE_SCALE,
}
