const toCdnUrl = path => (path && `https://cdn.8thwall.com/${path}`)

export const derivedImageTarget = target => ({
  ...target,
  originalImageSrc: target.originalImageUrl || toCdnUrl(target.originalImagePath),
  imageSrc: target.imageUrl || toCdnUrl(target.imagePath),
  thumbnailImageSrc: target.thumbnailImageUrl || toCdnUrl(target.thumbnailImagePath),
  geometryTextureImageSrc: target.geometryTextureUrl || toCdnUrl(target.geometryTexturePath),
})
