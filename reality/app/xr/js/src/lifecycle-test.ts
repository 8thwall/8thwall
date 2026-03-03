// @inliner-off
import {chai} from 'bzl/js/chai-js'

import {createContinuousLifecycle} from './lifecycle'

const {describe, it} = globalThis as any
const {assert} = chai

describe('createContinuousLifecycle', () => {
  it('Doesn\'t run start callback until attach', () => {
    let startCount = 0
    let stopCount = 0

    const lifecycle = createContinuousLifecycle(() => {
      startCount++
    }, () => {
      stopCount++
    })

    lifecycle.add('my-module')

    assert.equal(startCount, 0)
    assert.equal(stopCount, 0)
  })

  it('Runs start callback on attach', () => {
    let startCount = 0
    let stopCount = 0

    const lifecycle = createContinuousLifecycle((filter) => {
      startCount++

      assert.isTrue(filter('my-module'))
      assert.isFalse(filter('other-module'))
    }, () => {
      stopCount++
    })

    lifecycle.add('my-module')

    assert.equal(startCount, 0)
    assert.equal(stopCount, 0)

    lifecycle.attach()

    assert.equal(startCount, 1)
    assert.equal(stopCount, 0)
  })

  it('Runs stop callback in detach', () => {
    let startCount = 0
    let stopCount = 0

    const lifecycle = createContinuousLifecycle(() => {
      startCount++
    }, () => {
      stopCount++
    })

    lifecycle.add('before-module')

    lifecycle.attach()

    assert.equal(startCount, 1)
    assert.equal(stopCount, 0)

    lifecycle.detach()

    assert.equal(startCount, 1)
    assert.equal(stopCount, 1)
  })

  it('Doesn\'t run stop callback if never attached', () => {
    let startCount = 0
    let stopCount = 0

    const lifecycle = createContinuousLifecycle(() => {
      startCount++
    }, () => {
      stopCount++
    })

    lifecycle.add('my-module')
    lifecycle.remove('my-module')

    assert.equal(startCount, 0)
    assert.equal(stopCount, 0)
  })

  it('Runs start callback immediately if already engaged', () => {
    let startCount = 0
    let stopCount = 0

    const lifecycle = createContinuousLifecycle((filter) => {
      if (startCount === 0) {
        assert.isTrue(filter('before-module'))
        assert.isFalse(filter('after-module'))
      } else if (startCount === 1) {
        assert.isFalse(filter('before-module'))
        assert.isTrue(filter('after-module'))
      }
      startCount++
    }, () => {
      stopCount++
    })

    lifecycle.add('before-module')

    lifecycle.attach()

    lifecycle.add('after-module')

    assert.equal(startCount, 2)
    assert.equal(stopCount, 0)
  })

  it('Delays callbacks during pause', () => {
    let startCount = 0
    let stopCount = 0

    const lifecycle = createContinuousLifecycle((filter) => {
      if (startCount === 0) {
        assert.isTrue(filter('before-module'))
        assert.isFalse(filter('while-paused-module'))
      } else if (startCount === 1) {
        assert.isFalse(filter('before-module'))
        assert.isTrue(filter('while-paused-module'))
      }
      startCount++
    }, () => {
      stopCount++
    })

    lifecycle.add('before-module')

    lifecycle.attach()

    assert.equal(startCount, 1)
    assert.equal(stopCount, 0)

    lifecycle.pause()

    assert.equal(startCount, 1)
    assert.equal(stopCount, 0)

    lifecycle.add('while-paused-module')

    assert.equal(startCount, 1)
    assert.equal(stopCount, 0)

    lifecycle.resume()

    assert.equal(startCount, 2)
    assert.equal(stopCount, 0)
  })

  it('Modules removed during pause are detached', () => {
    let startCount = 0
    let stopCount = 0

    const lifecycle = createContinuousLifecycle((filter) => {
      assert.isTrue(filter('before-module'))
      assert.isFalse(filter('other-module'))
      startCount++
    }, (filter) => {
      assert.isTrue(filter('before-module'))
      assert.isFalse(filter('other-module'))
      stopCount++
    })

    lifecycle.add('before-module')

    lifecycle.attach()

    assert.equal(startCount, 1)
    assert.equal(stopCount, 0)

    lifecycle.pause()

    assert.equal(startCount, 1)
    assert.equal(stopCount, 0)

    lifecycle.detach()

    assert.equal(startCount, 1)
    assert.equal(stopCount, 1)
  })

  it('Modules added and removed before attach are never detached', () => {
    let startCount = 0
    let stopCount = 0

    const lifecycle = createContinuousLifecycle((filter) => {
      assert.isFalse(filter('while-paused-module'))
      startCount++
    }, (filter) => {
      assert.isFalse(filter('while-paused-module'))
      stopCount++
    })

    lifecycle.attach()

    assert.equal(startCount, 1)
    assert.equal(stopCount, 0)

    lifecycle.pause()

    assert.equal(startCount, 1)
    assert.equal(stopCount, 0)

    lifecycle.add('while-paused-module')

    lifecycle.detach()

    assert.equal(startCount, 1)
    assert.equal(stopCount, 1)
  })
})
