/* eslint-disable quote-props */
import React from 'react'
import {useLocation} from 'react-router-dom'
import Highlight, {defaultProps, Language} from 'prism-react-renderer'
import lightTheme from 'prism-react-renderer/themes/github'
import darkTheme from 'prism-react-renderer/themes/vsDark'
import {Message} from 'semantic-ui-react'
import {createUseStyles} from 'react-jss'
import {useTranslation} from 'react-i18next'

import {fileExt} from '../editor/editor-common'
import * as settings from '../static/styles/settings'
import withTranslationLoaded from '../i18n/with-translations-loaded'

const useStyles = createUseStyles({
  prismCode: {
    '& table': {
      borderSpacing: 0,
    },
    overflowX: 'auto',
  },
  gutter: {
    textAlign: 'right',
    padding: '0 0.5em',
    userSelect: 'none',
    '& a': {
      color: settings.gray2,
      '&:hover': {
        color: settings.gray4,
      },
    },
  },
  selectableCode: {
    userSelect: 'text',
    '& *': {
      fontFamily: settings.editorMonospace,
      fontSize: '0.9em',
    },
    padding: 0,
    margin: 0,
  },
})

const ANCHOR_PREFIX = 'L'

interface ICodeHighlight {
  content: string
  language: Language
  lineOffset?: number
  pathToFile?: string
  simpleMode?: boolean
  themeMode?: 'light' | 'dark'
}

const CodeHighlight: React.FC<ICodeHighlight> = (
  {content, language, lineOffset = 0, pathToFile = '', simpleMode = false, themeMode = 'light'}
) => {
  const classes = useStyles()
  const location = useLocation()
  const {t} = useTranslation(['public-featured-pages'])

  React.useEffect(() => {
    // if the code is loaded after rendering has been done, hash link won't be jumped
    if (location.hash && location.hash.startsWith(`#${ANCHOR_PREFIX}`)) {
      const lineDom = document.getElementById(location.hash.substr(1))
      if (lineDom) {
        lineDom.scrollIntoView({block: 'center'})
      }
    }
  }, [location.hash])

  if (content === undefined) {
    return null
  }

  if (content === '') {
    return <Message info>{t('code_highlight.file_empty')}</Message>
  }

  const theme = themeMode === 'light' ? lightTheme : darkTheme

  if (simpleMode) {
    return (
      <Highlight {...defaultProps} theme={theme} code={content} language={language}>
        {({className, style, tokens, getLineProps, getTokenProps}) => (
          <code className={`${className} ${classes.prismCode}`} style={style}>
            {tokens.map((line, i) => (
              // eslint-disable-next-line react/no-array-index-key
              <div key={i} {...getLineProps({line, key: i})}>
                {line.map((token, key) => (
                  // eslint-disable-next-line react/no-array-index-key
                  <span key={i} {...getTokenProps({token, key})} />
                ))}
              </div>
            ))}
          </code>
        )}
      </Highlight>
    )
  }

  return (
    <Highlight {...defaultProps} theme={theme} code={content} language={language}>
      {({className, style, tokens, getLineProps, getTokenProps}) => (
        <pre className={`${className} ${classes.prismCode}`} style={style}>
          <table>
            <tbody>
              {tokens.map((line, i) => {
                const lineNumber = i + 1 + lineOffset
                return (
                  // eslint-disable-next-line react/jsx-key
                  <tr {...getLineProps({line, key: i})} id={`${ANCHOR_PREFIX}${lineNumber}`}>
                    <td className={classes.gutter}>
                      <a href={`${pathToFile}#${ANCHOR_PREFIX}${lineNumber}`}>{lineNumber}</a>
                    </td>
                    <td className={classes.selectableCode}>
                      {line.map((token, key) => (
                        // eslint-disable-next-line react/jsx-key
                        <span {...getTokenProps({token, key})} />
                      ))}
                    </td>
                  </tr>
                )
              })}
            </tbody>
          </table>
        </pre>
      )}
    </Highlight>
  )
}

const EXT_TO_LANGUAGE = {
  css: 'css',
  html: 'html',
  js: 'javascript',
  json: 'json',
  jsx: 'jsx',
  md: 'markdown',
  scss: 'scss',
  ts: 'typescript',
  tsx: 'tsx',
  txt: 'text',
  vue: 'html',
} as const

const filenameToLanguage = (filename: string): Language => (
  EXT_TO_LANGUAGE[fileExt(filename)] || 'text'
)

const TranslatedCodeHighlight = withTranslationLoaded(CodeHighlight)

export {
  TranslatedCodeHighlight as default,
  filenameToLanguage,
}
