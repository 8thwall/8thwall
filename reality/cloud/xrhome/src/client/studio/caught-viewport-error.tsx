import React from 'react'
import {Trans, useTranslation} from 'react-i18next'

import {ErrorDisplay} from '../ui/components/error-display'
import {PrimaryButton} from '../ui/components/primary-button'
import {StandardLink} from '../ui/components/standard-link'
import CopyableBlock from '../widgets/copyable-block'

interface ICaughtViewportError {
  onReset: () => void
  error: Error
}

const CaughtViewportError: React.FC<ICaughtViewportError> = ({onReset, error}) => {
  const {t} = useTranslation(['caught-error-page', 'common'])
  return (
    <ErrorDisplay error={error}>
      <p>
        {t('caught_viewport_error.error_occurred')}
      </p>
      <p>
        <Trans
          ns='caught-error-page'
          i18nKey='caught_error_page.open_github_issue'
          components={{
            1: <StandardLink href='https://8th.io/report-desktop-error' newTab>1</StandardLink>,
          }}
        />
      </p>
      <PrimaryButton
        onClick={onReset}
      >
        {t('button.reset', {ns: 'common'})}
      </PrimaryButton>

      <details>
        <summary>{t('caught_error_page.button.show_details')}</summary>
        <p>
          {t('caught_error_page.client_version.label')} {Build8.VERSION_ID}
        </p>
        <CopyableBlock description='' text={String(error.stack)} />
      </details>
    </ErrorDisplay>
  )
}

export {
  CaughtViewportError,
}
