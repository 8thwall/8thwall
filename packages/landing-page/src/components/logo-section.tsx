import * as React from 'preact'
import type {FunctionComponent as FC} from 'preact'

import {AttributionLogo} from './attribution-logo'

interface ILogoSection {
  logoSrc?: string
  logoAlt?: string
}

const LogoSection: FC<ILogoSection> = ({logoSrc, logoAlt}) => (
  <div className='landing8-logo-section'>
    {logoSrc && <img className='landing8-logo' src={logoSrc} alt={logoAlt} />}
    <AttributionLogo />
  </div>
)

export {
  LogoSection,
}
