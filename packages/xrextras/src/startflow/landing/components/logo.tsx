import {h} from 'preact'
import type {FunctionComponent} from 'preact'

import {AttributionLogo} from './attribution-logo'
import styles from '../styles.module.scss'
import type {StartFlowScreenOptions, StartFlowStrings} from '../../types'

const LandingLogo: FunctionComponent<{
  options: StartFlowScreenOptions
  strings: StartFlowStrings
}> = ({options, strings}) => (
  <div className={styles.logoSection}>
    {options.logoSrc && (
      <img
        className={styles.logo}
        src={options.logoSrc}
        alt={strings.landingLogoAlt}
      />
    )}
    <AttributionLogo ariaLabel={strings.poweredByLabel} />
  </div>
)

export {LandingLogo}
