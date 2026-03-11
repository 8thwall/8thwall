import React from 'react'

import {StandardFieldLabel} from '../../ui/components/standard-field-label'
import {useStyles} from './row-styles'
import {UploadDrop} from '../../uiWidgets/upload-drop'
import {StandardFieldContainer} from '../../ui/components/standard-field-container'
import {useIdFallback} from '../use-id-fallback'

interface IRowUploadDrop {
  id?: string
  label: React.ReactNode
  onDrop: (file: File) => void
  disabled?: boolean
  dropMessage: string
  fileAccept: string
  dropContent: React.ReactNode
  dropClassName?: string
}

const RowUploadDrop: React.FC<IRowUploadDrop> = ({
  id: idOverride, label, onDrop, disabled, dropMessage, fileAccept, dropContent, dropClassName,
}) => {
  const id = useIdFallback(idOverride)
  const classes = useStyles()

  const handleDrop = (file: File) => {
    if (!disabled) {
      onDrop(file)
    }
  }

  return (
    <label htmlFor={id} className={classes.row}>
      <div className={classes.flexItem}>
        <StandardFieldLabel
          label={label}
          mutedColor
          disabled={disabled}
        />
      </div>
      <div className={classes.flexItem}>
        <StandardFieldContainer>
          <div className={classes.uploadContainer}>
            <UploadDrop
              uploadMessage=''
              elementClickInsteadOfButton
              dropMessage={dropMessage}
              onDrop={handleDrop}
              fileAccept={fileAccept}
              noButton
              className={dropClassName}
            >
              {dropContent}
            </UploadDrop>
            {disabled && (
              <div
                role='presentation'
                className={classes.disabledOverlay}
                onClick={(e) => {
                  e.preventDefault()
                  e.stopPropagation()
                }}
                onKeyDown={(e) => {
                  e.preventDefault()
                  e.stopPropagation()
                }}
                onDrop={(e) => {
                  e.preventDefault()
                  e.stopPropagation()
                }}
                onDragOver={(e) => {
                  e.preventDefault()
                  e.stopPropagation()
                }}
              />
            )}
          </div>
        </StandardFieldContainer>
      </div>
    </label>
  )
}

export {RowUploadDrop}
export type {IRowUploadDrop}
