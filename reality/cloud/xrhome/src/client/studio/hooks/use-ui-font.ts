import React from 'react'
import type {FontResource} from '@ecs/shared/scene-graph'
import {DEFAULT_FONT_NAME, getBuiltinFontUrls} from '@ecs/shared/fonts'
import type {RawFontSetData} from '@ecs/shared/msdf-font-type'
import {extractFontResourceUrl} from '@ecs/shared/resource'

import {useQuery} from '@tanstack/react-query'

import {useResourceUrl} from './resource-url'

type Font = {
  json: string
  png: string
  ttf: string
}

const LOADING: Font = {
  json: null,
  png: null,
  ttf: null,
}

const fetchFont = async (url: string): Promise<Font> => {
  if (!url) {
    return null
  }
  const res = await fetch(url)
  if (!res.ok) {
    throw new Error(`Failed to fetch font resource at ${url}`)
  }
  const baseUrl = new URL(url)
  const font8Data: RawFontSetData = await res.json()
  return {
    json: url,
    png: font8Data.pages.map(page => new URL(page, baseUrl).toString())[0],
    ttf: new URL(font8Data.fontFile, baseUrl).toString(),
  }
}

const useUiFont = (fontId: undefined | FontResource): Font => {
  const fontName = fontId ? extractFontResourceUrl(fontId) : DEFAULT_FONT_NAME
  const builtInUrls = React.useMemo(() => getBuiltinFontUrls(fontName), [fontName])
  const resourceUrl = useResourceUrl(builtInUrls ? null : fontName)
  const font8Content = useQuery({
    // TODO(christoph): Technically we need to bust the cache if any of the referenced files change
    // It doesn't work like asset bundles anymore.
    queryKey: ['font8', resourceUrl],
    queryFn: () => fetchFont(resourceUrl),
    staleTime: Infinity,
    gcTime: Infinity,
  })

  if (builtInUrls) {
    return builtInUrls
  }
  return font8Content.data || LOADING
}

export {
  useUiFont,
}
