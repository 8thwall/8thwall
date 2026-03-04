import type {World} from './world'
import type {Eid} from '../shared/schema'
import type {VideoQuery, VideoTimeResult} from './video-types'
import {getVideoManager, type VideoData} from './video-manager'

const matches = (video: VideoData, query: VideoQuery): boolean => {
  const isSrcMatch = !query.src || video.url === query.src
  const isTextureKeyMatch = !query.textureKey || video.textureKey === query.textureKey
  return isSrcMatch && isTextureKeyMatch
}

const findFirstVideoElementMatch = (world: World, eid: Eid, query?: VideoQuery) => {
  const videos = getVideoManager(world).getVideos(eid)
  if (!videos.length) {
    throw new Error(`Videos for ${eid} do not exist, cannot set or get current time`)
  }

  if (!query) {
    return videos[0].element
  }

  const match = videos.find(video => matches(video, query))
  return match ? match.element : null
}

const getCurrentTime = (world: World, eid: Eid, query?: VideoQuery): number => {
  const element = findFirstVideoElementMatch(world, eid, query)
  if (element) {
    return element.currentTime
  } else {
    throw new Error(`Cannot get time for video. No matching video found for eid ${eid} with query.`)
  }
}

const setCurrentTime = (world: World, eid: Eid, time: number, query?: VideoQuery) => {
  const element = findFirstVideoElementMatch(world, eid, query)
  if (element) {
    element.currentTime = time
  } else {
    throw new Error(`Cannot set time for video. No matching video found for eid ${eid} with query.`)
  }
}

const getCurrentTimes = (world: World, eid: Eid, filter?: VideoQuery): VideoTimeResult[] => {
  const videos = getVideoManager(world).getVideos(eid)
  if (!videos.length) {
    throw new Error(`Videos for ${eid} do not exist, cannot set current time`)
  }

  const times: VideoTimeResult[] = []
  videos.forEach((video) => {
    if (!filter || matches(video, filter)) {
      times.push({
        src: video.url,
        textureKey: video.textureKey,
        time: video.element.currentTime,
      })
    }
  })
  return times
}

const setCurrentTimes = (world: World, eid: Eid, time: number, filter?: VideoQuery) => {
  const videos = getVideoManager(world).getVideos(eid)
  if (!videos.length) {
    throw new Error(`Videos for ${eid} do not exist, cannot set current time`)
  }

  videos.forEach((video) => {
    if (!filter || matches(video, filter)) {
      video.element.currentTime = time
    }
  })
}

const video = {
  getCurrentTime,
  setCurrentTime,
  getCurrentTimes,
  setCurrentTimes,
}

export {
  video,
}
