import React from 'react'

import type {AssetPromptInput} from '../generate-request'
import useCurrentAccount from '../../common/use-current-account'
import {getImageUrl} from '../../../shared/genai/constants'

interface IImagePromptPreview {
  imagePrompt: AssetPromptInput
  alt?: string
}

const ImagePromptPreview: React.FC<IImagePromptPreview> = ({imagePrompt, alt}) => {
  const currentAccount = useCurrentAccount()

  let src = null
  if (imagePrompt instanceof File) {
    // as long as imagePrompt is a local file, it's cheap to have many
    src = URL.createObjectURL(imagePrompt)
  } else if (typeof imagePrompt === 'string') {
    src = getImageUrl(currentAccount?.uuid, imagePrompt)
  }

  return <img src={src} alt={alt} />
}

export {ImagePromptPreview}
