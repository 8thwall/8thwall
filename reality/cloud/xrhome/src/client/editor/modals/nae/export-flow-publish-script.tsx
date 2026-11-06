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

type PublishState =
  | {status: 'idle'}
  | {status: 'publishing', startTime: number}
  | {status: 'success'}
  | {status: 'failed', startTime: number, completeTime: number}

const ExportFlowPublishScript: React.FC = () => {
  const {t} = useTranslation(['cloud-editor-pages', 'common'])
  const classes = useNaeStyles()
  const {appKey} = useCurrentApp()
  const dispatch = useDispatch()
  const [publishState, setPublishState] = React.useState<PublishState>({status: 'idle'})
  const isPublishing = publishState.status === 'publishing'

  const systemLogs = useLogStreams(appKey)
    .find(stream => publishState.status === 'failed' && stream.name === SYSTEM_STREAM_NAME)

  const publishLogs = systemLogs && publishState.status === 'failed'
    ? systemLogs.logs
      .filter(({timestamp}) => timestamp > publishState.startTime &&
        timestamp < publishState.completeTime)
      .map(log => log.text).join('\n')
    : ''

  const handlePublish = async () => {
    dispatch({type: 'ERROR', msg: null})
    const startTime = Date.now()
    setPublishState({status: 'publishing', startTime})
    try {
      await runPublishScript(appKey)
      setPublishState({status: 'success'})
    } catch {
      setPublishState({status: 'failed', startTime, completeTime: Date.now()})
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
        {publishState.status === 'failed' && (
          <>
            <StaticBanner type='danger'>
              {t('editor_page.native_publish_modal.publish_script.publish_error')}
            </StaticBanner>
            {publishLogs && <CopyableBlock text={publishLogs} description='' />}
          </>
        )}
        {publishState.status === 'success' && (
          <StaticBanner type='success'>
            {t('editor_page.native_publish_modal.publish_script.success')}
          </StaticBanner>
        )}
      </div>
    </PublishPageWrapper>
  )
}

export {ExportFlowPublishScript}
