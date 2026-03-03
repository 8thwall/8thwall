import {describe, it} from 'node:test'
import assert from 'node:assert/strict'
import os from 'os'

import {
  normalizePath, selectPlanarGeometry, selectCylindricalGeometry, selectConicalGeometry,
} from './interactive.js'

const mockRl = (...values) => {
  let i = 0
  const getNextValue = async () => {
    if (i >= values.length) {
      throw new Error('No more mock values available')
    }
    return values[i++]
  }
  return {
    choose: getNextValue,
    confirm: getNextValue,
    promptInteger: getNextValue,
    promptFloat: getNextValue,
    prompt: getNextValue,
    close: () => {},
  }
}

describe('selectPlanarGeometry', () => {
  it('landscape + default crop', async () => {
    const crop = await selectPlanarGeometry(
      mockRl(true),
      {width: 1200, height: 600}
    )

    assert.equal(crop.isRotated, true)
    // Post-rotation: 600x800 rotated -> 800x600
    assert.equal(crop.originalWidth, 600)
    assert.equal(crop.originalHeight, 1200)
    assert.equal(crop.width, 600)
    assert.equal(crop.height, 800)
    assert.equal(crop.left, 0)
    assert.equal(crop.top, 200)
  })
  it('portrait + default crop', async () => {
    const crop = await selectPlanarGeometry(mockRl(true), {width: 600, height: 800})

    assert.equal(crop.isRotated, false)
    // Not rotated: 600x800. 600/3=200, 800/4=200 -> equal, else branch.
    // croppedHeight = Math.round(600 * 4 / 3) = 800
    assert.equal(crop.width, 600)
    assert.equal(crop.height, 800)
    assert.equal(crop.left, 0)
    assert.equal(crop.top, 0)
  })
  it('portrait + manual computes height for 3:4 crop', async () => {
    const rl = mockRl(false, 'portrait', 10, 20, 600)
    const crop = await selectPlanarGeometry(rl, {width: 600, height: 800})

    assert.equal(crop.top, 10)
    assert.equal(crop.left, 20)
    assert.equal(crop.width, 600)
    // Portrait height = Math.round(600 * 4 / 3) = 800
    assert.equal(crop.height, 800)
    assert.equal(crop.isRotated, false)
    assert.equal(crop.originalWidth, 600)
    assert.equal(crop.originalHeight, 800)
  })
  it('landscape + manual computes height for 4:3 crop', async () => {
    const crop = await selectPlanarGeometry(
      mockRl(false, 'landscape', 0, 0, 400),
      {width: 600, height: 800}
    )

    assert.equal(crop.width, 300)
    // Landscape height = Math.round(400 * 3 / 4) = 300
    assert.equal(crop.height, 400)
    assert.equal(crop.isRotated, true)
  })
})

