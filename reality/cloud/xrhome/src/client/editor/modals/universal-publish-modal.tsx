import React from 'react'
import {useTranslation} from 'react-i18next'

import {combine} from '../../common/styles'
import {
  gray4,
  mobileViewOverride,
  smallMonitorViewOverride,
} from '../../static/styles/settings'
import {Badge} from '../../ui/components/badge'
import {Icon} from '../../ui/components/icon'
import {createThemedStyles} from '../../ui/theme'
import {usePublishingStateContext} from '../publishing/publish-context'
import {IconButton} from '../../ui/components/icon-button'
import {NoTriggerWrapModal} from '../../ui/components/no-trigger-wrap-modal'
import {ExportFlowHtml} from './nae/export-flow-html'
import {ExportFlowIframe} from './nae/export-flow-iframe'
import {AllBuilds} from './nae/all-builds'

interface IPublishModal {
  trigger: React.ReactElement
}

const hover = '&:hover'
const useStyles = createThemedStyles(theme => ({
  universalPublishModal: {
    fontSize: '12px !important',
    backgroundColor: `${theme.publishModalBg} !important`,
    color: `${theme.fgMain} !important`,
    boxSizing: 'border-box !important',
    width: '65rem !important',
    maxHeight: '90vh !important',
    aspectRatio: '4/3 !important',
    borderRadius: '0.875rem !important',
    display: 'flex',
    flexDirection: 'column',
    position: 'relative',
    [smallMonitorViewOverride]: {
      width: '75vw !important',
      maxWidth: '100vw !important',
      maxHeight: '95vh !important',
    },
    [mobileViewOverride]: {
      width: '96vw !important',
      maxWidth: '96vw !important',
      maxHeight: '96vh !important',
      aspectRatio: '5/8 !important',
    },
  },
  closeButton: {
    position: 'absolute',
    right: '1rem',
    top: '1rem',
    color: theme.fgMuted,
    [hover]: {
      color: theme.fgMain,
    },
  },
  content: {
    display: 'flex',
    gap: '1.5rem',
    flex: '1 1 auto',
    height: '100%',
    overflow: 'hidden',
  },
  mainContent: {
    display: 'flex',
    padding: '1rem 1.5rem 1.5rem 0rem',
    flexDirection: 'column',
    justifyContent: 'space-between',
    alignItems: 'flex-start',
    flex: '1 0 0',
    alignSelf: 'stretch',
    [mobileViewOverride]: {
      overflow: 'hidden',
      flex: '1 1 100%',
    },
  },
  sidebar: {
    flex: '1 1 20%',
    maxWidth: '200px',
    display: 'flex',
    padding: '1rem 0.75rem',
    flexDirection: 'column',
    gap: '1.25rem',
    alignSelf: 'stretch',
    backgroundColor: theme.publishModalSidebarNav,
    borderRight: `1px solid ${theme.modalContainerBg}`,
    borderTopLeftRadius: 'inherit',
    borderBottomLeftRadius: 'inherit',
    position: 'relative',
  },
  sidebarSection: {
    display: 'flex',
    flexDirection: 'column',
    alignItems: 'flex-start',
    gap: '0.5rem',
    alignSelf: 'stretch',
  },
  sidebarHeader: {
    display: 'flex',
    height: '1.5rem',
    padding: '0.375rem 0rem',
    alignItems: 'center',
    gap: '0.5rem',
    alignSelf: 'stretch',
    color: gray4,
    [smallMonitorViewOverride]: {
      height: '3.5rem',
    },
  },
  sidebarLabel: {
    width: '14px',
    height: '14px',
    aspectRatio: '1/1',
  },
  sidebarItem: {
    display: 'flex',
    padding: '8px',
    justifyContent: 'space-between',
    fontWeight: 600,
    cursor: 'pointer',
    borderRadius: '0.25rem',
    alignSelf: 'stretch',
    color: theme.publishModalText,
    textAlign: 'left',
    lineHeight: '16px',
    alignItems: 'center',
    [hover]: {
      backgroundColor: `${theme.publishModalSidebarActiveBg} !important`,
      color: `${theme.studioBtnHoverFg} !important`,
    },
  },
  sideBarItemSelected: {
    backgroundColor: `${theme.publishModalSidebarActiveBg} !important`,
    color: `${theme.studioBtnHoverFg} !important`,
  },
  allBuildsButtonPosition: {
    position: 'absolute',
    bottom: '0.75rem',
    left: '0.75rem',
    right: '0.75rem',
  },
  hr: {
    border: `0.5px solid ${theme.modalContainerBg}`,
    margin: '0',
  },
}))

