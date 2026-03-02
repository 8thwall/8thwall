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

const DEFAULT_HAND_PROFILE_PATH =
  'https://cdn.jsdelivr.net/npm/@webxr-input-profiles/assets@1.0/dist/profiles/generic-hand/'

class XRHandMeshModel {
  constructor(handModel, controller, path, handedness) {
    const r3 = parseFloat(window.THREE.REVISION)
    if (!window.AFRAME && r3 < 125 && window.XRHand && !window.XRHand.LITTLE_PHALANX_TIP) {
      // Prevents errors on threejs 124-, but hand tracking is still broken. Without this change,
      // three.js won't create Groups for each of the hand joints, and then later it will crash
      // when it tries to access their properties.
      // See:
      //   https://github.com/mrdoob/three.js/blob/r123/src/renderers/webxr/WebXRController.js#L28
      // In 8frame-1.1.0 this is patched, so we can skip the workaround in the case of AFRAME.
      // eslint-disable-next-line no-console
      console.warn(`Detected THREE revision ${r3}, hand input is unavailable until revision 125.`)
      window.XRHand.LITTLE_PHALANX_TIP = 24
    }

    this.controller = controller
    this.handModel = handModel
    this.bones = []
    this.scale_ = 1

    const loader = makeGltfLoader()
    loader.setPath(path || DEFAULT_HAND_PROFILE_PATH)
    loader.load(`${handedness}.glb`, (gltf) => {
      const [object] = gltf.scene.children
      this.handModel.add(object)

      const mesh = object.getObjectByProperty('type', 'SkinnedMesh')
      mesh.frustumCulled = false
      mesh.castShadow = false
      mesh.receiveShadow = false

      // https://www.w3.org/TR/webxr-hand-input-1/#skeleton-joints-section
      const joints = [
        'wrist',
        'thumb-metacarpal',
        'thumb-phalanx-proximal',
        'thumb-phalanx-distal',
        'thumb-tip',
        'index-finger-metacarpal',
        'index-finger-phalanx-proximal',
        'index-finger-phalanx-intermediate',
        'index-finger-phalanx-distal',
        'index-finger-tip',
        'middle-finger-metacarpal',
        'middle-finger-phalanx-proximal',
        'middle-finger-phalanx-intermediate',
        'middle-finger-phalanx-distal',
        'middle-finger-tip',
        'ring-finger-metacarpal',
        'ring-finger-phalanx-proximal',
        'ring-finger-phalanx-intermediate',
        'ring-finger-phalanx-distal',
        'ring-finger-tip',
        'pinky-finger-metacarpal',
        'pinky-finger-phalanx-proximal',
        'pinky-finger-phalanx-intermediate',
        'pinky-finger-phalanx-distal',
        'pinky-finger-tip',
      ]

      joints.forEach((jointName) => {
        const bone = object.getObjectByName(jointName)

        if (bone !== undefined) {
          bone.jointName = jointName
        }

        this.bones.push(bone)
      })
    })
  }

  updateMesh() {
    // XR Joints
    const XRJoints = this.controller.joints

    for (let i = 0; i < this.bones.length; i++) {
      const bone = this.bones[i]

      if (bone) {
        const XRJoint = XRJoints[bone.jointName]

        if (XRJoint && XRJoint.visible) {
          const {position} = XRJoint

          if (bone) {
            bone.position.copy(position)
            bone.quaternion.copy(XRJoint.quaternion)
            bone.scale.setScalar(this.scale_)
          }
        }
      }
    }
  }

  setScale(scale) {
    this.scale_ = scale
  }
}

export {
  XRHandMeshModel,
}
