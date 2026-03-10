// @package(npm-ecs)
// @attr(commonjs = 1)
// @attr(externalize_npm = 0)
// @attr(esnext = 1)

import {describe, it, assert} from '@repo/bzl/js/chai-js'

import {convertSceneSkyToEffectsManagerSky} from './scene-sky'

describe('convertSceneSkyToEffectsManagerSky', () => {
  it('converts Resource to string correctly', () => {
    const urlResource = {type: 'url', url: 'https://example.com/skybox.jpg'} as const
    const assetResource = {type: 'asset', asset: 'skybox-asset'} as const

    const convertedUrlSky = convertSceneSkyToEffectsManagerSky({
      type: 'image',
      src: urlResource,
    })

    const convertedAssetSky = convertSceneSkyToEffectsManagerSky({
      type: 'image',
      src: assetResource,
    })

    assert.isOk(convertedUrlSky)
    assert.equal(convertedUrlSky!.type, 'image')
    if (convertedUrlSky && convertedUrlSky.type === 'image') {
      assert.equal(convertedUrlSky.src, 'https://example.com/skybox.jpg')
    }

    assert.isOk(convertedAssetSky)
    assert.equal(convertedAssetSky!.type, 'image')
    if (convertedAssetSky && convertedAssetSky.type === 'image') {
      assert.equal(convertedAssetSky.src, 'skybox-asset')
    }
  })

  it('handles undefined sky', () => {
    const result = convertSceneSkyToEffectsManagerSky(undefined)
    assert.isUndefined(result)
  })

  it('passes through color sky unchanged', () => {
    const colorSky = {type: 'color', color: '#ff0000'} as const
    const result = convertSceneSkyToEffectsManagerSky(colorSky)
    assert.strictEqual(result, colorSky)
  })

  it('passes through gradient sky unchanged', () => {
    const gradientSky = {
      type: 'gradient' as const,
      style: 'linear' as const,
      colors: ['#ff0000', '#00ff00'],
    }
    const result = convertSceneSkyToEffectsManagerSky(gradientSky)
    assert.strictEqual(result, gradientSky)
  })

  it('converts image sky with undefined src', () => {
    const imageSky = {type: 'image', src: undefined} as const
    const result = convertSceneSkyToEffectsManagerSky(imageSky)

    assert.isOk(result)
    assert.equal(result!.type, 'image')
    if (result && result.type === 'image') {
      assert.isUndefined(result.src)
    }
  })
})
