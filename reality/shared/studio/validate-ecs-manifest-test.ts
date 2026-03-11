import {describe, it, assert} from 'bzl/js/chai-js'

import {isValidManifest} from './validate-ecs-manifest'
import type {EcsManifest} from './ecs-manifest'

describe('isValidManifest', () => {
  it('returns true if manifest is valid', () => {
    const config: Required<EcsManifest['config']> = {
      runtimeUrl: 'https://runtime.com',
      dev8Url: 'https://dev8.com',
      app8Url: 'https://app8.com',
      preloadChunks: ['chunk1', 'chunk2'],
      deferXr8: true,
      scripts: [{src: 'https://script.com'}, {src: 'https://script2.com'}],
      runtimeTypeCheck: true,
      backgroundColor: '#FFFFFF',
    }

    const manifest: Required<EcsManifest> = {
      version: 1,
      config,
    }
    assert.isTrue(isValidManifest(manifest))
  })

  it('returns false if manifest is not an object', () => {
    assert.isFalse(isValidManifest(null))
    assert.isFalse(isValidManifest(''))
    assert.isFalse(isValidManifest(1))
    assert.isFalse(isValidManifest([]))
  })

  it('returns false if version is invalid', () => {
    assert.isFalse(isValidManifest({}))
    assert.isFalse(isValidManifest({version: 2}))
    assert.isFalse(isValidManifest({version: false}))
  })

  it('returns true if the manifest has nothing set', () => {
    assert.isTrue(isValidManifest({version: 1}))
    assert.isTrue(isValidManifest({version: 1, config: {}}))
  })

  it('returns false if manifest has invalid runtimeUrl', () => {
    const manifest = {
      version: 1,
      config: {
        runtimeUrl: 1,
      },
    }
    assert.isFalse(isValidManifest(manifest))
  })

  it('returns false if manifest has invalid dev8Url', () => {
    const manifest = {
      version: 1,
      config: {
        dev8Url: 1,
      },
    }
    assert.isFalse(isValidManifest(manifest))
  })

  it('returns false if manifest has invalid app8Url', () => {
    const manifest = {
      version: 1,
      config: {
        app8Url: 1,
      },
    }
    assert.isFalse(isValidManifest(manifest))
  })

  it('returns false if manifest has invalid preloadChunks', () => {
    const manifest = {
      version: 1,
      config: {
        preloadChunks: 1,
      },
    }
    assert.isFalse(isValidManifest(manifest))
  })

  it('returns false if manifest has invalid chunks', () => {
    const manifest = {
      version: 1,
      config: {
        preloadChunks: [1],
      },
    }

    assert.isFalse(isValidManifest(manifest))
  })

  it('returns false if deferXr is not a boolean', () => {
    const manifest = {
      version: 1,
      config: {
        deferXr8: 'test',
      },
    }

    assert.isFalse(isValidManifest(manifest))
  })

  it('returns false if scripts is not an array', () => {
    const manifest = {
      version: 1,
      config: {
        scripts: 'test',
      },
    }

    assert.isFalse(isValidManifest(manifest))
  })

  it('returns false if scripts is not an array of objects', () => {
    const manifest = {
      version: 1,
      config: {
        scripts: ['test'],
      },
    }

    assert.isFalse(isValidManifest(manifest))
  })

  it('returns false if scripts is not an array of objects with src: string', () => {
    const manifest = {
      version: 1,
      config: {
        scripts: [{src: 1}],
      },
    }

    assert.isFalse(isValidManifest(manifest))
  })
  it('returns false if runtimeTypeCheck is not a boolean', () => {
    const manifest = {
      version: 1,
      config: {
        runtimeTypeCheck: 'test',
      },
    }

    assert.isFalse(isValidManifest(manifest))
  })

  it('returns true if backgroundColor is unset', () => {
    assert.isTrue(isValidManifest({
      version: 1,
      config: {
      },
    }))
  })

  it('returns true if backgroundColor is a valid uppercase hex code', () => {
    assert.isTrue(isValidManifest({
      version: 1,
      config: {
        backgroundColor: '#019ABC',
      },
    }))
  })
  it('returns true if backgroundColor is a valid lowercase hex code', () => {
    assert.isTrue(isValidManifest({
      version: 1,
      config: {
        backgroundColor: '#fff123',
      },
    }))
  })
  it('returns false for invalid backgroundColors', () => {
    const invalidColors: any[] = [
      '#fff',  // too short
      '#fff1234',  // too long
      '#GGG123',  // invalid characters
      '000000',  // missing hash
      '0000000',  // digit instead of hash
      {},
      false,
      null,
      123456,
      'not-a-color',
      [],
    ]

    invalidColors.forEach((color) => {
      assert.isFalse(isValidManifest({
        version: 1,
        config: {
          backgroundColor: color,
        },
      }), `Expected ${color} to be invalid`)
    })
  })
})
