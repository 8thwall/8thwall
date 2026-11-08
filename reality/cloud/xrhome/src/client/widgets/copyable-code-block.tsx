import * as React from 'react'
import {CopyToClipboard} from 'react-copy-to-clipboard'
import {useTranslation} from 'react-i18next'
import type {Language} from 'prism-react-renderer'

import {createThemedStyles, UiTheme} from '../ui/theme'
import {PublishingPrimaryButton} from '../editor/publishing/publish-primary-button'
import {Icon} from '../ui/components/icon'
import {Tooltip} from '../ui/components/tooltip'
import CodeHighlight from '../browse/code-highlight'
import {useTheme} from '../user/use-theme'

const useStyles = createThemedStyles((theme: UiTheme) => ({
  wrapper: {
    overflow: 'hidden',
    width: '100%',
    borderRadius: '0.5rem',
    backgroundColor: theme.publishModalTextBg,
    position: 'relative',
  },
  label: {
    flex: '1 0 0',
    padding: '1rem 1rem 0',
    color: theme.fgMuted,
    minWidth: 0,
  },
  codeWrapper: {
    minWidth: 0,
    display: 'flex',
    flexDirection: 'column',
    gap: '1rem',
    margin: '1rem',
  },
  code: {
    whiteSpace: 'break-spaces',
    margin: 0,
  },
  actionContainer: {
    position: 'absolute',
    top: '1rem',
    right: '1rem',
    display: 'flex',
    flexDirection: 'row',
    gap: '0.5rem',
  },
}))

interface ICodeBlock {
  label?: string
  code: string
  language: Language
  actions?: React.ReactNode
}

const CodeBlock: React.FC<ICodeBlock> = ({label, code, language, actions}) => {
  const classes = useStyles()
  const theme = useTheme()

  return (
    <div className={classes.wrapper}>
      {label && <div className={classes.label}>{label}</div>}
      <div className={classes.codeWrapper}>
        {actions &&
          <div className={classes.actionContainer}>
            {actions}
          </div>}
        <pre className={classes.code}>
          <CodeHighlight
            content={code}
            language={language}
            themeMode={theme}
            simpleMode
          />
        </pre>
      </div>
    </div>
  )
}

interface ICopyableBlock {
  label?: string
  code: string
  disabled?: boolean
  tooltip?: string
  // If passed, we'll show a download button which downloads the code with this name.
  downloadFileName?: string
  language: Language
}

const CopyableCodeBlock: React.FunctionComponent<ICopyableBlock> = ({
  label, code, disabled, tooltip, downloadFileName, language,
}) => {
  const [isCopied, setIsCopied] = React.useState(false)
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
  }, [code])

  return (
    <CodeBlock
      language={language}
      label={label}
      code={code}
      actions={
        <>
          <CopyToClipboard text={code} onCopy={onCopy}>
            <PublishingPrimaryButton
              height='tiny'
              disabled={disabled}
            >
              <Tooltip
                content={tooltip}
                zIndex={501}
                openDelay={300}
                closeDelay={300}
              >
                <Icon stroke='copy' />
                {isCopied ? t('button.copied') : t('button.copy')}
              </Tooltip>
            </PublishingPrimaryButton>
          </CopyToClipboard>
          {!!downloadFileName &&
            <PublishingPrimaryButton
              height='tiny'
              disabled={disabled}
              onClick={() => {
                const blob = new Blob([code], {type: 'text/plain'})
                const url = window.URL.createObjectURL(blob)
                const a = document.createElement('a')
                a.href = url
                a.download = downloadFileName
                a.click()
                window.URL.revokeObjectURL(url)
              }}
            >
              <Icon stroke='downloadFile' />
              {t('button.download')}
            </PublishingPrimaryButton>}
        </>
      }
    />
  )
}

export {
  CopyableCodeBlock,
  CodeBlock,
}