describe('selectCylindricalGeometry', () => {
  it('landscape + default crop', async () => {
    const crop = await selectCylindricalGeometry(
      mockRl('mm', 1000, 500, true),
      {width: 1200, height: 600}
    )

    assert.deepEqual(crop, {
      isRotated: true,
      originalWidth: 600,
      originalHeight: 1200,
      width: 600,
      height: 800,
      left: 0,
      top: 200,
      targetCircumferenceTop: 500,
      cylinderSideLength: (600 / 1200) * 500,
      cylinderCircumferenceTop: 1000,
      cylinderCircumferenceBottom: 1000,
      arcAngle: 180,
      coniness: 0,
      inputMode: 'ADVANCED',
      unit: 'mm',
    })
  })

  it('portrait + default crop', async () => {
    const crop = await selectCylindricalGeometry(
      mockRl('mm', 3000, 1000, true),
      {width: 600, height: 1200}
    )

    assert.deepEqual(crop, {
      isRotated: false,
      originalWidth: 600,
      originalHeight: 1200,
      width: 600,
      height: 800,
      left: 0,
      top: 200,
      targetCircumferenceTop: 1000,
      cylinderSideLength: (1200 / 600) * 1000,
      cylinderCircumferenceTop: 3000,
      cylinderCircumferenceBottom: 3000,
      arcAngle: 120,
      coniness: 0,
      inputMode: 'ADVANCED',
      unit: 'mm',
    })
  })

  /* eslint quote-props: ["error", "as-needed"] */

  it('love.jpg + default crop', async () => {
    const crop = await selectCylindricalGeometry(
      mockRl('mm', 30, 10, true),
      {width: 1500, height: 1128}
    )

    // NOTE(christoph): This is comparing to the 8thwall.com behavior
    assert.deepEqual(crop, {
      top: 0,
      left: 2,
      width: 1125,
      height: 1500,
      isRotated: true,
      originalWidth: 1128,
      originalHeight: 1500,
      cylinderCircumferenceTop: 30,
      cylinderCircumferenceBottom: 30,
      targetCircumferenceTop: 10,
      cylinderSideLength: 7.52,
      inputMode: 'ADVANCED',
      coniness: 0,
      arcAngle: 120,
      unit: 'mm',
    })
  })

  it('tower.jpg + default crop', async () => {
    const crop = await selectCylindricalGeometry(
      // NOTE(christoph): Specifying the exact params because it seems like the 8thwall.com client
      // picked the initial crop according to different rounding.
      mockRl('mm', 100, 25, false, 'portrait', 82, 0, 1000, 1333),
      {width: 1000, height: 1500}
    )

    // NOTE(christoph): This is comparing to the 8thwall.com behavior
    assert.deepEqual(crop, {
      top: 82,
      left: 0,
      width: 1000,
      height: 1333,
      isRotated: false,
      originalWidth: 1000,
      originalHeight: 1500,
      cylinderCircumferenceTop: 100,
      cylinderCircumferenceBottom: 100,
      targetCircumferenceTop: 25,
      cylinderSideLength: 37.5,
      inputMode: 'ADVANCED',
      coniness: 0,
      arcAngle: 90,
      unit: 'mm',
    })
  })
})
describe('selectConicalGeometry', () => {
  it('handles wide top cone with default crop', async () => {
    const crop = await selectConicalGeometry(
      mockRl('top', 1500, 600, 180, true),
      {width: 2000, height: 1250}
    )

    // NOTE(christoph): This is comparing to the 8thwall.com behavior
    assert.deepEqual(crop, {
      top: 452,
      left: 0,
      width: 823,
      height: 1097,
      isRotated: true,
      originalWidth: 823,
      originalHeight: 2000,
      cylinderCircumferenceTop: 100,
      cylinderCircumferenceBottom: 40,
      targetCircumferenceTop: 50,
      cylinderSideLength: 20.575,
      inputMode: 'ADVANCED',
      coniness: 1.3219280948873624,
      arcAngle: 180,
      unit: 'mm',
      topRadius: 1500,
      bottomRadius: 600,
    })
  })

  it('handles wide top cone with portrait crop', async () => {
    const crop = await selectConicalGeometry(
      mockRl('top', 1500, 600, 180, false, 'portrait', 0, 692, 617, 823),
      {width: 2000, height: 1250}
    )

    assert.deepEqual(crop, {
      top: 0,
      left: 692,
      width: 617,
      height: 823,
      isRotated: false,
      originalWidth: 2000,
      originalHeight: 823,
      cylinderCircumferenceTop: 100,
      cylinderCircumferenceBottom: 40,
      targetCircumferenceTop: 50,
      cylinderSideLength: 20.575,
      inputMode: 'ADVANCED',
      coniness: 1.3219280948873624,
      arcAngle: 180,
      unit: 'mm',
      topRadius: 1500,
      bottomRadius: 600,
    })
  })

  it('handles wide bottom cone with default crop', async () => {
    const crop = await selectConicalGeometry(
      mockRl('bottom', 1500, 600, 180, true),
      {width: 2000, height: 1250}
    )

    // NOTE(christoph): This is comparing to the 8thwall.com behavior
    assert.deepEqual(crop, {
      top: 452,
      left: 0,
      width: 823,
      height: 1097,
      isRotated: true,
      originalWidth: 823,
      originalHeight: 2000,
      cylinderCircumferenceTop: 100,
      cylinderCircumferenceBottom: 250,
      targetCircumferenceTop: 50,
      cylinderSideLength: 51.4375,
      inputMode: 'ADVANCED',
      coniness: -1.3219280948873624,
      arcAngle: 180,
      unit: 'mm',
      topRadius: -1500,
      bottomRadius: 600,
    })
  })

  it('handles wide bottom cone with portrait crop', async () => {
    const crop = await selectConicalGeometry(
      mockRl('bottom', 1500, 600, 180, false, 'portrait', 0, 692, 617, 823),
      {width: 2000, height: 1250}
    )

    assert.deepEqual(crop, {
      top: 0,
      left: 692,
      width: 617,
      height: 823,
      isRotated: false,
      originalWidth: 2000,
      originalHeight: 823,
      cylinderCircumferenceTop: 100,
      cylinderCircumferenceBottom: 250,
      targetCircumferenceTop: 50,
      cylinderSideLength: 51.4375,
      inputMode: 'ADVANCED',
      coniness: -1.3219280948873624,
      arcAngle: 180,
      unit: 'mm',
      topRadius: -1500,
      bottomRadius: 600,
    })
  })
})

describe('normalizePath', () => {
  it('resolves a simple absolute path', () => {
    assert.equal(normalizePath('/foo/bar'), '/foo/bar')
  })

  it('strips double quotes', () => {
    assert.equal(normalizePath('"/foo/bar"'), '/foo/bar')
  })

  it('strips single quotes', () => {
    assert.equal(normalizePath('\'/foo/bar\''), '/foo/bar')
  })

  it('unescapes backslash-spaces', () => {
    assert.equal(normalizePath('/foo/bar\\ baz'), '/foo/bar baz')
  })

  it('expands tilde to home directory', () => {
    assert.equal(normalizePath('~/docs'), `${os.homedir()}/docs`)
  })

  it('throws on empty input', () => {
    assert.throws(() => normalizePath(''), /Invalid path input/)
  })
  it('throws on null', () => {
    assert.throws(() => normalizePath(null), /Invalid path input/)
  })
})
