import React from 'react'
import {useTranslation} from 'react-i18next'

import {createThemedStyles} from '../../ui/theme'
import ErrorMessage from '../../home/error-message'
import {HeadlineRow} from './publish-headline-row'
import {mango, blueberry} from '../../static/styles/settings'
import {bool, combine} from '../../common/styles'
import {TextButton} from '../../ui/components/text-button'
import {ProgressBar} from '../modals/nae/build-progress-bar'
import {Tooltip} from '../../ui/components/tooltip'

const useStyles = createThemedStyles({
  pageWrapper: {
    display: 'flex',
    flexDirection: 'column',
    width: '100%',
    height: '100%',
    justifyContent: 'space-between',
  },
  errorMessage: {
    width: '100%',
    padding: '0px',
  },
  topContent: {
    display: 'flex',
    flexDirection: 'column',
    flexGrow: 1,
    overflowY: 'auto',
    overflowX: 'hidden',
  },
  childrenContainer: {
    height: '100%',
    display: 'flex',
    flexDirection: 'row',
  },
  buttonContainer: {
    display: 'flex',
    justifyContent: 'flex-end',
    alignItems: 'center',
    gap: '1rem',
  },
  justifyStart: {
    justifyContent: 'flex-start',
  },
  justifyBetween: {
    justifyContent: 'space-between',
  },
  fixedTop: {
    flexShrink: 0,
  },
  scrollableContent: {
    flexGrow: 1,
    overflowY: 'auto',
    overflowX: 'hidden',
    display: 'flex',
    flexDirection: 'column',
    scrollbarGutter: 'stable',
  },
  progressBar: {
    paddingBottom: '1rem',
  },
})

type HeadLineType = 'web' | 'mobile' | 'build'

interface IPublishPageWrapper {
  // The page content.
  children: React.ReactNode
  showProgressBar?: boolean

  // The headline text for the page.
  headline: string
  headlineType: HeadLineType

  // Info text next to the buttons.
  displayText?: React.ReactNode

  // Buttons.
  onBack?: () => void
  secondaryButton?: React.ReactNode
  actionButton?: React.ReactNode
  actionButtonTooltip?: string
}

const headlineConfig = {
  web: {
    iconStroke: 'globe',
    iconColor: 'darkBlueberry',
    backgroundColor: `${blueberry}1a`,
  },
  mobile: {
    iconStroke: 'phone',
    iconColor: 'darkMango',
    backgroundColor: `${mango}26`,
  },
  build: {
    iconStroke: 'shelf',
    iconColor: 'info',
    backgroundColor: `${blueberry}55`,
  },
} as const

const headlineTypeToIconStroke = (type: HeadLineType) => headlineConfig[type].iconStroke

const headlineTypeToIconColor = (type: HeadLineType) => headlineConfig[type].iconColor

const headlineTypeToBackgroundColor = (
  type: HeadLineType
) => headlineConfig[type].backgroundColor

const PublishPageWrapper: React.FC<IPublishPageWrapper> = ({
  children,
  showProgressBar,
  headline,
  headlineType,
  displayText,
  onBack,
  secondaryButton,
  actionButton,
  actionButtonTooltip,
}) => {
  const classes = useStyles()
  const {t} = useTranslation(['common'])

  return (
    <div className={classes.pageWrapper}>
      <div className={classes.topContent}>
        {/* Fixed (non-scrolling) top area */}
        <div className={classes.fixedTop}>
          <HeadlineRow
            iconStroke={headlineTypeToIconStroke(headlineType)}
            iconSize={1}
            iconColor={headlineTypeToIconColor(headlineType)}
            backgroundColor={headlineTypeToBackgroundColor(headlineType)}
            text={headline}
          />
          <ErrorMessage className={classes.errorMessage} />
        </div>

        {/* Scrollable content */}
        <div className={classes.scrollableContent}>
          {children}
        </div>
      </div>
      {showProgressBar && (
        <div className={classes.progressBar}>
          <ProgressBar fauxProgressBar />
        </div>
      )}
      {/* The back, cancel, and action buttons at the bottom of the page. */}
      <div
        className={combine(
          classes.buttonContainer,
          bool((onBack && actionButton) || displayText, classes.justifyBetween),
          bool(onBack && !actionButton, classes.justifyStart)
        )}
      >
        {displayText}
        {onBack && (
          <TextButton
            height='small'
            onClick={onBack}
            a8='click;cloud-editor-export-flow;back'
          >
            {t('button.back')}
          </TextButton>
        )}

        {secondaryButton}
        {actionButtonTooltip
          ? (
            <Tooltip content={actionButtonTooltip} position='top' zIndex={1000}>
              {actionButton}
            </Tooltip>
          )
          : actionButton
        }
      </div>
    </div>
  )
}

export {PublishPageWrapper}
