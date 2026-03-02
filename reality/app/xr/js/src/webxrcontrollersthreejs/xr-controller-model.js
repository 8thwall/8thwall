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

/* globals THREE */
import {MotionControllerConstants} from './motion-controllers'

const XRControllerModel = (controller) => {
  const g_ = new THREE.Group()
  g_.controller = controller
  g_.envMap = null
  g_.motionController = null

  g_.setEnvironmentMap = (envMap) => {
    if (g_.envMap === envMap) {
      return g_
    }

    g_.envMap = envMap
    g_.traverse((child) => {
      if (child.isMesh) {
        child.material.envMap = g_.envMap
        child.material.needsUpdate = true
      }
    })

    return g_
  }

  const oldUpdateMatrixWorld = g_.updateMatrixWorld

  g_.updateMatrixWorld = (force) => {
    oldUpdateMatrixWorld.call(g_, force)

    if (!g_.motionController) return

    // Cause the MotionController to poll the Gamepad for data
    g_.motionController.updateFromGamepad()

    // Update the 3D model to reflect the button, thumbstick, and touchpad state
    Object.values(g_.motionController.components).forEach((component) => {
      // Update node data based on the visual responses' current states
      Object.values(component.visualResponses).forEach((visualResponse) => {
        const {valueNode, minNode, maxNode, value, valueNodeProperty} = visualResponse

        // Skip if the visual response node is not found. No error is needed,
        // because it will have been reported at load time.
        if (!valueNode) return

        // Calculate the new properties based on the weight supplied
        if (valueNodeProperty === MotionControllerConstants.VisualResponseProperty.VISIBILITY) {
          valueNode.visible = value
        } else if (
          valueNodeProperty === MotionControllerConstants.VisualResponseProperty.TRANSFORM) {
          if (valueNode.quaternion.slerpQuaternions) {
            valueNode.quaternion.slerpQuaternions(
              minNode.quaternion,
              maxNode.quaternion,
              value
            )
          } else {
            THREE.Quaternion.slerp(
              minNode.quaternion, maxNode.quaternion, valueNode.quaternion, value
            )
          }

          valueNode.position.lerpVectors(
            minNode.position,
            maxNode.position,
            value
          )
        }
      })
    })
  }
  return g_
}

export {
  XRControllerModel,
}
