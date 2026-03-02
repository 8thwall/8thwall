/* eslint-disable class-methods-use-this */
/* eslint-disable max-classes-per-file */

import {Window} from './window'

const {
  emptyFunction, ESC_KEY, F_KEY,
} = require('./constants')

class Document extends Window {
  static setImage(Image) {
    this.Image = Image
    global.HTMLImageElement = Image
  }

  static setWebgl(webgl) {
    this.webgl = webgl
    this.isWebglInited = false
  }

  constructor(opts = {}) {
    super(opts)

    if (Document.webgl && !Document.isWebglInited) {
      if (typeof Document.webgl.init === 'function') {
        Document.webgl.init()
      }
      Document.isWebglInited = true
    }
    if (Document.webgl) {
      Document.webgl.canvas = this
    }

    if (!opts.ignoreQuit) {
      const isUnix = process.platform !== 'win32'
      if (isUnix && process.listeners('SIGINT').indexOf(Document.exit) < 0) {
        process.on('SIGINT', Document.exit)
      }

      this.on('quit', () => process.exit(0))

      if (opts.autoEsc) {
        this.on('keydown', e => e.keyCode === ESC_KEY && process.exit(0))
      }
    }

    if (opts.autoFullscreen) {
      this.on('keydown', (e) => {
        if (e.keyCode === F_KEY && e.ctrlKey && e.shiftKey) {
          this.mode = 'windowed'
        } else if (e.keyCode === F_KEY && e.ctrlKey && e.altKey) {
          this.mode = 'fullscreen'
        } else if (e.keyCode === F_KEY && e.ctrlKey) {
          this.mode = 'borderless'
        }
      })
    }
  }

  makeCurrent() {
    if (Document.webgl) {
      Document.webgl.canvas = this
    }
    super.makeCurrent()
  }

  get body() { return this }

  get style() {
    const that = this
    return {
      get width() { return that.innerWidth },
      set width(v) { that.width = parseInt(v, 10) * that.devicePixelRatio },
      get height() { return that.innetHeight },
      set height(v) { that.height = parseInt(v, 10) * that.devicePixelRatio },
    }
  }

  get context() { return Document.webgl }

  getContext(kind) {
    return kind === '2d' ? new Document.Image() : Document.webgl
  }

  getElementById() { return this }

  getElementsByTagName() { return [this] }

  appendChild() {}

  createElementNS(_0, name) { return this.createElement(name) }

  createElement(name) {
    const nameLower = name.toLowerCase()

    if (nameLower.indexOf('img') >= 0) {
      return new Document.Image()
    }

    if (nameLower.indexOf('canvas') >= 0) {
      if (!this._isCanvasRequested) {
        this._isCanvasRequested = true
        return this
      }

      const that = this

      return {
        _ctx: null,
        width: this.width,
        height: this.height,

        getContext(kind) {
          this._ctx = that.getContext(kind)
          return this._ctx
        },

        get data() {
          return this._ctx && this._ctx.data
        },

        onkeydown: emptyFunction,
        onkeyup: emptyFunction,
        onmousedown: emptyFunction,
        onmouseup: emptyFunction,
        onwheel: emptyFunction,
        onmousewheel: emptyFunction,
        onresize: emptyFunction,

        dispatchEvent: emptyFunction,
        addEventListener: emptyFunction,
        removeEventListener: emptyFunction,
      }
    }

    return null
  }
}

global.HTMLCanvasElement = Document

Document.setImage(class FakeImage {
  get src() {
    // eslint-disable-next-line no-console
    console.error('Document.Image class not set.')
    return ''
  }

  set src(v) {
    // eslint-disable-next-line no-console
    console.error('Document.Image class not set.')
  }

  get complete() { return false }

  on() {}

  onload() {}

  onerror() {}
})

Document.setWebgl(null)

export {Document}
