import {h} from 'preact'
import type {FunctionComponent as FC} from 'preact'

import type {StartFlowContentProps} from '../../types'
import styles from '../styles.module.scss'

import {AttributionLogo} from './attribution-logo'

const LogoSection: FC<StartFlowContentProps> = ({options, strings}) => (
  <div className={styles.logoSection}>
    {options.logoSrc &&
      <img className={styles.logo} src={options.logoSrc} alt={strings.landingLogoAlt} />
    }
    <AttributionLogo ariaLabel={strings.poweredByLabel} />
  </div>
)

export {
  LogoSection,
}
