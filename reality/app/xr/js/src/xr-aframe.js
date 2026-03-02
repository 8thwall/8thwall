// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

import {xrlayersceneComponent} from './aframe/xr-layer-scene-component'
import {xrwebComponent, EngineKind} from './aframe/xr-web-component'
import {xrconfigComponent} from './aframe/xr-config-component'
import {layerscenePrimitive} from './aframe/layer-scene-primitive'

const LayerKind = {
  SKY: 'sky',
}

// IMPORTANT: Add all components which other than those added to a-scene to the
// NON_SCENE_COMPONENTS list in aframe/common.ts
const registerAFrameXrComponent = () => {
  if (!window.AFRAME) {
    throw new Error('window.ARAME does not exist but is required for registerAFrameXrComponent')
  }
  window.AFRAME.registerComponent('xrconfig', xrconfigComponent())
  window.AFRAME.registerComponent('xrweb', xrwebComponent(EngineKind.XR))
  window.AFRAME.registerComponent('xrface', xrwebComponent(EngineKind.FACE))
  window.AFRAME.registerComponent('xrlayers', xrwebComponent(EngineKind.LAYERS))
  window.AFRAME.registerComponent('xrlayerscene', xrlayersceneComponent())
}

const registerAFrameXrPrimitive = () => {
  if (!window.AFRAME) {
    throw new Error('window.ARAME does not exist but is required for registerAFrameXrPrimitive')
  }
  window.AFRAME.registerPrimitive('sky-scene', layerscenePrimitive(LayerKind.SKY))
}

/**
 * Entry point for A-Frame integration with 8th Wall Web.
 */
