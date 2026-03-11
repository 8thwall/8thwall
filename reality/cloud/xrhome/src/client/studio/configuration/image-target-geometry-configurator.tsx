import React from 'react'
import {useTranslation} from 'react-i18next'
import {degreesToRadians} from '@ecs/shared/angle-conversion'

import {createThemedStyles} from '../../ui/theme'
import {
  DEFAULT_TARGET_CIRCUMFERENCE, getTargetCircumferenceBottom, MAX_ARC_ANGLE, MAX_CONINESS,
  MIN_ARC_ANGLE, MIN_CONINESS, toHundredths, getCircumferenceRatio, CurvedGeometryUnit,
  CurvedGeometryUpdate,
} from '../../apps/image-targets/curved-geometry'
import {
  RowJointToggleButton, RowNumberField, RowRangeSliderInputField, useStyles as useRowStyles,
} from './row-fields'
import type {IImageTarget} from '../../common/types/models'

// because Row Field components require an onChange even when we're only using them for display
const noop = () => {}

enum InputMode {
  BASIC = 'BASIC',
  ADVANCED = 'ADVANCED'
}

const useStyles = createThemedStyles(theme => ({
  valueContainer: {
    display: 'flex',
    alignItems: 'center',
    justifyContent: 'center',
    userSelect: 'none',
    backgroundColor: theme.mainEditorPane,
    border: `1px solid ${theme.subtleBorder}`,
    borderRadius: '4px',
    color: theme.fgMuted,
    fontSize: '12px',
    fontWeight: 600,
    lineHeight: 'normal',
    fontFamily: 'inherit',
    minWidth: '54px',
    minHeight: '24px',
  },
}))

interface ImageTargetMetadata extends Required<CurvedGeometryUpdate> {
  top: number
  left: number
  width: number
  height: number
  isRotated: boolean
  originalWidth: number
  originalHeight: number
  topRadius: number
  bottomRadius: number
  inputMode: InputMode
  unit: CurvedGeometryUnit
  staticOrientation?: {rollAngle?: number; pitchAngle?: number}
}

const recalculateMetadata = (
  targetType: IImageTarget['type'], metadata: Partial<ImageTargetMetadata>
): Partial<ImageTargetMetadata> => {
  const {
    arcAngle, coniness, isRotated, originalWidth, originalHeight, topRadius, bottomRadius,
  } = metadata
  const arcAngleRadians = degreesToRadians(arcAngle)
  const radius = DEFAULT_TARGET_CIRCUMFERENCE / arcAngleRadians
  const cylinderCircumference = 2 * Math.PI * radius

  const scaledConiness = 2 ** coniness
  const newCylinderCircumferenceTop = cylinderCircumference * Math.sqrt(scaledConiness)
  const tempCylinderCircumferenceBottom = cylinderCircumference / Math.sqrt(scaledConiness)
  const newTargetCircumferenceTop = newCylinderCircumferenceTop * (arcAngle / 360)

  const circumferenceRatio = targetType === 'CYLINDER'
    ? 1
    : getCircumferenceRatio(
      newCylinderCircumferenceTop,
      tempCylinderCircumferenceBottom,
      topRadius,
      bottomRadius
    )

  const newCylinderCircumferenceBottom = toHundredths(
    newCylinderCircumferenceTop / circumferenceRatio
  )

  const widerTargetCircumference = Math.max(
    newTargetCircumferenceTop,
    getTargetCircumferenceBottom(
      newTargetCircumferenceTop,
      newCylinderCircumferenceTop,
      newCylinderCircumferenceBottom
    )
  )

  const originalAspectRatio = (originalWidth / originalHeight)

  const newCylinderSideLength = toHundredths(
    isRotated
      ? widerTargetCircumference * originalAspectRatio
      : widerTargetCircumference / originalAspectRatio
  )

  const newArcAngle = 360 * (newTargetCircumferenceTop / newCylinderCircumferenceTop)

  return {
    ...metadata,
    arcAngle: newArcAngle,
    cylinderCircumferenceTop: newCylinderCircumferenceTop,
    cylinderCircumferenceBottom: newCylinderCircumferenceBottom,
    targetCircumferenceTop: newTargetCircumferenceTop,
    cylinderSideLength: newCylinderSideLength,
  }
}

interface IImageTargetGeometryConfigurator {
  targetType: IImageTarget['type']
  metadata: ImageTargetMetadata
  onMetadataChange: (metadata: Partial<ImageTargetMetadata>) => void
}

