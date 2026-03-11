import * as React from 'react'

import icons from '../apps/icons'
import {fileExt} from './editor-common'
import {FILE_KIND, BACKEND_FILE_EXT} from '../common/editor-files'

const buildFileIconMap = () => {
  const iconsForKind = {
    model: icons.file_3d,
    splat: icons.file_3d,  // TODO: Add splat icon
    hologram: icons.file_hologram,
    audio: icons.file_audio,
    css: icons.file_css,
    html: icons.file_html,
    image: icons.file_image,
    js: icons.file_js,
    json: icons.file_json,
    md: icons.file_md,
    react: icons.file_reactjs,
    txt: icons.file_default,
    video: icons.file_video,
    font: icons.file_font,
  }

  const iconMap = {}
  Object.keys(iconsForKind).forEach((kind) => {
    FILE_KIND[kind].forEach((ext) => {
      iconMap[ext] = iconsForKind[kind]
    })
  })

  return iconMap
}

const FILE_EXT_TO_ICON = buildFileIconMap()

const getIconForFile = (fileName: string) => {
  if (!fileName) {
    return icons.file_default
  }

  if (fileName.endsWith(BACKEND_FILE_EXT)) {
    return icons.file_backend
  }

  const ext = fileExt(fileName)
  if (ext && FILE_EXT_TO_ICON[ext]) {
    return FILE_EXT_TO_ICON[ext]
  }

  // NOTE(dat): icons.file_default happens to be text icon used for SPECIAL_TEXT_FILES
  return icons.file_default
}

interface IFileListIcon {
  filename?: string
  icon?: string
}
const FileListIcon: React.FunctionComponent<IFileListIcon> = ({filename, icon}) => {
  const iconSrc = React.useMemo(() => (
    icon || getIconForFile(filename)
  ), [filename, icon])

  // Alt text is empty to denote a presentation role
  return <img className='file-icon' src={iconSrc} alt='' />
}

export default FileListIcon
export type {IFileListIcon}