interface IPublishModalOption {
  className?: string
  selected: boolean
  onSelect: () => void
  label: string
}

const PublishModalOption: React.FC<IPublishModalOption> = ({
  selected, onSelect, label, className,
}) => {
  const classes = useStyles()

  return (
    <button
      type='button'
      className={combine(
        'style-reset',
        classes.sidebarItem,
        className,
        selected ? classes.sideBarItemSelected : ''
      )}
      onClick={onSelect}
    >
      <span>{label}</span>
    </button>
  )
}

const UniversalPublishModal: React.FC<IPublishModal> = ({
  trigger,
}) => {
  const classes = useStyles()
  const {t} = useTranslation(['cloud-editor-pages', 'common'])
  const isStudio = true
  const {activePublishModalOption, setActivePublishModalOption} = usePublishingStateContext()

  const handleModalIsOpen = (isOpen: boolean) => {
    if (isOpen) {
      // This is called when we click on the top-left Publish button. We always want to open back
      // up to the web publish model when clicking that button.
      setActivePublishModalOption('html')
    } else {
      setActivePublishModalOption(null)
    }
  }

  return (
    <NoTriggerWrapModal
      trigger={trigger}
      isOpen={!!activePublishModalOption}
      setIsOpen={handleModalIsOpen}
    >
      {collapse => (
        <div className={classes.universalPublishModal}>
          <div className={classes.closeButton}>
            <IconButton
              text={t('button.close')}
              stroke='close'
              size={0.625}
              onClick={collapse}
            />
          </div>
          <div className={classes.content}>
            <div className={classes.sidebar}>
              <div className={classes.sidebarSection}>
                <div className={classes.sidebarHeader}>
                  <div className={classes.sidebarLabel}>
                    <Icon stroke='globe' size={0.875} color='gray4' />
                  </div>
                  <span>
                    {t('editor_page.native_publish_modal.publish')}
                  </span>
                </div>
                <PublishModalOption
                  selected={activePublishModalOption === 'html'}
                  onSelect={() => setActivePublishModalOption('html')}
                  label='HTML5'
                />
                <PublishModalOption
                  selected={activePublishModalOption === 'iframe'}
                  onSelect={() => setActivePublishModalOption('iframe')}
                  label={t('editor_page.native_publish_modal.iframe')}
                />
              </div>

              {BuildIf.NAE_MODALS_VISIBLE_20260202 && isStudio &&
                <><hr className={classes.hr} />
                  <div className={classes.sidebarSection}>
                    <Badge color='gray' variant='outlined' height='micro'>
                      {t('editor_page.native_publish_modal.beta')}
                    </Badge>
                    <div className={classes.sidebarHeader}>
                      <div className={classes.sidebarLabel}>
                        <Icon stroke='cubicle' size={1} color='gray4' />
                      </div>
                      <span>
                        {t('editor_page.native_publish_modal.export')}
                      </span>
                    </div>

                    <PublishModalOption
                      selected={activePublishModalOption === 'android'}
                      onSelect={() => setActivePublishModalOption('android')}
                      label='Android'
                    />
                    <PublishModalOption
                      selected={activePublishModalOption === 'ios'}
                      onSelect={() => setActivePublishModalOption('ios')}
                      label='iOS'
                    />
                  </div>
                </>}
              {BuildIf.NAE_MODALS_VISIBLE_20260202 && isStudio && (
                <PublishModalOption
                  className={classes.allBuildsButtonPosition}
                  selected={activePublishModalOption === 'all-builds'}
                  onSelect={() => setActivePublishModalOption('all-builds')}
                  label={t('editor_page.export_modal.recent_builds')}
                />
              )}
            </div>

            <div className={classes.mainContent}>
              {
                  activePublishModalOption === 'html' && (
                    <ExportFlowHtml
                      onClose={collapse}
                    />
                  )
              }
              {
                  activePublishModalOption === 'iframe' && (
                    <ExportFlowIframe />
                  )
              }
              {
                  activePublishModalOption === 'all-builds' && (
                    <AllBuilds />
                  )
                }
            </div>
          </div>
        </div>
      )}
    </NoTriggerWrapModal>
  )
}

export {UniversalPublishModal}
