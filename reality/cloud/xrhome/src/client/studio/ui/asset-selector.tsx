import React from 'react'
import type {Resource} from '@ecs/shared/scene-graph'
import {createUseStyles} from 'react-jss'
import {useTranslation} from 'react-i18next'

import {inferResourceObject} from '@ecs/shared/resource'

import {
  ASSET_EXT_TO_KIND, getFilesByKind, type Kind,
} from '../common/studio-files'
import {useCurrentRepoId} from '../../git/repo-id-context'
import {useScopedGit} from '../../git/hooks/use-current-git'
import {basename, fileExt} from '../../editor/editor-common'
import {isAssetPath} from '../../common/editor-files'
import {DropTarget} from './drop-target'
import {
  RowTextField,
  RowSelectFieldWithSearch,
} from '../configuration/row-fields'
import {useEphemeralEditState} from '../configuration/ephemeral-edit-state'
import {FloatingPanelButton} from '../../ui/components/floating-panel-button'
import {RowContent} from '../configuration/row-content'
import {twoSidedBannerFromRenderValue} from '../configuration/diff-chip'
import {
  renderValueDefault,
  renderValueNonVisualResource,
} from '../configuration/diff-chip-default-renderers'
import type {DiffRenderInfo} from '../configuration/diff-chip-types'

const useStyles = createUseStyles({
  hovered: {
    outline: '2px solid #44a !important',
  },
})

const compareResources = (a: Resource | null, b: Resource | null) => {
  if (!a || !b) {
    return a === b
  }
  if (a.type === 'url' && b.type === 'url') {
    return a.url === b.url
  }
  if (a.type === 'asset' && b.type === 'asset') {
    return a.asset === b.asset
  }
  return false
}

interface IAssetSelector {
  label: React.ReactNode
  assetKind?: Kind
  resource: string | Resource
  onChange: (resource: Resource) => void
  onDelete?: () => void
  leafPaths?: string[][]
  // Only allowing single path to keep short,
  // wrap component in scene provider if needed.
}

const AssetSelector: React.FC<IAssetSelector> = ({
  label, assetKind, resource, onChange, onDelete, leafPaths,
}) => {
  const {t} = useTranslation(['cloud-studio-pages', 'common'])
  const classes = useStyles()
  const repoId = useCurrentRepoId()
  const filesByPath = useScopedGit(repoId, git => git.filesByPath)

  const assetFiles = getFilesByKind(filesByPath, assetKind)

  const [hovering, setHovering] = React.useState(false)

  const currentResource: Resource | null = inferResourceObject(resource)

  const dropdownValue = useEphemeralEditState({
    value: currentResource,
    deriveEditValue: (value) => {
      if (!value) {
        return ''
      }
      if (typeof value === 'string' || value.type === 'url') {
        return 'url'
      }
      return value.asset
    },
    parseEditValue: (value) => {
      if (!value) {
        return [true, null]
      }
      if (value === 'url') {
        return [true, {type: 'url', url: ''}] as const
      }
      return [true, {type: 'asset', asset: value}] as const
    },
    compareValue: compareResources,
    onChange,
  })

  const urlInputState = useEphemeralEditState<Resource | null, string>({
    value: currentResource,
    deriveEditValue: (value) => {
      if (value?.type === 'url') {
        return value.url
      }
      return ''
    },
    compareValue: compareResources,
    // NOTE(christoph): We don't trigger changes from the change handler, we use a save button
    parseEditValue: () => [false] as const,
    onChange: () => {},
  })

  const showInput = dropdownValue.editValue === 'url'

  const selectId = `urls-${React.useId()}`

  const handleAssetSelect = (filePath: string) => {
    onChange({type: 'asset', asset: filePath})
  }

  const handleDrop = (e: React.DragEvent) => {
    e.preventDefault()
    e.stopPropagation()
    const filePath = e.dataTransfer.getData('filePath')
    if (!filePath || !isAssetPath(filePath)) {
      return
    }
    if (assetKind && ASSET_EXT_TO_KIND[fileExt(filePath)] !== assetKind) {
      return
    }
    handleAssetSelect(filePath)
  }

  const assetOptions = assetFiles.map(filePath => ({
    value: filePath,
    content: basename(filePath),
  }))

  assetOptions.sort((a, b) => {
    const aBasename = a.content.toLowerCase()
    const bBasename = b.content.toLowerCase()
    return aBasename.localeCompare(bBasename)
  })

  const chooseValueRenderer = (value: string | Resource) => {
    if (typeof value === 'string') {
      return renderValueDefault
    } else if (typeof value === 'object') {
      return renderValueNonVisualResource
    } else {
      // Render nothing if the value is undefined.
      return () => null
    }
  }

  const renderAssetDiff = (
    valuePrev: string | Resource,
    valueNow: string | Resource
  ): DiffRenderInfo => {
    if (valuePrev === undefined && valueNow !== undefined) {
      return {
        type: 'added',
        bannerContent: null,
      }
    } else if (valuePrev !== undefined && valueNow === undefined) {
      return {
        type: 'removed',
        bannerContent: null,
      }
    } else if (
      (valuePrev === undefined && valueNow === undefined) ||
      valueNow === valuePrev ||
      JSON.stringify(valueNow) === JSON.stringify(valuePrev)
    ) {
      return {
        type: 'unchanged',
        bannerContent: null,
      }
    } else {
      const leftRenderer = chooseValueRenderer(valuePrev)
      const rightRenderer = chooseValueRenderer(valueNow)
      return {
        type: 'changedDirectly',
        bannerContent: twoSidedBannerFromRenderValue(
          leftRenderer, rightRenderer, valuePrev, valueNow
        ),
      }
    }
  }

  return (
    <DropTarget
      onHoverStart={() => setHovering(true)}
      onHoverStop={() => setHovering(false)}
      onDrop={(e) => {
        handleDrop(e)
        setHovering(false)
      }}
    >
      <form
        onSubmit={(event) => {
          event.preventDefault()
          onChange({type: 'url', url: urlInputState.editValue})
        }}
        className={hovering ? classes.hovered : ''}
      >
        <RowSelectFieldWithSearch
          id={selectId}
          options={[
            {value: '', content: t('asset_selector.option.none')},
            {value: 'url', content: 'URL'},
            ...assetOptions,
          ]}
          expanseField={leafPaths
            ? {
              leafPaths,
              diffOptions: {
                renderDiff: renderAssetDiff,
              },
            }
            : undefined}
          onChange={value => dropdownValue.setEditValue(value)}
          value={dropdownValue.editValue}
          label={label}
          onDelete={onDelete}
        />
        {showInput &&
          <div>
            <RowTextField
              id={`${selectId}-url`}
              label={t('asset_selector.url.label')}
              name='url'
              value={urlInputState.editValue}
              onChange={e => urlInputState.setEditValue(e.target.value)}
            />
            <RowContent>
              <FloatingPanelButton
                type='submit'
              >{t('button.save', {ns: 'common'})}
              </FloatingPanelButton>
            </RowContent>
          </div>
        }
      </form>
    </DropTarget>
  )
}

export {
  AssetSelector,
  compareResources,
}
