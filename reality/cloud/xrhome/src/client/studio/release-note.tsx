import React from 'react'

import ReactMarkdown from 'react-markdown'
import Measure from 'react-measure'
import {useTranslation} from 'react-i18next'

import {createThemedStyles} from '../ui/theme'
import {FloatingPanelButton} from '../ui/components/floating-panel-button'
import {StandardLink} from '../ui/components/standard-link'
import {combine} from '../common/styles'
import AutoHeadingScope from '../widgets/auto-heading-scope'
import AutoHeading from '../widgets/auto-heading'

const BASE_URL = BuildIf.MATURE
  ? 'https://www-rc.8thwall.com/docs'
  : 'https://www.8thwall.com/docs'

const MAX_HEIGHT = 300

const useStyles = createThemedStyles(theme => ({
  releaseNote: {
    border: theme.studioSectionBorder,
    borderRadius: '8px',
    padding: '1em',
    margin: '1em 0',
    position: 'relative',
  },
  releaseNoteContent: {
    overflow: 'hidden',
    height: MAX_HEIGHT,
    marginBottom: '1em',
  },
  releaseNoteContentExpanded: {
    height: 'auto',
  },
  releaseDate: {
    position: 'absolute',
    right: '1em',
    top: '1em',
    color: theme.fgMuted,
  },
  runtimeVersion: {
    color: theme.fgMuted,
    marginRight: '1em',
  },
  innerHeading: {
    fontSize: '1.17em',
    textDecoration: 'underline',
  },
  embeddedImage: {
    maxWidth: '50%',
    maxHeight: '50vh',
    display: 'block',
    margin: '1em 0',
  },
}))

interface IHeadingRenderer {
  children: React.ReactNode
}

const HeadingRenderer: React.FC<IHeadingRenderer> = ({children}) => (
  <AutoHeading className={useStyles().innerHeading}>
    {children}
  </AutoHeading>
)

interface IImageRenderer {
  src: string
  alt: string
}

const ImageRenderer: React.FC<IImageRenderer> = ({src, alt}) => (
  <img
    className={useStyles().embeddedImage}
    src={src.startsWith('/') ? `${BASE_URL}${src}` : src}
    decoding='async'
    loading='lazy'
    alt={alt}
  />
)

interface ILinkRenderer {
  href: string
  children: React.ReactNode
}

const LinkRenderer: React.FC<ILinkRenderer> = ({href, children}) => (
  <StandardLink
    href={href.startsWith('/') ? `${BASE_URL}${href}` : href}
    underline
    newTab
  >
    {children}
  </StandardLink>
)

interface IReleaseNote {
  title: string
  year: number
  month: number
  day: number
  contents: string
  runtimeVersion?: string
  isLatest: boolean
}

const ReleaseNote: React.FC<IReleaseNote> = ({
  title, year, month, day, contents, runtimeVersion, isLatest,
}) => {
  const {t, i18n: {language}} = useTranslation(['cloud-studio-pages'])
  const classes = useStyles()

  const [isExpanded, setIsExpanded] = React.useState(isLatest)

  return (
    <div className={classes.releaseNote}>
      <AutoHeadingScope>
        <Measure scroll>
          {({measureRef, contentRect}) => {
            const isTruncated = contentRect.scroll.height > MAX_HEIGHT
            return (
              <>
                <div
                  className={combine(
                    classes.releaseNoteContent,
                    (isExpanded || !isTruncated) && classes.releaseNoteContentExpanded
                  )}
                  ref={measureRef}
                >
                  <AutoHeading>{title}</AutoHeading>
                  <span className={classes.releaseDate}>
                    {window.Intl.DateTimeFormat(language, {
                      // TODO(yuhsiang): Check how we format dates in other places.
                      year: 'numeric',
                      month: 'long',
                      day: 'numeric',
                    }).format(new Date(year, month - 1, day))}
                  </span>
                  {runtimeVersion && (
                    <>
                      <span className={classes.runtimeVersion}>
                        {t('release_notes.runtime_version', {version: runtimeVersion})}
                      </span>
                      <StandardLink
                        href={`${BASE_URL}/api/studio/changelog/#${runtimeVersion}`}
                        color='muted'
                        underline
                        newTab
                      >
                        {t('release_notes.view_changelog')}
                      </StandardLink>
                    </>
                  )}
                  <AutoHeadingScope>
                    <ReactMarkdown
                      source={contents}
                      renderers={{
                        heading: HeadingRenderer,
                        image: ImageRenderer,
                        link: LinkRenderer,
                      }}
                    />
                  </AutoHeadingScope>
                </div>
                {isTruncated && (
                  <FloatingPanelButton onClick={() => setIsExpanded(!isExpanded)}>
                    {isExpanded
                      ? t('release_notes.button.collapse')
                      : t('release_notes.button.expand')}
                  </FloatingPanelButton>
                )}
              </>
            )
          }}
        </Measure>
      </AutoHeadingScope>
    </div>
  )
}

export {ReleaseNote}
