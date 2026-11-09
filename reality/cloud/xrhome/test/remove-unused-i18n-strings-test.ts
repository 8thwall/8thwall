import {assert} from 'chai'
import path from 'path'

import {
  findPotentialTranslationKeys,
  searchKeysInCodebase,
} from '../tools/i18n/remove-unused-i18n-strings'

describe('remove unused i18n strings', () => {
  it('only treats dynamic translation calls as translation key families', () => {
    const source = [
      'const {t: translate} = useTranslation([\'cloud-editor-pages\'])',
      `const platformKey = \`editor_page.publish.\${platform}\``,
      `const unrelated = \`home_page.\${section}\``,
      'translate(platformKey)',
      'translate(\'editor_page.\' + \'split_key\')',
      `translate(\`editor_page.mode.\${enabled ? 'on' : 'off'}\`)`,
      'i18n.t(useShort ? \'common.short\' : \'common.long\')',
      `const helper = (t: TFunction) => t(\`asset_lab.family.\${kind}\`)`,
      `const component = <Trans i18nKey={\`common.trans.\${variant}\`} />`,
    ].join('\n')

    const references = findPotentialTranslationKeys(source, 'source.tsx')

    assert.deepEqual([...references.keys].sort(), [
      'common.long',
      'common.short',
      'editor_page.mode.off',
      'editor_page.mode.on',
      'editor_page.split_key',
    ])
    assert.deepEqual([...references.prefixes].sort(), [
      'asset_lab.family.',
      'common.trans.',
      'editor_page.publish.',
    ])
    assert.notInclude([...references.prefixes], 'home_page.')
  })

  it('preserves translation keys reached through typed mappings and dynamic families', () => {
    const protectedKeys = [
      'asset_lab.model.flux_1_schnell.description',
      'asset_lab.model.flux_1_schnell.name',
      'asset_lab.model.flux_pro_1_1.description',
      'asset_lab.model.flux_pro_1_1.name',
      'asset_lab.model.flux_pro_kontext.description',
      'asset_lab.model.flux_pro_kontext.name',
      'asset_lab.model.gpt_image_1.description',
      'asset_lab.model.gpt_image_1.name',
      'asset_lab.model.hunyuan_3d_v2.description',
      'asset_lab.model.hunyuan_3d_v2.name',
      'asset_lab.model.hunyuan_3d_v2_1.description',
      'asset_lab.model.hunyuan_3d_v2_1.name',
      'asset_lab.model.hunyuan_3d_v2_mini.description',
      'asset_lab.model.hunyuan_3d_v2_mini.name',
      'asset_lab.model.meshy_ai.description',
      'asset_lab.model.meshy_ai.name',
      'asset_lab.model.stable_image_core.description',
      'asset_lab.model.stable_image_core.name',
      'asset_lab.model.trellis.description',
      'asset_lab.model.trellis.name',
      'diff_chip.added',
      'diff_chip.changed',
      'diff_chip.removed',
      'diff_chip.unchanged',
      'editor_page.export_modal.create_certificate_signing_request_description_with_p12',
      'editor_page.export_modal.provisioning_profile.provisioned_devices',
      'editor_page.export_modal.provisioning_profile.entitlements',
      'editor_page.native_publish_modal.publish_headline_configure.android',
      'editor_page.native_publish_modal.publish_headline_configure.ios',
      'editor_page.native_publish_modal.publish_headline_first.android',
      'editor_page.native_publish_modal.publish_headline_first.ios',
      'reflection.preset.option.basic',
      'reflection.preset.option.cape_hill',
      'reflection.preset.option.cathedral',
      'reflection.preset.option.esplanade',
      'reflection.preset.option.golden_bay',
      'reflection.preset.option.hills',
      'reflection.preset.option.studio',
      'reflection.preset.option.winter_lake',
      'tooltip.audio.text',
    ]

    const matchedKeys = searchKeysInCodebase(
      protectedKeys, [
        path.join(__dirname, '../src'),
        path.join(__dirname, '../../../../c8/ecs/src'),
      ]
    )

    assert.deepEqual([...matchedKeys].sort(), [...protectedKeys].sort())
  })
})
