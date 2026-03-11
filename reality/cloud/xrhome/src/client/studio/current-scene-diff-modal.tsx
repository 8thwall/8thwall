import React from 'react'

import {useTranslation} from 'react-i18next'

import {useSceneDiffFromLanded} from './use-scene-diff-from-landed'
import {SceneDiffModal} from './scene-diff-modal'
import {useStudioStateContext} from './studio-state-context'

const CurrentSceneDiffModal: React.FC<{}> = () => {
  const sceneDiffContext = useSceneDiffFromLanded()
  const stateCtx = useStudioStateContext()
  const {t} = useTranslation(['cloud-studio-pages'])

  if (!sceneDiffContext) {
    return null
  }

  return (
    BuildIf.SCENE_DIFF_20250730 &&
      <SceneDiffModal
        onClose={() => stateCtx.update({sceneDiffOpen: false})}
        title={t('scene_diff.current_modal_title')}
        initialSpace={stateCtx.state.activeSpace}
        diffContext={sceneDiffContext}
      />
  )
}

export {
  CurrentSceneDiffModal,
}
