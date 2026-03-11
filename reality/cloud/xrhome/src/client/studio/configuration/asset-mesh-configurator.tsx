import React from 'react'

import {useTranslation} from 'react-i18next'

import {AnimationMixer, Box3, Matrix4, Mesh, Object3D, Material} from 'three'

import {createUseStyles} from 'react-jss'

import {GLTFExporter} from 'three/examples/jsm/exporters/GLTFExporter'
import {GLTFLoader} from 'three/examples/jsm/loaders/GLTFLoader'

import {Document, NodeIO, PropertyType} from '@gltf-transform/core'
import {prune} from '@gltf-transform/functions'
import {KHRONOS_EXTENSIONS} from '@gltf-transform/extensions'

import {MAX_RECOMMENDED_TEXTURE_SIZE} from '@ecs/shared/material-constants'

import {FloatingTraySection} from '../../ui/components/floating-tray-section'
import {RowNumberField, RowSelectField} from './row-fields'
import {useGltfWithDraco} from '../hooks/gltf'
import {PrimaryButton} from '../../ui/components/primary-button'
import {TertiaryButton} from '../../ui/components/tertiary-button'
import {useStyles} from './row-fields'
import {combine} from '../../common/styles'
import {useFileUploadState} from '../../editor/hooks/use-file-upload-state'
import useCurrentApp from '../../common/use-current-app'
import {EditorFileLocation, extractFilePath, extractRepoId} from '../../editor/editor-file-location'
import {basename, dirname} from '../../editor/editor-common'
import {
  traverseTwo, containsTexture, replaceMaterial,
  getObject3DInfo, replaceMesh, getGltfTransformDocument,
} from '../asset-previews/asset-utils'
import {
  changeTextureSize, simplifyMesh, resetGltfTransformWorker,
} from '../asset-previews/gltf-transform-utils'
import {useAbandonableEffect} from '../../hooks/abandonable-effect'
import {AssetMaterialsConfigurator} from './asset-materials-configurator'
import {Tooltip} from '../../ui/components/tooltip'
import {Icon} from '../../ui/components/icon'
import {useStudioModelPreviewContext} from '../asset-previews/studio-model-preview-context'
import {StaticBanner} from '../../ui/components/banner'
import {findMaterialsInThreeScene} from './three-material-util'

const textureCache: Record<string, boolean> = {}

const useCustomStyles = createUseStyles({
  buttonsContainer: {
    marginTop: '1rem',
    padding: '0em 1em 1em',
  },
  warningTextureContainer: {
    paddingBottom: '1em',
  },
})

interface IAssetMeshConfigurator {
  selectedAssetLocation: EditorFileLocation
  modelUrl: string
  previewScene: Object3D
  mixer: AnimationMixer
}