const AFrameFactory = xrccPromise => ({
  // ------------------------------- xrconfig -------------------------------
  // xrconfig is used to configure the 8th Wall camera session or experience. Should be used
  // alongside xrweb, xrface, or xrlayers. If it is not added and xrweb, xrface, or xrlayers is
  // used, it will be added under the hood and attributes will be passed along to it.
  //
  // ------------------------------- xrconfig Attributes -------------------------------
  // cameraDirection; type: string; default: 'back'
  //   Desired camera to use. Choose from: 'back' or 'front'. Use 'cameraDirection: front;' with
  //   'mirroredDisplay: true;' for selfie mode. Note that when using xrweb or xrlayers, world
  //   tracking is only  supported with cameraDirection 'back'.
  // allowedDevices; type: string; default: 'mobile'
  //   Supported device classes. Choose from: 'mobile' or 'any'. Use 'any' to enable laptop- or
  //   desktop-type devices with built-in or attached webcams. Note that when using xrweb or
  //   xrlayers, world  tracking is only supported in 'mobile'.
  // mirroredDisplay; type: bool; default: false
  //   If true, flip left and right in the output geometry and reverse the direction of the camera
  //   feed. Use 'mirroredDisplay: true' with 'cameraDirection: front;' with selfie mode.

  // ------------------------------- xrconfig AFrame Events -------------------------------
  // These are the events you can listen for in your web application.
  //
  // Event: realityready
  // Description: This event is emitted when 8th Wall Web has initialized and at least one frame has
  //   been successfully processed. This is the recommended time at which any loading elements
  //   should be hidden.
  // Example:
  //    let scene = this.el.sceneEl
  //    scene.addEventListener('realityready', () => {
  //      // Hide loading UI
  //    })
  //
  //
  // Event: realityerror
  // Description: This event is emitted when an error has occurred when initializing 8th Wall Web.
  //   This is the recommended time at which any error messages should be displayed. The XrDevice
  //   API can help with determining what type of error messaging should be displayed.
  // Example:
  //    let scene = this.el.sceneEl
  //    scene.addEventListener('realityerror', (event) => {
  //      if (XR8.XrDevice.isDeviceBrowserCompatible) {
  //        // Browser is compatible. Print the exception for more information.
  //        console.log(event.detail.error)
  //        return
  //      }
  //
  //      // Browser is not compatible. Check the reasons why it may not be.
  //      for (let reason of XR8.XrDevice.incompatibleReasons()) {
  //        // Handle each XR8.XrDevice.IncompatibleReason
  //      }
  //    })
  //
  //
  // Event: camerastatuschange
  // Description: This event is emitted when the status of the camera changes. See
  //   `onCameraStatusChange` from XR8.addCameraPipelineModule for more information on the possible
  //   status.
  // Example:
  //   var handleCameraStatusChange = function handleCameraStatusChange(event) {
  //     console.log('status change', event.detail.status);
  //
  //     switch (event.detail.status) {
  //       case 'requesting':
  //         // Do something
  //         break;
  //
  //       case 'hasStream':
  //         // Do something
  //         break;
  //
  //       case 'failed':
  //         event.target.emit('realityerror');
  //         break;
  //     }
  //   };
  //  let scene = this.el.sceneEl
  //  scene.addEventListener('camerastatuschange', handleCameraStatusChange)
  //
  //
  // Event: screenshotready
  // Description: This event is emitted in response to the `screenshotrequest` event being
  // being completed. The JPEG compressed image of the AFrame canvas will be provided.
  // Example:
  //    let scene = this.el.sceneEl
  //    scene.addEventListener('screenshotready', (event) => {
  //      // screenshotPreview is an <img> HTML element
  //      const image = document.getElementById('screenshotPreview')
  //      image.src = 'data:image/jpeg;base64,' + event.detail
  //    })
  //
  //
  // Event: screenshoterror
  // Description: This event is emitted in response to the `screenshotrequest` event resulting
  // in an error.
  // Example:
  //    let scene = this.el.sceneEl
  //    scene.addEventListener('screenshoterror', (event) => {
  //       console.log(event.detail)
  //       // Handle screenshot error.
  //    })
  //
  // ------------------------------- xrconfig AFrame Event Listeners -------------------------------
  // These are the events you can emit from your web application.
  //
  // Event: hidecamerafeed
  // Description: Hides the camera feed. Tracking does not stop.
  // Example:
  //    let scene = this.el.sceneEl
  //    scene.emit('hidecamerafeed')
  //
  // Event: showcamerafeed
  // Description: Shows the camera feed.
  // Example:
  //    let scene = this.el.sceneEl
  //    scene.emit('showcamerafeed')
  //
  // Event: screenshotrequest
  // Description: Emits a request to the engine to capture a screenshot of the AFrame canvas.
  //   The engine will emit a `screenshotready` event with the JPEG compressed image,
  //   or `screenshoterror` if an error has occured.
  // Example:
  //    let scene = this.el.sceneEl
  //    scene.emit('screenshotrequest')
  //
  // Event: recenter
  // Description: Recenters the camera feed to its origin. If a new origin is provided as an
  //   argument, the camera's origin will be reset to that, then it will recenter. If origin and
  //   facing are not provided, camera is reset to origin previously specified by a call to recenter
  //   or the last call to configure the origin. Note that the origin is initially set based on
  //   initial camera position in the scene. When using xrface, this will move the position of the
  //   graphics camera in the scene to the new origin, and reposition detected faces relative to the
  //   new camera position.
  // Arguments (Optional):
  //  {
  //    // The location of the new origin.
  //    origin: {x, y, z},
  //    // A quaternion representing direction the camera should face at the origin.
  //    facing: {w, x, y, z}
  //  }
  // Examples:
  //    let scene = this.el.sceneEl
  //    scene.emit('recenter')
  //  OR
  //    let scene = this.el.sceneEl
  //    scene.emit('recenter', {
  //       origin: {x: 1, y: 4, z: 0},
  //       facing: {w: 0.9856, x:0, y:0.169, z:0}
  //    })
  //
  // Event: stopxr
  // Description: Stop the current XR session. While stopped, the camera feed is stopped and
  //   device motion is not tracked.
  // Example:
  //    let scene = this.el.sceneEl
  //    scene.emit('stopxr')
  //
  // Creates an A-Frame component which can be registered with AFRAME.registerComponent().
  // This, however, generally won't need to be called directly. On 8th Wall Web script load, this
  // component will be registered if it is detected that A-Frame has loaded (i.e if window.AFRAME
  // exists).
  //
  // Example:
  //
  // window.AFRAME.registerComponent('xrconfig', XR8.AFrame.xrconfigComponent())
  xrconfigComponent: () => xrconfigComponent(xrccPromise),

  // ------------------------------- xrweb -------------------------------
  // xrweb is used for world tracking, image targets, and VPS. xrconfig should also be added
  // alongside xrweb, though for backwards compatibility if xrconfig is not added, xrweb will
  // automatically add xrconfig and pass along provided attributes to xrconfig.
  //
  // ------------------------------- xrweb Attributes -------------------------------
  // disableWorldTracking; type: bool; default: false
  //   If true, turn off SLAM tracking for efficiency.
  // scale; type: string; default: 'responsive'
  //   If 'responsive', the engine will return values based on the virtual camera starting at the
  //   origin defined with XrController.updateCameraProjectionMatrix(). If 'absolute', the camera,
  //   image targets, etc will be in meters.
  // enableVps; type: bool; default: false
  //   If true, enables VPS.
  // projectWayspots; type: array; default: []
  //   Comma separated strings of project wayspot names to exclusively localize against. If unset or
  //   an empty string is passed, we will localize all nearby Project Wayspots.
  //
  // ------------------------------- xrweb AFrame Events -------------------------------
  // These are the events you can listen for in your web application.
  //
  // Event: xrimageloading
  // Description: This event is emitted when detection image loading begins.
  //   detail : { imageTargets: [{name, metadata, properties}] }
  // Example:
  //  const componentMap = {}
  //  const addComponents = ({detail}) => {
  //    detail.imageTargets.forEach(({name, metadata, properties}) => {
  //     ...
  //    })
  //  }
  //  this.el.sceneEl.addEventListener('xrimageloading', addComponents)
  //
  //
  // Event: xrimagescanning
  // Description: This event is emitted when all detection images have been loaded and scanning has
  //   begun.
  //   detail : { imageTargets: [{name, metadata, properties}] }
  //
  //
  // Event: xrimagefound
  // Description: This event is emitted when an image target is first found.
  //   detail: {
  //     name: The image's name.
  //     position {x, y, z}: The 3d position of the located image.
  //     rotation {w, x, y, z}: The 3d local orientation of the located image.
  //     scale: A scale factor that should be applied to object attached to this image.
  //     scaledWidth: The width of the image in the scene, when multiplied by scale.
  //     scaledHeight: The height of the image in the scene, when multiplied by scale.
  //   }
  // Example:
  // AFRAME.registerComponent('my-named-image-target', {
  //   schema: {
  //     name: { type: 'string' }
  //   },
  //   init: function () {
  //     const object3D = this.el.object3D
  //     const name = this.data.name
  //     object3D.visible = false
  //     const showImage = ({detail}) => {
  //       if (name != detail.name) {
  //         return
  //       }
  //       object3D.position.copy(detail.position)
  //       object3D.quaternion.copy(detail.rotation)
  //       object3D.scale.set(detail.scale, detail.scale, detail.scale)
  //       object3D.visible = true
  //     }
  //     const hideImage = ({detail}) => {
  //       if (name != detail.name) {
  //         return
  //       }
  //       object3D.visible = false
  //     }
  //     this.el.sceneEl.addEventListener('xrimagefound', showImage)
  //     this.el.sceneEl.addEventListener('xrimageupdated', showImage)
  //     this.el.sceneEl.addEventListener('xrimagelost', hideImage)
  //   }
  // })
  //
  //
  // Event: xrimageupdated
  // Description: This event is emitted when an image target changes position, rotation or scale.
  //   detail: {
  //     name: The image's name.
  //     position {x, y, z}: The 3d position of the located image.
  //     rotation {w, x, y, z}: The 3d local orientation of the located image.
  //     scale: A scale factor that should be applied to object attached to this image.
  //     scaledWidth: The width of the image in the scene, when multiplied by scale.
  //     scaledHeight: The height of the image in the scene, when multiplied by scale.
  //   }
  // Example:
  // AFRAME.registerComponent('my-named-image-target', {
  //   schema: {
  //     name: { type: 'string' }
  //   },
  //   init: function () {
  //     const object3D = this.el.object3D
  //     const name = this.data.name
  //     object3D.visible = false
  //     const showImage = ({detail}) => {
  //       if (name != detail.name) {
  //         return
  //       }
  //       object3D.position.copy(detail.position)
  //       object3D.quaternion.copy(detail.rotation)
  //       object3D.scale.set(detail.scale, detail.scale, detail.scale)
  //       object3D.visible = true
  //     }
  //     const hideImage = ({detail}) => {
  //       if (name != detail.name) {
  //         return
  //       }
  //       object3D.visible = false
  //     }
  //     this.el.sceneEl.addEventListener('xrimagefound', showImage)
  //     this.el.sceneEl.addEventListener('xrimageupdated', showImage)
  //     this.el.sceneEl.addEventListener('xrimagelost', hideImage)
  //   }
  // })
  //
  //
  // Event: xrimagelost
  // Description: This event is emitted when an image target is no longer being tracked.
  //   detail: {
  //     name: The image's name.
  //   }
  // Example:
  // AFRAME.registerComponent('my-named-image-target', {
  //   schema: {
  //     name: { type: 'string' }
  //   },
  //   init: function () {
  //     const object3D = this.el.object3D
  //     const name = this.data.name
  //     object3D.visible = false
  //     const showImage = ({detail}) => {
  //       if (name != detail.name) {
  //         return
  //       }
  //       object3D.position.copy(detail.position)
  //       object3D.quaternion.copy(detail.rotation)
  //       object3D.scale.set(detail.scale, detail.scale, detail.scale)
  //       object3D.visible = true
  //     }
  //     const hideImage = ({detail}) => {
  //       if (name != detail.name) {
  //         return
  //       }
  //       object3D.visible = false
  //     }
  //     this.el.sceneEl.addEventListener('xrimagefound', showImage)
  //     this.el.sceneEl.addEventListener('xrimageupdated', showImage)
  //     this.el.sceneEl.addEventListener('xrimagelost', hideImage)
  //   }
  // })
  //
  //
  // Event: xrprojectwayspotscanning
  // Description: Fires when all Project Wayspots have been loaded for scanning.
  //   detail: {
  //     wayspots: [{
  //       id: An id for this Wayspot this is stable within a session.
  //       name: The Project Wayspot name.
  //       imageUrl: URL to a representative image for this Project Wayspot.
  //       title: The Wayspot title.
  //       lat: Latitude of this Project Wayspot.
  //       lng: Longitude of this Project Wayspot.
  //     }]
  //   }
  //
  // Event: xrprojectwayspotfound
  // Description: Fires when a Project Wayspot is first found.
  //   detail: {
  //     name: The Project Wayspot name.
  //     position {x, y, z}: The 3d position of the located Project Wayspot.
  //     rotation {w, x, y, z}: The 3d local orientation of the located Project Wayspot.
  //   }
  //
  // Event: xrprojectwayspotupdated
  // Description: Fires when a Project Wayspot changes position or rotation.
  //   detail: {
  //     name: The Project Wayspot name.
  //     position {x, y, z}: The 3d position of the located Project Wayspot.
  //     rotation {w, x, y, z}: The 3d local orientation of the located Project Wayspot.
  //   }
  //
  // Event: xrprojectwayspotlost
  // Description: Fires when a Project Wayspot changes position or rotation.
  //   detail: {
  //     name: The Project Wayspot name.
  //     position {x, y, z}: The 3d position of the located Project Wayspot.
  //     rotation {w, x, y, z}: The 3d local orientation of the located Project Wayspot.
  //   }
  //
  //
  // Event: xrmeshfound:
  // Description: Fires when a mesh is first found either after start or after a recenter().
  //   detail: {
  //     position {x, y, z}: The 3d position of the located mesh.
  //     rotation {w, x, y, z}: The 3d local orientation of the located mesh.
  //     bufferGeometry THREE.BufferGeometry: A Three.js mesh.
  //     id: Unique id for the mesh that lasts for the length current session
  //   }
  //
  // Event: xrmeshupdated
  // Description: Fires when a the **first** mesh we found changes position or rotation.
  //   detail: {
  //     position {x, y, z}: The position of the mesh in the scene.
  //     rotation {w, x, y, z}: The orientation (quaternion) of the mesh in the scene.
  //     id: Unique id for the mesh that lasts for the length current session
  //   }
  //
  // Event: xrmeshlost
  // Description: Fires when recenter is called.
  //   detail: {
  //     id: Unique id for the mesh that lasts for the length current session
  //   }
  //
  // ------------------------------- xrweb AFrame Event Listeners -------------------------------
  // These are the events you can emit from your web application.
  // None.
  //
  // Creates an A-Frame component which can be registered with AFRAME.registerComponent().
  // This, however, generally won't need to be called directly. On 8th Wall Web script load, this
  // component will be registered if it is detected that A-Frame has loaded (i.e if window.AFRAME
  // exists).
  //
  // Example:
  //
  // window.AFRAME.registerComponent('xrweb', XR8.AFrame.xrwebComponent())
  xrwebComponent: () => xrwebComponent(EngineKind.XR),

  // ------------------------------- xrface -------------------------------
  // xrface is used for face tracking. xrconfig should also be added alongside xrface, though for
  // backwards compatibility if xrconfig is not added, xrface will automatically add xrconfig and
  // pass along provided attributes to xrconfig.
  //
  // ------------------------------- xrface Attributes -------------------------------
  // meshGeometry; type: array; default: ['face']
  //   Comma separated strings that configures which portions of the face mesh will have returned
  //   triangle indices. Can be any combination of 'face', 'eyes' and 'mouth'.
  // maxDetections; type: number; default: 1
  //   Number of faces that can be tracked at once. The available choices are 1, 2, or 3.
  // uvType; type: string; default: 'standard'
  //   Specifies which uvs are returned in the facescanning and faceloading event. Options are:
  //   XR8.FaceController.UvType.PROJECTED or XR8.FaceController.UvType.STANDARD. The default is
  //   XR8.FaceController.UvType.STANDARD.
  //
  // ------------------------------- xrface AFrame Events -------------------------------
  // These are the events you can listen for in your web application.
  // Event: xrfaceloading
  // Description: This event is emitted when when loading begins for additional face AR resources.
  // detail: {
  //   maxDetections: the maximum number of faces that can be simultaneously processed.
  //   pointsPerDetection: number of vertices that will be extracted per face.
  //   indices [{a, b, c]: indexes into the vertices array that form the triangles of the
  //     requested mesh, as specified with meshGeometry on configure.
  //   uvs [{u, v]: uv positions into a texture map corresponding to the returned vertex
  //     points.
  // }
  // Example:
  //   const initMesh = ({detail}) => {
  //     const {pointsPerDetection, uvs, indices} = detail
  //     this.el.object3D.add(generateMeshGeometry({pointsPerDetection, uvs, indices}))
  //   }
  //   this.el.sceneEl.addEventListener('xrfaceloading', initMesh)
  //
  //
  // Event: xrfacescanning
  // Description: This event is emitted when all face AR resources have been loaded and scanning has
  //   begun.
  // detail: {
  //   maxDetections: the maximum number of faces that can be simultaneously processed.
  //   pointsPerDetection: number of vertices that will be extracted per face.
  //   indices: [{a, b, c]: indexes into the vertices array that form the triangles of the
  //     requested mesh, as specified with meshGeometry on configure.
  //   uvs: [{u, v]: uv positions into a texture map corresponding to the returned vertex
  //     points.
  // }
  //
  //
  // Event: xrfacefound
  // Description: This event is emitted when a face is first found.
  // detail: {
  //   id: A numerical id of the located face.
  //   transform: {
  //     position {x, y, z}: The 3d position of the located face.
  //     rotation {w, x, y, z}: The 3d local orientation of the located face.
  //     scale: A scale factor that should be applied to objects attached to this face.
  //     scaledWidth: Approximate width of the head in the scene when multiplied by scale.
  //     scaledHeight: Approximate height of the head in the scene when multiplied by scale.
  //     scaledDepth: Approximate depth of the head in the scene when multiplied by scale.
  //   }
  //   vertices [{x, y, z}]: Position of face points, relative to transform.
  //   normals [{x, y, z}]: Normal direction of vertices, relative to transform.
  //   attachmentPoints: {
  //     'name': {
  //        position: {x, y, z}: position of attachment point on the face, relative to the
  //          transform
  //     }
  //   }
  // }
  // Example:
  // const faceRigidComponent = {
  //   init: function () {
  //     const object3D = this.el.object3D
  //     object3D.visible = false
  //     const show = ({detail}) => {
  //       const {position, rotation, scale} = detail.transform
  //       object3D.position.copy(position)
  //       object3D.quaternion.copy(rotation)
  //       object3D.scale.set(scale, scale, scale)
  //       object3D.visible = true
  //     }
  //     const hide = ({detail}) => { object3D.visible = false }
  //     this.el.sceneEl.addEventListener('xrfacefound', show)
  //     this.el.sceneEl.addEventListener('xrfaceupdated', show)
  //     this.el.sceneEl.addEventListener('xrfacelost', hide)
  //   }
  // }
  //
  //
  // Event: xrfaceupdated
  // Description: This event is emitted when face is subsequently found.
  // detail: {
  //   id: A numerical id of the located face.
  //   transform: {
  //     position {x, y, z}: The 3d position of the located face.
  //     rotation {w, x, y, z}: The 3d local orientation of the located face.
  //     scale: A scale factor that should be applied to objects attached to this face.
  //     scaledWidth: Approximate width of the head in the scene when multiplied by scale.
  //     scaledHeight: Approximate height of the head in the scene when multiplied by scale.
  //     scaledDepth: Approximate depth of the head in the scene when multiplied by scale.
  //   }
  //   vertices [{x, y, z}]: Position of face points, relative to transform.
  //   normals [{x, y, z}]: Normal direction of vertices, relative to transform.
  //   attachmentPoints: {
  //     'name': {
  //        position: {x, y, z}: position of attachment point on the face, relative to the
  //          transform
  //     }
  //   }
  // }
  // Example:
  // const faceRigidComponent = {
  //   init: function () {
  //     const object3D = this.el.object3D
  //     object3D.visible = false
  //     const show = ({detail}) => {
  //       const {position, rotation, scale} = detail.transform
  //       object3D.position.copy(position)
  //       object3D.quaternion.copy(rotation)
  //       object3D.scale.set(scale, scale, scale)
  //       object3D.visible = true
  //     }
  //     const hide = ({detail}) => { object3D.visible = false }
  //     this.el.sceneEl.addEventListener('xrfacefound', show)
  //     this.el.sceneEl.addEventListener('xrfaceupdated', show)
  //     this.el.sceneEl.addEventListener('xrfacelost', hide)
  //   }
  // }
  //
  //
  // Event: xrfacelost
  // Description: This event is emitted when a face is no longer being tracked.
  // detail: {
  //   id: A numerical id of the face that was lost.
  // }
  // Example:
  // const faceRigidComponent = {
  //   init: function () {
  //     const object3D = this.el.object3D
  //     object3D.visible = false
  //     const show = ({detail}) => {
  //       const {position, rotation, scale} = detail.transform
  //       object3D.position.copy(position)
  //       object3D.quaternion.copy(rotation)
  //       object3D.scale.set(scale, scale, scale)
  //       object3D.visible = true
  //     }
  //     const hide = ({detail}) => { object3D.visible = false }
  //     this.el.sceneEl.addEventListener('xrfacefound', show)
  //     this.el.sceneEl.addEventListener('xrfaceupdated', show)
  //     this.el.sceneEl.addEventListener('xrfacelost', hide)
  //   }
  // }
  //
  //
  // TODO(nathan): add face gesture event descriptions once the API has settled.
  //
  // ------------------------------- xrface AFrame Event Listeners -------------------------------
  //
  // These are the events you can emit from your web application.
  // None.
  //
  // Creates an A-Frame component which can be registered with AFRAME.registerComponent().
  // This, however, generally won't need to be called directly. On 8th Wall Web script load, this
  // component will be registered if it is detected that A-Frame has loaded (i.e if window.AFRAME
  // exists).
  //
  // Example:
  //
  // window.AFRAME.registerComponent('xrface', XR8.AFrame.xrfaceComponent())
  xrfaceComponent: () => xrwebComponent(EngineKind.FACE),

  // ------------------------------- xrlayers -------------------------------
  // xrlayers is used for face tracking. xrconfig should also be added alongside xrlayers, though
  // for backwards compatibility if xrconfig is not added, xrlayers will automatically add xrconfig
  // and pass along provided attributes to xrconfig.
  //
  // ------------------------------- xrlayers Attributes -------------------------------
  // None.
  //
  // ------------------------------- xrlayers AFrame Events -------------------------------
  // These are the events you can listen for in your web application.
  // None.
  //
  // ------------------------------- xrlayers AFrame Event Listeners -------------------------------
  // These are the events you can emit from your web application.
  // None.
  //
  // Creates an A-Frame component which can be registered with AFRAME.registerComponent().
  // This, however, generally won't need to be called directly. On 8th Wall Web script load, this
  // component will be registered if it is detected that A-Frame has loaded (i.e if window.AFRAME
  // exists).
  //
  // Example:
  //
  // window.AFRAME.registerComponent('xrlayers', XR8.AFrame.xrlayersComponent())
  xrlayersComponent: () => xrwebComponent(EngineKind.LAYERS),

  // ------------------------------- xrlayerscene Attributes -------------------------------
  //
  // name; type: string; default: ''
  //   The layer name. Should correspond to a layer from XR8.LayersController
  // invertLayerMask; type: bool; default: false
  //   If false then virtual content added to the layer scene will be visible only in regions where
  //   the layer is found (i.e. the cube is shown in the sky). If false the mask will be flipped
  //   (i.e. the cube is shown everywhere but the sky).
  // edgeSmoothness; type: number; default: '1.0'
  //   How much to smooth the edges of the layer. Valid values: [0.0, 1.0].
  //
  // ------------------------------- xrlayerscene AFrame Events -------------------------------
  // These are the events you can listen for in your web application.
  //
  // Event: layertextureprovider
  // Description: This event is emitted to provide a function that callers can call to render
  //   xrlayerscene's object3D to a texture.
  // Arguments:
  //  {
  //    // The name of the layer. Should match a configured layer in LayersController.
  //    name: string,
  //    // A function to call that will return a texture to draw. Will be masked by the layer
  //    // specified by name.
  //    provideTexture(): WebGlTexture
  //  }
  //
  // ------------------------------- xrlayersscene AFrame Event Listeners --------------------------
  // None.
  //
  // Creates an A-Frame component which can be registered with AFRAME.registerComponent().
  // This, however, generally won't need to be called directly. On 8th Wall Web script load, this
  // component will be registered if it is detected that A-Frame has loaded (i.e if window.AFRAME
  // exists).
  //
  // Example:
  //
  // window.AFRAME.registerComponent('xrlayerscene', XR8.AFrame.xrlayersceneComponent())
  xrlayersceneComponent,

  // ------------------------------- sky-scene -------------------------------
  // This primitive adds the xrlayerscene component.
  //
  // ------------------------------- sky-scene Attributes -------------------------------
  // invert-layer-mask; type: string; default: false
  //
  // ------------------------------- sky-scene AFrame Events -------------------------------
  // Events from xrlayerscene.
  //
  // ------------------------------- sky-scene AFrame Event Listeners --------------------------
  // Event listeners from xrlayerscene.
  //
  // Creates an A-Frame primitive which can be registered with AFRAME.registerPrimitive().
  // This, however, generally won't need to be called directly. On 8th Wall Web script load, this
  // primitive will be registered if it is detected that A-Frame has loaded (i.e if window.AFRAME
  // exists).
  //
  // Example:
  //
  // window.AFRAME.registerPrimitive('sky-scene', XR8.AFrame.skyscenePrimitive())
  skyscenePrimitive: () => layerscenePrimitive(LayerKind.SKY),
})

export {
  AFrameFactory,
  registerAFrameXrComponent,
  registerAFrameXrPrimitive,
}
