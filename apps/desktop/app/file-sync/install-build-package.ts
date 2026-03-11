import log from 'electron-log'

import {makeCodedError} from '../../errors'
import {runInstallCommand} from './run-commands'

const installBuildPackage = async (savePath: string): Promise<void> => {
  try {
    await runInstallCommand(savePath)
  } catch (error) {
    log.error('error installing build package', error)
    throw makeCodedError('Error installing build package', 500)
  }
}

export {installBuildPackage}
