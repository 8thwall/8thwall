const layerscenePrimitive = name => ({
  defaultComponents: {
    'xrlayerscene': {name},
  },
  mappings: {
    'invert-layer-mask': 'xrlayerscene.invertLayerMask',
    'edge-smoothness': 'xrlayerscene.edgeSmoothness',
  },
})

export {layerscenePrimitive}
