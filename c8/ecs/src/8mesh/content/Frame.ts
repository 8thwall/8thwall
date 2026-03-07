// @ts-nocheck
const {PlaneGeometry} = window.THREE

/**
 * Returns a basic plane mesh.
 */
export default class Frame extends window.THREE.Mesh {
  constructor(material) {
    const geometry = new PlaneGeometry()

    super(geometry, material)
    this.castShadow = true
    this.receiveShadow = true

    this.name = 'MeshUI-Frame'
  }
}
