const MONTHS_PER_YEAR = 12
const DAYS_PER_WEEK = 7
const DAYS_PER_MONTH = 30  // Approximation
const DAYS_PER_YEAR = 365
const HOURS_PER_DAY = 24
const MINUTES_PER_HOUR = 60
const SECONDS_PER_MINUTE = 60
const SECONDS_PER_HOUR = 3600
const SECONDS_PER_DAY = SECONDS_PER_HOUR * HOURS_PER_DAY
const SECONDS_PER_WEEK = SECONDS_PER_DAY * DAYS_PER_WEEK
const SECONDS_PER_YEAR = SECONDS_PER_DAY * DAYS_PER_YEAR
const SECONDS_PER_MONTH = SECONDS_PER_DAY * DAYS_PER_MONTH
const MILLISECONDS_PER_SECOND = 1000
const MILLISECONDS_PER_MINUTE = MILLISECONDS_PER_SECOND * SECONDS_PER_MINUTE
const MILLISECONDS_PER_HOUR = MILLISECONDS_PER_MINUTE * MINUTES_PER_HOUR
const MILLISECONDS_PER_DAY = MILLISECONDS_PER_HOUR * HOURS_PER_DAY
const MILLISECONDS_PER_MONTH = MILLISECONDS_PER_DAY * DAYS_PER_MONTH

/**
 * Stripe uses seconds to represent the current time, while standard Javascript
 * Dates are represented in milliseconds. This will return the equivalent Date
 * object to the integer time provided.
 *
 * @param {int} time - time in seconds
 */
const stripeTimeToDate = (time: number) => new Date(time * MILLISECONDS_PER_SECOND)

/**
 * Standard Javascript are represented in milliseconds, while Stripe uses
 * seconds to represent time. This will return the integer value, in seconds,
 * represented by the provided Date.
 *
 * @param {Date} date - a standard Javascript Date object
 */
const dateToStripeTime = (date: Date) => Math.round(date.getTime() / MILLISECONDS_PER_SECOND)

const floorToInterval = (timeMs: number, intervalMs: number) => (
  Math.floor(timeMs / intervalMs) * intervalMs
)

const ceilToInterval = (timeMs: number, intervalMs: number) => (
  Math.ceil(timeMs / intervalMs) * intervalMs
)

/**
 * Floors a date object to the nearest hour (UTC).
 * @param date Standard Javascript Date object.
 * @returns Date floored to the start of the current UTC hour.
 */
const floorToHour = (date: Date): Date => new Date(
  floorToInterval(date.getTime(), MILLISECONDS_PER_HOUR)
)

/**
 * Floors a date object to the nearest day (UTC).
 * @param date Standard Javascript Date object.
 * @returns Date floored to the start of the UTC day.
 */
const floorToDay = (date: Date): Date => new Date(
  floorToInterval(date.getTime(), MILLISECONDS_PER_DAY)
)

/**
 * Takes the ceiling of a date object to the nearest day (UTC).
 * 00:00:00 stays 00:00:00.
 * @param date Standard Javascript Date object.
 * @returns Date set to start of the next UTC day.
 */
const ceilToDay = (date: Date): Date => new Date(
  ceilToInterval(date.getTime(), MILLISECONDS_PER_DAY)
)

/**
 * Generic function to generate an array of Date objects spaced according to
 * the provided intervalMs. The array will contain all Date objects spaced
 * between the start and end dates, both floored to the nearest interval time.
 * @param start Start date to generate dates from, inclusive.
 * @param end End date to generate dates to, exclusive.
 * @param intervalMs Interval in milliseconds to space the dates. Can not be 0.
 * @returns Array of Date objects spaced according to the intervalMs.
 */
const generateIntervalTimestamps = (start: Date, end: Date, intervalMs: number): Date[] => {
  const stamps: Date[] = []
  const truncatedEnd = floorToInterval(end.getTime(), intervalMs)
  let currentTimestamp = floorToInterval(start.getTime(), intervalMs)
  while (currentTimestamp < truncatedEnd) {
    stamps.push(new Date(currentTimestamp))
    currentTimestamp += intervalMs
  }
  return stamps
}

/**
 * Generates an array of Date objects representing each UTC hour between the start
 * and end dates, both floored to the nearest hour (UTC).
 * @param start Start date to generate hourly dates from, inclusive.
 * @param end End date to generate hourly dates to, exclusive.
 * @returns Array of Date objects for each UTC hour between the start and end dates.
 */
const generateHourTimestamps = (start: Date, end: Date): Date[] => generateIntervalTimestamps(
  start, end, MILLISECONDS_PER_HOUR
)

/**
 * Takes a date object and returns a date object with the time set to beginning
 * of the month in UTC time. Do not use with Date objects outside of UTC time.
 * @param time Object to get the beginning of the month from.
 * @returns Beginning of the UTC month date object.
 */
const floorToMonth = (time: Date): Date => {
  // Unfortunately can not use floor since month times vary
  const date = new Date(time)
  date.setUTCDate(1)
  date.setUTCHours(0)
  date.setUTCMinutes(0)
  date.setUTCSeconds(0)
  date.setUTCMilliseconds(0)
  return date
}

/**
 * Takes a date object and returns a date object with the time set to the beginning
 * of the day in local time.
 * @param date Input date object.
 * @returns Date object with time set to the beginning of the local day.
 */
const floorLocalDay = (date: Date) => {
  const flooredDate = new Date(date)
  flooredDate.setHours(0, 0, 0, 0)
  return flooredDate
}

/**
 * Takes a date object and returns a date object with the time set to the beginning
 * of the next day in local time.
 * @param date Input date object.
 * @returns Date object with time set to the beginning of the next local day.
 */
const ceilLocalDay = (date: Date) => {
  const ceiledDate = new Date(date)
  if (ceiledDate.getHours() === 0 &&
    ceiledDate.getMinutes() === 0 &&
    ceiledDate.getSeconds() === 0 &&
    ceiledDate.getMilliseconds() === 0) {
    return ceiledDate
  }
  ceiledDate.setHours(0, 0, 0, 0)
  ceiledDate.setDate(ceiledDate.getDate() + 1)
  return ceiledDate
}

export {
  MONTHS_PER_YEAR,
  DAYS_PER_WEEK,
  DAYS_PER_YEAR,
  HOURS_PER_DAY,
  MINUTES_PER_HOUR,
  SECONDS_PER_MINUTE,
  SECONDS_PER_HOUR,
  SECONDS_PER_DAY,
  SECONDS_PER_WEEK,
  SECONDS_PER_MONTH,
  SECONDS_PER_YEAR,
  MILLISECONDS_PER_SECOND,
  MILLISECONDS_PER_MINUTE,
  MILLISECONDS_PER_HOUR,
  MILLISECONDS_PER_DAY,
  MILLISECONDS_PER_MONTH,
  stripeTimeToDate,
  dateToStripeTime,
  floorToInterval,
  ceilToInterval,
  floorToHour,
  floorToDay,
  ceilToDay,
  generateHourTimestamps,
  generateIntervalTimestamps,
  floorToMonth,
  floorLocalDay,
  ceilLocalDay,
}
