import React from 'react'
import {Trans, useTranslation} from 'react-i18next'

import type {IImageTarget} from '../common/types/models'
import {useFileContent} from '../git/hooks/use-file-content'
import {useEnclosedAppKey} from '../apps/enclosed-app-context'
import {StaticBanner} from '../ui/components/banner'
import {useStyles as useRowStyles} from './configuration/row-styles'
import {RowContent} from './configuration/row-content'
import {SpaceBetween} from '../ui/layout/space-between'
import {BoldButton} from '../ui/components/bold-button'
import {StandardLink} from '../ui/components/standard-link'

interface IMissingTargetRegisterWarning {
  target: IImageTarget
}

const MissingTargetRegisterWarning: React.FC<IMissingTargetRegisterWarning> = ({
  target,
}) => {
  const {t} = useTranslation('cloud-studio-pages')
  const appKey = useEnclosedAppKey()

  const appJsContent = useFileContent('app.js')
  const appTsContent = useFileContent('app.ts')

  const containsRegister = [appJsContent, appTsContent]
    .some(e => e?.includes(`${target.name}.json`))

  const [ignoredAtAppLevel, setIgnoredAtAppLevel] = React.useState(() => (
    localStorage.getItem(`missing-target-register-${appKey}`) === 'ignore'
  ))

  const [ignoredGlobally, setIgnoredGlobally] = React.useState(() => (
    localStorage.getItem('missing-target-register') === 'ignore'
  ))

  if (ignoredAtAppLevel || ignoredGlobally || containsRegister) {
    return null
  }

  return (
    <RowContent>
      <StaticBanner type='info'>
        <SpaceBetween direction='vertical' narrow>
          <p>
            <Trans
              ns='cloud-studio-pages'
              i18nKey='missing_target_register_warning.info.not_configured'
              components={{
                1: <StandardLink
                  href='https://8th.io/configuring-image-targets'
                  newTab
                />,
              }}
            />
          </p>
          <SpaceBetween>
            <BoldButton onClick={() => {
              setIgnoredAtAppLevel(true)
              localStorage.setItem(`missing-target-register-${appKey}`, 'ignore')
            }}
            >{t('missing_target_register_warning.button.hide_for_app')}
            </BoldButton>
            <BoldButton onClick={() => {
              setIgnoredGlobally(true)
              localStorage.setItem('missing-target-register', 'ignore')
            }}
            >{t('missing_target_register_warning.button.never_warn_again')}
            </BoldButton>
          </SpaceBetween>
        </SpaceBetween>
      </StaticBanner>
    </RowContent>

  )
}

export {
  MissingTargetRegisterWarning,
}
