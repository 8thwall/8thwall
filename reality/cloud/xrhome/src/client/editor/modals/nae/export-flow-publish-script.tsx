import React from 'react'
import {useTranslation} from 'react-i18next'

import useCurrentApp from '../../../common/use-current-app'
import {runPublishScript} from '../../../studio/local-sync-api'
import {PrimaryButton} from '../../../ui/components/primary-button'
import {PublishPageWrapper} from '../../publishing/publish-page-wrapper'
import {useNaeStyles} from './nae-styles'

const ExportFlowPublishScript: React.FC = () => {
  const {t} = useTranslation(['cloud-editor-pages', 'common'])
  const classes = useNaeStyles()
  const {appKey} = useCurrentApp()
  const [isPublishing, setIsPublishing] = React.useState(false)

  const handlePublish = async () => {
    setIsPublishing(true)
    try {
      await runPublishScript(appKey)
    } finally {
      setIsPublishing(false)
    }
  }

  return (
    <PublishPageWrapper
      headline={t('editor_page.native_publish_modal.publish_script.headline')}
      headlineType='web'
      showProgressBar={isPublishing}
      displayText={isPublishing
        ? (
          <span className={classes.displayText}>
            {t('editor_page.native_publish_modal.publish_script.running')}
          </span>
        )
        : null}
      actionButton={(
        <PrimaryButton
          onClick={handlePublish}
          disabled={isPublishing}
        >
          {t('button.publish', {ns: 'common'})}
        </PrimaryButton>
      )}
    >
      <div />
    </PublishPageWrapper>
  )
}

export {ExportFlowPublishScript}