const AssetMeshConfigurator: React.FC<IAssetMeshConfigurator> = ({
  selectedAssetLocation, modelUrl, previewScene, mixer,
}) => {
  const app = useCurrentApp()
  const {uploadFiles} = useFileUploadState(
    extractRepoId(selectedAssetLocation) ?? app.repoId,
    app.assetLimitOverrides
  )
  const {
    modelInfo, showTextureSizeWarning,
    setModelInfo, setShowSimplifyMeshWarning, setShowTextureSizeWarning,
    setFileSize, setShowGlbComputingLoader, setLoadingFileSize, setLoadingTriCount,
  } = useStudioModelPreviewContext()

  const rowFieldClasses = useStyles()
  const classes = useCustomStyles()

  const gltf = useGltfWithDraco(modelUrl)
  const {scene: sourceScene} = gltf

  const [hasTexture, setHasTexture] = React.useState(false)
  const [sourceSceneExport, setSourceSceneExport] = React.useState<ArrayBuffer>(null)

  const [isChangingTextureSize, setIsChangingTextureSize] = React.useState(false)
  const [isSimplifyingMesh, setIsSimplifyingMesh] = React.useState(false)
  const [materialsArray, setMaterialsArray] = React.useState<Material[]>([])
  const loaderTimeoutRef = React.useRef(null)

  React.useEffect(() => {
    if (isSimplifyingMesh || isChangingTextureSize) {
      loaderTimeoutRef.current = setTimeout(() => {
        setShowGlbComputingLoader(true)
        setLoadingFileSize(true)
        if (isSimplifyingMesh) {
          setLoadingTriCount(true)
        }
      }, 750)
    } else {
      setShowGlbComputingLoader(false)
    }

    return () => {
      clearTimeout(loaderTimeoutRef.current)
    }
  }, [isSimplifyingMesh, isChangingTextureSize])

  React.useEffect(() => {
    if (modelUrl in textureCache) {
      setHasTexture(textureCache[modelUrl])
    } else {
      const textureStatus = containsTexture(previewScene)
      textureCache[modelUrl] = textureStatus
      setHasTexture(textureStatus)
    }
  }, [modelUrl])

  React.useEffect(() => {
    setSourceSceneExport(null)
  }, [sourceScene])

  const {t} = useTranslation(['cloud-studio-pages'])

  const [selectedPivotPoint, setSelectedPivotPoint] = React.useState<string>('')
  const [assetScaleFactor, setAssetScaleFactor] = React.useState<number>(1)
  const [hasChanged, setHasChanged] = React.useState(false)
  const [selectedTextureSize, setSelectedTextureSize] = React.useState<string>('')
  const [simplifyMeshRatio, setSimplifyMeshRatio] = React.useState<number>(null)
  const [isSavingGltf, setIsSavingGltf] = React.useState(false)

  const textureSizeOptions = [
    {value: '128', content: '128 x 128'},
    {value: '256', content: '256 x 256'},
    {value: '512', content: '512 x 512'},
    {value: '1024', content: '1024 x 1024'},
    {value: '2048', content: '2048 x 2048'},
  ]

  const pivotPointOptions = [
    {value: 'top', content: t('asset_configurator.asset_mesh_configurator.pivot_point.top')},
    {value: 'bottom', content: t('asset_configurator.asset_mesh_configurator.pivot_point.bottom')},
    {value: 'center', content: t('asset_configurator.asset_mesh_configurator.pivot_point.center')},
    {value: 'front', content: t('asset_configurator.asset_mesh_configurator.pivot_point.front')},
    {value: 'back', content: t('asset_configurator.asset_mesh_configurator.pivot_point.back')},
  ]

  const applyTranslation = (x: number, y: number, z: number) => {
    const matrix = new Matrix4().makeTranslation(x, y, z)
    previewScene.applyMatrix4(matrix)
  }

  const changeModelPivotPoint = (newPivotPoint: string) => {
    const scaledBbox = new Box3().setFromObject(previewScene)
    const centerX = 0.5 * (scaledBbox.min.x + scaledBbox.max.x)
    const centerY = 0.5 * (scaledBbox.min.y + scaledBbox.max.y)
    const centerZ = 0.5 * (scaledBbox.min.z + scaledBbox.max.z)

    switch (newPivotPoint) {
      case 'top':
        applyTranslation(-centerX, -scaledBbox.max.y, -centerZ)
        break
      case 'bottom':
        applyTranslation(-centerX, -scaledBbox.min.y, -centerZ)
        break
      case 'front':
        applyTranslation(-centerX, -scaledBbox.min.y, -scaledBbox.max.z)
        break
      case 'back':
        applyTranslation(-centerX, -scaledBbox.min.y, -scaledBbox.min.z)
        break
      case 'center':
        applyTranslation(-centerX, -centerY, -centerZ)
        break
      default:
        previewScene.position.set(0, 0, 0)
        break
    }
  }

  const changeScale = (newScale: number) => {
    if (previewScene) {
      previewScene.scale.set(newScale, newScale, newScale)
      changeModelPivotPoint(selectedPivotPoint)
    }
  }

  const recalculateGltfFileSize = async () => {
    setLoadingFileSize(true)
    const exporter = new GLTFExporter()
    try {
      const result = await new Promise((resolve, reject) => {
        exporter.parse(
          previewScene,
          parseResult => resolve(parseResult),
          error => reject(error),
          {binary: true}
        )
      })
      if (result instanceof ArrayBuffer) {
        const fileSize = result.byteLength
        setFileSize(fileSize)
      } else {
        const json = JSON.stringify(result)
        const fileSize = new Blob([json]).size
        setFileSize(fileSize)
      }
    } catch (error) {
      // eslint-disable-next-line no-console
      console.error('Error calculating GLTF file size:', error)
    }
    setLoadingFileSize(false)
  }

  const resetScene = async () => {
    resetGltfTransformWorker()
    changeModelPivotPoint('')
    setSelectedPivotPoint('')

    replaceMaterial(previewScene, sourceScene)
    setSelectedTextureSize('')

    replaceMesh(previewScene, sourceScene)
    setSimplifyMeshRatio(null)

    previewScene.scale.set(1, 1, 1)
    setAssetScaleFactor(1.0)

    await recalculateGltfFileSize()
    setHasChanged(false)
    setIsChangingTextureSize(false)
    setIsSimplifyingMesh(false)
  }

  React.useEffect(() => {
    setMaterialsArray(Array.from(findMaterialsInThreeScene(previewScene)))
  }, [hasChanged])

  // Reset model configuration options
  React.useEffect(() => () => {
    resetScene()
  }, [modelUrl])

  const getGltfExport = (
    sceneToExport: Object3D, options: Object = {binary: true}
  ) => new Promise<ArrayBuffer>((
    resolve, reject
  ) => {
    if (sourceSceneExport) {
      resolve(sourceSceneExport)
      return
    }
    const exporter = new GLTFExporter()
    exporter.parse(sceneToExport, async (result) => {
      if (!result) {
        reject(new Error('fail to export gltf'))
      }
      if (result instanceof ArrayBuffer) {
        setSourceSceneExport(result)
        resolve(result)
      } else {
        reject(new Error('fail to export gltf as binary'))
      }
    }, (e) => { reject(e) }, options)
  })

  const saveGltfTransformToScene = async (
    glbBinary: Uint8Array, replaceSceneContents: (scene: Object3D, replacement: Object3D) => void
  ): Promise<void> => {
    if (!glbBinary) {
      throw new Error('glbBinary is null')
    }
    const loader = new GLTFLoader()
    const blob = new Blob([glbBinary], {type: 'application/octet-stream'})
    const url = URL.createObjectURL(blob)

    return new Promise<void>((resolve, reject) => {
      loader.load(
        url,
        (loadedGltf) => {
          replaceSceneContents(previewScene, loadedGltf.scene.children[0])
          recalculateGltfFileSize()
          URL.revokeObjectURL(url)
          resolve()
        },
        undefined,
        (e) => {
          URL.revokeObjectURL(url)
          reject(e)
        }
      )
    })
  }

  const updateModelInfo = () => {
    const newModelInfo = getObject3DInfo(previewScene)
    if (modelInfo.triangles === newModelInfo.triangles) {
      setShowSimplifyMeshWarning(true)
    }
    setModelInfo(newModelInfo)
    setLoadingTriCount(false)
  }

  useAbandonableEffect(async (maybeAbandon) => {
    if (!selectedTextureSize) {
      return
    }
    setHasChanged(true)
    setIsChangingTextureSize(true)
    try {
      const gltfExport = await maybeAbandon(getGltfExport(sourceScene))
      const glbBinary = await maybeAbandon(changeTextureSize(selectedTextureSize, gltfExport))
      if (!glbBinary) return

      await maybeAbandon(saveGltfTransformToScene(glbBinary, replaceMaterial))
    } catch (e) {
      throw new Error(`Failed to transform ThreeJS glb to gltf-transform glb: ${e}`)
    }
    setIsChangingTextureSize(false)
  }, [selectedTextureSize])

  useAbandonableEffect(async (maybeAbandon) => {
    if (simplifyMeshRatio === null) {
      return
    }
    setShowSimplifyMeshWarning(false)
    setHasChanged(true)
    setIsSimplifyingMesh(true)
    try {
      const gltfExport = await maybeAbandon(getGltfExport(sourceScene))
      const glbBinary = await maybeAbandon(simplifyMesh(simplifyMeshRatio, gltfExport))
      if (!glbBinary) return

      await maybeAbandon(saveGltfTransformToScene(glbBinary, replaceMesh))
      updateModelInfo()
    } catch (e) {
      throw new Error(`Failed to transform ThreeJS glb to gltf-transform glb: ${e}`)
    }
    setIsSimplifyingMesh(false)
  }, [simplifyMeshRatio])

  const handlePivotPointSelect = (newValue: string) => {
    setSelectedPivotPoint(newValue)
    changeModelPivotPoint(newValue)
    setHasChanged(true)
  }

  // Note (jgarciakimble) Using gltf-transform lets us get rid of auxiliary node
  async function promoteRootScene(document: Document) {
    const root = document.getRoot()
    const scenes = root.listScenes()
    if (scenes.length === 0) {
      // eslint-disable-next-line no-console
      console.error('No scenes found in the document')
      return
    }

    // The first scene is the auxiliary scene
    const auxScene = scenes[0]
    const children = auxScene.listChildren()

    if (children.length !== 1) {
      // eslint-disable-next-line no-console
      console.error('The auxiliary scene does not have exactly one child')
      return
    }

    // The only child of auxScene is an auxiliary node which used to be the root scene prior to
    // the gltf-transform export. The auxiliary node is removed, else the scene hierarchy will
    // grow by one node per save. To preserve naming, the root scene is renamed to the name of the
    // auxNode (i.e. the previous root).
    const auxNode = children[0]
    auxScene.removeChild(auxNode)
    auxScene.setName(auxNode.getName())

    for (const child of auxNode.listChildren()) {
      auxScene.addChild(child)
    }

    await document.transform(prune({propertyTypes: [PropertyType.NODE, PropertyType.SCENE]}))
  }

  const resetPreviewOptions = () => {
    traverseTwo(previewScene, sourceScene, (object1, object2) => {
      if (object1 instanceof Mesh && object2 instanceof Mesh &&
        object1.material && object2.material) {
        object1.material.wireframe = object2.material.wireframe
        object1.material.envMap = object2.material.envMap
        object1.material.needsUpdate = true
      }
    })
    mixer.stopAllAction()
    mixer.update(0)
  }

  const saveGltf = async () => {
    setIsSavingGltf(true)
    const io = new NodeIO().registerExtensions(KHRONOS_EXTENSIONS)
    const options = {
      binary: true,
      animations: previewScene.animations,
    }

    resetPreviewOptions()
    const glbDocument = await getGltfTransformDocument(previewScene, options)
    await promoteRootScene(glbDocument)
    const gltfBlob = await io.writeBinary(glbDocument)

    const gltfFile: File = new File(
      [gltfBlob], basename(extractFilePath(selectedAssetLocation)), {type: 'model/gltf-binary'}
    )
    const selectedAssetFolder = dirname(extractFilePath(selectedAssetLocation))
    await uploadFiles([gltfFile], selectedAssetFolder)
    setIsSavingGltf(false)
  }

  const handleAssetScaleNumber = (newNumber: number) => {
    setAssetScaleFactor(newNumber)
    changeScale(newNumber)
    setHasChanged(true)
  }

  const pivotPointField = (
    <RowSelectField
      id='asset-pivot-point'
      label={t('asset_configurator.asset_mesh_configurator.pivot_point.title')}
      options={pivotPointOptions}
      onChange={handlePivotPointSelect}
      value={selectedPivotPoint}
      placeholder={t('asset_configurator.asset_mesh_configurator.pivot_point.placeholder')}
      disabled={isSavingGltf}
    />
  )

  const buttons = (
    <div className={combine(rowFieldClasses.row, classes.buttonsContainer)}>
      <div className={combine(rowFieldClasses.flexItem, rowFieldClasses.flexItemGroup)}>
        <div className={rowFieldClasses.flexItem}>
          <TertiaryButton
            height='small'
            onClick={resetScene}
            spacing='full'
            disabled={!hasChanged || isSavingGltf}
          >
            {t('asset_configurator.asset_mesh_configurator.reset_button.label')}
          </TertiaryButton>
        </div>
        <div className={rowFieldClasses.flexItem}>
          <PrimaryButton
            height='small'
            onClick={saveGltf}
            spacing='full'
            disabled={!hasChanged || isSavingGltf || isChangingTextureSize || isSimplifyingMesh}
            loading={isSavingGltf}
          >
            {t('asset_configurator.asset_mesh_configurator.save_button.label')}
          </PrimaryButton>
        </div>
      </div>
    </div>
  )

  const assetScaleField = (
    <RowNumberField
      id='asset-scale'
      onChange={handleAssetScaleNumber}
      value={assetScaleFactor}
      min={0}
      step={0.1}
      label={t('asset_configurator.asset_mesh_configurator.scale.label')}
      disabled={isSavingGltf}
    />
  )

  const textureSizeField = (
    <RowSelectField
      id='asset-texture-size'
      label={t('asset_configurator.asset_mesh_configurator.texture_size.label')}
      options={textureSizeOptions}
      onChange={setSelectedTextureSize}
      value={selectedTextureSize}
      placeholder={t('asset_configurator.asset_mesh_configurator.texture_size.placeholder')}
      disabled={isSavingGltf}
    />
  )

  const simplifyMeshField = (
    <RowNumberField
      id='asset-simplify-mesh'
      onChange={setSimplifyMeshRatio}
      value={simplifyMeshRatio ?? 1}
      min={0}
      max={1}
      step={0.1}
      label={(
        <>
          {t('asset_configurator.asset_mesh_configurator.simplify_mesh.title')}
          <Tooltip content={t('asset_configurator.asset_mesh_configurator.simplify_mesh.tooltip')}>
            <Icon size={0.75} stroke='info' inline />
          </Tooltip>
        </>
      )}
      disabled={isSavingGltf}
    />
  )

  return (
    <>
      <FloatingTraySection
        title={t('asset_configurator.asset_mesh_configurator.title')}
        borderBottom={false}
      >
        {showTextureSizeWarning && (
          <div className={classes.warningTextureContainer}>
            <StaticBanner
              type='warning'
              onClose={() => setShowTextureSizeWarning(false)}
            >
              <span>
                {t(
                  'asset_configurator.asset_name_configurator.texture_size.warning',
                  {size: MAX_RECOMMENDED_TEXTURE_SIZE}
                )}
              </span>
            </StaticBanner>
          </div>
        )}
        {pivotPointField}
        {hasTexture && textureSizeField}
        {simplifyMeshField}
        {assetScaleField}
      </FloatingTraySection>
      <AssetMaterialsConfigurator
        previewScene={previewScene as Object3D}
        materialsArray={materialsArray}
        setHasChanged={e => setHasChanged(e)}
        key={previewScene.id}
      />
      {buttons}
    </>
  )
}

export default AssetMeshConfigurator
