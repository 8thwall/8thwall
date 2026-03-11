import {chai} from 'bzl/js/chai-js'

import {getArgs, checkArgErrs} from './args'

const {describe, it} = globalThis as any

const {expect} = chai

describe('getArgs', () => {
  it('should parse auth8 args', () => {
    process.argv = ['node', 'auth8', '--help', '--dataRealm=dev']
    expect(getArgs()).to.deep.equal(
      {_node: 'node', _script: 'auth8', _ordered: [], dataRealm: 'dev', help: true}
    )
  })
  it('should parse dataset-sync args', () => {
    process.argv = [
      'node',
      'dataset-sync',
      '--help',
      '--dry-run',
      '--dataset=selfies',
      '--direction=down',
      '--local=/Users/me/datasets',
    ]
    expect(getArgs()).to.deep.equal({
      '_node': 'node',
      '_script': 'dataset-sync',
      '_ordered': [],
      'dataset': 'selfies',
      'direction': 'down',
      'dry-run': true,
      'help': true,
      'local': '/Users/me/datasets',
    })
  })
})

describe('checkArgErrs', () => {
  const datasetSyncSpec = {
    optionalFlags: ['direction', 'dataset', 'local', 'dry-run'],
    requiredFlags: ['dataset', 'direction', 'local'],
    optionsForFlag: {
      direction: ['up', 'down'],
      dataset: [
        'cylindrical-image-targets',
        'flat-image-targets',
        'low-motion',
        'newfeatures',
        'selfies',
      ],
    },
    maxOrdered: 0,
    help: 'help string',
  }
  it('should validate dataset-sync args', () => {
    process.argv = [
      'node',
      'dataset-sync',
      '--help',
      '--dry-run',
      '--dataset=selfies',
      '--direction=down',
      '--local=/Users/me/datasets',
    ]
    expect(checkArgErrs(datasetSyncSpec, getArgs())).to.deep.equal([])
  })
  it('should validate dataset-sync missing optional', () => {
    process.argv = [
      'node',
      'dataset-sync',
      '--dataset=selfies',
      '--direction=down',
      '--local=/Users/me/datasets',
    ]
    expect(checkArgErrs(datasetSyncSpec, getArgs())).to.deep.equal([])
  })
  it('should error for invalid dataset-sync dataset', () => {
    process.argv = [
      'node',
      'dataset-sync',
      '--help',
      '--dry-run',
      '--dataset=foo',
      '--direction=down',
      '--local=/Users/me/datasets',
    ]
    expect(checkArgErrs(datasetSyncSpec, getArgs())).to.deep.equal(['Unexpected dataset: foo'])
  })
  it('should error for missing required', () => {
    process.argv = [
      'node',
      'dataset-sync',
      '--help',
      '--dry-run',
      '--local=/Users/me/datasets',
    ]
    expect(checkArgErrs(datasetSyncSpec, getArgs()))
      .to.deep.equal(['Missing required arguments: dataset, direction'])
  })
  it('should error for too many ordered params', () => {
    process.argv = [
      'node',
      'dataset-sync',
      '--help',
      'foo',
      '--dry-run',
      '--dataset=selfies',
      '--direction=down',
      'bar',
      '--local=/Users/me/datasets',
    ]
    expect(checkArgErrs(datasetSyncSpec, getArgs()))
      .to.deep.equal(['Too many command line options: foo, bar'])
  })
  it('should accept flags from either optionalFlags or requiredFlags', () => {
    process.argv = [
      'node',
      'script',
      '--flag1=value',
      '--flag2=value',
    ]
    expect(checkArgErrs({
      optionalFlags: ['flag1'],
      requiredFlags: ['flag2'],
    }, getArgs())).to.deep.equal([])
  })
  it('should only validate flags if optionalFlags or requiredFlags is provided', () => {
    process.argv = [
      'node',
      'script',
      '--flag1=value',
    ]
    expect(checkArgErrs({}, getArgs())).to.deep.equal([])

    expect(checkArgErrs({optionalFlags: []}, getArgs()))
      .to.deep.equal(['Unexpected command line option: flag1'])

    expect(checkArgErrs({requiredFlags: []}, getArgs()))
      .to.deep.equal(['Unexpected command line option: flag1'])
  })
  it('should only check ordered if limits are provided', () => {
    process.argv = [
      'node',
      'script',
      'file1',
      'file2',
    ]
    expect(checkArgErrs({}, getArgs())).to.deep.equal([])

    expect(checkArgErrs({maxOrdered: 1}, getArgs()))
      .to.deep.equal(['Too many command line options: file1, file2'])

    expect(checkArgErrs({minOrdered: 3}, getArgs()))
      .to.deep.equal(['Not enough command line options: file1, file2'])
  })
  it('should only check optionsForFlag if the flag is provided', () => {
    process.argv = [
      'node',
      'script',
      '--flag1=baz',
    ]
    expect(checkArgErrs({
      optionalFlags: ['flag1', 'flag2'],
      optionsForFlag: {
        flag1: ['foo', 'bar'],
        flag2: ['1', '2'],
      },
    }, getArgs())).to.deep.equal(['Unexpected flag1: baz'])
  })
})
