import React from 'react'
import type {Meta} from '@storybook/react'
import {createUseStyles} from 'react-jss'

import {Trans, useTranslation} from 'react-i18next'

import {Tag, ITag} from '../components/tag'
import {SpaceBetween} from '../layout/space-between'

const useStyles = createUseStyles({
  grid: {
    display: 'grid',
    gridTemplateColumns: 'repeat(auto-fill, minmax(calc(var(--grid-size) * 1px), 1fr))',
    gridGap: '2em',
    justifyItems: 'center',
  },
})

const TAG_VARIANTS: Array<ITag['height']> = ['medium', 'small', 'tiny']

type TagShowCaseProps = {size: string}

const TagShowCase: React.FC<TagShowCaseProps> = ({size}) => {
  const {t} = useTranslation(['studio-desktop-pages'])
  const classes = useStyles()
  return (
    <SpaceBetween direction='vertical'>
      <h2>Normal</h2>

      {/* eslint-disable-next-line no-constant-binary-expression */}
      {false &&
        <>
          {t('project_list_item.menu.option.reveal_finder')}
          <Trans i18nKey='button.learn_more' />
          <Trans ns='common' i18nKey='button.learn_more' />
          <Trans ns='studio-desktop-pages' i18nKey='project_list_item.menu.option.reveal_finder' />

          {/* @ts-expect-error */}
          {t('button.learn_more', {ns: 'common'})}
          {/* @ts-expect-error */}
          {t('project_list_item.menu.option.reveal_finder', {ns: 'common'})}
          {/* @ts-expect-error */}
          {t('studio-desktop-pages:project_list_item.menu.option.reveal_finder')}
          {/* @ts-expect-error */}
          <Trans i18nKey='common:button.learn_more' />
          {/* @ts-expect-error */}
          <Trans i18nKey='project_list_item.menu.option.reveal_finder' />
          <Trans
            ns='studio-desktop-pages'
            // @ts-expect-error
            i18nKey='studio-desktop-pages:project_list_item.menu.option.reveal_finder'
          />
          <Trans
            // @ts-expect-error
            ns='studio-desktop-pages2 '
            i18nKey='studio-desktop-pages:project_list_item.menu.option.reveal_finder'
          />
        </>}
      <div className={classes.grid} style={{'--grid-size': size} as any}>
        {TAG_VARIANTS.map(variant => (
          <Tag key={variant} height={variant}>
            {variant}
          </Tag>
        ))}
      </div>
    </SpaceBetween>
  )
}

export default {
  title: 'Views/TagShowCase',
  render: TagShowCase,
  argTypes: {
    size: {
      control: 'select',
      options: ['64', '160', '400'],
    },
  },
} as Meta<typeof TagShowCase>

export const All = {
  args: {
    badgeSize: '160',
    badgeHeight: 'tiny',
    badgeSpacing: 'normal',
  },
}
