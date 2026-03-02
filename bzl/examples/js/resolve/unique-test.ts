import * as Absolute from 'bzl/examples/js/resolve/unique'

import * as Nia from '@nia/bzl/examples/js/resolve/unique'

import * as Relative from './unique'

const uniqueObjects = new Set([Absolute, Nia, Relative].map(e => e.get()))

if (uniqueObjects.size !== 1) {
  throw new Error('Expected all objects to be the same.')
}
