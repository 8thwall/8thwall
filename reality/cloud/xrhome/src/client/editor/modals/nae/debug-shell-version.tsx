import React from 'react'
import {useTranslation} from 'react-i18next'

import {
  RowTextField,
  RowJointToggleButton,
} from '../../../studio/configuration/row-fields'
import {useStyles as useNaeModalsStyles} from './nae-modals-styles'

interface IDebugShellVersion {
  shellVersion: string
  setShellVersion: (shellVersion: string) => void
  removeExistingShellVersion: boolean
  setRemoveExistingShellVersion: (removeExistingShellVersion: boolean) => void
}

const DebugShellVersion: React.FC<IDebugShellVersion> = ({
  shellVersion,
  setShellVersion,
  removeExistingShellVersion,
  setRemoveExistingShellVersion,
}) => {
  const {t} = useTranslation(['cloud-editor-pages'])
  const naeModalsStyles = useNaeModalsStyles()

  return BuildIf.ALL_QA
    ? (
      <div className={naeModalsStyles.inputGroup}>
        <RowTextField
          id='shellVersion'
          label={t('editor_page.export_modal.shell_version_debug')}
          value={shellVersion}
          onChange={e => setShellVersion(e.target.value)}
        />
        <RowJointToggleButton
          id='removeExistingShellVersion'
          label={t('editor_page.export_modal.remove_existing_shell_version_debug')}
          options={[
            {
              // eslint-disable-next-line local-rules/hardcoded-copy
              content: 'Yes',
              value: 'true',
            },
            {
              // eslint-disable-next-line local-rules/hardcoded-copy
              content: 'No',
              value: 'false',
            },
          ]}
          onChange={(value) => { setRemoveExistingShellVersion(value === 'true') }}
          value={removeExistingShellVersion ? 'true' : 'false'}
        />
      </div>
    )
    : null
}

const useDebugShellVersion = () => {
  const [shellVersion, setShellVersion] = React.useState<string>('')
  const [removeExistingShellVersion, setRemoveExistingShellVersion] = React.useState(false)

  return {shellVersion, setShellVersion, removeExistingShellVersion, setRemoveExistingShellVersion}
}

export {DebugShellVersion, useDebugShellVersion}
