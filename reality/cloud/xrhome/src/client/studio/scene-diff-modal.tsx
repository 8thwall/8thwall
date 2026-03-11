import React from 'react'

import {createUseStyles} from 'react-jss'

import {useTranslation} from 'react-i18next'

import {SceneEdit} from './scene-edit'
import {StudioAgentStateContextProvider} from './studio-agent/studio-agent-context'
import {Loader} from '../ui/components/loader'
import {FloatingTrayModal} from '../ui/components/floating-tray-modal'
import {MemoryRouterContext} from './memory-router-context'
import {PrefabsList} from './prefabs-list'
import {ReadonlySceneEditContext} from './readonly-scene-edit-context'
import {SceneDiffInfoContext, SceneDiffContext} from './scene-diff-context'
import {SceneStateContext} from './scene-state-context'
import {StandardToggleInput} from '../ui/components/standard-toggle-input'
import {Icon} from '../ui/components/icon'
import {BoldButton} from '../ui/components/bold-button'
import {YogaParentContextProvider} from './yoga-parent-context'

const MODAL_PADDING = '1rem'

const useStyles = createUseStyles({
  title: {
    padding: '1em',
    textAlign: 'center',
    marginTop: '0',
    // Using flexbox, with one aligned center, and the other
    // aligned to the left
    position: 'absolute',
    left: '50%',
    transform: 'translateX(-50%)',
  },
  sceneContainer: {
    // for absolutely positioned elements inside the scene
    position: 'relative',
    width: `calc(95vw - ${MODAL_PADDING} * 2)`,
    height: '80vh',
  },
  modalOverlay: {
    // this is the z-index of the select menus in the bars, as well as the side bars themselves,
    // only z-index that works so that the modal is above the old editing bars, but below the
    // menu popups we generate.
    zIndex: 10,
  },
  modalInner: {
    padding: MODAL_PADDING,
  },
  flexRow: {
    display: 'flex',
    flexDirection: 'row',
    alignItems: 'center',
    justifyContent: 'center',
    gap: '1rem',
  },
  titleDiv: {
    display: 'flex',
    flexDirection: 'row',
    paddingBlock: '2rem',
    paddingInline: '0.5rem',
    alignItems: 'center',
    position: 'relative',
  },
})

interface ISceneDiffModal {
  onClose: () => void
  diffContext: SceneDiffContext | null
  title: string
  initialSpace?: string
}

const SceneDiffModal: React.FC<ISceneDiffModal> = ({
  onClose, diffContext, title, initialSpace,
}) => {
  const {t} = useTranslation(['cloud-studio-pages', 'common'])
  const classes = useStyles()
  const [viewAfter, setViewAfter] = React.useState(true)

  const sceneDiffHeader = (
    <>
      <div
        className={classes.titleDiv}
      >
        <BoldButton onClick={onClose}>
          <Icon stroke='arrowLeft' inline />
          {t('button.back', {ns: 'common'})}
        </BoldButton>
        <h2 className={classes.title}>{title}</h2>
      </div>
      <label htmlFor='scenediff-view-toggle'>
        <div className={classes.flexRow}>
          <StandardToggleInput
            id='scenediff-view-toggle'
            label={t('scene_diff.before')}
            checked={viewAfter}
            onChange={b => setViewAfter(b)}
          />
          <span>{t('scene_diff.after')}</span>
        </div>
      </label>
    </>
  )

  return (
    <FloatingTrayModal
      startOpen
      trigger={null}
      onOpenChange={onClose}
      overlayClass={classes.modalOverlay}
    >
      {() => (
        <div>
          {sceneDiffHeader}
          <div className={classes.modalInner}>
            <div className={classes.sceneContainer} aria-busy={!diffContext}>
              {!diffContext
                ? (
                  <Loader />
                )
                : (
                  <MemoryRouterContext initialSpace={initialSpace}>
                    <SceneDiffInfoContext diffContext={diffContext}>
                      <SceneStateContext>
                        <ReadonlySceneEditContext
                          scene={
                          viewAfter ? diffContext.afterScene : diffContext.beforeScene
                        }
                        >
                          <YogaParentContextProvider>
                            <StudioAgentStateContextProvider>
                              <SceneEdit
                                fileBrowser={<PrefabsList />}
                              />
                            </StudioAgentStateContextProvider>
                          </YogaParentContextProvider>
                        </ReadonlySceneEditContext>
                      </SceneStateContext>
                    </SceneDiffInfoContext>
                  </MemoryRouterContext>
                )}
            </div>
          </div>
        </div>
      )}
    </FloatingTrayModal>
  )
}

export {
  SceneDiffModal,
}

export type {
  ISceneDiffModal,
}
