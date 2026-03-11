import React from 'react'

import {useTranslation} from 'react-i18next'

import {createThemedStyles} from '../../ui/theme'
import {Icon, IconColor, IconStroke} from '../../ui/components/icon'
import {combine} from '../../common/styles'
import LinkOut from '../../uiWidgets/link-out'
import {tinyViewOverride} from '../../static/styles/settings'

const useStyles = createThemedStyles(theme => ({
  publishTipBanner: {
    padding: '0.75rem 1rem',
    borderRadius: '0.5em',
    display: 'flex',
    flexDirection: 'row',
    backgroundColor: theme.publishModalTextBg,
    gap: '1em',
    color: theme.fgMuted,
    lineHeight: '1.5em',
    alignItems: 'center',
    width: '100%',
    fontSize: '12px',
  },
  hoverable: {
    '&:hover': {
      'backgroundColor': theme.publishModalBannerHoverBg,
      'color': theme.fgMuted,
      '& .learn-more-text': {
        color: theme.fgMain,
      },
    },
    'cursor': 'pointer',
  },
  iconContainer: {
    display: 'flex',
    padding: '0.25em',
    justifyContent: 'center',
    alignItems: 'center',
    width: '32px',
    height: '32px',
    flex: '0 0 auto',
    borderRadius: '0.25em',
    backgroundColor: theme.publishModalBg,
  },
  bannerTitle: {
    fontSize: '12px',
    fontWeight: 600,
    lineHeight: '16px',
  },
  content: {
    display: 'flex',
    flex: '1 1 auto',
    width: '100%',
    flexDirection: 'column',
    color: theme.fgMuted,
    gap: '0.5rem',
  },
  flexRow: {
    display: 'flex',
    flexDirection: 'row',
    [tinyViewOverride]: {
      flexDirection: 'column',
    },
  },
  bannerContentLeft: {
    display: 'flex',
    flexDirection: 'column',
    alignItems: 'flex-start',
    gap: '0.5rem',
    flex: '1 0 0',
  },
  bannerTextLeft: {
    fontSize: '12px',
    color: theme.fgMuted,
  },
  learnMoreLinkRightContainer: {
    display: 'flex',
    gap: '0.5rem',
  },
  learnMoreTextRight: {
    display: 'flex',
    justifyContent: 'space-between',
    alignItems: 'center',
    gap: '0.5rem',
    lineHeight: '20px',
  },
}))

interface IPublishTipBanner {
  content: React.ReactNode
  title?: string
  iconStroke?: IconStroke
  iconColor?: IconColor
  url?: string
  showLearnMore?: boolean
}

const PublishTipBanner: React.FC<IPublishTipBanner> = ({
  content,
  title,
  iconStroke,
  iconColor,
  url,
  showLearnMore = false,
}) => {
  const classes = useStyles()
  const {t} = useTranslation('common')

  const bannerContent = (
    <>
      {iconStroke && (
        <div className={classes.iconContainer}>
          <Icon stroke={iconStroke} color={iconColor} />
        </div>
      )}
      <div className={classes.content}>
        {showLearnMore
          ? (
          // Layout with "Learn More" on the right
            <div className={classes.flexRow}>
              <div className={classes.bannerContentLeft}>
                {title && (
                  <div className={classes.bannerTitle}>
                    {title}
                  </div>
                )}
                {content}
              </div>
              <div className={classes.learnMoreLinkRightContainer}>
                <div className={combine(classes.learnMoreTextRight, 'learn-more-text')}>
                  <Icon stroke='external' size={0.75} />
                  {t('button.learn_more', {ns: 'common'})}
                </div>
              </div>
            </div>
          )
          : (
            <>
              {title && <div className={classes.bannerTitle}>{title}</div>}
              {content}
            </>
          )}
      </div>
    </>
  )

  return url
    ? (
      <LinkOut className={combine(classes.publishTipBanner, classes.hoverable)} url={url}>
        {bannerContent}
      </LinkOut>
    )
    : (
      <div className={classes.publishTipBanner}>
        {bannerContent}
      </div>
    )
}

export {
  PublishTipBanner,
}
