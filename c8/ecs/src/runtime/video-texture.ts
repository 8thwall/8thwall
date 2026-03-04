import THREE from './three'
import type * as THREE_TYPES from './three-types'

function createVideoTexture(src: string) {
  const video = Object.assign(document.createElement('video'), {
    crossOrigin: 'anonymous',
    src,
    playsInline: true,
    autoplay: false,
  })
  return new THREE.VideoTexture(video)
}

const createBlobVideoTexture = (blob: Blob) => {
  const url = URL.createObjectURL(blob)
  const texture = createVideoTexture(url)
  texture.userData.disposableUrl = url
  return texture
}

const deleteVideoTexture = (texture: THREE_TYPES.VideoTexture) => {
  const video = texture.image
  if (video) {
    video.pause()
    video.removeAttribute('src')
    video.load()
  }

  if (texture.userData.disposableUrl) {
    URL.revokeObjectURL(texture.userData.disposableUrl)
  }

  texture.dispose()
}

export {
  createVideoTexture,
  deleteVideoTexture,
  createBlobVideoTexture,
}
