import React from 'react'
import {useTranslation} from 'react-i18next'

import {formatBytesToText} from '../../../../shared/asset-size-limits'
import type {HistoricalBuildData, HtmlShell} from '../../../../shared/nae/nae-types'
import {combine} from '../../../common/styles'
import useActions from '../../../common/use-actions'
import useCurrentApp from '../../../common/use-current-app'
import {useAbandonableEffect} from '../../../hooks/abandonable-effect'
import naeActions from '../../../studio/actions/nae-actions'
import {FloatingPanelIconButton} from '../../../ui/components/floating-panel-icon-button'
import {createThemedStyles} from '../../../ui/theme'
import {PublishPageWrapper} from '../../publishing/publish-page-wrapper'
import {downloadBuild, getBuildModeName, getEnvName, getFormattedDateTimeText} from './utils'

import {gray4} from '../../../static/styles/settings'
import {Icon} from '../../../ui/components/icon'

const useStyles = createThemedStyles(theme => ({
  buildsTable: {
    borderRadius: '0.5rem',
    width: '100%',
    border: `1px solid ${theme.subtleBorder}`,
    overflow: 'auto',
  },
  tableContainer: {
    display: 'flex',
    flexDirection: 'column',
    gap: '1px',
    minWidth: '45rem',
  },
  headerRow: {
    backgroundColor: theme.modalBg,
    padding: '0.5rem 0.75rem',
    display: 'flex',
    alignItems: 'center',
  },
  tableCell: {
    fontWeight: 600,
    fontSize: '12px',
    lineHeight: '16px',
    display: 'flex',
    flexDirection: 'column',
    justifyContent: 'center',
  },
  textMuted: {
    color: theme.fgMuted,
  },
  textMain: {
    color: theme.fgMain,
  },
  wSm: {
    width: '10%',
  },
  wMd: {
    width: '15%',
  },
  wLg: {
    width: '26%',
  },
  bgModal: {
    backgroundColor: theme.modalBg,
  },
  buildMainRow: {
    padding: '0.75rem 0.75rem 0 0.75rem',
    display: 'flex',
    gap: '0.75rem',
    alignItems: 'flex-end',
  },
  buildMainContent: {
    display: 'flex',
    alignItems: 'flex-end',
    flex: 1,
    fontSize: '12px',
    lineHeight: '16px',
  },
  platformColumn: {
    display: 'flex',
    gap: '0.75rem',
    overflow: 'hidden',
  },
  platformDetails: {
    display: 'flex',
    flexDirection: 'column',
    overflow: 'hidden',
    textOverflow: 'ellipsis',
    whiteSpace: 'nowrap',
  },
  platformName: {
    fontWeight: 700,
    color: theme.fgMuted,
  },
  displayName: {
    fontWeight: 600,
    color: theme.fgMain,
    overflow: 'hidden',
    textOverflow: 'ellipsis',
    whiteSpace: 'nowrap',
  },
  buildDetailsRow: {
    padding: '0.5rem 0.75rem 0.75rem 3.5rem',
    display: 'flex',
    alignItems: 'center',
  },
  buildDetails: {
    fontSize: '12px',
    lineHeight: '16px',
    color: theme.fgMuted,
  },
  italic: {
    fontStyle: 'italic',
  },
  textPublishModal: {
    color: theme.publishModalText,
  },
  noBuilds: {
    flex: '1 0 0',
    display: 'flex',
    flexDirection: 'column',
    alignItems: 'center',
    justifyContent: 'center',
    gap: '1rem',
    paddingBottom: '25rem',
  },
  noBuildsText: {
    color: gray4,
    textAlign: 'center',
    fontSize: '14px',
    lineHeight: '24px',
  },
}))

const platformNameMap = new Map<HtmlShell, string>([
  ['ios', 'iOS'],
  ['android', 'Android'],
])

const getPlatformDisplayName = (platform: HtmlShell) => platformNameMap.get(platform) || platform

