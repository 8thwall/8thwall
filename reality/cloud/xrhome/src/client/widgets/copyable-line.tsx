import * as React from 'react'
import {CopyToClipboard} from 'react-copy-to-clipboard'
import {useTranslation} from 'react-i18next'

import {combine} from '../common/styles'
import {
  gray1, gray2, gray3, gray4, gray5, gray6, brandWhite, brandBlack,
  editorMonospace, editorFontSize,
} from '../static/styles/settings'
import {createCustomUseStyles} from '../common/create-custom-use-styles'

const useStyles = createCustomUseStyles<boolean>()({
  wrapper: {
    display: 'inline-flex',
    overflow: 'hidden',
    maxWidth: '100%',
  },
  leftPane: {
    display: 'flex',
    alignItems: 'center',
    flex: '1 0 0',
    borderTopLeftRadius: '0.5em',
    borderBottomLeftRadius: '0.5em',
    backgroundColor: dark => (dark ? gray6 : gray1),
    color: dark => (dark ? brandWhite : brandBlack),
    padding: '0 1rem',
    overflow: 'hidden',
  },
  text: {
    width: '100%',
    overflow: 'hidden',
    textOverflow: 'ellipsis',
    userSelect: 'all',
    whiteSpace: 'pre',
  },
  mono: {
    fontFamily: editorMonospace,
    fontSize: editorFontSize,
  },
  blurred: {
    filter: 'blur(0.2em)',
    userSelect: 'none',
    pointerEvents: 'none',
  },
  button: {
    'minWidth': '5.5em',
    'textAlign': 'center',
    'padding': '0.5em',
    'backgroundColor': dark => (dark ? gray5 : gray2),
    'cursor': 'pointer',
    'border': '2px solid transparent',
    'borderRadius': 0,
    'borderTopRightRadius': '0.5em',
    'borderBottomRightRadius': '0.5em',
    'color': dark => (dark ? brandWhite : brandBlack),
    'fontWeight': 600,
    '&:hover:not(:disabled)': {
      backgroundColor: dark => (dark ? gray4 : gray3),
    },
    '&:focus': {
      borderColor: dark => (dark ? gray1 : gray4),
    },
    '@supports selector(:focus-visible)': {
      '&:focus': {
        borderColor: 'transparent',
      },
      '&:focus-visible': {
        borderColor: dark => (dark ? gray1 : gray4),
      },
    },
    '&:disabled': {
      cursor: 'default',
      color: gray4,
    },
  },
})

interface ICopyableLine {
  text: string
  disabled?: boolean
  monospace?: boolean
  theme?: 'dark' | 'light'
}

const CopyableLine: React.FunctionComponent<ICopyableLine> = ({
  text, disabled, monospace = true, theme,
}) => {
  const {t} = useTranslation(['common'])
  const [isCopied, setIsCopied] = React.useState(false)
  const classes = useStyles(theme === 'dark')
  const timeoutRef = React.useRef<ReturnType<typeof setTimeout>>()

  const onCopy = () => {
    setIsCopied(true)
    timeoutRef.current = setTimeout(() => {
      setIsCopied(false)
    }, 3000)
  }

  React.useEffect(() => () => {
    clearTimeout(timeoutRef.current)
  }, [])

  React.useEffect(() => {
    setIsCopied(false)
  }, [text])

  return (
    <div className={classes.wrapper}>
      <div className={classes.leftPane}>
        <p className={combine(classes.text,
          disabled && classes.blurred,
          monospace && classes.mono)}
        >
          {text}
        </p>
      </div>
      <CopyToClipboard text={text} onCopy={onCopy}>
        <button
          type='button'
          className={combine('style-reset', classes.button)}
          disabled={disabled}
        >
          {isCopied ? t('button.copied') : t('button.copy')}
        </button>
      </CopyToClipboard>
    </div>
  )
}

export default CopyableLine
