const {Object3D} = (window as any).THREE

const setLayersOnObject = (object: typeof Object3D, layers: number[]) => {
  object.layers.disableAll()
  layers.forEach((layer) => {
    object.layers.enable(layer)
  })
}

export {setLayersOnObject}
