import React from 'react'
import {useTranslation} from 'react-i18next'
import {createUseStyles} from 'react-jss'
import {useHistory} from 'react-router-dom'

import {PrimaryButton} from '../ui/components/primary-button'
import {headerSanSerif} from '../static/styles/settings'
import {SpaceBetween} from '../ui/layout/space-between'
import {ROOT_PATH} from './desktop-paths'

const useStyles = createUseStyles({
  notFoundPage: {
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

const NotFoundPage: React.FC = () => {
  const {t} = useTranslation(['caught-error-page', 'common'])
  const history = useHistory()

  const classes = useStyles()

  return (
    <div className={classes.notFoundPage}>
      <SpaceBetween direction='vertical' wide>
        <h1 className={classes.headingText}>
          {t('caught_error_page.not_found', {ns: 'caught-error-page'})}
        </h1>
        <div>
          <PrimaryButton onClick={() => history.push(ROOT_PATH)}>
            {t('button.back', {ns: 'common'})}
          </PrimaryButton>
        </div>
      </SpaceBetween>
    </div>
  )
}

export {
  NotFoundPage,
}
