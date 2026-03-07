#!/usr/bin/env node

/* eslint-disable no-console */

//
// Usage:
// $ ./deploy.js
//
// Script to build and upload a rebuilt version of g8 which can be deployed to the team through
// `brew upgrade outdated`. This script will create a new versioned file, where the version string
// is the base-32 encoded build timestamp. After uploading a new version to s3, the homebrew-tools8
//  config needs to be updated to point to the new version.
//
// TODO(tony): Write a description of the process here.
const fs = require('fs')
const {promisify} = require('util')
const exec = promisify(require('child_process').exec)

/* eslint-disable import/no-unresolved */
const readline = require('readline/promises')
/* eslint-enable import/no-unresolved */

// TODO(pawel) This requires the following patches (as of february 10 2023)
// eslint-disable-next-line max-len
// https://gitlab.<REMOVED_BEFORE_OPEN_SOURCING>.com/repo/<REMOVED_BEFORE_OPEN_SOURCING>/-/merge_requests/2664
// eslint-disable-next-line max-len
// https://gitlab.<REMOVED_BEFORE_OPEN_SOURCING>.com/repo/<REMOVED_BEFORE_OPEN_SOURCING>/-/merge_requests/2539
const buildInliner = true

// Utility to find the bazel workspace root as a parent of the current directory by recursive
// search.
const findWorkspace = (dirname) => {
  if (!dirname) {
    throw new Error('Couldn\'t find WORKSPACE root.')
  }
  const contents = fs.readdirSync(dirname)
  if (contents.find(v => v === 'WORKSPACE')) {
    return dirname
  }
  const path = dirname.split('/')
  const parent = path.slice(0, path.length - 1).join('/')
  return findWorkspace(parent)
}

// Infer the next version of g8 from by querying brew.
const inferNextVersion = async () => {
  const brewInfo = await exec(
    'HOMEBREW_NO_AUTO_UPDATE=1 HOMEBREW_NO_INSTALL_CLEANUP=1 brew info g8'
  )
  if (brewInfo.stderr) {
    console.warn(brewInfo.stderr)
  }
  if (brewInfo.error) {
    console.warn(`Warning: Failed to get current version from brew info: ${brewInfo.error.message}`)
    return null
  }

  const currentVersion = brewInfo.stdout.match(/stable (\d+\.\d+\.\d+)/)?.[1]

  if (!currentVersion) {
    console.warn('Warning: Failed to parse current version from brew info')
    return null
  }

  console.log(`Current g8 version = ${currentVersion}`)

  const incrementVersion =
    semver => semver.replace(/(\d+)$/, (match, patch) => parseInt(patch, 10) + 1)

  return incrementVersion(currentVersion)
}

// Prompt the user to choose the next version of g8.
const chooseVersion = async () => {
  let nextVersion = (await inferNextVersion()) || '0.0.0'

  const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout,
  })

  try {
    const userVersion = await rl.question(`Select next g8 version [${nextVersion}]: `)
    const trimmed = userVersion.trim()
    const isValidVersion = version => /^\d+\.\d+\.\d+$/.test(version)
    if (trimmed !== '') {
      if (isValidVersion(trimmed)) {
        nextVersion = trimmed
      } else {
        console.error(`${JSON.stringify(trimmed)} is an invalid version`)
        nextVersion = null
      }
    }
  } catch (err) {
    console.log('Error: ', err)
  } finally {
    rl.close()
  }
  return nextVersion
}

// Async function to drive the script.
const runDeploy = async () => {
  const workspace = findWorkspace(__dirname)

  const nextVersion = await chooseVersion()

  if (!nextVersion) {
    return
  }

  const token = Date.now().toString(36)
  console.log(`Collecting build artifacts for build ${token}`)
  const tmpDir = `/tmp/g8-deploy/${token}`
  await exec(`mkdir -p ${tmpDir}`)
  const tmpDir2 = `/tmp/g8-inliner/${token}`
  await exec(`mkdir -p ${tmpDir2}`)

  // Build a new version of g8.
  console.log('Building g8.')
  // eslint-disable-next-line max-len
  await exec('bazel build //apps/g8:g8 -c opt --platforms=//bzl:osx_universal ' +
    '--copt=-mmacos-version-min=11.0 --linkopt=-mmacos-version-min=11.0 ' +
    `--//apps/g8:version=${nextVersion}`)
  await exec(`cp ${workspace}/bazel-bin/apps/g8/g8 ${tmpDir}`)

  console.log('Building auth8.')
  // eslint-disable-next-line max-len
  await exec('bazel build //apps/client/auth:auth8 -c opt --platforms=//bzl:osx_universal ' +
    '--copt=-mmacos-version-min=11.0 --linkopt=-mmacos-version-min=11.0')
  await exec(`cp ${workspace}/bazel-bin/apps/client/auth/auth8 ${tmpDir}`)

  if (buildInliner) {
    console.log('Downloading inliner.')
    // Old info on how to build inliner
    // NOTE(datchu): To be revived at a later time
    // await exec('bazel build //apps/client/inliner:parse-build-rules-sh ' +
    //   '--platforms=//bzl:osx_universal')
    // await exec(`cp ${workspace}/bazel-bin/apps/client/inliner/parse-build-rules-sh ${tmpDir}`)

    // await exec('bazel build //apps/client/inliner:inliner')
    // await exec(`cp ${workspace}/bazel-bin/apps/client/inliner/parse-build-rules ${tmpDir}`)
    // await exec(`cp -R -L ${workspace}/bazel-bin/apps/client/inliner/inliner.runfiles/` +
    //   `niantic/apps/client/inliner/inliner.py ${tmpDir}`)
    // await exec(`chmod +x ${tmpDir}/inliner.py`)
    await exec('curl -s -L https://8w-mdm-dist.s3.us-west-2.amazonaws.com/g8/g8-ljxhhh7u.tar.gz |' +
      ` tar xz - -C ${tmpDir2}`)
    await exec(`cp ${tmpDir2}/tmp/g8-deploy/` +
      `ljxhhh7u/{inliner.py,parse-build-rules,parse-build-rules-sh} ${tmpDir}`)
  }

  await exec(`cp ${workspace}/apps/g8/_g8 ${tmpDir}`)

  await exec(`open ${tmpDir}`)

  // Create a new uniquely timestamped version of the file, compress it, and move the compressed
  // file to tmp.
  console.log('Compressing')
  const tmpFileG8 = `g8-${token}`
  await exec(`tar -czvf ${tmpFileG8}.tar.gz ${tmpDir}`)
  await exec(`mv ${tmpFileG8}.tar.gz /tmp/${tmpFileG8}.tar.gz`)

  // Upload the file to s3.
  console.log('Uploading to s3')
  const BUCKET = '8w-mdm-dist'
  const KEY = `g8/${tmpFileG8}.tar.gz`
  await exec(`aws s3 cp /tmp/${tmpFileG8}.tar.gz s3://${BUCKET}/${KEY}`)
  await exec(`aws s3api put-object-acl --bucket ${BUCKET} --key ${KEY} --acl public-read`)

  // Done.
  console.log(`Uploaded ${BUCKET}/${KEY}`)

  // Print out sha256 hash.
  await exec(`openssl dgst -sha256 < /tmp/${tmpFileG8}.tar.gz`, (error, stdout, stderr) => {
    if (error) {
      console.log(`Error: ${error.message}`)
      return
    }
    if (stderr) {
      console.log(`${stderr}`)
      return
    }
    console.log(`version = ${nextVersion}`)
    console.log(`token = ${token}`)
    console.log(`sha256 = ${stdout}`)
  })
}

runDeploy()
