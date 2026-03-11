import type {LayoutMetrics} from '@ecs/runtime/ui-types'
import {Vector3} from 'three'

const calculateUiContainerOffset = (layoutMetric: LayoutMetrics): Vector3 => {
  if (layoutMetric && layoutMetric.parentWidth && layoutMetric.parentHeight) {
    const bottom = layoutMetric.parentHeight - layoutMetric.top - layoutMetric.height
    const x = layoutMetric.left + (layoutMetric.width / 2)
    const y = bottom + (layoutMetric.height / 2)
    const cx = layoutMetric.parentWidth / 2
    const cy = layoutMetric.parentHeight / 2
    return new Vector3(cx - x, cy - y, 0)
  } else {
    return new Vector3(0, 0, 0)
  }
}

export {
  calculateUiContainerOffset,
}
