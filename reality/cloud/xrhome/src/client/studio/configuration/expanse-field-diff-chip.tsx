import React from 'react'

import type {
  DefaultableConfigDiffInfo,
  DefaultRenderConfig,
} from './diff-chip-types'
import {useScenePathContext} from '../scene-path-context'
import type {ExpanseField} from './expanse-field-types'
import {DiffChip} from './diff-chip'

interface IExpanseFieldDiffChip<VALUETYPE> {
  field: ExpanseField<DefaultableConfigDiffInfo<VALUETYPE, string[][]>>
  defaultRenderers?: DefaultRenderConfig<VALUETYPE>
}

// eslint-disable-next-line arrow-parens
const ExpanseFieldDiffChip = <VALUETYPE, >({
  field,
  defaultRenderers,
}: IExpanseFieldDiffChip<VALUETYPE>): React.JSX.Element => {
  const previousPath = useScenePathContext()

  if (!previousPath) {
    return null
  }

  if (typeof field === 'string') {
    const finalPaths = [[...previousPath, field]]
    if (defaultRenderers?.defaultRenderDiff) {
      return (
        <DiffChip
          finalPaths={finalPaths}
          renderDiff={defaultRenderers.defaultRenderDiff}
        />
      )
    } else if (defaultRenderers?.defaultRenderValue) {
      return (
        <DiffChip
          finalPaths={finalPaths}
          renderValue={defaultRenderers.defaultRenderValue}
        />
      )
    } else {
      // Maybe error here to make it more obvious that DiffChip will not be displayed.
      return null
    }
  }

  if (!field) {
    return null
  }

  const {leafPaths: rawLeafPaths, diffOptions} = field

  const defaultedDiffOptions = diffOptions ?? {}

  const leafPaths = Array.isArray(rawLeafPaths) ? rawLeafPaths : [[rawLeafPaths]]

  if (!leafPaths.length) {
    return null
  }
  const finalPaths = leafPaths.map(leaf => [...previousPath, ...leaf])

  if (diffOptions?.renderDiff) {
    return (
      <DiffChip
        {...defaultedDiffOptions}
        finalPaths={finalPaths}
      />
    )
  } else if (diffOptions?.renderValue) {
    return (
      <DiffChip
        {...defaultedDiffOptions}
        finalPaths={finalPaths}
      />
    )
  } else if (defaultRenderers?.defaultRenderDiff) {
    return (
      <DiffChip
        {...defaultedDiffOptions}
        finalPaths={finalPaths}
        renderDiff={defaultRenderers.defaultRenderDiff}
      />
    )
  } else if (defaultRenderers?.defaultRenderValue) {
    return (
      <DiffChip
        {...defaultedDiffOptions}
        finalPaths={finalPaths}
        renderValue={defaultRenderers.defaultRenderValue}
      />
    )
  } else {
    return null
  }
}

const CURRENT_SCOPE = [[]]

export {
  ExpanseFieldDiffChip,
  CURRENT_SCOPE,
}
