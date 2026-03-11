import React from 'react'
import {Trans, useTranslation} from 'react-i18next'

import {useDismissibleModal} from '../dismissible-modal-context'
import {StandardModal} from '../standard-modal'
import {StandardModalHeader} from '../standard-modal-header'
import {BasicModalContent} from '../basic-modal-content'
import {StandardModalActions} from '../standard-modal-actions'
import {LinkButton} from '../../ui/components/link-button'
import FileListIcon from '../file-list-icon'
import {createThemedStyles} from '../../ui/theme'
import {combine} from '../../common/styles'
import icons from '../../apps/icons'
import {StandardLink} from '../../ui/components/standard-link'
import {PrimaryButton} from '../../ui/components/primary-button'
import {DropTarget} from '../../studio/ui/drop-target'
import {IconButton} from '../../ui/components/icon-button'
import {StaticBanner} from '../../ui/components/banner'
import {containsFolderEntry} from '../../common/editor-files'
import {FOLDER_UPLOAD_ERROR} from '../editor-errors'
import {ConversionStatus, useAssetConverter} from '../../studio/hooks/use-asset-converter'
import {Loader} from '../../ui/components/loader'
import {useTheme} from '../../user/use-theme'
import useCurrentApp from '../../common/use-current-app'
import {useFileUploadState} from '../hooks/use-file-upload-state'
import {Tooltip} from '../../ui/components/tooltip'
import {Icon} from '../../ui/components/icon'
import StudioThreeModelPreview from '../../studio/studio-model-preview-with-loader'

const MODELS_DOC_LINK = 'https://8th.io/studio3dModel'

const useStyles = createThemedStyles(theme => ({
  contentContainer: {
    display: 'flex',
    flexDirection: 'row',
    marginBottom: '1em',
    width: '100%',
    gap: '0.75em',
  },
  contentChild: {
    flex: 1,
    height: '20em',
    position: 'relative',
    display: 'flex',
    flexDirection: 'column',
    overflow: 'hidden',
  },
  fileList: {
    height: '75%',
    padding: '0.75em 0em 0em 0em',
    backgroundColor: theme.sfcBackgroundDefault,
    border: `1px solid ${theme.sfcBorderDefault}`,
    display: 'flex',
    flexDirection: 'column',
    overflow: 'auto',
    borderRadius: '6px',
  },
  fileItemContainer: {
    'position': 'relative',
    '&:hover': {
      backgroundColor: theme.studioBtnHoverBg,
    },
  },
  fileItem: {
    display: 'flex',
    fontSize: '12px',
    alignItems: 'center',
    padding: '0.25em 0.75em 0.25em 1em',
    gap: '0.5em',
    width: '100%',
    cursor: 'pointer',
    textOverflow: 'ellipsis',
    overflow: 'hidden',
  },
  fileFailed: {
    color: theme.fgError,
  },
  iconContainer: {
    position: 'absolute',
    right: '0.75em',
    top: 0,
    display: 'flex',
    alignItems: 'center',
  },
  active: {
    backgroundColor: theme.studioBtnActiveBg,
  },
  previewContainer: {
    height: '100%',
    backgroundColor: theme.modalContentBg,
    display: 'flex',
    justifyContent: 'center',
    textAlign: 'center',
    flexGrow: 1,
    borderRadius: '6px',
  },
  uploadPrompt: {
    flexGrow: 1,
    alignContent: 'center',
    justifyContent: 'center',
    textAlign: 'center',
    color: theme.sfcDisabledColor,
    minHeight: '3em',
  },
  errorBanner: {
    width: '100%',
  },
  loader: {
    paddingTop: '0.25em',
  },
  fileListContainer: {
    display: 'flex',
    flexDirection: 'column',
    gap: '0.75em',
    height: '100%',
    overflow: 'hidden',
  },
  consoleContainer: {
    backgroundColor: theme.sfcBackgroundDefault,
    border: `1px solid ${theme.sfcBorderDefault}`,
    flex: 1,
    borderRadius: '6px',
    overflow: 'auto',
  },
  consoleText: {
    padding: '0.5em',
    display: 'flex',
    flexDirection: 'column',
    gap: '0.5em',
    fontFamily: 'Menloish, monospace',
    fontSize: '12px',
  },
  scrollbar: {
    '&::-webkit-scrollbar': {
      width: '6px',
    },
    '&::-webkit-scrollbar-track': {
      background: theme.scrollbarTrackBackground,
    },
    '&::-webkit-scrollbar-thumb': {
      background: theme.scrollbarThumbColor,
    },
    '&::-webkit-scrollbar-thumb:hover': {
      background: theme.scrollbarThumbHoverColor,
    },
  },
  noFilePreviewText: {
    color: theme.sfcDisabledColor,
  },
}))

