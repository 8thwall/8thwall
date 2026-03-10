import {chai} from '@repo/bzl/js/chai-js'

const {describe, it} = globalThis as any

const {expect} = chai

describe('MathExample', () => {
  describe('Addition', () => {
    it('should add integers', () => {
      expect(1 + 1).to.equal(2)
    })
    it('should add negative numbers', () => {
      expect(-1 + -1).to.equal(-2)
    })
  })
  describe('Subtraction', () => {
    it('should subtract integers', () => {
      expect(1 - 1).to.equal(0)
    })
    it('should subtract negative numbers', () => {
      expect(-1 - -1).to.equal(0)
    })
  })
  describe('Multiplication', () => {
    it('should multiply integers', () => {
      expect(2 * 2).to.equal(4)
    })
    it('should multiply negative numbers', () => {
      expect(-2 * -2).to.equal(4)
    })
  })
})
