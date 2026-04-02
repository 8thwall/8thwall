import {describe, it, assert} from '@repo/bzl/js/chai-js'

import {sortStringList} from './sort-string-list'

describe('sortStringList', () => {
  it('sorts packages normally', () => {
    assert.deepEqual(
      sortStringList([
        '//a:b',
        '//b:a',
        '//c:c',
        '//a:c',
        '//a:a',
        '//b:b',
      ]), [
        '//a:a',
        '//a:b',
        '//a:c',
        '//b:a',
        '//b:b',
        '//c:c',
      ]
    )
  })

  it('puts local targets at the front', () => {
    assert.deepEqual(
      sortStringList([
        '//b:a',
        '//a:b',
        ':z',
        ':a',
      ]), [
        ':a',
        ':z',
        '//a:b',
        '//b:a',
      ]
    )
  })

  it('puts direct files even earlier', () => {
    assert.deepEqual(
      sortStringList([
        '//a:a',
        'z',
        ':a',
      ]), [
        'z',
        ':a',
        '//a:a',
      ]
    )
  })

  it('puts remote targets last', () => {
    assert.deepEqual(
      sortStringList([
        '//a:a',
        ':z',
        '@x//:y',
        '@x//:x',
        '@x//:z',
        '@a//:z',
        ':a',
      ]), [
        ':a',
        ':z',
        '//a:a',
        '@a//:z',
        '@x//:x',
        '@x//:y',
        '@x//:z',
      ]
    )
  })

  it('places substrings before extensions', () => {
    assert.deepEqual(
      sortStringList([
        '//a:a-c',
        '//a:a',
        '//a:a-b',
      ]), [
        '//a:a',
        '//a:a-b',
        '//a:a-c',
      ]
    )
  })

  it('Sorts dots before dashes', () => {
    assert.deepEqual(
      sortStringList([
        'file-2.png',
        'file.png',
      ]), [
        'file.png',
        'file-2.png',
      ]
    )
    assert.deepEqual(
      sortStringList([
        '"file-2.png"',
        '"file.png"',
      ]), [
        '"file.png"',
        '"file-2.png"',
      ]
    )
  })

  it('Sorts capital letters before lowercase', () => {
    assert.deepEqual(
      sortStringList([
        '//a:Z',
        '//a:a',
      ]), [
        '//a:Z',
        '//a:a',
      ]
    )
  })

  it('handles a real life example', () => {
    // From reality/cloud/aws/lambda/studio-api/BUILD with a couple extras thrown in
    const input = `
        "//reality/cloud/aws/lambda/studio-api:challenge",
        "//reality/cloud/aws/lambda/studio-api:db",
        "//reality/cloud/aws/lambda/studio-api/backend:backend-secrets",
        "//reality/cloud/aws/lambda/studio-api:event",
        "//reality/cloud/aws/lambda/studio-api:module-import",
        "@test2//:something",
        "//reality/cloud/aws/lambda/studio-api:list-repos",
        "//reality/cloud/aws/lambda/studio-api:module-deploy",
        "//reality/shared:dynamodb-impl",
        "//reality/shared/integration/lambda:lambda-impl",
        "//reality/cloud/aws/lambda/studio-api:repo",
        "@test-dep//:something",
        "//reality/cloud/aws/lambda/studio-api:sns",
        "@test//:something",
        ":local-rule",
        "//reality/cloud/aws/lambda/studio-api:webapp",
        "//reality/cloud/aws/lambda/studio-api/backend:backend-account",
        "//reality/cloud/aws/lambda/studio-api/integration:step-function",
        "//reality/cloud/aws/lambda/studio-api/integration:step-function-impl",
        "//reality/cloud/aws/lambda/studio-api/shared:sns-impl",
        "//reality/cloud/aws/lambda/studio-api:module-export",
        "//reality/cloud/aws/lambda/studio-api:module-target-handler",
        "//reality/cloud/aws/lambda/studio-api/shared:storage-api",
        "//reality/cloud/aws/lambda/studio-api/integration:lambda",
        "//reality/cloud/aws/lambda/studio-api:deploy",
        "//reality/cloud/aws/lambda/studio-api:codecommit-client-selector",
        "//reality/cloud/aws/lambda/studio-api:coded-error",
        "//reality/cloud/xrhome/src/shared:repo-id",
        "//reality/shared:dynamodb",
        "plain-file",
        "//reality/shared/module:module-target",
        "//reality/cloud/aws/lambda/studio-api:storage",
`.split('\n').map(e => e.trim().replace(/,/, '')).filter(Boolean)

    const expected = `
        "plain-file",
        ":local-rule",
        "//reality/cloud/aws/lambda/studio-api:challenge",
        "//reality/cloud/aws/lambda/studio-api:codecommit-client-selector",
        "//reality/cloud/aws/lambda/studio-api:coded-error",
        "//reality/cloud/aws/lambda/studio-api:db",
        "//reality/cloud/aws/lambda/studio-api:deploy",
        "//reality/cloud/aws/lambda/studio-api:event",
        "//reality/cloud/aws/lambda/studio-api:list-repos",
        "//reality/cloud/aws/lambda/studio-api:module-deploy",
        "//reality/cloud/aws/lambda/studio-api:module-export",
        "//reality/cloud/aws/lambda/studio-api:module-import",
        "//reality/cloud/aws/lambda/studio-api:module-target-handler",
        "//reality/cloud/aws/lambda/studio-api:repo",
        "//reality/cloud/aws/lambda/studio-api:sns",
        "//reality/cloud/aws/lambda/studio-api:storage",
        "//reality/cloud/aws/lambda/studio-api:webapp",
        "//reality/cloud/aws/lambda/studio-api/backend:backend-account",
        "//reality/cloud/aws/lambda/studio-api/backend:backend-secrets",
        "//reality/cloud/aws/lambda/studio-api/integration:lambda",
        "//reality/cloud/aws/lambda/studio-api/integration:step-function",
        "//reality/cloud/aws/lambda/studio-api/integration:step-function-impl",
        "//reality/cloud/aws/lambda/studio-api/shared:sns-impl",
        "//reality/cloud/aws/lambda/studio-api/shared:storage-api",
        "//reality/cloud/xrhome/src/shared:repo-id",
        "//reality/shared:dynamodb",
        "//reality/shared:dynamodb-impl",
        "//reality/shared/integration/lambda:lambda-impl",
        "//reality/shared/module:module-target",
        "@test-dep//:something",
        "@test//:something",
        "@test2//:something",
`.split('\n').map(e => e.trim().replace(/,/, '')).filter(Boolean)

    const actual = sortStringList(input)
    assert.deepEqual(actual, expected)
  })
})
