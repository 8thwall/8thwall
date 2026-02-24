import {describe, it} from 'node:test'
import assert from 'node:assert/strict'
import os from 'os'

import {normalizePath, selectPlanarGeometry} from './interactive.js'

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
  it('portrait + manual crop computes 4:3 height', async () => {
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
  it('landscape + manual crop computes 3:4 height', async () => {
    const crop = await selectPlanarGeometry(
      mockRl(false, 'landscape', 0, 0, 400),
      {width: 600, height: 800}
    )

    assert.equal(crop.width, 400)
    // Landscape height = Math.round(400 * 3 / 4) = 300
    assert.equal(crop.height, 300)
    assert.equal(crop.isRotated, true)
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
