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

import {XRHandModelFactory} from './xr-hand-model-factory'
import {XRControllerModelFactory} from './xr-controller-model-factory'

const MAX_TRACER_LENGTH = 5

const CreateControllers = (renderer, scene) => {
  const r3 = parseFloat(window.THREE.REVISION)
  // Factories
  const handModelFactory = new XRHandModelFactory()
  const controllerModelFactory = new XRControllerModelFactory()

  const controllersGroup = new window.THREE.Group()
  controllersGroup.name = 'xrweb-controllers'
  scene.add(controllersGroup)

  const handModels = {
    left: {},
    right: {},
    controllersGroup,
    setOriginTransform: (originTransform) => {
      // Offset the object's transform by the origin and scene scale.
      controllersGroup.matrix.copy(originTransform)
      // Update the object's position, etc. to reflect the new matrix.
      controllersGroup.matrix.decompose(
        controllersGroup.position, controllersGroup.quaternion, controllersGroup.scale
      )
      // Recompute obj.matrixWorld and obj.matrixWorldInverse
      controllersGroup.updateMatrixWorld(true)
    },
    update: (frame) => {
      if (!frame) {
        return
      }
      // We need the input ray space data in hand inputs. However, this feature is not supported
      // before ThreeJS r127
      // See:
      //  https://github.com/mrdoob/three.js/releases/tag/r127
      // eslint-disable-next-line max-len
      //  https://github.com/mrdoob/three.js/commit/f1a577e6d4ffa98cba0ed07f2a123812f82e22d5#diff-5fa11b4a2b3fc499aa3a6c03b3e7d4caf9724ba2636fea4d577d93ceb2184e65R122
      if (r3 >= 127) {
        return
      }
      const referenceSpace = renderer.xr.getReferenceSpace()
      renderer.xr.getSession().inputSources.forEach((input, idx) => {
        const raySpace = renderer.xr.getController(idx)
        if (!input.hand) {
          return
        }

        const inputPose = frame.getPose(input.targetRaySpace, referenceSpace)
        if (!inputPose) {
          return
        }
        raySpace.matrix.fromArray(inputPose.transform.matrix)
        raySpace.matrix.decompose(raySpace.position, raySpace.rotation, raySpace.scale)
        if (inputPose.linearVelocity && raySpace.linearVelocity) {
          raySpace.hasLinearVelocity = true
          raySpace.linearVelocity.copy(inputPose.linearVelocity)
        } else {
          raySpace.hasLinearVelocity = false
        }

        if (inputPose.angularVelocity && raySpace.angularVelocity) {
          raySpace.hasAngularVelocity = true
          raySpace.angularVelocity.copy(inputPose.angularVelocity)
        } else {
          raySpace.hasAngularVelocity = false
        }
        raySpace.visible = true
      })
    },
  }

  // Controllers
  const controller0 = renderer.xr.getController(0)
  const controller1 = renderer.xr.getController(1)
  controllersGroup.add(controller0)
  controllersGroup.add(controller1)
  handModels.left.controller = controller0
  handModels.right.controller = controller1

  // Hand 1 - left
  const controllerGrip1 = renderer.xr.getControllerGrip(0)
  const gripModel1 = controllerModelFactory.createControllerModel(controllerGrip1)
  controllerGrip1.add(gripModel1)
  controllersGroup.add(controllerGrip1)
  handModels.left.grip = gripModel1

  const hand1 = renderer.xr.getHand(0)
  hand1.userData.currentHandModel = 0
  controllersGroup.add(hand1)

  const leftHand = handModelFactory.createHandModel(hand1)
  leftHand.visible = true
  hand1.add(leftHand)
  handModels.left.hand = leftHand

  // Hand 2 - right
  const controllerGrip2 = renderer.xr.getControllerGrip(1)
  const gripModel2 = controllerModelFactory.createControllerModel(controllerGrip2)
  controllerGrip2.add(gripModel2)
  controllersGroup.add(controllerGrip2)
  handModels.right.grip = gripModel2

  const hand2 = renderer.xr.getHand(1)
  hand2.userData.currentHandModel = 0
  controllersGroup.add(hand2)

  const rightHand = handModelFactory.createHandModel(hand2)
  rightHand.visible = true
  hand2.add(rightHand)
  handModels.right.hand = rightHand

  controller0.name = 'controller-1'
  controller1.name = 'controller-2'
  controllerGrip1.name = 'controller-grip-1'
  controllerGrip2.name = 'controller-grip-2'
  hand1.name = 'hand-1'
  hand2.name = 'hand-2'

  // Add tracer lines to controllers
  const geometry = new window.THREE.BufferGeometry().setFromPoints(
    [new window.THREE.Vector3(0, 0, 0), new window.THREE.Vector3(0, 0, -1)]
  )
  const line = new window.THREE.Line(geometry)
  line.name = 'tracer'
  line.scale.z = MAX_TRACER_LENGTH
  handModels.left.tracer = line.clone()
  controller0.add(handModels.left.tracer)
  handModels.right.tracer = line.clone()
  controller1.add(handModels.right.tracer)

  return handModels
}

export {
  CreateControllers,
  MAX_TRACER_LENGTH,
}
