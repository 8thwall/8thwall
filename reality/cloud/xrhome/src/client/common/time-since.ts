import {timeBetween, timeBetweenI18n} from './time-between'

type TranslateFunction = (key: string, options?: Record<string, any>) => string

const timeSince = (
  date: Date | number, showDays: boolean = false
): string => timeBetween(date, new Date(), showDays)

const timeSinceI18n = (
  date: Date | number, t: TranslateFunction, showDays: boolean = false
): string => timeBetweenI18n(date, new Date(), t, showDays)

export {
  timeSince,
  timeSinceI18n,
}
