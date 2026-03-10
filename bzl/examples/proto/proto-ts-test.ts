const {describe, it} = globalThis as any

// import * as fs from 'fs'
import fs from 'fs'
import {chai, chaiBytes} from 'bzl/js/chai-js'
import {Details} from 'bzl/examples/proto/api/details'
import {Hello} from 'bzl/examples/proto/api/hello'

const expect = chai.expect;
chai.use(chaiBytes)

describe('deserialize', () => {
  const hello = Hello.deserialize(fs.readFileSync('bzl/examples/proto/data/hello.pb'))
  it('= hello', () => expect(hello.greeting).to.equal("hello"))
  it('= the answer', () => expect(hello.details.answer).to.equal(42))
  it('= fib', () => expect(hello.details.fib).to.equalBytes([1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89]))
  // These tests are problematic. Here int64 and uint64 are being deserialized to Number, which has
  // less precision. These tests pass because both values are converted to Number in the same way.
  // If both numbers are cast to BigInt using e.g. BigInt('-9223372036854775808'), then the test
  // fails.
  it('~ the min', () => expect(hello.details.min).to.equal(-9223372036854775808))
  it('~ the max', () => expect(hello.details.max).to.equal(18446744073709551615))
})

describe('serialize', () => {
  const hello = Hello.fromObject({
    greeting: 'hello',
    details: {
      answer: 42,
      min: -9223372036854774000,  // just smaller than the limit, which ends in 75808
      max: 18446744073709550000,  // just smaller than the limit, which ends in 51615
      fib: new Uint8Array([1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89]),
    },
  })
  const serialized = hello.serialize()
  const expected = fs.readFileSync('bzl/examples/proto/data/hello.pb')
  // This test is problematic. Here int64 and uint64 are being serialized to Number, which has less
  // precision. If we try to set these to the correct values, e.g. -9223372036854775808, then
  // serialization will throw an error because the rounded values exceed the limits of the data
  // types. This is why we have to truncate the values sufficiently above. Since the values that
  // are ultimately serialized are not the same, we can't assert that the saved bytes are equal.
  // Instead we just assert that their saved size is equal.  This is actually a more meaningful
  // test than you might expect. Because proto uses varint encoding, if we set min and max to 0,
  // this test will fail. It only passes because the values we encoded are of the same order of
  // magnitude as the actual serialized ones.
  it ('= saved length', () => expect(serialized.length).to.equals(expected.length))
})
