// @ts-nocheck
/**

Job: nothing yet, but adding a isInline parameter to an inline component

Knows: parent dimensions

 */
export default function InlineComponent(Base) {
  // eslint-disable-next-line @typescript-eslint/no-shadow
  return class InlineComponent extends Base {
    constructor(options) {
      super(options)
      this.isInline = true
    }
  }
}
