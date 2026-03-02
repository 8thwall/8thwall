/* eslint-disable class-methods-use-this */

/* eslint-disable import/no-unresolved */
import glfw from 'external/glfw-raub/glfw-raub'
/* eslint-enable import/no-unresolved */

const EventEmitter = require('events')

const {
  emptyFunction, keyNames, codeNames, extraCodes,
} = require('./constants')

class Window extends EventEmitter {
  constructor(opts = {}) {
    super()

    this._noApi = opts.noApi

    this._prevX = null
    this._prevY = null
    this._prevWidth = null
    this._prevHeight = null
    this._prevDecorated = null

    this._major = opts.major === undefined ? 2 : opts.major
    this._minor = opts.minor === undefined ? 1 : opts.minor

    this._title = null
    this._icon = null
    this._modeCache = {}
    this._pendingKeydown = null

    this._width = opts.width || 800
    this._pxWidth = this._width
    this._height = opts.height || 600
    this._pxHeight = this._height
    this._ratio = 1

    this._display = opts.display
    this._monitors = glfw.getMonitors()
    ;[this._primaryDisplay] = this._monitors.filter(d => d.is_primary)

    this._vsync = opts.vsync ? 1 : 0  // 0 for vsync off
    this._autoIconify = opts.autoIconify !== false

    const mode = opts.fullscreen ? 'fullscreen' : (opts.mode || 'windowed')

    this._decorated = true
    if (opts.decorated !== undefined) {
      this._decorated = opts.decorated
    }

    this._msaa = opts.msaa === undefined ? 2 : opts.msaa

    if (this._noApi) {
      glfw.windowHint(glfw.CLIENT_API, glfw.NO_API)
    } else {
      glfw.windowHint(glfw.CONTEXT_VERSION_MAJOR, this._major)
      glfw.windowHint(glfw.CONTEXT_VERSION_MINOR, this._minor)
      glfw.windowHint(glfw.RED_BITS, 8)
      glfw.windowHint(glfw.GREEN_BITS, 8)
      glfw.windowHint(glfw.BLUE_BITS, 8)
      glfw.windowHint(glfw.DEPTH_BITS, 24)
      glfw.windowHint(glfw.DOUBLEBUFFER, glfw.TRUE)
      glfw.windowHint(glfw.SAMPLES, this._msaa)
    }
    glfw.windowHint(glfw.RESIZABLE, glfw.TRUE)
    glfw.windowHint(glfw.VISIBLE, glfw.TRUE)
    glfw.windowHint(glfw.REFRESH_RATE, 0)
    glfw.windowHint(glfw.AUTO_ICONIFY, this._autoIconify ? glfw.TRUE : glfw.FALSE)
    glfw.windowHint(glfw.DECORATED, this._decorated ? glfw.TRUE : glfw.FALSE)

    this._emitter = {emit: (t, e) => this.emit(t, e)}

    // This CREATES window, as mode switches from `undefined`
    this.mode = mode

    const sizeWin = this.size
    const sizeFB = this.framebufferSize

    this._ratio = sizeFB.width / sizeWin.width

    this.icon = opts.icon

    if (!opts.title) {
      const dirname = process.cwd()
      this.title = dirname
    } else {
      this.title = opts.title
    }

    if (!this._noApi) {
      // can only be called after window creation!
      this._major = glfw.getWindowAttrib(this._window, glfw.CONTEXT_VERSION_MAJOR)
      this._minor = glfw.getWindowAttrib(this._window, glfw.CONTEXT_VERSION_MINOR)
      this._rev = glfw.getWindowAttrib(this._window, glfw.CONTEXT_REVISION)
      this._prof = glfw.getWindowAttrib(this._window, glfw.OPENGL_PROFILE)
    }

    this.on('window_pos', ({x, y}) => {
      this._x = x
      this._y = y
    })
    this.on('wresize', ({width, height}) => {
      this._width = width
      this._height = height
    })
    this.on('resize', ({width, height}) => {
      this._pxWidth = width
      this._pxHeight = height
    })
    if (!this._noApi) {
      this.swapBuffers()
    }

    this.requestAnimationFrame = this._requestAnimationFrame.bind(this)
    this.cancelAnimationFrame = this._cancelAnimationFrame.bind(this)

    this.event = null
  }

  get ratio() { return this._ratio }

