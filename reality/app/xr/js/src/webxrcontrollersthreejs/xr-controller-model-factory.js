// Original source: https://github.com/mrdoob/three.js/tree/master/examples/jsm/webxr
//
// The MIT License
// Copyright © 2010-2021 three.js authors
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
// associated documentation files (the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute,
// sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

import {makeGltfLoader} from '../threejs-support'
import {
  MotionControllerConstants,
  fetchProfile,
  MotionController,
} from './motion-controllers'

import {XRControllerModel} from './xr-controller-model'

// TODO(nb): move to 8w cdn.
const DEFAULT_PROFILES_PATH =
  'https://cdn.jsdelivr.net/npm/@webxr-input-profiles/assets@1.0/dist/profiles'
const DEFAULT_PROFILE = 'generic-trigger'

/**
 * Walks the model's tree to find the nodes needed to animate the components and
 * saves them to the motionContoller components for use in the frame loop. When
 * touchpads are found, attaches a touch dot to them.
 */
function findNodes(motionController, scene) {
  // Loop through the components and find the nodes needed for each components' visual responses
  Object.values(motionController.components).forEach((component) => {
    const {type, touchPointNodeName, visualResponses} = component

    if (type === MotionControllerConstants.ComponentType.TOUCHPAD) {
      component.touchPointNode = scene.getObjectByName(touchPointNodeName)
      if (component.touchPointNode) {
        // Attach a touch dot to the touchpad.
        const sphereGeometry = new window.THREE.SphereGeometry(0.001)
        const material = new window.THREE.MeshBasicMaterial({color: 0x0000FF})
        const sphere = new window.THREE.Mesh(sphereGeometry, material)
        component.touchPointNode.add(sphere)
      }
    }

    // Loop through all the visual responses to be applied to this component
    Object.values(visualResponses).forEach((visualResponse) => {
      const {valueNodeName, minNodeName, maxNodeName, valueNodeProperty} = visualResponse

      // If animating a transform, find the two nodes to be interpolated between.
      if (valueNodeProperty === MotionControllerConstants.VisualResponseProperty.TRANSFORM) {
        visualResponse.minNode = scene.getObjectByName(minNodeName)
        visualResponse.maxNode = scene.getObjectByName(maxNodeName)

        // If the extents cannot be found, skip this animation
        if (!visualResponse.minNode) {
          return
        }

        if (!visualResponse.maxNode) {
          return
        }
      }

      // If the target node cannot be found, skip this animation
      visualResponse.valueNode = scene.getObjectByName(valueNodeName)
    })
  })
}

function addAssetSceneToControllerModel(controllerModel, scene) {
  // Find the nodes needed for animation and cache them on the motionController.
  findNodes(controllerModel.motionController, scene)

  // Apply any environment map that the mesh already has set.
  if (controllerModel.envMap) {
    scene.traverse((child) => {
      if (child.isMesh) {
        child.material.envMap = controllerModel.envMap
        child.material.needsUpdate = true
      }
    })
  }

  // Add the glTF scene to the controllerModel.
  controllerModel.add(scene)
}

class XRControllerModelFactory {
  constructor(gltfLoader = null) {
    this.gltfLoader = gltfLoader
    this.path = DEFAULT_PROFILES_PATH
    this._assetCache = {}

    // If a GLTFLoader wasn't supplied to the constructor create a new one.
    if (!this.gltfLoader) {
      this.gltfLoader = makeGltfLoader()
    }
  }

  createControllerModel(controller) {
    const controllerModel = XRControllerModel(controller)
    let scene = null

    controller.addEventListener('connected', (event) => {
      const xrInputSource = event.data

      if (xrInputSource.targetRayMode !== 'tracked-pointer' || !xrInputSource.gamepad) return

      fetchProfile(xrInputSource, this.path, DEFAULT_PROFILE).then(({profile, assetPath}) => {
        controllerModel.motionController = new MotionController(
          xrInputSource,
          profile,
          assetPath
        )

        const cachedAsset = this._assetCache[controllerModel.motionController?.assetUrl]
        if (cachedAsset) {
          scene = cachedAsset.scene.clone()

          addAssetSceneToControllerModel(controllerModel, scene)
        } else {
          if (!this.gltfLoader) {
            throw new Error('GLTFLoader not set.')
          }

          this.gltfLoader.setPath('')
          this.gltfLoader.load(controllerModel.motionController?.assetUrl, (asset) => {
            this._assetCache[controllerModel.motionController?.assetUrl] = asset

            scene = asset.scene.clone()

            addAssetSceneToControllerModel(controllerModel, scene)
          },
          null,
          () => {
            throw new Error(
              `Asset ${controllerModel.motionController?.assetUrl} missing or malformed.`
            )
          })
        }
      }).catch(() => {
        // Ignore.
      })
    })

    controller.addEventListener('disconnected', () => {
      controllerModel.motionController = null
      controllerModel.remove(scene)
      scene = null
    })

    return controllerModel
  }
}

export {
  XRControllerModelFactory,
}