type GlbConversionResult = {
  glbFile?: File
  modelUrl?: string
  err?: string
  stdout?: string
}

interface ISelectedFileResult {
  conversionResult: GlbConversionResult
}

const SelectedFilePreview: React.FC<ISelectedFileResult> = ({conversionResult}) => {
  const {t} = useTranslation(['cloud-studio-pages'])
  const {modelUrl, err} = conversionResult || {}

  let inner: React.ReactNode
  if (!conversionResult) {
    inner = <p>{t('fbx_to_glb_modal.file_preview.loading', {ns: 'cloud-studio-pages'})}</p>
  } else if (err) {
    inner = <p>{err}</p>
  } else if (modelUrl) {
    inner = (
      <StudioThreeModelPreview
        src={modelUrl}
        srcExt='glb'
        showDebugUi
      />
    )
  } else {
    inner = <p>{t('fbx_to_glb_modal.error.conversion_error', {ns: 'cloud-studio-pages'})}</p>
  }
  return inner
}

const SelectedFileConsole: React.FC<ISelectedFileResult> = ({conversionResult}) => {
  const {stdout = ''} = conversionResult || {}
  const classes = useStyles()

  const logLines = stdout.split('\n').map(line => (
    <span key={line}>{line}</span>
  ))

  return (
    <div className={classes.consoleText}>
      {logLines}
    </div>
  )
}

interface IFileView {
  filePath: string
  onSelect(filePath: string): void
  activePath: string
  onDelete(filePath: string): void
  status: ConversionStatus
  failed?: boolean
}

const FileView: React.FC<IFileView> = ({
  filePath, onSelect, activePath, onDelete, status, failed,
}) => {
  const classes = useStyles()
  return (
    <div className={classes.fileItemContainer}>
      <button
        className={combine(
          'style-reset', classes.fileItem, activePath === filePath && classes.active
        )}
        type='button'
        onClick={() => onSelect(filePath)}
      >
        <FileListIcon icon={icons.file_3d} />
        <span className={combine('file-name', failed && classes.fileFailed)}>
          {filePath}
        </span>
      </button>
      <div className={classes.iconContainer}>
        {status === 'pending'
          ? (
            <div className={classes.loader}>
              <Loader size='tiny' inline centered />
            </div>
          )
          : (
            <IconButton
              text='delete'
              stroke='delete12'
              color='main'
              onClick={() => onDelete(filePath)}
            />
          )}
      </div>
    </div>
  )
}

interface IFbxToGlbModal {
  files: File[]
  onClose: () => void
}

