// TODO(christoph): Fixing this error requires bumping to Typescript 5.0 for
// moduleResolution: 'bundler'
// @ts-ignore
import {loadYoga, Overflow, type Yoga} from 'yoga-layout/load'

const yoga: Yoga = {} as Yoga
let loaded = false

const yogaPromise = loadYoga().then((mod) => {
  Object.assign(yoga, mod)
  loaded = true
})

const yogaReady = async () => {
  await yogaPromise
}

const isYogaReady = () => loaded

export {
  yoga as default,
  yogaReady,
  isYogaReady,
  Overflow,
}
