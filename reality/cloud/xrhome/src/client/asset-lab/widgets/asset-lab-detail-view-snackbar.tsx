import React from 'react'
import {useTranslation} from 'react-i18next'
import formatDistanceToNow from 'date-fns/formatDistanceToNow'
import {format} from 'date-fns'

import {combine} from '../../common/styles'
import {createThemedStyles} from '../../ui/theme'
import {StandardChip} from '../../ui/components/standard-chip'
import {AllModelToDescription} from '../constants'
import type {AssetGenerationWithCost} from '../types'
import {convertBipsToCredits} from '../../../shared/feature-utils'
import {SelectMenu} from '../../studio/ui/select-menu'
import {useStudioMenuStyles} from '../../studio/ui/studio-menu-styles'
import {useSelector} from '../../hooks'
import {DetailViewButton} from './detail-view-button-widgets'

const useStyles = createThemedStyles(theme => ({
  row: {
    'display': 'flex',
    'flexDirection': 'row',
    'alignItems': 'center',
    'justifyContent': 'space-between',
  },
  contextMenu: {
    zIndex: 200,
    backgroundColor: theme.studioBgMain,
  },
  viewDetailRow: {
    padding: '0.3rem 0',
  },
  mutedText: {
    color: theme.fgMuted,
  },
  mainText: {
    color: theme.fgMain,
  },
}))

interface ViewDetail {
  fieldLabel: string
  fieldValue: string
}
const ViewDetailRow: React.FC<ViewDetail> = ({fieldLabel, fieldValue}) => {
  const classes = useStyles()

  return (
    <div className={classes.viewDetailRow}>
      <span className={classes.mutedText}>{fieldLabel}{'  '}</span>
      <span className={classes.mainText}>{fieldValue}</span>
    </div>
  )
}

interface DetailViewLeftSnackbarProps {
  user: { givenName: string; familyName: string }
  assetGeneration: AssetGenerationWithCost
}

const DetailViewLeftSnackbar: React.FC<DetailViewLeftSnackbarProps> = ({
  user,
  assetGeneration,
}) => {
  const classes = useStyles()
  const menuStyles = useStudioMenuStyles()
  const {t} = useTranslation('asset-lab')
  const assetRequest = useSelector(s => s.assetLab.assetRequests[assetGeneration.RequestUuid])

  const {modelName, createdAt} = assetGeneration
  const {aspect_ratio: aspectRatio, width, height, background, quality} = assetGeneration.metadata

  const formattedModel = AllModelToDescription[modelName] || t('asset_lab.unknown_model')
  const formattedCredits = convertBipsToCredits(
    // NOTE(kyle)(unique-tag-alrd): Delete assetGeneration.totalActionQuantity fallback when
    // backfill is complete.
    assetRequest.totalActionQuantity || assetGeneration.totalActionQuantity
  )
  const agoText = `${formatDistanceToNow(new Date(createdAt))} ago`

  return (
    <div className={classes.row}>
      <StandardChip
        text={agoText}
        secondaryText={user
          ? `${user.givenName ?? ''} ${user.familyName ?? ''}`
          : t('asset_lab.unknown_user')}
      />
      <StandardChip
        text={formattedModel}
      />
      <StandardChip
        text={t('asset_lab.credits_other',
          {count: formattedCredits})}
        iconStroke='credits'
      />
      <SelectMenu
        id='asset-lab-view-details'
        menuWrapperClassName={combine(menuStyles.studioMenu, classes.contextMenu)}
        trigger={(
          <DetailViewButton
            onClick={() => {}}
            text={t('asset_lab.view_detail.label')}
          />
        )}
        placement='top-start'
        margin={5}
      >
        {() => (
          <div className={classes.mainText}>
            <ViewDetailRow
              fieldLabel={t('asset_lab.view_detail.model')}
              fieldValue={formattedModel}
            />
            <ViewDetailRow
              fieldLabel={t('asset_lab.view_detail.created')}
              // eslint-disable-next-line local-rules/hardcoded-copy
              fieldValue={format(new Date(createdAt), 'hh:mm aa MMM dd, yyyy')}
            />
            {aspectRatio && <ViewDetailRow
              fieldLabel={t('asset_lab.view_detail.resolution')}
              fieldValue={`(${aspectRatio}), ${width}x${height}`}
            />
            }
            {background && <ViewDetailRow
              fieldLabel={t('asset_lab.view_detail.background')}
              fieldValue={background as string}
            />}
            {/* NOTE(coco): We are assuming for now that quality
              is a string, but could change with new models */}
            {quality && typeof quality === 'string' &&
              <ViewDetailRow
                fieldLabel={t('asset_lab.view_detail.quality')}
                fieldValue={quality.charAt(0).toUpperCase() + quality.slice(1)}
              />
            }
            <ViewDetailRow
              fieldLabel={t('asset_lab.view_detail.cost')}
              fieldValue={t('asset_lab.credits_other', {count: formattedCredits})}
            />
          </div>
        )}
      </SelectMenu>
    </div>
  )
}

export {
  DetailViewLeftSnackbar,
}
