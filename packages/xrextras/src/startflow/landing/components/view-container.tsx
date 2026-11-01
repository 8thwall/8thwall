import {h} from 'preact'
import type {FunctionComponent as FC} from 'preact'

import {combine} from '../../../classnames'
import sharedStyles from '../../shared.module.scss'

interface IViewContainer {
  centered?: boolean
}

const ViewContainer: FC<IViewContainer> = ({children, centered = true}) => (
  <main
    className={combine(sharedStyles.screen, centered && sharedStyles.centered)}
    data-screen='landing'
  >
    {children}
  </main>
)

export {
  ViewContainer,
}
