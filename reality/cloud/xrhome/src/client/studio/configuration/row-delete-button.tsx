import React from 'react'

import {IconButton} from '../../ui/components/icon-button'

interface IDeleteButton {
  onDelete: () => void
}

const DeleteButton: React.FC<IDeleteButton> = ({onDelete}) => (
  <IconButton
    onClick={(e) => {
      e.stopPropagation()
      onDelete()
    }}
    text='delete'
    stroke='delete12'
    inline
  />
)

export {DeleteButton}
export type {IDeleteButton}
