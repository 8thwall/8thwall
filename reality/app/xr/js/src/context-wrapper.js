// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Christoph Bartschat (christoph@8thwall.com)
//
// Wrapper for OpenGL context.  We cache the values of the WebGL context for performance purposes.

/* global WebGLVertexArrayObjectOES:readonly */

const objectHasKey = (obj, key) => Object.prototype.hasOwnProperty.call(obj, key)

const itemInContainer = (container, key) => {
  if ('has' in container) {
    return container.has(key)
  } else if ('includes' in container) {
    return container.includes(key)
  } else if (typeof container === 'object') {
    return objectHasKey(container, key)
  } else {
    throw Error('[context-wrapper@itemInContainer] No known item look up for this container.')
  }
}

const isVaoOES = vao => (
  (window.WebGLVertexArrayObjectOES && vao instanceof WebGLVertexArrayObjectOES) ||
  (window.WebGLVertexArrayObject && vao instanceof WebGLVertexArrayObject) ||
  (vao.toString().includes('WebGLVertexArrayObject'))
)

const createVertexObjectState = () => {
  const vao = {
    // gets bound on the global state.  Pointer to a WebGL buffer.
    elementArrayBufferBinding: null,

    // we are not actually caching the corresponding get attribs locations
    // attribs: [],
  }

  return vao
}

