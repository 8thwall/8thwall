import {describe, it} from 'node:test'
import assert from 'node:assert/strict'
import fs from 'fs/promises'
import os from 'os'
import path from 'path'
import sharp from 'sharp'

import {applyCrop} from './apply.js'

/** @type {import("./types").CropResult} */
const crop = {
  type: 'PLANAR',
  properties: {
    left: 0,
    top: 0,
    width: 480,
    height: 640,
    isRotated: false,
    originalWidth: 480,
    originalHeight: 640,
  },
}

const makeImage = () => sharp({
  create: {width: 480, height: 640, channels: 3, background: {r: 128, g: 64, b: 32}},
}).jpeg()

describe('applyCrop', () => {
  it('sets imagePath from the output folder name', async () => {
    const tmp = await fs.mkdtemp(path.join(os.tmpdir(), 'image-target-cli-'))
    try {
      const folder = path.join(tmp, 'my-targets')
      const {dataPath, imagePath} = await applyCrop(
        sharp(await makeImage().toBuffer()), crop, folder, 'poster', false
      )
      const data = JSON.parse(await fs.readFile(dataPath, 'utf8'))
      assert.equal(data.imagePath, 'my-targets/poster_luminance.jpg')
      assert.equal(imagePath, data.imagePath)
      await fs.access(path.join(folder, 'poster_luminance.jpg'))
    } finally {
      await fs.rm(tmp, {recursive: true, force: true})
    }
  })

  it('keeps image-targets/ for the image-targets folder', async () => {
    const tmp = await fs.mkdtemp(path.join(os.tmpdir(), 'image-target-cli-'))
    try {
      const {imagePath} = await applyCrop(
        sharp(await makeImage().toBuffer()), crop, path.join(tmp, 'image-targets'), 'poster', false
      )
      assert.equal(imagePath, 'image-targets/poster_luminance.jpg')
    } finally {
      await fs.rm(tmp, {recursive: true, force: true})
    }
  })
})
