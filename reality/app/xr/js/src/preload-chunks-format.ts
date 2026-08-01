import type {ChunkName, ChunkSpecifier} from './types/api'

const parsePreloadChunks = (attribute: string): ChunkSpecifier[] => {
  if (!attribute) {
    return []
  }

  return attribute.split(',').map(e => e.trim()).filter(Boolean).map((segment) => {
    const urlStartsAfter = segment.indexOf(':')
    if (urlStartsAfter === -1) {
      return {name: segment as ChunkName, url: null}
    }

    return {
      name: segment.slice(0, urlStartsAfter).trim() as ChunkName,
      url: segment.slice(urlStartsAfter + 1).trim() || null,
    }
  })
}

export {
  parsePreloadChunks,
}
