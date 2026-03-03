class ImageData {
  /** @type {number} */
  width

  /** @type {number} */
  height

  /** @type {Uint8ClampedArray} */
  data

  /**
   * @param {number | Iterable<number>} dataOrWidth
   * @param {number} widthOrHeight
   * @param {number=} maybeHeight
   */
  constructor(
    dataOrWidth,
    widthOrHeight,
    maybeHeight
  ) {
    if (typeof dataOrWidth === 'number') {
      this.width = dataOrWidth
      this.height = widthOrHeight
      this.data = new Uint8ClampedArray(this.width * this.height * 4)
    } else {
      this.data = Uint8ClampedArray.from(dataOrWidth)
      this.width = widthOrHeight
      this.height = maybeHeight || (this.data.length / this.width / 4)
    }
  }
}

export {
  ImageData,
}
