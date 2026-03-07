// @ts-nocheck
import InlineComponent from './core/InlineComponent'
import BoxComponent from './core/BoxComponent'
import InlineManager from './core/InlineManager'
import MeshUIComponent from './core/MeshUIComponent'
import MaterialManager from './core/MaterialManager'

import Frame from '../content/Frame'
import {mix} from '../utils/mix'
import {setLayersOnObject} from '../utils/set-layers-on-object'

const {Object3D, Vector2} = window.THREE

/**
 * Job:
 * - computing its own size according to user measurements or content measurement
 * - creating an 'inlines' object with info, so that the parent component can
 *   organise it along with other inlines
 *
 * Knows:
 * - Its measurements parameter
 * - Parent block
 */
export default class InlineBlock extends mix.withBase(Object3D)(
  InlineComponent,
  BoxComponent,
  InlineManager,
  MaterialManager,
  MeshUIComponent
) {
  constructor(options) {
    super(options)
    this.isInlineBlock = true

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

  /// ////////
  // UPDATES
  /// ////////

  parseParams() {
    // Get image dimensions

    // eslint-disable-next-line no-console
    if (!this.width) console.warn('inlineBlock has no width. Set to 0.3 by default')
    // eslint-disable-next-line no-console
    if (!this.height) console.warn('inlineBlock has no height. Set to 0.3 by default')
    this.inlines = [{
      height: this.height || 0.3,
      width: this.width || 0.3,
      anchor: 0,
      lineBreak: 'possible',
    }]
  }

  //

  /**
   * Create text content
   *
   * At this point, text.inlines should have been modified by the parent
   * component, to add xOffset and yOffset properties to each inlines.
   * This way, TextContent knows were to position each character.
   *
   */
  updateLayout() {
    const width = this.getWidth()
    const height = this.getHeight()

    if (this.inlines) {
      const options = this.inlines[0]

      // basic translation to put the plane's left bottom corner at the center of its space
      this.position.set(options.width / 2, options.height / 2, 0)

      // translation required by inlineManager to position this component inline
      this.position.x += options.offsetX
      this.position.y += options.offsetY
    }

    this.size.set(width, height)
    this.frame.scale.set(width, height, 1)
    if (this.frame) this.updateBackgroundMaterial()

    this.frame.renderOrder = this.overrideRenderOrder ?? this.getParentsNumber()

    // Position inner elements according to dimensions and layout parameters.
    // Delegate to BoxComponent.

    if (this.childrenInlines.length) {
      this.computeInlinesPosition()
    }

    this.computeChildrenPosition()

    this.position.z = this.getOffset()
  }

  //

  updateInner() {
    this.position.z = this.getOffset()

    if (this.frame) this.updateBackgroundMaterial()
  }
}
