import * as React from 'react'
import {CopyToClipboard} from 'react-copy-to-clipboard'
import {useTranslation} from 'react-i18next'

import {brandWhite, csBlack, mobileViewOverride} from '../static/styles/settings'
import type {UiTheme} from '../ui/theme'
import {createCustomUseStyles} from '../common/create-custom-use-styles'
import {PublishingPrimaryButton} from '../editor/publishing/publish-primary-button'
import {Icon} from '../ui/components/icon'
import {Tooltip} from '../ui/components/tooltip'
import CodeHighlight from '../browse/code-highlight'

const useStyles = createCustomUseStyles<{}>()((theme: UiTheme) => ({
  wrapper: {
    overflow: 'hidden',
    width: '100%',
    color: brandWhite,
    borderRadius: '0.5rem',
  },
  label: {
    flex: '1 0 0',
    padding: '0.75rem 1rem',
    color: theme.fgMuted,
    minWidth: 0,
    backgroundColor: theme.publishModalTextBg,
  },
  codeWrapper: {
    padding: '0.75rem 1rem',
    minWidth: 0,
    backgroundColor: theme.publishModalTextBg,
  },
  codeWrapperInner: {
    display: 'flex',
    flexDirection: 'row',
    justifyContent: 'space-between',
    paddingLeft: '1rem',
    paddingRight: '1rem',
    width: '100%',
    borderRadius: '0.5rem',
    backgroundColor: csBlack,
    [mobileViewOverride]: {
      flexDirection: 'column',
      paddingBottom: '1rem',
    },
  },
  code: {
    whiteSpace: 'break-spaces',
    width: '100%',
  },
  buttonContainer: {
    paddingTop: '0.75rem',
    display: 'flex',
    flexDirection: 'column',
    gap: '0.25rem',
    [mobileViewOverride]: {
      paddingTop: '0rem',
      flexDirection: 'row',
    },
  },
  hidden: {
    // Keep the item in the DOM so that the buttons are the same width even when the download button
    // is hidden.
    visibility: 'hidden',
  },
}))

interface ICopyableBlock {
  label?: string
  code: string
  disabled?: boolean
  tooltip?: string
  // If passed, we'll show a download button which downloads the code with this name.
  downloadFileName?: string
}

const CopyableCodeBlock: React.FunctionComponent<ICopyableBlock> = ({
  label, code, disabled, tooltip, downloadFileName,
}) => {
  const [isCopied, setIsCopied] = React.useState(false)
  const classes = useStyles({})
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

  const downloadCode = () => {
    const blob = new Blob([code], {type: 'text/plain'})
    const url = window.URL.createObjectURL(blob)
    const a = document.createElement('a')
    a.href = url
    a.download = downloadFileName
    a.click()
    window.URL.revokeObjectURL(url)
  }

  return (
    <div className={classes.wrapper}>
      {label && <div className={classes.label}>{label}</div>}
      <div className={classes.codeWrapper}>
        <div className={classes.codeWrapperInner}>
          <pre className={classes.code}>
            <CodeHighlight
              content={code}
              language='markup'
              themeMode='dark'
              simpleMode
            />
          </pre>
          <div className={classes.buttonContainer}>
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
            <PublishingPrimaryButton
              height='tiny'
              disabled={disabled}
              onClick={downloadCode}
              className={!downloadFileName ? classes.hidden : undefined}
            >
              <Icon stroke='downloadFile' />
              {t('button.download')}
            </PublishingPrimaryButton>
          </div>
        </div>
      </div>
    </div>
  )
}

export default CopyableCodeBlock
