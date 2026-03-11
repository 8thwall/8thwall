/* eslint-disable quote-props */
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
    overflow: 'hidden',
    color: dark => (dark ? brandWhite : brandBlack),
  },
  topPane: {
    display: 'flex',
    alignItems: 'stretch',
    borderTopLeftRadius: '0.5em',
    borderTopRightRadius: '0.5em',
    overflow: 'hidden',
  },
  description: {
    flex: '1 0 0',
    padding: '0.75rem 1rem',
    minWidth: 0,
    backgroundColor: dark => (dark ? gray5 : gray2),
  },
  mainPane: {
    width: '100%',
    borderBottomLeftRadius: '0.5em',
    borderBottomRightRadius: '0.5em',
    overflow: 'auto',
    backgroundColor: dark => (dark ? gray6 : gray1),
    padding: '0.5rem 1rem',
    margin: 0,
    '&.mono': {
      whiteSpace: 'pre',
      fontFamily: editorMonospace,
      fontSize: editorFontSize,
    },
  },
  button: {
    minWidth: '5.5em',
    textAlign: 'center',
    padding: '0.5em',
    backgroundColor: dark => (dark ? gray5 : gray2),
    cursor: 'pointer',
    border: '2px solid transparent',
    borderRadius: 0,
    borderTopRightRadius: '0.5em',
    color: dark => (dark ? brandWhite : brandBlack),
    fontWeight: 600,
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

interface ICopyableBlock {
  text: string
  description: string
  disabled?: boolean
  monospace?: boolean
  theme?: 'dark' | 'light'
  children?: React.ReactNode
}

const CopyableBlock: React.FunctionComponent<ICopyableBlock> = ({
  description, text, children, disabled, monospace = true, theme,
}) => {
  const [isCopied, setIsCopied] = React.useState(false)
  const classes = useStyles(theme === 'dark')
  const {t} = useTranslation(['common'])
  const timeoutRef = React.useRef<ReturnType<typeof setTimeout>>()

  const onCopy = () => {
    setIsCopied(true)
    clearTimeout(timeoutRef.current)
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
      <div className={classes.topPane}>
        <div className={classes.description}>{description}</div>
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
      <p className={combine(classes.mainPane, monospace && 'mono')}>
        {children || text}
      </p>
    </div>
  )
}

export default CopyableBlock
