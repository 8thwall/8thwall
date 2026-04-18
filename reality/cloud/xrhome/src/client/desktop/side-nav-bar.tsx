import React from 'react'
import {useTranslation} from 'react-i18next'

import {SpaceBetween} from '../ui/layout/space-between'
import {Popup} from '../ui/components/popup'
import type {IIconButton} from '../ui/components/icon-button'
import {combine} from '../common/styles'
import {Icon, IIcon} from '../ui/components/icon'
import {useBooleanUrlState} from '../hooks/url-state'
import {PreferencesModal} from './preferences-modal'
import {createThemedStyles} from '../ui/theme'

const DOCS_BASE_URL = 'https://8thwall.org/docs'
const FORUM_URL = 'https://github.com/orgs/8thwall/discussions'
const TUTORIALS_URL = 'https://8th.io/tutorials'

const useStyles = createThemedStyles(theme => ({
  sidebarButton: {
    'padding': '0.5rem',
    'cursor': 'pointer',
    'display': 'block',
    'borderRadius': '0.5rem',
    '&:disabled': {
      cursor: 'default',
      opacity: '0.5',
    },
    '&:hover': {
      'background': theme.studioBtnHoverBg,
    },
    '&:focus-visible': {
      boxShadow: `0 0 0 1px ${theme.sfcBorderFocus}`,
    },
  },
}))

interface ISidebarButton {
  onClick: () => void
  stroke: IIcon['stroke']
  text: string
}

const SideBarButton: React.FC<ISidebarButton> = ({stroke, text, onClick}) => {
  const classes = useStyles()
  return (
    <button
      type='button'
      className={combine('style-reset', classes.sidebarButton)}
      onClick={onClick}
      aria-label={text}
    >
      <Icon
        block
        stroke={stroke}
      />
    </button>
  )
}

interface ISideBarLinkOut extends Pick<IIconButton, 'stroke' | 'text'> {
  url: string
}

const SideBarLinkOut: React.FC<ISideBarLinkOut> = ({url, stroke, text}) => {
  const popupContent = (
    <SpaceBetween direction='horizontal' narrow centered>
      <span>{text}</span>
      <Icon stroke='arrowUpRight' color='main' />
    </SpaceBetween>
  )

  return (
    <Popup content={popupContent} position='right' alignment='center' size='tiny' delay={250}>
      <SideBarButton stroke={stroke} text={text} onClick={() => window.open(url, '_blank')} />
    </Popup>
  )
}

// TODO(Johnny): Update the icons to match the design when they are complete.
const SideNavBar: React.FC = () => {
  const {t} = useTranslation(['studio-desktop-pages'])
  const [preferencesOpen, setPreferencesOpen] = useBooleanUrlState('preferences', false)

  return (
    <>
      <SpaceBetween direction='vertical' extraNarrow>
        <SideBarLinkOut
          url={TUTORIALS_URL}
          stroke='tutorials'
          text={t('sidebar.tooltip.tutorials')}
        />
        <SideBarLinkOut
          url={DOCS_BASE_URL}
          stroke='documentation'
          text={t('sidebar.tooltip.docs')}
        />
        <SideBarLinkOut
          url={FORUM_URL}
          stroke='forum'
          text={t('sidebar.tooltip.forum')}
        />
        <SideBarButton
          onClick={() => setPreferencesOpen(true)}
          stroke='settings'
              // eslint-disable-next-line local-rules/hardcoded-copy
          text='Preferences'
        />
      </SpaceBetween>
      {preferencesOpen && <PreferencesModal onClose={() => setPreferencesOpen(false)} />}
    </>
  )
}

export {
  SideNavBar,
}
