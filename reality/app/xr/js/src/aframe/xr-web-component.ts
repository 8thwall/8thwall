// Copyright (c) 2023 8th Wall, Inc.
// Original Author: Paris Morgan (paris@8thwall.com)

/* globals XR8:readonly */

import {
  addSceneListener,
  clearSceneListeners,
  xrConfigSchema,
  xrFaceSchema,
  xrWebSchema,
} from './common'

interface XrWebState {
  aScene: any
  sceneListeners: any
  xrconfigAdded: boolean
}

const EngineKind = {
  XR: 0,
  FACE: 1,
  LAYERS: 2,
}

const xrwebComponent = (engine) => {
  const state: XrWebState = {
    aScene: null,
    sceneListeners: [],
    xrconfigAdded: false,
  }

  function remove() {
    if (state.xrconfigAdded && this.el.sceneEl.components?.xrconfig) {
      state.aScene.removeAttribute('xrconfig')
      state.xrconfigAdded = false
    }

    let xrcomponent = ''
    if (engine === EngineKind.XR) {
      xrcomponent = 'reality'
    } else if (engine === EngineKind.FACE) {
      xrcomponent = 'facecontroller'
    } else if (engine === EngineKind.LAYERS) {
      xrcomponent = 'layerscontroller'
    }

    XR8.removeCameraPipelineModule(xrcomponent)

    clearSceneListeners(state.sceneListeners, state.aScene)
  }

  // NOTE: When using this.data you only use data from xrConfigSchema in updateWithXrConfigData().
  // This is because xrweb/xrface/xrlayers may be initialized before xrconfig. And if they are and
  // the user mistakenly set xrConfigSchema attributes on both xrface/xrweb/xrlayers and on
  // xrconfig, we don't want to use the attributes on xrface/xrweb/xrlayers (b/c xrconfig should
  // take precedence). So instead we just have xrconfig send the 'xrconfigdata' event in its init().
  // Then here we call updateWithXrConfigData() a second time. Note that this data is also available
  // on getDOMAttribute() but that method has not been overridden by AFrame yet and so returns a
  // string instead of an object. We could parse the string but using an event from xrconfig is
  // safer.
  function init() {
    state.aScene = this.el.sceneEl  // TODO(nb): throw if this.el !== this.el.sceneEl

    // this.el.sceneEl.flushToDOM(false)
    const xrlayersPresent = this.el.sceneEl.getDOMAttribute('xrlayers') != null
    const xrfacePresent = this.el.sceneEl.getDOMAttribute('xrface') != null
    const xrwebPresent = this.el.sceneEl.getDOMAttribute('xrweb') != null
    const xrconfigPresent = this.el.sceneEl.getDOMAttribute('xrconfig') != null
    /* eslint-disable no-console */
    if (xrlayersPresent && xrfacePresent) {
      console.error('[XR] Using xrlayers and xrface together is not supported.')
      return
    }
    if (xrwebPresent && xrfacePresent) {
      console.error('[XR] Using xrface and xrweb together is not supported.')
      return
    }
    if ((xrwebPresent && xrlayersPresent) && !xrconfigPresent) {
      console.error('[XR] When using xrlayers and xrweb together, xrconfig must also be added.')
      return
    }

    // You must only access attributes of this.data from xrConfigSchema in this method. This is
    // because those attributes need to be updated with data from xrconfig which can happen in two
    // different code paths.
    const updateWithXrConfigData = () => {
      if (engine === EngineKind.XR) {
        XR8.XrController.configure({mirroredDisplay: !!this.data.mirroredDisplay})
      } else if (engine === EngineKind.FACE) {
        XR8.FaceController.configure({coordinates: {mirroredDisplay: !!this.data.mirroredDisplay}})
      } else if (engine === EngineKind.LAYERS) {
        XR8.LayersController.configure({
          coordinates: {mirroredDisplay: !!this.data.mirroredDisplay},
        })
      }
    }

    const onXrLoaded = () => {
      if (xrconfigPresent) {
        // If xrconfig is present:
        Object.entries(this.data).forEach(([key, val]) => {
          // 1) Warn if we set any of its attributes here instead of on xrconfig. Note that we
          // only know if an attribute is different than the default value.
          if (key in xrConfigSchema && xrConfigSchema[key].default !== val) {
            console.warn(
              `[XR] ${key} was set on ${this.attrName} but should instead be set on xrconfig - ` +
              'it will have no effect.'
            )
          }

          // 2) If xrconfig is initialized as an AFrame component then we can copy over
          // its attributes directly. Else xrconfig will fire 'xrconfigdata' and
          // we will do this then.
          if (key in xrConfigSchema && this.el.sceneEl.components?.xrconfig?.data) {
            this.data[key] = this.el.sceneEl.components.xrconfig.data[key]
          }
        })
      } else {
        // Otherwise only either xrweb, xrface, xrhand, or xrlayers is present.
        // Add xrconfig under the hood and pass along our data to it.
        this.el.sceneEl.setAttribute('xrconfig', Object.fromEntries(
          Object.entries(this.data).filter(([key]) => key in xrConfigSchema)
        ))
        state.xrconfigAdded = true
      }
      /* eslint-enable no-console */

      updateWithXrConfigData()

      if (engine === EngineKind.XR) {
        XR8.XrController.configure({
          disableWorldTracking: this.data.disableWorldTracking,
          scale: this.data.scale,
          enableVps: this.data.enableVps,
          projectWayspots: this.data.projectWayspots,
          mapSrcUrl: this.data.mapSrcUrl,
        })
        XR8.addCameraPipelineModule(XR8.XrController.pipelineModule())
      } else if (engine === EngineKind.FACE) {
        if (this.data.meshGeometry || this.data.uvType || this.data.maxDetections ||
          this.data.enableEars) {
          XR8.FaceController.configure({
            meshGeometry: this.data.meshGeometry,
            uvType: this.data.uvType,
            maxDetections: this.data.maxDetections,
            enableEars: this.data.enableEars,
          })
        }
        XR8.addCameraPipelineModule(XR8.FaceController.pipelineModule())
      } else if (engine === EngineKind.LAYERS) {
        XR8.addCameraPipelineModule(XR8.LayersController.pipelineModule())
      }
    }

    addSceneListener(state.sceneListeners, state.aScene, 'recenter', (event) => {
      // Update camera origin if one is provided.
      if (event && event.detail && event.detail.origin && event.detail.facing) {
        const {x, y, z} = event.detail.origin
        const {w: qw, x: qx, y: qy, z: qz} = event.detail.facing

        const origin = {x, y, z}
        const facing = {w: qw, x: qx, y: qy, z: qz}

        if (engine === EngineKind.XR) {
          XR8.XrController.updateCameraProjectionMatrix({
            origin,
            facing,
          })
        } else if (engine === EngineKind.FACE) {
          XR8.FaceController.configure({
            coordinates: {
              origin: {
                position: origin,
                rotation: facing,
              },
            },
          })
        } else if (engine === EngineKind.LAYERS) {
          XR8.LayersController.configure({
            coordinates: {
              origin: {
                position: origin,
                rotation: facing,
              },
            },
          })
        }
      }

      if (engine === EngineKind.XR) {
        XR8.XrController.recenter()
      } else if (engine === EngineKind.LAYERS) {
        XR8.LayersController.recenter()
      }
    })

    addSceneListener(state.sceneListeners, state.aScene, 'configurecamera', (event) => {
      if (engine === EngineKind.XR) {
        XR8.XrController.updateCameraProjectionMatrix({
          cam: {
            pixelRectWidth: event.detail.dw,
            pixelRectHeight: event.detail.dh,
            nearClipPlane: event.detail.clipPlane.n,
            farClipPlane: event.detail.clipPlane.f,
          },
          origin: event.detail.origin,
          facing: event.detail.facing,
        })
      } else if (engine === EngineKind.FACE) {
        XR8.FaceController.configure({
          nearClip: event.detail.clipPlane.n,
          farClip: event.detail.clipPlane.f,
          coordinates: {
            origin: {
              position: event.detail.origin,
              rotation: event.detail.facing,
            },
          },
        })
      } else if (engine === EngineKind.LAYERS) {
        XR8.LayersController.configure({
          nearClip: event.detail.clipPlane.n,
          farClip: event.detail.clipPlane.f,
          coordinates: {
            origin: {
              position: event.detail.origin,
              rotation: event.detail.facing,
            },
          },
        })
      }
    })

    addSceneListener(state.sceneListeners, state.aScene, 'xrconfigdata', (event) => {
      // Copy over data from xrconfig.
      this.data = {...this.data, ...event.detail.data}

      // Update with the new data.
      updateWithXrConfigData()
    })

    if (window.XR8) { onXrLoaded() } else { window.addEventListener('xrloaded', onXrLoaded) }
  }

  return {
    schema: {
      ...xrConfigSchema,
      ...(engine === EngineKind.XR && xrWebSchema),
      ...(engine === EngineKind.FACE && xrFaceSchema),
    },
    init,
    remove,
  }
}

export {xrwebComponent, EngineKind}
