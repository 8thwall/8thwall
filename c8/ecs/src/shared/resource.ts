import type {DeepReadonly} from 'ts-essentials'

import type {Resource, FontResource} from './scene-graph'
import {FONTS} from './fonts'

const extractResourceUrl = (resource: undefined | string | DeepReadonly<Resource>): string => {
  if (!resource) {
    return ''
  }

  if (typeof resource === 'string') {
    return resource
  }
  if (resource.type === 'url') {
    return resource.url
  }
  if (resource.type === 'asset') {
    return resource.asset
  }
  throw new Error('Unexpected resource type')
}

const inferFontResourceObject = (resource: string | FontResource): FontResource => {
  if (typeof resource === 'string') {
    if (resource.startsWith('assets/')) {
      return {type: 'asset', asset: resource}
    } else if (FONTS.has(resource)) {
      return {type: 'font', font: resource}
    } else {
      return {type: 'url', url: resource}
    }
  }
  return resource
}

const extractFontResourceUrl = (resource: FontResource): string => {
  if (resource.type === 'font') {
    return resource.font
  }
  return extractResourceUrl(resource)
}

// Upgrade a string resource into a Resource object type url or asset
const inferResourceObject = (resource: string | Resource): Resource => {
  if (typeof resource === 'string') {
    if (resource.startsWith('assets/')) {
      return {type: 'asset', asset: resource}
    } else {
      return {type: 'url', url: resource}
    }
  }
  return resource
}

export {
  extractResourceUrl,
  inferResourceObject,
  inferFontResourceObject,
  extractFontResourceUrl,
}
