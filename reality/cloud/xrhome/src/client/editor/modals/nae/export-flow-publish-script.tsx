import React from 'react'
import {useTranslation} from 'react-i18next'
import {useDispatch} from 'react-redux'
import {useQueryClient} from '@tanstack/react-query'

import useCurrentApp from '../../../common/use-current-app'
import {
  getProjectConfigStatusQuery, useProjectConfigStatus,
} from '../../../hooks/use-project-config'
import {openFile, runPublishScript} from '../../../studio/local-sync-api'
import {StaticBanner} from '../../../ui/components/banner'
import {FloatingPanelButton} from '../../../ui/components/floating-panel-button'
import {PrimaryButton} from '../../../ui/components/primary-button'
import {CodeBlock} from '../../../widgets/copyable-code-block'
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
  const {publishScript} = useProjectConfigStatus()
  const queryClient = useQueryClient()
  const publishScriptConfig = `{
  ...
  "scripts": {
    ...
    "publish": ${JSON.stringify(publishScript ||
      t('editor_page.native_publish_modal.publish_script.example_command'))}
  }
}`

  const logs = useLogStreams(appKey)

  const publishLogs = publishState.status === 'failed'
    ? logs
      .find(stream => stream.name === SYSTEM_STREAM_NAME)?.logs
      .filter(log => log.timestamp > publishState.startTime &&
        log.timestamp < publishState.completeTime)
      .map(log => log.text)
      .join('\n') || ''
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

  React.useEffect(() => {
    const focusHandler = () => {
      queryClient.invalidateQueries(getProjectConfigStatusQuery(appKey))
    }

    window.addEventListener('focus', focusHandler)

    return () => {
      window.removeEventListener('focus', focusHandler)
    }
  }, [])

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
          disabled={isPublishing || !publishScript}
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

        <CodeBlock
          language='json'
          label={t(publishScript
            ? 'editor_page.native_publish_modal.publish_script.configured_label'
            : 'editor_page.native_publish_modal.publish_script.example_label')}
          code={publishScriptConfig}
          actions={(
            <FloatingPanelButton onClick={() => openFile(appKey, 'package.json', 'root')}>
              {t('editor_page.native_publish_modal.publish_script.open_package_json')}
            </FloatingPanelButton>
          )}
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