const ImageTargetGeometryConfigurator: React.FC<IImageTargetGeometryConfigurator> = ({
  targetType, metadata,
  onMetadataChange,
}) => {
  const {t} = useTranslation(['cloud-studio-pages', 'app-pages'])
  const classes = useStyles()
  const rowClasses = useRowStyles()

  const {
    arcAngle, coniness, cylinderCircumferenceTop, cylinderCircumferenceBottom,
    cylinderSideLength, targetCircumferenceTop, inputMode, unit,
    isRotated, topRadius, bottomRadius, originalWidth, originalHeight,
  } = metadata

  const handleLabelCurveChange = (inputArcAngle: number, newConiness: number = coniness) => {
    onMetadataChange(recalculateMetadata(targetType, {
      arcAngle: inputArcAngle,
      coniness: newConiness,
      isRotated,
      originalWidth,
      originalHeight,
      topRadius,
      bottomRadius,
    }))
  }

  const handleInputChangeMode = (value: InputMode) => {
    if (value === InputMode.BASIC) {
      handleLabelCurveChange(Math.min(MAX_ARC_ANGLE, Math.max(
        (targetCircumferenceTop / cylinderCircumferenceTop) * 360,
        MIN_ARC_ANGLE
      )),
      Math.min(MAX_CONINESS, Math.max(
        Math.log2(cylinderCircumferenceTop / cylinderCircumferenceBottom),
        MIN_CONINESS
      )))
    }

    onMetadataChange({inputMode: value})
  }

  const handleCircumferenceChange = (value: number) => {
    const maxTargetCircumference = Math.max(
      targetCircumferenceTop,
      getTargetCircumferenceBottom(
        targetCircumferenceTop, cylinderCircumferenceTop, cylinderCircumferenceBottom
      )
    )

    const newCylinderSideLength = isRotated
      ? maxTargetCircumference * (originalWidth / originalHeight)
      : maxTargetCircumference * (originalHeight / originalWidth)

    onMetadataChange({
      arcAngle: 360 * (targetCircumferenceTop / value),
      cylinderCircumferenceTop: value,
      cylinderCircumferenceBottom: (targetType === 'CONICAL')
        ? (
          toHundredths(value / getCircumferenceRatio(
            cylinderCircumferenceTop,
            cylinderCircumferenceBottom,
            topRadius,
            bottomRadius
          ))
        )
        : value,
      cylinderSideLength: toHundredths(newCylinderSideLength),
    })
  }

  const handleTargetCircumferenceTopChange = (value: number) => {
    const maxTargetCircumference = Math.max(
      value,
      getTargetCircumferenceBottom(
        value, cylinderCircumferenceTop, cylinderCircumferenceBottom
      )
    )

    const newCylinderSideLength = isRotated
      ? maxTargetCircumference * (originalWidth / originalHeight)
      : maxTargetCircumference * (originalHeight / originalWidth)

    onMetadataChange({
      targetCircumferenceTop: value,
      arcAngle: 360 * (value / cylinderCircumferenceTop),
      cylinderSideLength: toHundredths(newCylinderSideLength),
    })
  }

  return (
    <>
      <RowJointToggleButton
        id='image-target-geometry-type'
        label={t('asset_configurator.image_target_configurator.geometry.input_type')}
        options={[
          {
            value: InputMode.BASIC,
            content: t('asset_configurator.image_target_configurator.geometry.input_type.sliders'),
          },
          {
            value: InputMode.ADVANCED,
            content: t('asset_configurator.image_target_configurator.geometry.input_type.input'),
          },
        ]}
        value={inputMode}
        onChange={handleInputChangeMode}
      />
      {inputMode === InputMode.BASIC
        ? (
          <div className={rowClasses.indent}>
            <RowRangeSliderInputField
              id='image-target-geometry-curve'
              label={t('asset_configurator.image_target_configurator.geometry.label_curve')}
              value={arcAngle}
              min={MIN_ARC_ANGLE}
              max={MAX_ARC_ANGLE}
              step={4}
              onChange={handleLabelCurveChange}
              rightContent={(
                <div className={classes.valueContainer}>
                  {`${((arcAngle / MAX_ARC_ANGLE) * 100).toFixed(2)}%`}
                </div>
              )}
            />
          </div>
        )
        : (
          <div className={rowClasses.indent}>
            <RowJointToggleButton
              id='image-target-geometry-units'
              label={t('asset_configurator.image_target_configurator.geometry.units')}
              options={[
                {
                  value: 'in',
                  content: t('asset_configurator.image_target_configurator.geometry.units.inch'),
                },
                {
                  value: 'mm',
                  content:
                  t('asset_configurator.image_target_configurator.geometry.units.millimeter'),
                },
              ]}
              value={unit}
              onChange={value => onMetadataChange({unit: value})}
            />
            <RowNumberField
              id='image-target-geometry-circumference'
              label={
              t(targetType === 'CONICAL'
                ? 'asset_configurator.image_target_configurator.geometry.top_circumference'
                : 'asset_configurator.image_target_configurator.geometry.circumference')
            }
              value={cylinderCircumferenceTop}
              onChange={handleCircumferenceChange}
              step={1}
            />
            {targetType === 'CONICAL' && (
              <RowNumberField
                id='image-target-geometry-bottom-circumference'
                label={
                t('asset_configurator.image_target_configurator.geometry.bottom_circumference')
              }
                value={cylinderCircumferenceBottom}
                onChange={noop}
                step={1}
                disabled
              />
            )}
            <RowNumberField
              id='image-target-geometry-label-arc-length'
              label={t('asset_configurator.image_target_configurator.geometry.label_arc_length')}
              value={targetCircumferenceTop}
              onChange={handleTargetCircumferenceTopChange}
              step={1}
            />
            <RowNumberField
              id='image-target-geometry-label-height'
              label={t('asset_configurator.image_target_configurator.geometry.label_height')}
              value={cylinderSideLength}
              onChange={noop}
              disabled
              step={1}
            />
          </div>
        )}
    </>
  )
}

export {
  ImageTargetGeometryConfigurator,
  recalculateMetadata,
  InputMode,
  ImageTargetMetadata,
}
