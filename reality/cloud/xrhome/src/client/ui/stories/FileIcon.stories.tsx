import React from 'react'
import type {Meta} from '@storybook/react'
import {createUseStyles} from 'react-jss'

import FileListIcon from '../../editor/file-list-icon'
import icons from '../../apps/icons'

const useStyles = createUseStyles({
  grid: {
    display: 'grid',
    gridTemplateColumns: 'repeat(4, 1fr)',
    gridGap: '1em',
    justifyItems: 'center',
  },
  icon: {
    'backgroundColor': '#e5e5f7',
    'backgroundImage': `linear-gradient(#444cf7 1px, transparent 1px)
    , linear-gradient(to right, #444cf7 1px, #e5e5f7 1px)`,
    'backgroundSize': '20px 20px',
    '& .file-icon': {
      display: 'block',
      width: '320px',
      opacity: 0.5,
      filter: 'brightness(0)',
    },
  },
})

const EXTENSIONS = `
.txt, ., .js, .mp4, .mp3, .hcap, .ttf, .css, .html, .jpg, .tsx, .json, .gltf
`.split(', ')

const IconPage: React.FC = () => {
  const classes = useStyles()
  return (
    <div className={classes.grid}>
      {EXTENSIONS.map(e => (
        <div key={e} className={classes.icon}>
          <FileListIcon filename={e} />
        </div>
      ))}
      <div className={classes.icon}>
        <FileListIcon icon={icons.folder_empty} />
      </div>
      <div className={classes.icon}>
        <FileListIcon icon={icons.folder_collapsed} />
      </div>
      <div className={classes.icon}>
        <FileListIcon icon={icons.folder_expanded} />
      </div>
      <div className={classes.icon}>
        <FileListIcon icon={icons.file_bundle} />
      </div>
      <div className={classes.icon}>
        <FileListIcon icon={icons.file_md} />
      </div>
    </div>
  )
}

export default {
  title: 'Views/FileListIcon',
  component: IconPage,
} as Meta<typeof IconPage>

export const All = {
  args: {},
}