  get devicePixelRatio() { return this._ratio }

  get handle() { return this._window }

  getCurrentMonitor() {
    if (!this._window) {
      return this._primaryDisplay
    }

    let bestoverlap = 0
    let bestmonitor = null

    const {x: wx, y: wy} = this.pos
    const {width: ww, height: wh} = this.size

    this._monitors = glfw.getMonitors()

    this._monitors.forEach((monitor) => {
      const {width: mw, height: mh} = monitor
      const {pos_x: mx, pos_y: my} = monitor
      const overlap = (
        Math.max(0, Math.min(wx + ww, mx + mw) - Math.max(wx, mx)) *
    Math.max(0, Math.min(wy + wh, my + mh) - Math.max(wy, my))
      )
      if (bestoverlap < overlap) {
        bestoverlap = overlap
        bestmonitor = monitor
      }
    })

    return bestmonitor
  }

  get mode() { return this._mode }

  set mode(v) {
    if (this._mode === v) {
      return
    }

    const currentMonitor = this.getCurrentMonitor()

    const prevMode = this._mode
    this._mode = v

    if (this._window) {
      // Fullscreen can't be hidden (uh-oh)
      if (prevMode === 'fullscreen') {
        this.destroy()
        this._modeCache[prevMode] = null
      } else {
        this.hide()
      }
    }

    if (this._monitors.length < 1) {
      throw new Error('No suitable display found for a new GLFW Window.')
    }

    this._display = this._monitors.indexOf(currentMonitor)

    if (this._mode === 'windowed') {
      this._x = this._prevX || this._x
      this._y = this._prevY || this._y
      this._width = this._prevWidth || this._width
      this._height = this._prevHeight || this._height
      this._decorated = this._prevDecorated || this._decorated
      this._prevX = null
      this._prevY = null
      this._prevWidth = null
      this._prevHeight = null
      this._prevDecorated = null
    } else if (
      this._width !== currentMonitor.width ||
    this._height !== currentMonitor.height
    ) {
      this._prevX = this._x
      this._prevY = this._y
      this._prevWidth = this._width
      this._prevHeight = this._height
      this._x = currentMonitor.pos_x
      this._y = currentMonitor.pos_y
      this._width = currentMonitor.width
      this._height = currentMonitor.height
    }

    if (!this._modeCache[this._mode]) {
      this._create()
      this._modeCache[this._mode] = this._window
    } else {
      this._window = this._modeCache[this._mode]
      this.show()
    }

    if (this._mode === 'windowed') {
      if (this._x && this._y) {
        glfw.setWindowPos(this._window, this._x, this._y)
      }
    } else if (this._mode === 'borderless') {
      const monitor = this._monitors[this._display]
      glfw.setWindowPos(this._window, monitor.pos_x, monitor.pos_y)
      glfw.setWindowSize(this._window, monitor.width, monitor.height)
    }

    if (!this._noApi) {
      this.makeCurrent()

      // Vertical sync (on cards that support it)
      glfw.swapInterval(this._vsync)
    }

    this._pxWidth = this._width * this._ratio
    this._pxHeight = this._height * this._ratio

    this.emit('resize', {width: this._pxWidth, height: this._pxHeight})
  }

  get width() { return this._pxWidth }

  get height() { return this._pxHeight }

  set width(v) {
    if (this._pxWidth === v) {
      return
    }
    this._width = Math.floor(v / this._ratio)
    this._pxWidth = v

    glfw.setWindowSize(this._window, this._width, this._height)
  }

  set height(v) {
    if (this._pxHeight === v) {
      return
    }
    this._height = Math.floor(v / this._ratio)
    this._pxHeight = v
    glfw.setWindowSize(this._window, this._width, this._height)
  }

  get offsetWidth() { return this._pxWidth }

  set offsetWidth(v) { this.width = v }

  get offsetHeight() { return this._pxHeight }

  set offsetHeight(v) { this.height = v }

  get w() { return this.width }

  set w(v) { this.width = v }

  get h() { return this.height }

  set h(v) { this.height = v }

  get wh() { return [this.width, this.height] }

  set wh([width, height]) {
    if (this._pxWidth === width && this._pxHeight === height) {
      return
    }
    this._width = Math.floor(width / this._ratio)
    this._pxWidth = width
    this._height = Math.floor(height / this._ratio)
    this._pxHeight = height
    glfw.setWindowSize(this._window, this._width, this._height)
  }