/* eslint-disable prefer-destructuring */
function contextWrapper(ctx, verbose = false, emulateVertexArray = true, isWebGL2 = false) {
  const TEXTURE_TARGETS = {
    [ctx.TEXTURE_2D]: ctx.TEXTURE_BINDING_2D,
    [ctx.TEXTURE_CUBE_MAP]: ctx.TEXTURE_BINDING_CUBE_MAP,
  }
  if (isWebGL2) {
    Object.assign(TEXTURE_TARGETS, {
      [ctx.TEXTURE_3D]: ctx.TEXTURE_BINDING_3D,
      [ctx.TEXTURE_2D_ARRAY]: ctx.TEXTURE_BINDING_2D_ARRAY,
    })
  }

  const FRAMEBUFFER_TARGETS = {
    [ctx.FRAMEBUFFER]: ctx.FRAMEBUFFER_BINDING,
  }
  if (isWebGL2) {
    Object.assign(FRAMEBUFFER_TARGETS, {
      [ctx.DRAW_FRAMEBUFFER]: ctx.DRAW_FRAMEBUFFER_BINDING,
      [ctx.READ_FRAMEBUFFER]: ctx.READ_FRAMEBUFFER_BINDING,
    })
  }

  const BUFFER_TARGETS = {
    [ctx.ARRAY_BUFFER]: ctx.ARRAY_BUFFER_BINDING,
    [ctx.ELEMENT_ARRAY_BUFFER]: ctx.ELEMENT_ARRAY_BUFFER_BINDING,
  }
  if (isWebGL2) {
    Object.assign(BUFFER_TARGETS, {
      [ctx.COPY_READ_BUFFER]: ctx.COPY_READ_BUFFER_BINDING,
      [ctx.COPY_WRITE_BUFFER]: ctx.COPY_WRITE_BUFFER_BINDING,
      [ctx.TRANSFORM_FEEDBACK_BUFFER]: ctx.TRANSFORM_FEEDBACK_BUFFER_BINDING,
      [ctx.UNIFORM_BUFFER]: ctx.UNIFORM_BUFFER_BINDING,
      [ctx.PIXEL_PACK_BUFFER]: ctx.PIXEL_PACK_BUFFER_BINDING,
      [ctx.PIXEL_UNPACK_BUFFER]: ctx.PIXEL_UNPACK_BUFFER_BINDING,
    })
  }

  const ENABLE_ENUMS = new Set([
    ctx.BLEND,
    ctx.CULL_FACE,
    ctx.DEPTH_TEST,
    ctx.DITHER,
    ctx.POLYGON_OFFSET_FILL,
    ctx.SAMPLE_ALPHA_TO_COVERAGE,
    ctx.SAMPLE_COVERAGE,
    ctx.SCISSOR_TEST,
    ctx.STENCIL_TEST,
  ])
  if (isWebGL2) {
    ENABLE_ENUMS.add(ctx.RASTERIZER_DISCARD)
  }

  // This is the OpenGL global state cache (in contrast to the VAO cache, which we'll be adding
  // soon). If byActiveTexture is true, then the parameter keyword is dependent on what the current
  // active texture is.
  const globalState = {}
  Object.values(TEXTURE_TARGETS).forEach((textureBinding) => {
    globalState[textureBinding] = {byActiveTexture: true}
  })

  // The default VAO cache.  It has slightly different semantics.  If you delete a buffer and only
  // the default is pointing at it, it actually gets deleted.  It's as if you don't have one at all.
  const defaultVAOState = createVertexObjectState()
  // activeVAOState is the active VAO's state.  That means calls to bindBuffer with
  // ELEMENT_ARRAY_BUFFER will be recorded on this state.
  let activeVAOState = defaultVAOState
  // Stores the VAOs except the default VAO state.  The key is the VAO, the value is the state
  // object.
  const vaoState = new Map()

  // Wrap getting an extension so we can wrap the extension for VAO for webgl1
  const extensions = {}

  // A map of the original functions. We will be resetting these functions if undoWrapper is called
  // This is first is indexed by the object, which then indexes into the javascript object of
  // original functions for that object. For example {ctx: {'a':foo()}, ext:{'b':bar()}}
  const originalObjectFunctions = new Map()
  // List of functions that we added to the object and eventually need to remove in undoWrapper
  // For example {ctx: ['foo', 'bar'] , ext: ['baz']}
  const addedObjectFunctions = new Map()

  // Adds the function to object, if the function was previously defined on object save the original
  // definition otherwise keep track that we defined this function. We clean these up in
  // undoWrapper. Do not call twice with the same (object, funcName) pair. Calling this function
  // twice for the same pair is an error, since it would delete the previous definition.
  const addFunctionOnObject = (object, funcName, func) => {
    // Check if we've already defined this (object, funcName) pair ourselves.
    const alreadyWrapped = map => map.has(object) && itemInContainer(map.get(object), funcName)
    if ([originalObjectFunctions, addedObjectFunctions].some(alreadyWrapped)) {
      throw Error('[context-wrapper@addFunctionOnObject] Trying to add function definition for ' +
      `"${funcName}" that has already been added by addFunctionOnObject().`)
    }

    // If there already exists a definition on this object
    if (object[funcName]) {
      if (!originalObjectFunctions.get(object)) {
        originalObjectFunctions.set(object, {})
      }
      // Save the original function with its 'this' bound to the object it came from
      originalObjectFunctions.get(object)[funcName] = object[funcName].bind(object)
    } else {
      // Function not defined on object, add it, and keep track that we added it
      if (!addedObjectFunctions.get(object)) {
        addedObjectFunctions.set(object, [])
      }
      addedObjectFunctions.get(object).push(funcName)
    }
    // Set the new definition on the object
    object[funcName] = func
  }

  // Symbols are how we can discretely store attributes on WebGL objects.
  const deletedSym = Symbol('deleted')
  const wrappedSym = Symbol('wrapped')

  // These function bindings are simple enough that we're able to wrap them and call them with
  // updateParameter
  const functionBindings = {
    activeTexture: ctx.ACTIVE_TEXTURE,
    viewport: ctx.VIEWPORT,
    useProgram: ctx.CURRENT_PROGRAM,
    frontFace: ctx.FRONT_FACE,
    pixelStorei: {
      // WEBGL 1 only
      [ctx.PACK_ALIGNMENT]: ctx.PACK_ALIGNMENT,
      [ctx.UNPACK_ALIGNMENT]: ctx.UNPACK_ALIGNMENT,
      [ctx.UNPACK_FLIP_Y_WEBGL]: ctx.UNPACK_FLIP_Y_WEBGL,
      [ctx.UNPACK_PREMULTIPLY_ALPHA_WEBGL]: ctx.UNPACK_PREMULTIPLY_ALPHA_WEBGL,
      [ctx.UNPACK_COLORSPACE_CONVERSION_WEBGL]: ctx.UNPACK_COLORSPACE_CONVERSION_WEBGL,
    },
  }

  if (isWebGL2) {
    Object.assign(functionBindings.pixelStorei, {
      [ctx.PACK_ROW_LENGTH]: ctx.PACK_ROW_LENGTH,
      [ctx.PACK_SKIP_PIXELS]: ctx.PACK_SKIP_PIXELS,
      [ctx.PACK_SKIP_ROWS]: ctx.PACK_SKIP_ROWS,
      [ctx.UNPACK_ROW_LENGTH]: ctx.UNPACK_ROW_LENGTH,
      [ctx.UNPACK_IMAGE_HEIGHT]: ctx.UNPACK_IMAGE_HEIGHT,
      [ctx.UNPACK_SKIP_PIXELS]: ctx.UNPACK_SKIP_PIXELS,
      [ctx.UNPACK_SKIP_ROWS]: ctx.UNPACK_SKIP_ROWS,
      [ctx.UNPACK_SKIP_IMAGES]: ctx.UNPACK_SKIP_IMAGES,
    })
  }

  // These bindings will be the ones that ctx.getParameter will use our cache for.
  const cachedBindingEnums = new Set()
  Object.keys(functionBindings).forEach((value) => {
    if (typeof value !== 'object') {
      cachedBindingEnums.add(value)
    } else {
      Object.values(value).forEach(v => cachedBindingEnums.add(v))
    }
  })
  ;[TEXTURE_TARGETS, FRAMEBUFFER_TARGETS, BUFFER_TARGETS].forEach((target) => {
    Object.values(target).forEach(v => cachedBindingEnums.add(v))
  })

  // Returns keys of functionBindings with values being those functions.
  const originalFunctions = Object.keys(functionBindings).reduce((o, name) => {
    o[name] = ctx[name]
    return o
  }, {})

  // Returns the active texture unit.  For example, gl.TEXTURE0 which is '33984'.
  const getActiveTexture = () => {
    let activeTexture = globalState[ctx.ACTIVE_TEXTURE]
    if (!activeTexture) {
      activeTexture = originalObjectFunctions.get(ctx).getParameter(ctx.ACTIVE_TEXTURE)
      globalState[ctx.ACTIVE_TEXTURE] = activeTexture
    }
    return activeTexture
  }

  // Wrapper around getParameter.  The pass in a parameter like GLctx.TEXTURE_BINDING_2D which
  // is a number.
  addFunctionOnObject(ctx, 'getParameter', (parameter) => {
    // ELEMENT_ARRAY_BUFFER_BINDING is set on the active VAO, so we should query this from the
    // active VAO instead of the global cache
    if (parameter === ctx.ELEMENT_ARRAY_BUFFER_BINDING) {
      return activeVAOState.elementArrayBufferBinding
    }

    const cached = globalState[parameter]
    let p = cached
    if (cached && cached.byActiveTexture) {
      p = cached[getActiveTexture()]
    }

    if (p === undefined) {
      p = originalObjectFunctions.get(ctx).getParameter(parameter)
      if (cachedBindingEnums.has(parameter)) {
        if (cached && cached.byActiveTexture) {
          // this is indirectly updating globalState, since cached is one value of parameter map
          // that is an object.  If p is null, then we update globalState to null.
          cached[getActiveTexture()] = p
        } else {
          globalState[parameter] = p
        }
      }
    }

    return p
  })

  // Set cached value
  const updateParameter = (name, ...args) => {
    const fn = functionBindings[name]
    if (!fn) {
      // eslint-disable-next-line no-console
      console.error(`[XR:WebGL] No binding known for ${name}`)
      return
    }
    switch (args.length) {
      case 1:
        globalState[fn] = args[0]
        return
      case 2: {
        // The corresponding _BINDING for the argument.  For example:
        //   ARRAY_BUFFER -> ARRAY_BUFFER_BINDING.
        //   ELEMENT_ARRAY_BUFFER -> ELEMENT_ARRAY_BUFFER_BINDING
        const binding = fn[args[0]]
        if (binding) {
          globalState[binding] = args[1]
          return
        }
        break
      }
      case 4:
        globalState[fn] = args
        return
      default:
    }
    if (verbose) {
      // eslint-disable-next-line no-console
      console.error(`[XR:WebGL] Can't update parameter for ${name}[${args}]`)
    }
  }

  // Override setters
  Object.keys(functionBindings).forEach((name) => {
    ctx[name] = (...args) => {
      const result = originalFunctions[name].call(ctx, ...args)
      updateParameter(name, ...args)
      return result
    }
  })

  // If extName is OES_vertex_array_object we wrap the OES_vertex_array_object functions on ext
  addFunctionOnObject(ctx, 'getExtension', (extName) => {
    // we need to cache the extension so the user doesn't make multiple.
    if (objectHasKey(extensions, extName)) {
      return extensions[extName]
    }

    const extension = originalObjectFunctions.get(ctx).getExtension(extName)
    extensions[extName] = extension

    // Wrapping and define these VAO extension so that it updates our internal cache
    if (extension && extName === 'OES_vertex_array_object') {
      // creating a vao
      addFunctionOnObject(extension, 'createVertexArrayOES', () => {
        const vao = originalObjectFunctions.get(extension).createVertexArrayOES()
        // mark that the vao was created inside the wrapped context
        vao[wrappedSym] = true
        // don't add it to our vertex cache until the VAO has been bound
        return vao
      })

      // deleting a vao
      addFunctionOnObject(extension, 'deleteVertexArrayOES', (vao) => {
        originalObjectFunctions.get(extension).deleteVertexArrayOES(vao)

        if (!isVaoOES(vao) || vao[deletedSym]) {
          return
        }

        vao[deletedSym] = true
        // if the VAO being deleted is the active VAO, set the activeVAOState back to the default
        if (vaoState.get(vao) === activeVAOState) {
          activeVAOState = defaultVAOState
        }

        vaoState.delete(vao)
      })

      // setting a vao to be the current active vao
      addFunctionOnObject(extension, 'bindVertexArrayOES', (vao) => {
        // if null, go back to default
        if (vao == null) {
          activeVAOState = defaultVAOState
        } else if (isVaoOES(vao) && !vao[deletedSym]) {
          if (!vaoState.has(vao)) {
            vaoState.set(vao, createVertexObjectState())
          }
          // set the active VAO to this vao
          activeVAOState = vaoState.get(vao)
        }

        originalObjectFunctions.get(extension).bindVertexArrayOES(vao)

        // If the VAO was not created by a wrapped context, then it could have an EBO that is not
        // part of our cache. Since we are also caching the EBO, we should query and set the true
        // EBO using the original getParameter call.
        if (vao && vao[wrappedSym] !== true) {
          vaoState.get(vao).elementArrayBufferBinding =
            originalObjectFunctions.get(ctx).getParameter(ctx.ELEMENT_ARRAY_BUFFER_BINDING)
          vao[wrappedSym] = true
        }
      })

      // checking if a VAO is valid.  It's only considered valid if it's been bound and is not
      // deleted
      addFunctionOnObject(extension, 'isVertexArrayOES', vao => vaoState.has(vao))
    }

    return extension
  })

  if (!isWebGL2) {
    // Adds support for gl.UNSIGNED_INT types to WebGLRenderingContext.drawElements().
    originalObjectFunctions.get(ctx).getExtension('OES_element_index_uint')
  }

  // If we are using WebGL1, we'll set the VAO extensions on ctx by calling our wrapped
  // ctx.getExtension
  if (!isWebGL2) {
    const ext = ctx.getExtension('OES_vertex_array_object')
    if (ext) {
      addFunctionOnObject(ctx, 'createVertexArray', () => ext.createVertexArrayOES())
      addFunctionOnObject(ctx, 'deleteVertexArray', (vao) => { ext.deleteVertexArrayOES(vao) })
      addFunctionOnObject(ctx, 'bindVertexArray', (vao) => { ext.bindVertexArrayOES(vao) })
      addFunctionOnObject(ctx, 'isVertexArray', vao => ext.isVertexArrayOES(vao))
      ctx.VERTEX_ARRAY_BINDING = ext.VERTEX_ARRAY_BINDING_OES
    }
  } else {
    // wrap VAO logic for WebGL2.  We have to make a duplicate since:
    // 1) this one uses WebGLVertexArrayObject and not WebGLVertexArrayObjectOES
    // 2) WebGL1 uses the extension, WebGL2 uses the context
    // creating a vao
    addFunctionOnObject(ctx, 'createVertexArray', () => {
      const vao = originalObjectFunctions.get(ctx).createVertexArray()
      // mark that the vao was created inside the wrapped context
      vao[wrappedSym] = true
      // don't add it to our vertex cache until the VAO has been bound
      return vao
    })

    // deleting a vao
    addFunctionOnObject(ctx, 'deleteVertexArray', (vao) => {
      originalObjectFunctions.get(ctx).deleteVertexArray(vao)

      if (!(vao instanceof WebGLVertexArrayObject) || vao[deletedSym]) {
        return
      }

      vao[deletedSym] = true
      // if the VAO being deleted is the active VAO, set the activeVAOState back to the default
      if (vaoState.get(vao) === activeVAOState) {
        activeVAOState = defaultVAOState
      }

      vaoState.delete(vao)
    })

    // setting a vao to be the current active vao
    addFunctionOnObject(ctx, 'bindVertexArray', (vao) => {
      // if null, go back to default
      if (vao == null) {
        activeVAOState = defaultVAOState
      } else if (vao instanceof WebGLVertexArrayObject && !vao[deletedSym]) {
        if (!vaoState.has(vao)) {
          vaoState.set(vao, createVertexObjectState())
        }
        // set the active VAO to this vao
        activeVAOState = vaoState.get(vao)
      }

      originalObjectFunctions.get(ctx).bindVertexArray(vao)

      // If the VAO was not created by a wrapped context, then it could have an EBO that is not
      // part of our cache. Since we are also caching the EBO, we should query and set the true
      // EBO using the original getParameter call.
      if (vao && vao[wrappedSym] !== true) {
        vaoState.get(vao).elementArrayBufferBinding =
          originalObjectFunctions.get(ctx).getParameter(ctx.ELEMENT_ARRAY_BUFFER_BINDING)
        vao[wrappedSym] = true
      }
    })

    // checking if a VAO is valid
    addFunctionOnObject(ctx, 'isVertexArray', vao => vaoState.has(vao))
  }

  addFunctionOnObject(ctx, 'bindBuffer', (target, buffer) => {
    if (objectHasKey(BUFFER_TARGETS, target) &&
        (buffer == null || (buffer instanceof WebGLBuffer && !buffer[deletedSym]))) {
      // if the target is ELEMENT_ARRAY_BUFFER, then we need to set it on the activeVAOState
      if (target === ctx.ELEMENT_ARRAY_BUFFER) {
        activeVAOState.elementArrayBufferBinding = buffer || null
      } else {
        globalState[BUFFER_TARGETS[target]] = buffer || null
      }
    }

    originalObjectFunctions.get(ctx).bindBuffer(target, buffer)
  })

  addFunctionOnObject(ctx, 'bindTexture', (target, texture) => {
    if (objectHasKey(TEXTURE_TARGETS, target) &&
        (texture == null || (texture instanceof WebGLTexture && !texture[deletedSym]))) {
      globalState[TEXTURE_TARGETS[target]][getActiveTexture()] = texture || null
    }

    originalObjectFunctions.get(ctx).bindTexture(target, texture)
  })

  addFunctionOnObject(ctx, 'bindFramebuffer', (target, framebuffer) => {
    if (objectHasKey(FRAMEBUFFER_TARGETS, target) &&
        (framebuffer == null ||
            (framebuffer instanceof WebGLFramebuffer && !framebuffer[deletedSym]))) {
      globalState[FRAMEBUFFER_TARGETS[target]] = framebuffer || null
    }
    originalObjectFunctions.get(ctx).bindFramebuffer(target, framebuffer)
  })

  // Given a list of bindings (ARRAY_BUFFER_BINDING, FRAMEBUFFER_BINDING, etc) and a buffer, check
  // to see if the buffer is in the global state for those bindings.  If so, remove it.
  const removeBufferFromGlobalState = (bindings, buffer) => {
    const bindingToClear = bindings.find(binding => buffer === globalState[binding])
    if (!bindingToClear) {
      return
    }

    globalState[bindingToClear] = null
  }

  // Make sure that the texture is removed from our cache when ctx.deleteTexture is called
  addFunctionOnObject(ctx, 'deleteTexture', (texture) => {
    if (texture == null || texture[deletedSym]) {
      return
    }

    if (texture instanceof WebGLTexture) {
      texture[deletedSym] = true
      const activeTexture = getActiveTexture()
      const bindingToClear = Object.values(TEXTURE_TARGETS).find(
        binding => texture === globalState[binding][activeTexture]
      )
      if (bindingToClear) {
        globalState[bindingToClear] = {byActiveTexture: true}
      }
    }

    originalObjectFunctions.get(ctx).deleteTexture(texture)
  })

  // Make sure that the buffer is removed from our cache when ctx.deleteBuffer is called
  addFunctionOnObject(ctx, 'deleteBuffer', (buffer) => {
    // We should only unset the buffer from the cache if it's a valid buffer / not deleted
    if (buffer == null || buffer[deletedSym]) {
      return
    }

    if (buffer instanceof WebGLBuffer) {
      // mark that the buffer is deleted.  That way, if the user tries to re-bind the buffer, we
      // won't cache a deleted buffer.
      buffer[deletedSym] = true

      if (buffer === activeVAOState.elementArrayBufferBinding) {
        activeVAOState.elementArrayBufferBinding = null
      } else {
        removeBufferFromGlobalState(Object.values(BUFFER_TARGETS), buffer)
      }
    }
    originalObjectFunctions.get(ctx).deleteBuffer(buffer)
  })

  // Make sure that the framebuffer is removed from our cache when ctx.deleteFramebuffer is called
  addFunctionOnObject(ctx, 'deleteFramebuffer', (framebuffer) => {
    if (framebuffer == null || framebuffer[deletedSym]) {
      return
    }

    if (framebuffer instanceof WebGLFramebuffer) {
      framebuffer[deletedSym] = true
      removeBufferFromGlobalState(Object.values(FRAMEBUFFER_TARGETS), framebuffer)
    }

    originalObjectFunctions.get(ctx).deleteFramebuffer(framebuffer)
  })

  addFunctionOnObject(ctx, 'enable', (...args) => {
    if (ENABLE_ENUMS.has(args[0])) {
      if (globalState[args[0]] === true) {
        return
      }
      globalState[args[0]] = true
    }

    originalObjectFunctions.get(ctx).enable(...args)
  })

  addFunctionOnObject(ctx, 'isEnabled', (...args) => {
    if (ENABLE_ENUMS.has(args[0])) {
      if (globalState[args[0]] === undefined) {
        globalState[args[0]] = originalObjectFunctions.get(ctx).isEnabled(...args)
      }
      return globalState[args[0]]
    } else {
      return originalObjectFunctions.get(ctx).isEnabled(...args)
    }
  })

  // Removes the wrapper
  const undoWrapper = () => {
    Object.keys(functionBindings).forEach((name) => { ctx[name] = originalFunctions[name] })

    // For every object that we have in our map
    originalObjectFunctions.forEach((functionsOfObject, object) => {
      // For every saved original function for this object
      Object.entries(functionsOfObject).forEach(([funcName, originalFunc]) => {
        // Reset the function name back to its original definition
        object[funcName] = originalFunc
      })
    })
    originalObjectFunctions.clear()

    // For every object that we added functions for
    addedObjectFunctions.forEach((addedFunctions, object) => {
      // For every new function definition on this object
      addedFunctions.forEach((name) => {
        // Delete this definition from this object
        delete object[name]
      })
    })
    addedObjectFunctions.clear()
  }

  addFunctionOnObject(ctx, 'disable', (...args) => {
    // this is our way of disabling the context wrapper without adding an additional method to
    // the context
    if (args[0] === ctx.NO_ERROR) {
      undoWrapper()
      return
    }

    if (ENABLE_ENUMS.has(args[0])) {
      if (globalState[args[0]] === false) {
        return
      }
      globalState[args[0]] = false
    }
    originalObjectFunctions.get(ctx).disable(...args)
  })

  return ctx
}

export {
  contextWrapper,
}
