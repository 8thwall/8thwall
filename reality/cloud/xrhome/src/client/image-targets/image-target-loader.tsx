import React from 'react'
import {useTranslation} from 'react-i18next'
import {createUseStyles} from 'react-jss'

import {useEvent} from '../hooks/use-event'
import {useEnclosedApp} from '../apps/enclosed-app-context'
import {useSelector} from '../hooks'
import useActions from '../common/use-actions'
import imageTargetsActions from './actions'
import {FloatingPanelButton} from '../ui/components/floating-panel-button'
import {Loader} from '../ui/components/loader'

const useStyles = createUseStyles({
  loadMoreContainer: {
    display: 'flex',
    justifyContent: 'center',
    padding: '1rem',
  },
  loadingAdditional: {
    margin: '12px auto',
  },
  loadingInitial: {
    position: 'absolute',
    top: '50%',
    left: '50%',
    transform: 'translate(-50%, -50%)',
  },
})

interface IImageTargetLoaderProps {
  galleryUuid: string
}

// NOTE(chloe): This component is designed to be used at the bottom of a listed gallery of image
// targets where the load-additional Loader will appear inline at the bottom of list and the
// load-initial Loader will appear centered in the list's container.
const ImageTargetLoader: React.FC<IImageTargetLoaderProps> = ({galleryUuid}) => {
  const app = useEnclosedApp()
  const {t} = useTranslation(['cloud-studio-pages'])
  const classes = useStyles()
  const targetInfo = useSelector(s => s.imageTargets.targetInfoByApp[app.uuid])
  const gallery = targetInfo?.galleries?.[galleryUuid]
  const loadingAdditional = gallery?.status === 'loading-additional'
  const loadingInitial = gallery?.status === 'loading-initial'
  const hasMoreTargets = !!gallery?.continuation
  const {fetchAdditionalGalleryTargets} = useActions(imageTargetsActions)

  const canLoadMore = !loadingInitial && !loadingAdditional && hasMoreTargets
  const loadMoreTargets = () => {
    if (canLoadMore) {
      fetchAdditionalGalleryTargets(app.uuid, galleryUuid)
    }
  }

  const observerRef = React.useRef<IntersectionObserver>(null)
  const loadingRefCallback = useEvent((el) => {
    observerRef.current?.disconnect()
    if (el) {
      observerRef.current = new IntersectionObserver((entries) => {
        if (entries?.some(({isIntersecting}) => isIntersecting)) {
          loadMoreTargets()
        }
      },
      {
        root: null,
        threshold: 0,
      })
      observerRef.current.observe(el)
    } else {
      observerRef.current = null
    }
  })

  return (
    <>
      {loadingAdditional &&
        <div className={classes.loadingAdditional}>
          <Loader inline centered />
        </div>
      }
      {canLoadMore &&
        <div className={classes.loadMoreContainer} ref={loadingRefCallback}>
          <FloatingPanelButton onClick={loadMoreTargets}>
            {t('file_browser.image_targets.load_more')}
          </FloatingPanelButton>
        </div>}
      {loadingInitial &&
        <div className={classes.loadingInitial}>
          <Loader />
        </div>
      }
    </>
  )
}

export {
  ImageTargetLoader,
}
