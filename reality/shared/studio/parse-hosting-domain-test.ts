// @attr(npm_rule = "@npm-lambda//:npm-lambda")
import {describe, it, assert} from 'bzl/js/chai-js'

import {parseHostingDomain} from './parse-hosting-domain'

describe('Parse Hosting Domain Test', () => {
  describe('For cmb-default-8w.dev.8thwall.app', () => {
    const result = parseHostingDomain('cmb-default-8w.dev.8thwall.app')!
    it('should correctly parse the stage', () => {
      assert.equal(result.stage, 'dev')
    })
    it('should correctly parse the branch', () => {
      assert.equal(result.branch, 'cmb-default')
    })
    it('should correctly parse the workspace', () => {
      assert.equal(result.workspace, '8w')
    })
    it('should have a null guestWorkspace', () => {
      assert.isNull(result.guestWorkspace)
    })
  })

  describe('For master-8w.dev.8thwall.app', () => {
    const result = parseHostingDomain('master-8w.dev.8thwall.app')!
    it('should correctly parse the stage', () => {
      assert.equal(result.stage, 'dev')
    })
    it('should correctly parse the branch', () => {
      assert.equal(result.branch, 'master')
    })
    it('should correctly parse the workspace', () => {
      assert.equal(result.workspace, '8w')
    })
    it('should have a null guestWorkspace', () => {
      assert.isNull(result.guestWorkspace)
    })
  })

  describe('For 8w.staging.8thwall.app', () => {
    const result = parseHostingDomain('8w.staging.8thwall.app')!
    it('should correctly parse the stage', () => {
      assert.equal(result.stage, 'staging')
    })
    it('should correctly parse the branch', () => {
      assert.equal(result.branch, 'staging')
    })
    it('should correctly parse the workspace', () => {
      assert.equal(result.workspace, '8w')
    })
    it('should have a null guestWorkspace', () => {
      assert.isNull(result.guestWorkspace)
    })
  })

  describe('For my-example-id-8w.build.8thwall.app', () => {
    const result = parseHostingDomain('my-example-id-8w.build.8thwall.app')!
    it('should correctly parse the stage', () => {
      assert.equal(result.stage, 'production')
    })
    it('should correctly parse the branch', () => {
      assert.equal(result.branch, 'production:build:my-example-id')
    })
    it('should correctly parse the workspace', () => {
      assert.equal(result.workspace, '8w')
    })
    it('should have a null guestWorkspace', () => {
      assert.isNull(result.guestWorkspace)
    })
  })

  describe('For 8w.8thwall.app', () => {
    const result = parseHostingDomain('8w.8thwall.app')!
    it('should correctly parse the stage', () => {
      assert.equal(result.stage, 'production')
    })
    it('should correctly parse the branch', () => {
      assert.equal(result.branch, 'production')
    })
    it('should correctly parse the workspace', () => {
      assert.equal(result.workspace, '8w')
    })
    it('should have a null guestWorkspace', () => {
      assert.isNull(result.guestWorkspace)
    })
  })

  describe('For user1-guestaccount-clientname-hostaccount.dev.8thwall.app', () => {
    const result = parseHostingDomain('user1-guestaccount-clientname-hostaccount.dev.8thwall.app')!
    it('should correctly parse the stage', () => {
      assert.equal(result.stage, 'dev')
    })
    it('should correctly parse the branch', () => {
      assert.equal(result.branch, 'user1-guestaccount-clientname')
    })
    it('should correctly parse the workspace', () => {
      assert.equal(result.workspace, 'hostaccount')
    })
    it('should correctly parse the guestWorkspace', () => {
      assert.equal(result.guestWorkspace, 'guestaccount')
    })
  })

  describe('For user1-guestaccount-client_name-hostaccount.dev.8thwall.app', () => {
    const result = parseHostingDomain('user1-guestaccount-client_name-hostaccount.dev.8thwall.app')!
    it('should correctly parse the stage', () => {
      assert.equal(result.stage, 'dev')
    })
    it('should correctly parse the branch name with bounded underscore', () => {
      assert.equal(result.branch, 'user1-guestaccount-client_name')
    })
    it('should correctly parse the workspace', () => {
      assert.equal(result.workspace, 'hostaccount')
    })
    it('should correctly parse the guestWorkspace', () => {
      assert.equal(result.guestWorkspace, 'guestaccount')
    })
  })

  describe('For user1-client_name-8w.dev.8thwall.app', () => {
    const result = parseHostingDomain('user1-client_name-8w.dev.8thwall.app')!
    it('should correctly parse the stage', () => {
      assert.equal(result.stage, 'dev')
    })
    it('should correctly parse the branch name with bounded underscore', () => {
      assert.equal(result.branch, 'user1-client_name')
    })
    it('should correctly parse the workspace', () => {
      assert.equal(result.workspace, '8w')
    })
    it('should have a null guestWorkspace', () => {
      assert.isNull(result.guestWorkspace)
    })
  })
})
