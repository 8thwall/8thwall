import {h} from 'preact'
import type {FunctionComponent as FC} from 'preact'

import styles from '../styles.module.scss'

import {AttributionLogo} from './attribution-logo'

interface ILogoSection {
  logoSrc?: string
  logoAlt?: string
  poweredByLabel: string
}

const LogoSection: FC<ILogoSection> = ({logoSrc, logoAlt, poweredByLabel}) => (
  <div className={styles.logoSection}>
    {logoSrc && <img className={styles.logo} src={logoSrc} alt={logoAlt} />}
    <AttributionLogo ariaLabel={poweredByLabel} />
  </div>
)

export {
  LogoSection,
}