const FbxToGlbModal: React.FC<IFbxToGlbModal> = ({files: initialFiles, onClose}) => {
  useDismissibleModal(onClose)
  const classes = useStyles()
  const themeName = useTheme()
  const {t} = useTranslation(['cloud-studio-pages', 'common'])
  const [currentSelectedFile, setCurrentSelectedFile] = React.useState<string>(initialFiles[0].name)
  const [files, setFiles] = React.useState<File[]>(initialFiles)
  const [dropTargetActive, setDropTargetActive] = React.useState(false)
  const [error, setError] = React.useState<string | null>(null)
  const {convertAssets, conversionState, removeFileFromConversionState} = useAssetConverter()
  const [
    conversionResults, setConversionResults,
  ] = React.useState<{[fileName: string]: GlbConversionResult}>({})
  const modelUrlsRef = React.useRef([])

  const app = useCurrentApp()
  const {uploadFiles} = useFileUploadState(app.repoId, app.assetLimitOverrides)
  const [saving, setSaving] = React.useState(false)
  const canSave = !saving && files.every(f => !!conversionResults[f.name]) &&
  files.some(f => !!conversionResults[f.name]?.glbFile)

  const handleSubmit = async (e: React.FormEvent) => {
    e.preventDefault()
    setSaving(true)
    const glbFiles = Object.values(conversionResults)
      .filter(fileState => !!fileState.glbFile)
      .map(fileState => fileState.glbFile)
    await uploadFiles(glbFiles)
    setSaving(false)
    onClose()
  }

  const handleFileDelete = (filePath: string) => {
    const newFiles = files.filter(f => f.name !== filePath)
    setFiles(newFiles)
    setConversionResults((prev) => {
      const newResults = {...prev}
      delete newResults[filePath]
      return newResults
    })
    removeFileFromConversionState(filePath)
    if (currentSelectedFile === filePath) {
      setCurrentSelectedFile(newFiles.length > 0 ? newFiles[0].name : '')
    }
  }

  const handleFileDrop = async (e: React.DragEvent) => {
    e.persist()
    if (containsFolderEntry(e.dataTransfer.items)) {
      setError(FOLDER_UPLOAD_ERROR)
      return
    }
    const filesArray = Array.from(e.dataTransfer.files)
    const validFiles = filesArray.filter(
      f => f.name.endsWith('.fbx') && !files.find(prev => prev.name === f.name)
    )
    const invalidFiles = filesArray.filter(f => !f.name.endsWith('.fbx'))
    if (invalidFiles.length > 0) {
      setError(
        t('fbx_to_glb_modal.content.upload_type_error', {ns: 'cloud-studio-pages'})
      )
    }
    setFiles([...files, ...validFiles])
    setDropTargetActive(false)
    if (!currentSelectedFile) {
      setCurrentSelectedFile(validFiles[0].name)
    }
    await convertAssets(validFiles, 'fbxToGlb')
  }

  const parseConvertedFiles = async (convertedFiles: File[]) => {
    // handle "completed" file conversion
    // NOTE (cindyhu): "completed" can have two cases:
    // 1) FBX2glTF succeeded and we have a .glb file and res.json
    // 2) FBX2glTF failed and we have res.json with error message in stderr
    const glbFile = convertedFiles.find(f => f.name.endsWith('.glb'))
    const resFile = convertedFiles.find(f => f.name === 'res.json')
    let err = ''
    let modelUrl = ''
    let stdout = ''
    if (!glbFile && !resFile) {
      err = t('fbx_to_glb_modal.error.conversion_error', {ns: 'cloud-studio-pages'})
    }
    if (glbFile) {
      modelUrl = URL.createObjectURL(new Blob([glbFile]))
    }
    if (resFile) {
      const fileBuffer = await resFile.arrayBuffer()
      const res = JSON.parse(new TextDecoder().decode(fileBuffer))
      err = res.stderr || ''
      stdout = res.stdout || ''
    }
    return {glbFile, modelUrl, err, stdout}
  }

  React.useEffect(() => {
    const convertFiles = async () => {
      await convertAssets(initialFiles, 'fbxToGlb')
    }
    convertFiles()

    return () => {
      // clean up model urls
      modelUrlsRef.current.forEach(url => URL.revokeObjectURL(url))
    }
  }, [])

  React.useEffect(() => {
    const handleFileConversionRes = async () => {
      await Promise.all(Object.entries(conversionState).map(async ([fileName, fileState]) => {
        if (fileState.status === 'pending' || conversionResults[fileName]) {
          return
        }
        if (fileState.status === 'completed') {
          const {glbFile, modelUrl, err, stdout} = await parseConvertedFiles(fileState.files)
          setConversionResults(prev => ({
            ...prev,
            [fileName]: {glbFile, modelUrl, err, stdout},
          }))
        } else {
          setConversionResults(prev => ({
            ...prev,
            [fileName]: {err: fileState.error},
          }))
        }
      }))
    }

    handleFileConversionRes()
  }, [conversionState])

  return (
    <StandardModal
      onClose={onClose}
      closeOnDimmerClick={false}
    >
      <StandardModalHeader>
        <h2>{t('fbx_to_glb_modal.header', {ns: 'cloud-studio-pages'})}</h2>
        <p>
          <Trans
            i18nKey='fbx_to_glb_modal.description'
            ns='cloud-studio-pages'
            components={{
              docLink: <StandardLink underline color={null} newTab href={MODELS_DOC_LINK} />,
            }}
          />
        </p>
      </StandardModalHeader>
      <BasicModalContent>
        {error && (
          <div className={classes.errorBanner}>
            <StaticBanner type='danger' onClose={() => setError(null)}>
              {error}
            </StaticBanner>
          </div>
        )}
        <div className={classes.contentContainer}>
          <div className={classes.contentChild}>
            <p>
              {t('fbx_to_glb_modal.content.fbx_list', {ns: 'cloud-studio-pages'})}
              <Tooltip
                content={t('fbx_to_glb_modal.content.fbx_list_tooltip', {ns: 'cloud-studio-pages'})}
                zIndex={2000}
              >
                <Icon stroke='info' inline color='muted' />
              </Tooltip>
            </p>
            <div className={classes.fileListContainer}>
              <DropTarget
                className={combine(
                  classes.fileList, classes.scrollbar, dropTargetActive && classes.active
                )}
                onDrop={handleFileDrop}
                onHoverStart={() => setDropTargetActive(true)}
                onHoverStop={() => setDropTargetActive(false)}
              >
                {files.map(file => (
                  <FileView
                    key={file.name}
                    filePath={file.name}
                    onSelect={filePath => setCurrentSelectedFile(filePath)}
                    activePath={currentSelectedFile}
                    onDelete={handleFileDelete}
                    status={conversionState[file.name]?.status || 'pending'}
                    failed={!!conversionResults[file.name]?.err}
                  />
                ))}
                <p className={classes.uploadPrompt}>
                  {t('fbx_to_glb_modal.content.upload_prompt', {ns: 'cloud-studio-pages'})}
                </p>
              </DropTarget>
              <div className={combine(classes.consoleContainer, classes.scrollbar)}>
                {currentSelectedFile && conversionResults[currentSelectedFile] &&
                  <SelectedFileConsole
                    conversionResult={conversionResults[currentSelectedFile]}
                  />}
              </div>
            </div>
          </div>
          <div className={classes.contentChild}>
            <p>
              {t('fbx_to_glb_modal.content.glb_preview', {ns: 'cloud-studio-pages'})}
            </p>
            <div
              className={combine(
                'studio-editor', themeName, classes.previewContainer
              )}
            >
              {currentSelectedFile
                ? <SelectedFilePreview conversionResult={conversionResults[currentSelectedFile]} />
                : (
                  <p className={classes.noFilePreviewText}>
                    {t('fbx_to_glb_modal.content.no_file_preview', {ns: 'cloud-studio-pages'})}
                  </p>
                )}
            </div>
          </div>
        </div>
      </BasicModalContent>
      <StandardModalActions align='right'>
        <LinkButton
          onClick={onClose}
        >
          {t('button.cancel', {ns: 'common'})}
        </LinkButton>
        <PrimaryButton
          onClick={handleSubmit}
          disabled={!canSave}
          loading={saving}
        >
          {t('fbx_to_glb_modal.button.save', {ns: 'cloud-studio-pages'})}
        </PrimaryButton>
      </StandardModalActions>
    </StandardModal>
  )
}

export default FbxToGlbModal
