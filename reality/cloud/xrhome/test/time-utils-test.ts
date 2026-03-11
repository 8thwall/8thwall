import {assert} from 'chai'

import {
  floorToHour,
  floorToDay,
  ceilToDay,
  generateIntervalTimestamps,
  floorToMonth,
  floorLocalDay,
  ceilLocalDay,
  MILLISECONDS_PER_MINUTE,
  generateHourTimestamps,
} from '../src/shared/time-utils'

// JS parses date strings with a time but no timezone as local time, but
// those with the zero offset "Z" at the end are parsed as UTC.
// eslint-disable-next-line max-len
// https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/parse#examples

describe('time-utils.ts Test', () => {
  const floorDayLocal = new Date('2024-01-01T00:00:00.000')
  describe('floorLocalDay', () => {
    it('Floor Mid Hour', () => {
      assert.equal(
        floorLocalDay(new Date('2024-01-01T12:00:30.000')).getTime(), floorDayLocal.getTime()
      )
    })
    it('Floor Mid Day', () => {
      assert.equal(
        floorLocalDay(new Date('2024-01-01T12:00:00.000')).getTime(), floorDayLocal.getTime()
      )
    })
    it('Floor on Hour', () => {
      assert.equal(floorLocalDay(floorDayLocal).getTime(), floorDayLocal.getTime())
    })
  })

  describe('ceilLocalDay', () => {
    const ceilDayLocal = new Date('2024-01-02T00:00:00.000')
    it('Ceil Mid Hour', () => {
      assert.equal(
        ceilLocalDay(new Date('2024-01-01T12:00:30.000')).getTime(), ceilDayLocal.getTime()
      )
    })
    it('Ceil Mid Day', () => {
      assert.equal(
        ceilLocalDay(new Date('2024-01-01T12:00:00.000')).getTime(), ceilDayLocal.getTime()
      )
    })
    it('Ceil on Hour', () => {
      assert.equal(ceilLocalDay(ceilDayLocal).getTime(), ceilDayLocal.getTime())
    })
  })

  describe('floorToMonth', () => {
    const UTC2024Jan1 = new Date('2024-01-01T00:00:00.000Z')
    it('Floor Mid Hour', () => {
      assert.equal(
        floorToMonth(new Date('2024-01-01T12:00:30.000Z')).getTime(), UTC2024Jan1.getTime()
      )
    })
    it('Floor Mid Day', () => {
      assert.equal(
        floorToMonth(new Date('2024-01-01T12:00:00.000Z')).getTime(), UTC2024Jan1.getTime()
      )
    })
    it('Floor on Hour', () => {
      assert.equal(
        floorToMonth(new Date('2024-01-01T00:00:00.000Z')).getTime(), UTC2024Jan1.getTime()
      )
    })
    it('Floor Advanced Day', () => {
      assert.equal(
        floorToMonth(new Date('2024-01-03T00:00:00.000Z')).getTime(), UTC2024Jan1.getTime()
      )
    })
    it('Floor Local Day', () => {
      assert.equal(
        floorToMonth(new Date('2024-01-03T00:00:00.000')).getTime(), UTC2024Jan1.getTime()
      )
    })
  })

  describe('floorToHour', () => {
    const floorHourUTC = new Date('2024-01-01T12:00:00.000Z')
    it('Floor Mid Hour', () => {
      assert.equal(
        floorToHour(new Date('2024-01-01T12:00:30.000Z')).getTime(), floorHourUTC.getTime()
      )
    })
    it('Floor Mid Day', () => {
      assert.equal(floorToHour(floorHourUTC).getTime(), floorHourUTC.getTime())
    })
  })

  describe('generateHourTimestamps', () => {
    it('Generate Hour Stamps', () => {
      const stamps = generateHourTimestamps(
        new Date('2024-01-01T00:00:00.000Z'), new Date('2024-01-02T00:00:00.000Z')
      )
      const expectedStamps = [
        new Date('2024-01-01T00:00:00.000Z'),
        new Date('2024-01-01T01:00:00.000Z'),
        new Date('2024-01-01T02:00:00.000Z'),
        new Date('2024-01-01T03:00:00.000Z'),
        new Date('2024-01-01T04:00:00.000Z'),
        new Date('2024-01-01T05:00:00.000Z'),
        new Date('2024-01-01T06:00:00.000Z'),
        new Date('2024-01-01T07:00:00.000Z'),
        new Date('2024-01-01T08:00:00.000Z'),
        new Date('2024-01-01T09:00:00.000Z'),
        new Date('2024-01-01T10:00:00.000Z'),
        new Date('2024-01-01T11:00:00.000Z'),
        new Date('2024-01-01T12:00:00.000Z'),
        new Date('2024-01-01T13:00:00.000Z'),
        new Date('2024-01-01T14:00:00.000Z'),
        new Date('2024-01-01T15:00:00.000Z'),
        new Date('2024-01-01T16:00:00.000Z'),
        new Date('2024-01-01T17:00:00.000Z'),
        new Date('2024-01-01T18:00:00.000Z'),
        new Date('2024-01-01T19:00:00.000Z'),
        new Date('2024-01-01T20:00:00.000Z'),
        new Date('2024-01-01T21:00:00.000Z'),
        new Date('2024-01-01T22:00:00.000Z'),
        new Date('2024-01-01T23:00:00.000Z'),
      ]
      assert.equal(stamps.length, 24)
      expectedStamps.forEach((stamp, i) => {
        assert.equal(stamps[i].getTime(), stamp.getTime())
      })
    })
  })

  describe('generateIntervalTimestamps', () => {
    it('Generate Spaced Stamps 30 mins', () => {
      const stamps = generateIntervalTimestamps(
        new Date('2024-01-01T00:00:00.000Z'),
        new Date('2024-01-01T02:00:00.000Z'),
        MILLISECONDS_PER_MINUTE * 30
      )
      const expectedStamps = [
        new Date('2024-01-01T00:00:00.000Z'),
        new Date('2024-01-01T00:30:00.000Z'),
        new Date('2024-01-01T01:00:00.000Z'),
        new Date('2024-01-01T01:30:00.000Z'),
      ]
      assert.equal(stamps.length, expectedStamps.length)
      expectedStamps.forEach((stamp, i) => {
        assert.equal(stamps[i].getTime(), stamp.getTime())
      })
    })

    it('Generate Spaced Stamps 25 ms', () => {
      const stamps = generateIntervalTimestamps(
        new Date('2024-01-01T00:00:00.000Z'),
        new Date('2024-01-01T00:00:00.100Z'),
        25
      )
      const expectedStamps = [
        new Date('2024-01-01T00:00:00.000Z'),
        new Date('2024-01-01T00:00:00.025Z'),
        new Date('2024-01-01T00:00:00.050Z'),
        new Date('2024-01-01T00:00:00.075Z'),
      ]
      assert.equal(stamps.length, expectedStamps.length)
      expectedStamps.forEach((stamp, i) => {
        assert.equal(stamps[i].getTime(), stamp.getTime())
      })
    })
  })

  describe('floorToDay', () => {
    const floorDayUTC = new Date('2024-01-01T00:00:00.000Z')
    it('Floor Mid Hour', () => {
      assert.equal(
        floorToDay(new Date('2024-01-01T12:00:30.000Z')).getTime(), floorDayUTC.getTime()
      )
    })
    it('Floor Mid Day', () => {
      assert.equal(
        floorToDay(new Date('2024-01-01T12:00:00.000Z')).getTime(), floorDayUTC.getTime()
      )
    })
  })

  describe('ceilToDay', () => {
    const ceilDayUTC = new Date('2024-01-02T00:00:00.000Z')
    it('Ceil Mid Hour', () => {
      assert.equal(
        ceilToDay(new Date('2024-01-01T12:00:30.000Z')).getTime(), ceilDayUTC.getTime()
      )
    })
    it('Ceil Mid Day', () => {
      assert.equal(
        ceilToDay(new Date('2024-01-01T12:00:00.000Z')).getTime(), ceilDayUTC.getTime()
      )
    })
  })
})
