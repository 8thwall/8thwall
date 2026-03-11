import React from 'react'

type Resource<T> = T & {release?: () => void}

type ResourcePool<T> = {get: () => Resource<T>}

// eslint-disable-next-line arrow-parens
const createResourcePool = <T extends {}>(create: () => Resource<T>): ResourcePool<T> => {
  const resources: Resource<T>[] = []

  const get = () => {
    const available = resources.pop()
    if (available) {
      return available
    }

    const resource = create()
    resource.release = () => {
      resources.push(resource)
    }
    return resource
  }

  return {get}
}

const createUseResourcePool = <T extends {}>(create: () => Resource<T>) => (
  () => React.useState(() => createResourcePool(create))[0]
)

const useImgPool = createUseResourcePool(() => document.createElement('img'))

type ImgResource = Resource<HTMLImageElement>
type ImgPool = ResourcePool<HTMLImageElement>

const useCanvasPool = createUseResourcePool(() => document.createElement('canvas'))

type CanvasResource = Resource<HTMLCanvasElement>
type CanvasPool = ResourcePool<HTMLCanvasElement>

export {
  createResourcePool,
  createUseResourcePool,
  useImgPool,
  useCanvasPool,
}

export type {
  CanvasPool,
  CanvasResource,
  ImgPool,
  ImgResource,
  Resource,
  ResourcePool,
}
