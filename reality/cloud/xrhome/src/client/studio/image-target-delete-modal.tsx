import React from 'react'
import {useTranslation} from 'react-i18next'

import {createThemedStyles, useUiTheme} from '../ui/theme'
import {FloatingTrayModal} from '../ui/components/floating-tray-modal'
import {Icon} from '../ui/components/icon'
import type {IImageTarget} from '../common/types/models'
import useActions from '../common/use-actions'
import appsActions from '../apps/apps-actions'
import {useStudioStateContext} from './studio-state-context'

const useStyles = createThemedStyles(theme => ({
  content: {
    background: theme.modalBg,
    color: theme.modalFg,
    borderRadius: '15px',
    whiteSpace: 'pre-wrap',
    display: 'flex',
    flexDirection: 'column',
    padding: '2em',
    justifyContent: 'center',
    alignItems: 'center',
    textAlign: 'center',
    gap: '1.5em',
  },
  header: {
    margin: 'auto',
  },
  buttonRow: {
    display: 'flex',
    gap: '1em',
    justifyContent: 'center',
  },
  cancelButton: {
    background: 'none',
    color: theme.modalFg,
    border: 'none',
    cursor: 'pointer',
  },
  deleteButton: {
    background: theme.studioDangerBtnFg,
    color: 'white',
    border: 'none',
    padding: '0.5em 1em',
    borderRadius: '5px',
    cursor: 'pointer',
  },
}))

interface IDeleteImageTargetModal {
  trigger: React.ReactNode
  imageTarget: IImageTarget
}

const DeleteImageTargetModal: React.FC<IDeleteImageTargetModal> = ({
  trigger, imageTarget,
}) => {
  const {t} = useTranslation(['cloud-studio-pages'])
  const theme = useUiTheme()
  const stateCtx = useStudioStateContext()
  const classes = useStyles()
  const {deleteImageTarget} = useActions(appsActions)

  const deleteTarget = (collapse) => {
    deleteImageTarget(imageTarget.AppUuid, imageTarget.uuid)
    stateCtx.update((state) => {
      if (state.selectedImageTarget === imageTarget.uuid) {
        return {...state, selectedImageTarget: undefined}
      }
      return state
    })
    collapse()
  }

  return (
    <FloatingTrayModal trigger={trigger}>
      {collapse => (
        <div className={classes.content}>
          <Icon stroke='delete12' color={theme.modalFg} size={2.5} />
          <h1 className={classes.header}>{t('file_browser.image_targets.delete_modal.header')}</h1>
          <p>{t('file_browser.image_targets.delete_modal.body')}</p>
          <div className={classes.buttonRow}>
            <button
              type='button'
              className={classes.cancelButton}
              onClick={collapse}
            >
              {t('file_browser.image_targets.delete_modal.cancel')}
            </button>
            <button
              type='button'
              className={classes.deleteButton}
              onClick={() => deleteTarget(collapse)}
            >
              {t('file_browser.image_targets.delete_modal.confirm')}
            </button>
          </div>
        </div>
      )}
    </FloatingTrayModal>
  )
}

export {
  DeleteImageTargetModal,
}