  get pxSize() {
    const size = glfw.getFramebufferSize(this._window)
    this._pxWidth = size.width
    this._pxHeight = size.height
    this._width = this._pxWidth / this._ratio
    this._height = this._pxHeight / this._ratio
    return size
  }

  set pxSize({width, height}) {
    this.wh = [width, height]
  }

  get innerWidth() { return this._width }

  set innerWidth(v) { this.width = v }

  get innerHeight() { return this._height }

  set innerHeight(v) { this.height = v }

  get clientWidth() { return this._width }

  set clientWidth(v) { this.width = v }

  get clientHeight() { return this._height }

  set clientHeight(v) { this.height = v }

  get onkeydown() { return this.listeners('keydown') }

  set onkeydown(cb) { this.on('keydown', cb) }

  get onkeyup() { return this.listeners('keyup') }

  set onkeyup(cb) { this.on('keyup', cb) }

  get onmousedown() { return this.listeners('mousedown') }

  set onmousedown(cb) { this.on('mousedown', cb) }

  get onmouseup() { return this.listeners('mouseup') }

  set onmouseup(cb) { this.on('mouseup', cb) }

  get onwheel() { return this.listeners('wheel') }

  set onwheel(cb) { this.on('wheel', cb) }

  get onmousewheel() { return this.listeners('mousewheel') }

  set onmousewheel(cb) { this.on('mousewheel', cb) }

  get onresize() { return this.listeners('resize') }

  set onresize(cb) { this.on('resize', cb) }

  getBoundingClientRect() {
    return {
      x: 0,
      y: 0,
      width: this._pxWidth,
      height: this._pxHeight,
      left: 0,
      top: 0,
      right: this._pxWidth,
      bottom: this._pxHeight,
    }
  }

  get size() {
    const size = glfw.getWindowSize(this._window)
    this._width = size.width
    this._height = size.height
    this._pxWidth = size.width * this._ratio
    this._pxHeight = size.height * this._ratio
    return size
  }

  set size({width, height}) {
    if (this._width === width && this._height === height) {
      return
    }
    this._width = width
    this._height = height
    this._pxWidth = width * this._ratio
    this._pxHeight = height * this._ratio
    glfw.setWindowSize(this._window, width, height)
  }

  get scrollX() { return 0 }

  get scrollY() { return 0 }

  get title() { return this._title }

  set title(v) {
    this._title = v || 'Untitled'
    glfw.setWindowTitle(this._window, this._title)
  }

  get icon() { return this._icon }

  set icon(v) {
    if (!(v && typeof v === 'object')) {
      this._icon = null
      return
    }
    this._icon = v
    glfw.setWindowIcon(this._window, this._icon)
  }

  get msaa() { return this._msaa }

  get version() {
    return `GL ${this._major}.${this._minor}.${this._rev} Profile: ${this._prof}`
  }

  get shouldClose() { return glfw.windowShouldClose(this._window) }

  set shouldClose(v) { glfw.setWindowShouldClose(v) }

  get platformDevice() { return glfw.platformDevice() }

  get platformWindow() { return glfw.platformWindow(this._window) }

  get platformContext() { return glfw.platformContext(this._window) }

  get x() { return this._x }

  set x(v) {
    if (this._x === v) {
      return
    }
    this._x = v
    glfw.setWindowPos(this._window, this._x, this._y)
  }

  get y() { return this._y }

  set y(v) {
    if (this._y === v) {
      return
    }
    this._y = v
    glfw.setWindowPos(this._window, this._x, this._y)
  }

  get pos() {
    const pos = glfw.getWindowPos(this._window)
    this._x = pos.x
    this._y = pos.y
    return pos
  }

  set pos({x, y}) {
    if (this._x === x && this._y === y) {
      return
    }
    this._x = x
    this._y = y
    glfw.setWindowPos(this._window, x, y)
  }

  get framebufferSize() { return glfw.getFramebufferSize(this._window) }

  get currentContext() { return glfw.getCurrentContext(this._window) }

  get cursorPos() { return glfw.getCursorPos(this._window) }

  set cursorPos({x, y}) { glfw.setCursorPos(this._window, x, y) }

  getKey(key) { return glfw.getKey(this._window, key) }

