import {h, ComponentChildren, FunctionComponent} from 'preact'

import {combine} from '../classnames'
import sharedStyles from './shared.module.scss'

interface PanelProps {
  animate?: boolean
  ariaLabel?: string
  children: ComponentChildren
  className?: string
  style?: h.JSX.CSSProperties
}

const Panel: FunctionComponent<PanelProps> = ({
  animate = true, ariaLabel, children, className, style,
}) => (
  <section
    className={combine(sharedStyles.panel, animate && sharedStyles.panelEnter, className)}
    aria-label={ariaLabel}
    style={style}
  >
    {children}
  </section>
)

export {Panel}
