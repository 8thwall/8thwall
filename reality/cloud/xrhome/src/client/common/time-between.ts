import {
  MILLISECONDS_PER_SECOND, SECONDS_PER_DAY, SECONDS_PER_HOUR, SECONDS_PER_MINUTE, SECONDS_PER_MONTH,
  SECONDS_PER_YEAR,
} from '../../shared/time-utils'

type TranslateFunction = (key: string, options?: Record<string, any>) => string

const timeBetweenInSeconds = (fromDate: Date | number, toDate: Date | number): number => {
  const fromDateNum = typeof fromDate === 'number' ? fromDate : fromDate.getTime()
  const toDateNum = typeof toDate === 'number' ? toDate : toDate.getTime()

  return Math.floor((toDateNum - fromDateNum) / MILLISECONDS_PER_SECOND)
}

const timeBetween = (
  fromDate: Date | number, toDate: Date | number, showDays: boolean = false
): string => {
  const seconds = timeBetweenInSeconds(fromDate, toDate)
  const daysSince = Math.floor(seconds / SECONDS_PER_DAY)

  let interval = Math.floor(seconds / SECONDS_PER_YEAR)
  if (interval > 1) {
    if (showDays) {
      return `${interval} years (${daysSince} days)`
    }
    return `${interval} years`
  }

  interval = Math.floor(seconds / SECONDS_PER_MONTH)
  if (interval > 1) {
    if (showDays) {
      return `${interval} months (${daysSince} days)`
    }
    return `${interval} months`
  }

  interval = Math.floor(seconds / SECONDS_PER_DAY)
  if (interval > 1) {
    return `${interval} days`
  }

  interval = Math.floor(seconds / SECONDS_PER_HOUR)
  if (interval > 1) {
    return `${interval} hours`
  }

  interval = Math.floor(seconds / SECONDS_PER_MINUTE)
  if (interval > 1) {
    return `${interval} minutes`
  }

  return seconds === 1 ? '1 second' : `${seconds} seconds`
}

const timeBetweenI18n = (
  fromDate: Date | number, toDate: Date | number, t: TranslateFunction, showDays: boolean = false
): string => {
  const seconds = timeBetweenInSeconds(fromDate, toDate)
  const daysSince = Math.floor(seconds / SECONDS_PER_DAY)

  let interval = Math.floor(seconds / SECONDS_PER_YEAR)
  if (interval > 1) {
    if (showDays) {
      return `${interval} ${t('time_since.years', {ns: 'common'})} (${daysSince} ${
        t('time_since.days', {ns: 'common'})
      })`
    }
    return `${interval} ${t('time_since.years', {ns: 'common'})}`
  }

  interval = Math.floor(seconds / SECONDS_PER_MONTH)
  if (interval > 1) {
    if (showDays) {
      return `${interval} ${t('time_since.months', {ns: 'common'})} (${daysSince} ${
        t('time_since.days', {ns: 'common'})
      })`
    }
    return `${interval} ${t('time_since.months', {ns: 'common'})}`
  }

  interval = Math.floor(seconds / SECONDS_PER_DAY)
  if (interval > 1) {
    return `${interval} ${t('time_since.days', {ns: 'common'})}`
  }

  interval = Math.floor(seconds / SECONDS_PER_HOUR)
  if (interval > 1) {
    return `${interval} ${t('time_since.hours', {ns: 'common'})}`
  }

  interval = Math.floor(seconds / SECONDS_PER_MINUTE)
  if (interval > 1) {
    return `${interval} ${t('time_since.minutes', {ns: 'common'})}`
  }

  return seconds === 1
    ? `1 ${t('time_since.second', {ns: 'common'})}`
    : `${seconds} ${t('time_since.seconds', {ns: 'common'})}`
}

export {
  timeBetween,
  timeBetweenI18n,
}
