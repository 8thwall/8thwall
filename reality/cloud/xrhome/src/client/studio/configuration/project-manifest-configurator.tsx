/* eslint-disable local-rules/hardcoded-copy */
import React from 'react'

import {DEFAULT_ECS_PAGE_BACKGROUND_COLOR} from '@repo/reality/shared/studio/ecs-config'

import {FloatingTraySection} from '../../ui/components/floating-tray-section'
import {useProjectManifest} from '../hooks/project-manifest'
import {RowBooleanField} from './row-boolean-field'
import {RowTextField} from './row-text-field'
import {useEphemeralEditStateWithSubmit} from './ephemeral-edit-state'
import {IconButton} from '../../ui/components/icon-button'
import {SpaceBetween} from '../../ui/layout/space-between'
import {StandardFieldLabel} from '../../ui/components/standard-field-label'
import {RowContent} from './row-content'
import {RowColorField} from './row-color-field'

const DEFAULT_PRELOAD_CHUNKS = ['slam', 'face']

const validateOptionalUrl = (value: string): boolean => {
  if (!value) {
    return true
  }
  try {
    const url = new URL(value)
    return value === url.toString()
  } catch {
    return false
  }
}

interface IRowSubmittableTextField {
  label: string
  value: string
  onSubmit: (newValue: string) => void
  validate?: (value: string) => boolean
}

const RowSubmittableTextField: React.FC<IRowSubmittableTextField> = ({
  label, value, onSubmit, validate,
}) => {
  const id = React.useId()

  const editState = useEphemeralEditStateWithSubmit({
    value,
    deriveEditValue: v => v,
    parseEditValue: (editValue) => {
      if (!validate) {
        return [true, editValue]
      }
      const isValid = validate(editValue)
      return [isValid, editValue]
    },
    onSubmit,
  })

  return (
    <RowTextField
      id={id}
      label={label}
      value={editState.editValue}
      onChange={e => editState.setEditValue(e.target.value)}
      onBlur={() => {
        editState.submit()
        editState.clear()
      }}
    />
  )
}

interface IRowDeferredColorField {
  label: string
  value: string
  onChange: (newValue: string) => Promise<void>
}

const RowDeferredColorField: React.FC<IRowDeferredColorField> = ({
  label, value, onChange,
}) => {
  const id = React.useId()

  const [deferredValue, setDeferredValue] = React.useState<string | null>(null)
  const latestEditRef = React.useRef<string | null>(null)
  const isFlushingRef = React.useRef(false)

  const flush = async () => {
    if (isFlushingRef.current) {
      return
    }
    isFlushingRef.current = true

    await new Promise(resolve => setTimeout(resolve, 500))

    const valueToSave = latestEditRef.current
    if (!valueToSave) {
      return
    }
    latestEditRef.current = null

    try {
      await onChange(valueToSave)
    } catch (err) {
      // eslint-disable-next-line no-console
      console.error('Failed to save deferred color field:', err)
    } finally {
      isFlushingRef.current = false
      if (latestEditRef.current) {
        flush()
      } else {
        setDeferredValue(null)
      }
    }
  }

  const handleChange = (newValue: string) => {
    setDeferredValue(newValue)
    latestEditRef.current = newValue
    flush()
  }

  return (
    <RowColorField
      id={id}
      label={label}
      value={deferredValue || value}
      onChange={handleChange}
    />
  )
}

const ProjectManifestConfigurator: React.FC = () => {
  const manifest = useProjectManifest()

  const updateConfig = (newConfig: Partial<typeof manifest.config>) => {
    manifest.update(old => ({
      ...old,
      config: {
        ...old.config,
        ...newConfig,
      },
    }))
  }

  return (
    <FloatingTraySection title='Project Manifest'>
      <RowBooleanField
        id='runtimeTypeCheck'
        label='runtimeTypeCheck'
        checked={!!manifest.config.runtimeTypeCheck}
        onChange={e => updateConfig({runtimeTypeCheck: !!e.target.checked})}
      />

      <RowSubmittableTextField
        label='runtimeUrl'
        value={manifest.config.runtimeUrl || ''}
        validate={validateOptionalUrl}
        onSubmit={(newValue: string) => updateConfig({runtimeUrl: newValue || undefined})}
      />

      <RowSubmittableTextField
        label='dev8Url'
        value={manifest.config.dev8Url || ''}
        validate={validateOptionalUrl}
        onSubmit={(newValue: string) => updateConfig({dev8Url: newValue || undefined})}
      />

      <RowSubmittableTextField
        label='app8Url'
        value={manifest.config.app8Url || ''}
        validate={validateOptionalUrl}
        onSubmit={(newValue: string) => updateConfig({app8Url: newValue || undefined})}
      />

      <RowSubmittableTextField
        label='preloadChunks'
        value={(manifest.config.preloadChunks || DEFAULT_PRELOAD_CHUNKS).join(', ')}
        onSubmit={(newValue: string) => {
          const preloadChunks = newValue.split(',').map(chunk => chunk.trim()).filter(Boolean)
          manifest.update(old => ({
            ...old,
            config: {
              ...old.config,
              preloadChunks,
            },
          }))
        }}
      />

      <RowDeferredColorField
        label='backgroundColor'
        value={manifest.config.backgroundColor || DEFAULT_ECS_PAGE_BACKGROUND_COLOR}
        onChange={(newValue: string) => manifest.update(old => ({
          ...old,
          config: {
            ...old.config,
            backgroundColor: newValue,
          },
        }))}
      />

      <RowBooleanField
        id='deferXr8'
        label='deferXr8'
        checked={!!manifest.config.deferXr8}
        onChange={(e) => {
          const deferXr8 = !!e.target.checked
          manifest.update(old => ({
            ...old,
            config: {
              ...old.config,
              deferXr8,
            },
          }))
        }}
      />

      <RowContent>
        <StandardFieldLabel label='scripts' />
      </RowContent>
      {manifest.config.scripts && manifest.config.scripts.length > 0 &&
        <ul>
          {manifest.config.scripts.map((script, index) => (
            // eslint-disable-next-line react/no-array-index-key
            <li key={index}>
              <SpaceBetween direction='horizontal'>
                {script.src}
                <IconButton
                  onClick={() => {
                    manifest.update(old => ({
                      ...old,
                      config: {
                        ...old.config,
                        scripts: (old.config?.scripts || []).filter((_, i) => i !== index),
                      },
                    }))
                  }}
                  stroke='delete12'
                  text='Delete'
                />
              </SpaceBetween>
            </li>
          ))}
        </ul>
      }

      <RowSubmittableTextField
        label='New Script URL'
        value=''
        validate={validateOptionalUrl}
        onSubmit={(newValue: string) => {
          if (!newValue) {
            return
          }
          manifest.update(old => ({
            ...old,
            config: {
              ...old.config,
              scripts: (old.config?.scripts || []).concat({src: newValue}),
            },
          }))
        }}
      />
    </FloatingTraySection>
  )
}

export {
  ProjectManifestConfigurator,
}
