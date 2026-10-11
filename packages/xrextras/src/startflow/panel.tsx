import {h, ComponentChildren, FunctionComponent} from 'preact'

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
    className={`${sharedStyles.panel}${animate ? ` ${sharedStyles.panelEnter}` : ''}${
      className ? ` ${className}` : ''
    }`}
    aria-label={ariaLabel}
    style={style}
  >
    {children}
  </section>
)

export {Panel}
