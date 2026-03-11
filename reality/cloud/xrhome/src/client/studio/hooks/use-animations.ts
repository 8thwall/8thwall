import * as React from 'react'
import {useTranslation} from 'react-i18next'
import {Object3D, AnimationClip, AnimationAction, AnimationMixer} from 'three'

type Api<T extends AnimationClip> = {
  ref: React.RefObject<Object3D | undefined | null>
  clips: AnimationClip[]
  mixer: AnimationMixer
  names: T['name'][]
  actions: { [key in T['name']]: AnimationAction | null }
}

// Same as drei useAnimations but does not call useFrame
function useAnimationsNoRaf<T extends AnimationClip>(
  clips: T[],
  root?: React.RefObject<Object3D | undefined | null> | Object3D
): Api<T> {
  const ref = React.useRef<Object3D>(null)
  // eslint-disable-next-line max-len, no-nested-ternary
  const [actualRef] = React.useState(() => (root
    ? (root instanceof Object3D
      ? {current: root}
      : root)
    : ref))
  const [mixer] = React.useState(() => new AnimationMixer(undefined as unknown as Object3D))
  React.useLayoutEffect(() => {
    // @ts-ignore
    if (root) actualRef.current = root instanceof Object3D ? root : root.current
    ;(mixer as any)._root = actualRef.current
  })
  const lazyActions = React.useRef({})
  const api = React.useMemo<Api<T>>(() => {
    const actions = {} as { [key in T['name']]: AnimationAction | null }
    clips.forEach(clip => Object.defineProperty(actions, clip.name, {
      enumerable: true,
      // eslint-disable-next-line consistent-return, getter-return
      get() {
        if (actualRef.current) {
          // eslint-disable-next-line no-return-assign
          return (
            lazyActions.current[clip.name] ||
              (lazyActions.current[clip.name] = mixer.clipAction(clip, actualRef.current))
          )
        }
      },
      configurable: true,
    }))
    return {ref: actualRef, clips, actions, names: clips.map(c => c.name), mixer}
  }, [clips])
  React.useEffect(() => {
    const currentRoot = actualRef.current
    return () => {
      // Clean up only when clips change, wipe out lazy actions and uncache clips
      lazyActions.current = {}
      mixer.stopAllAction()
      Object.values(api.actions).forEach((action) => {
        if (currentRoot) {
          mixer.uncacheAction(action as AnimationClip, currentRoot)
        }
      })
    }
  }, [clips])

  return api
}

const useAnimationControls = (
  mixer: AnimationMixer,
  actions: { [key: string]: AnimationAction }
) => {
  const [animationClipToPlay, setAnimationClipToPlay] = React.useState<string | null>(null)
  const [isAnimationPaused, setIsAnimationPaused] = React.useState<boolean>(true)
  const setAnimationClip = (clipName: string | null) => {
    setAnimationClipToPlay(clipName)
    mixer.stopAllAction()
    if (!clipName) {
      return
    }
    // If useAnimationsNoRaf is used without root, this can be undefined
    actions[clipName]?.play()
    mixer.update(0)  // Reset the mixer to the start of the animation
  }

  const animateFrame = (state: any, delta: number) => {
    if (!isAnimationPaused) {
      mixer.update(delta)
    }
  }

  return {
    animationClipToPlay,
    setAnimationClipToPlay,
    isAnimationPaused,
    setIsAnimationPaused,
    setAnimationClip,
    animateFrame,
  }
}

const useAnimationOptions = (clips: AnimationClip[] | undefined) => {
  const {t} = useTranslation('asset-lab')
  const animationClipOptions = React.useMemo(() => ([
    {value: null, content: t('asset_lab.animation_clips_none')},
    ...(clips?.map(clip => ({
      value: clip.name,
      content: clip.name,
    })) || []),
  ]), [clips])
  return {
    animationClipOptions,
  }
}

export {
  useAnimationsNoRaf,
  useAnimationControls,
  useAnimationOptions,
}
