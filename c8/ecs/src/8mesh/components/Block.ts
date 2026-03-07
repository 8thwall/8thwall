// @ts-nocheck
import BoxComponent from './core/BoxComponent'
import InlineManager from './core/InlineManager'
import MeshUIComponent from './core/MeshUIComponent'
import MaterialManager from './core/MaterialManager'

import Frame from '../content/Frame'
import {mix} from '../utils/mix'
import {setLayersOnObject} from '../utils/set-layers-on-object'

const {Object3D, Vector2} = window.THREE

/**

Job:
- Update a Block component
- Calls BoxComponent's API to position its children box components
- Calls InlineManager's API to position its children inline components
- Call creation and update functions of its background planes

 */
export default class Block extends mix.withBase(Object3D)(
  BoxComponent,
  InlineManager,
  MaterialManager,
  MeshUIComponent
) {
  constructor(options) {
    super(options)
    this.isBlock = true

    //

    this.size = new Vector2(1, 1)
    this.frame = new Frame(this.getBackgroundMaterial())
    setLayersOnObject(this.frame, this.getActiveLayers())

    // This is for hiddenOverflow to work
    this.frame.onBeforeRender = () => {
      if (this.updateClippingPlanes) {
        this.updateClippingPlanes()
      }
    }
    this.add(this.frame)

    // Lastly set the options parameters to this object, which will trigger an update

    this.set(options)
  }

  /// /////////
  //  UPDATE
  /// /////////

  parseParams() {
    const bestFit = this.getBestFit()

    if (bestFit !== 'none' && this.childrenTexts.length) {
      this.calculateBestFit(bestFit)
    } else {
      this.childrenTexts.forEach((child) => {
        child._fitFontSize = undefined
      })
    }
  }

  updateLayout() {
    // Get temporary dimension

    const width = this.getWidth()

    const height = this.getHeight()

    if ((width || height) && this.autoLayout) {
      // eslint-disable-next-line no-console
      console.warn('Block got no dimension from its parameters or from children parameters')
      return
    }

    this.size.set(width, height)
    this.frame.scale.set(width, height, 1)
    if (this.frame) this.updateBackgroundMaterial()

    this.frame.renderOrder = this.overrideRenderOrder ?? this.getParentsNumber()

    // Position this element according to earlier parent computation.
    // Delegate to BoxComponent.

    if (this.autoLayout) {
      this.setPosFromParentRecords()
    }

    // Position inner elements according to dimensions and layout parameters.
    // Delegate to BoxComponent.

    if (this.childrenInlines.length) {
      this.computeInlinesPosition()
    }

    this.computeChildrenPosition()

    // We check if this block is the root component,
    // because most of the time the user wants to set the
    // root component's z position themselves
    if (this.parentUI) {
      this.position.z = this.getOffset()
    }
  }

  //

  updateInner() {
    // We check if this block is the root component,
    // because most of the time the user wants to set the
    // root component's z position themselves
    if (this.parentUI) {
      this.position.z = this.getOffset()
    }

    if (this.frame) this.updateBackgroundMaterial()
  }
}
