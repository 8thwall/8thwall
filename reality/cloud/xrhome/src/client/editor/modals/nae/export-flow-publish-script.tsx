import React from 'react'
import {useTranslation} from 'react-i18next'
import {useDispatch} from 'react-redux'

import useCurrentApp from '../../../common/use-current-app'
import {runPublishScript} from '../../../studio/local-sync-api'
import {StaticBanner} from '../../../ui/components/banner'
import {PrimaryButton} from '../../../ui/components/primary-button'
import CopyableBlock from '../../../widgets/copyable-block'
import {SYSTEM_STREAM_NAME} from '../../logs/log-constants'
import {useLogStreams} from '../../logs/use-log-streams'
import {PublishPageWrapper} from '../../publishing/publish-page-wrapper'
import {PublishTipBanner} from '../../publishing/publish-tip-banner'
import {useNaeStyles} from './nae-styles'

const ExportFlowPublishScript: React.FC = () => {
  const {t} = useTranslation(['cloud-editor-pages', 'common'])
  const classes = useNaeStyles()
  const {appKey} = useCurrentApp()
  const dispatch = useDispatch()
  const [isPublishing, setIsPublishing] = React.useState(false)
  const [publishStartTime, setPublishStartTime] = React.useState(0)
  const [publishCompleteTime, setPublishCompleteTime] = React.useState(0)
  const [publishFailed, setPublishFailed] = React.useState(false)
  const [didPublish, setDidPublish] = React.useState(false)

  const systemLogs = useLogStreams(appKey)
    .find(stream => publishFailed && stream.name === SYSTEM_STREAM_NAME)

  const publishLogs = systemLogs
    ? systemLogs.logs
      .filter(({timestamp}) => timestamp > publishStartTime &&
        (!publishCompleteTime || timestamp < publishCompleteTime))
      .map(log => log.text).join('\n')
    : ''

  const handlePublish = async () => {
    dispatch({type: 'ERROR', msg: null})
    setIsPublishing(true)
    setDidPublish(false)
    try {
      setPublishStartTime(Date.now())
      setPublishCompleteTime(0)
      setPublishFailed(false)
      await runPublishScript(appKey)
      setDidPublish(true)
    } catch {
      setPublishFailed(true)
    } finally {
      setPublishCompleteTime(Date.now())
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
      <div className={classes.rightCol}>
        <PublishTipBanner
          iconStroke='pointLight'
          content={t('editor_page.native_publish_modal.publish_script.description')}
        />
        {publishFailed && (
          <>
            <StaticBanner type='danger'>
              {t('editor_page.native_publish_modal.publish_script.publish_error')}
            </StaticBanner>
            {publishLogs && <CopyableBlock text={publishLogs} description='' />}
          </>
        )}
        {didPublish && (
          <StaticBanner type='success'>
            {t('editor_page.native_publish_modal.publish_script.success')}
          </StaticBanner>
        )}
      </div>
    </PublishPageWrapper>
  )
}

export {ExportFlowPublishScript}
