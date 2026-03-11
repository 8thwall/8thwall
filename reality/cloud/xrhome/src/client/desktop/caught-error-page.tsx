import React from 'react'
import {Trans, useTranslation} from 'react-i18next'
import {createUseStyles} from 'react-jss'

import {PrimaryButton} from '../ui/components/primary-button'
import CopyableBlock from '../widgets/copyable-block'
import {StandardLink} from '../ui/components/standard-link'
import {headerSanSerif} from '../static/styles/settings'
import {SpaceBetween} from '../ui/layout/space-between'

const useStyles = createUseStyles({
  caughtErrorPage: {
    maxWidth: '40em',
    width: '100%',
    margin: '3em auto',
    padding: '0 2em',
  },
  headingText: {
    fontWeight: 900,
    fontFamily: headerSanSerif,
    fontSize: '2.25em',
    lineHeight: '1.25em',
    marginBottom: '0.5em',
  },
  details: {
    marginTop: '1em',
  },
})

interface ICaughtErrorPage {
  onReset: () => void
  error: Error
}

const CaughtErrorPage: React.FC<ICaughtErrorPage> = ({error, onReset}) => {
  const {t} = useTranslation(['caught-error-page', 'common'])

  const classes = useStyles()

  return (
    <div className={classes.caughtErrorPage}>
      <SpaceBetween direction='vertical' wide>
        <h1 className={classes.headingText}>
          {t('caught_error_page.heading')}
        </h1>
        <p>
          <Trans
            ns='caught-error-page'
            i18nKey='caught_error_page.description'
            components={{
              supportLink: <StandardLink href='mailto:support@8thwall.com'>1</StandardLink>,
            }}
          />
        </p>
        <div>
          <PrimaryButton onClick={onReset}>{t('button.back', {ns: 'common'})}</PrimaryButton>
        </div>
        <details>
          <summary>{t('caught_error_page.button.show_details')}</summary>
          <p>
            {t('caught_error_page.client_version.label')} {Build8.VERSION_ID}
          </p>
          <CopyableBlock description='' text={String(error.stack)} />
        </details>
      </SpaceBetween>
    </div>
  )
}

export {
  CaughtErrorPage,
}
