import {assert} from 'chai'
import path from 'path'

import {searchKeysInCodebase} from '../tools/i18n/remove-unused-i18n-strings'

describe('remove unused i18n strings', () => {
  it('finds translation keys referenced by supported source patterns', () => {
    const translationKeys = [
      'asset_lab.detail_view.download_assets',
      'asset_lab.drag_and_drop_image_prompt',
      'asset_lab.drag_and_drop_model_prompt',
      'asset_lab.model_optimizer.bottom',
      'asset_lab.model_optimizer.center',
      'asset_lab.model_optimizer.front',
      'asset_lab.model_optimizer.left',
      'asset_lab.model_optimizer.right',
      'asset_lab.model_optimizer.top',
      'reflection.preset.option.basic',
      'reflection.preset.option.cape_hill',
      'reflection.preset.option.cathedral',
      'reflection.preset.option.esplanade',
      'reflection.preset.option.golden_bay',
      'reflection.preset.option.hills',
      'reflection.preset.option.studio',
      'reflection.preset.option.winter_lake',
    ]

    const matchedKeys = searchKeysInCodebase(
      translationKeys, path.join(__dirname, '../src')
    )

    assert.deepEqual([...matchedKeys].sort(), [...translationKeys].sort())
  })
})