  getMouseButton(button) { return glfw.getMouseButton(this._window, button) }

  getWindowAttrib(attrib) { return glfw.getWindowAttrib(this._window, attrib) }

  setInputMode(mode, value) { glfw.setInputMode(this._window, mode, value) }

  swapBuffers() { glfw.swapBuffers(this._window) }

  makeCurrent() { glfw.makeContextCurrent(this._window) }

  destroy() { glfw.destroyWindow(this._window) }

  getNativeWindow() { return glfw.getNativeWindow(this._window) }

  iconify() { glfw.iconifyWindow(this._window) }

  restore() { glfw.restoreWindow(this._window) }

  hide() { glfw.hideWindow(this._window) }

  show() { glfw.showWindow(this._window) }

  emit(type, event) {
    if (type === 'keydown' || type === 'keyup') {
      const glfwCode = event.which
      event.which = extraCodes[glfwCode] || glfwCode
      event.keyCode = event.which
      event.key = (
        (event.charCode && String.fromCharCode(event.charCode)) ||
        event.code ||
        keyNames[glfwCode] ||
        '?')
      event.code = (
        codeNames[glfwCode] || (event.code && `Key${event.code.toUpperCase()}`) || 'UNKNOWN')
    }

    event.target = this
    event.type = type
    event.preventDefault = emptyFunction
    event.stopPropagation = emptyFunction

    if (!this._noApi) {
      this.makeCurrent()
    }

    this.event = event
    super.emit(type, event)
    this.event = null
  }

  // Create a new window according to the current 'mode'
  _create() {
    if (this._mode === 'windowed') {
      glfw.windowHint(glfw.DECORATED, this._decorated ? glfw.TRUE : glfw.FALSE)
      this._window = glfw.createWindow(
        this._width,
        this._height,
        this._emitter,
        this._title,
        null,
        this._noApi
      )
    } else if (this._mode === 'borderless') {
      this._prevDecorated = this._decorated
      this._decorated = false

      glfw.windowHint(glfw.DECORATED, glfw.FALSE)

      this._window = glfw.createWindow(
        this._width,
        this._height,
        this._emitter,
        this._title,
        null,
        this._noApi
      )
    } else if (this._mode === 'fullscreen') {
      this._adjustFullscreen()

      this._window = glfw.createWindow(
        this._width,
        this._height,
        this._emitter,
        this._title,
        this._display,
        this._noApi

      )
    } else {
      throw new Error(`Not supported display mode: '${this._mode}'.`)
    }

    if (!this._window) {
      throw new Error('Failed to open a new GLFW Window')
    }
  }

  _requestAnimationFrame(cb) {
    if (!this._noApi) {
      this.swapBuffers()
    }
    glfw.pollEvents()
    return setImmediate(() => cb(Date.now()))
  }

  _cancelAnimationFrame(id) { clearImmediate(id) }

  dispatchEvent(event) { this.emit(event.type, event) }

  addEventListener(name, callback) { this.on(name, callback) }

  removeEventListener(name, callback) { this.removeListener(name, callback) }

  static exit() {
    process.exit(0)
  }

  // ----- Fullscreen mode helpers

  _areaDiff(mode) {
    return Math.abs(mode.width * mode.height - this._width * this._height)
  }

  _sizeEqual(mode) {
    return mode.width === this._width && mode.height === this._height
  }

  _sortByAreaDiff(modes) {
    const sorted = modes.sort((a, b) => this._areaDiff(a) - this._areaDiff(b))
    const best = this._areaDiff(sorted[0])
    return sorted.filter(mode => this._areaDiff(mode) === best)
  }

  _sortByRate(modes) {
    return modes.sort((a, b) => this._areaDiff(a) - this._areaDiff(b))
  }

  _adjustFullscreen() {
    const mode = (() => {
      const {modes} = this._monitors[this._display]
      const exact = modes.filter(m => this._sizeEqual(m))
      const chosen = (exact.length ? exact : this._sortByAreaDiff(modes))
      return chosen.sort((a, b) => b.rate - a.rate)[0]
    })()

    this._width = mode.width
    this._height = mode.height

    glfw.windowHint(glfw.REFRESH_RATE, mode.rate)
  }
}

Window.keyNames = keyNames
Window.codeNames = codeNames
Window.extraCodes = extraCodes

export {Window}