const AllBuilds = () => {
  const classes = useStyles()
  const {t} = useTranslation(['cloud-editor-pages', 'common'])
  const {getRecentNaeBuilds} = useActions(naeActions)
  const app = useCurrentApp()
  const [recentNaeBuilds, setRecentNaeBuilds] = React.useState<HistoricalBuildData[]>(null)
  const {signedUrlForNaeBuild} = useActions(naeActions)
  useAbandonableEffect(async (executor) => {
    const res = await executor(getRecentNaeBuilds(app.uuid))
    setRecentNaeBuilds(res?.recentNaeBuilds || [])
  }, [app.uuid])
  return (
    <PublishPageWrapper
      headline={t('editor_page.export_modal.all_builds.headline')}
      headlineType='build'
    >
      {recentNaeBuilds?.length === 0 && (
        <div className={classes.noBuilds}>
          <Icon stroke='diagonalLine' size={3} color='gray4' />
          <div className={classes.noBuildsText}>
            {t('editor_page.export_modal.all_builds.no_builds')}
          </div>
        </div>
      )}
      {recentNaeBuilds?.length > 0 && (
        <div className={classes.buildsTable}>
          <div className={classes.tableContainer}>
            {/* Header Row */}
            <div className={classes.headerRow}>
              <div className={combine(classes.tableCell, classes.textMuted, classes.wLg)}>
                {t('editor_page.export_modal.all_builds.platform')}
              </div>
              <div className={combine(classes.tableCell, classes.textMuted, classes.wSm)}>
                {t('editor_page.export_modal.version_name')}
              </div>
              <div className={combine(classes.tableCell, classes.textMuted, classes.wMd)}>
                {t('editor_page.export_modal.build_mode')}
              </div>
              <div className={combine(classes.tableCell, classes.textMuted, classes.wMd)}>
                {t('editor_page.export_modal.build_env')}
              </div>
              <div className={combine(classes.tableCell, classes.textMuted, classes.wLg)}>
                {t('editor_page.export_modal.all_builds.date_time')}
              </div>
              <div className={combine(classes.tableCell,
                classes.textMuted,
                classes.wSm)}
              >
                {t('editor_page.export_modal.download_page_metadata.size')}
              </div>
            </div>
            {recentNaeBuilds.map((build) => {
            // TODO(xiaokai): add status message when app is sent to the App Store
              const statusMessage = ''
              const shouldDisplayVersionName = build.platform !== 'html'
              return (
                <div key={build.buildId} className={classes.bgModal}>
                  {/* Main Row */}
                  <div className={classes.buildMainRow}>
                    <div className={classes.buildMainContent}>
                      <div className={combine(classes.platformColumn, classes.wLg)}>
                        {build.status === 'SUCCESS'
                          ? <FloatingPanelIconButton
                              text={t('button.download', {ns: 'common'})}
                              stroke='download'
                              iconSize={0.875}
                              buttonSize='large'
                              onClick={async () => {
                                const {signedUrl} = await signedUrlForNaeBuild({
                                  appUuid: app.uuid,
                                  buildId: build.buildId,
                                  appExtension: build.exportType,
                                })

                                downloadBuild(signedUrl)
                              }}
                          />
                          : <FloatingPanelIconButton
                              text={t('status.failed', {ns: 'common'})}
                              stroke='info'
                              iconSize={0.875}
                              iconColor='danger'
                              buttonSize='large'
                              disabled
                          />
                      }
                        <div className={classes.platformDetails}>
                          <div className={classes.platformName}>
                            {getPlatformDisplayName(build.platform)}
                          </div>
                          <div className={classes.displayName}>
                            {build.displayName}
                          </div>
                        </div>
                      </div>
                      <div
                        className={combine(classes.tableCell, classes.textMain, classes.wSm)}
                      >
                        {shouldDisplayVersionName ? build.versionName : ''}
                      </div>
                      <div
                        className={combine(classes.tableCell, classes.textMain, classes.wMd)}
                      >
                        {getBuildModeName(t, build.buildMode)}
                      </div>
                      <div
                        className={combine(classes.tableCell, classes.textMain, classes.wMd)}
                      >
                        {getEnvName(t, build.refHead)}
                      </div>
                      <div
                        className={combine(classes.tableCell, classes.textMain, classes.wLg)}
                      >
                        {getFormattedDateTimeText(build.buildRequestTimestampMs)}
                      </div>
                      <div
                        className={combine(classes.tableCell,
                          classes.textMain,
                          classes.wSm)}
                      >
                        {formatBytesToText(build.sizeBytes)}
                      </div>
                    </div>
                  </div>

                  {/* Details Row */}
                  <div className={classes.buildDetailsRow}>
                    <div className={classes.buildDetails}>
                      <span className={classes.italic}>
                        .{build.exportType}
                      </span>
                      {build.status === 'SUCCESS' &&
                        <>
                          <span> | </span>
                          <span
                            className={combine(classes.italic, classes.textMuted)}
                          >
                            {t('editor_page.export_modal.all_builds.commit_id')}{' '}
                            {build.commitId.slice(0, 5)}
                          </span>
                        </>
                      }
                      {statusMessage && (
                        <>
                          <span> | </span>
                          <span className={classes.italic}>
                            {statusMessage}
                          </span>
                        </>
                      )}
                    </div>
                  </div>
                </div>
              )
            })}
          </div>
        </div>
      )}
    </PublishPageWrapper>
  )
}

export {AllBuilds}
