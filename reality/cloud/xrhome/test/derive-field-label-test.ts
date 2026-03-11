import {describe, it} from 'mocha'
import {assert} from 'chai'

import {deriveFieldLabel} from '../src/client/editor/module-config/derive-field-label'

describe('deriveFieldLabel', () => {
  it('Transforms field names to more readable labels', () => {
    assert.strictEqual(deriveFieldLabel('example'), 'Example')
    assert.strictEqual(deriveFieldLabel('myField'), 'My Field')
    assert.strictEqual(deriveFieldLabel('defaultHTML'), 'Default HTML')
    assert.strictEqual(deriveFieldLabel('http2Enabled'), 'Http2 Enabled')
  })
})
