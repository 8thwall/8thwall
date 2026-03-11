import React from 'react'
import type {StudioComponentError} from '@ecs/shared/studio-component'
import {createUseStyles} from 'react-jss'
import {Link} from 'react-router-dom'

import {
  deriveEditorRouteParams, deriveLocationFromKey, extractFilePath, type EditorFileLocation,
} from '../editor/editor-file-location'
import {useStudioComponentsContext} from './studio-components-context'
import {StaticBanner} from '../ui/components/banner'
import {blueberry} from '../static/styles/settings'
import {useAppPathsContext} from '../common/app-container-context'
import {getHashForFile} from '../common/paths'

// TODO: Finish translating
/* eslint-disable local-rules/hardcoded-copy */

const useStyles = createUseStyles({
  errorButton: {
    border: 'none',
    background: 'none',
    cursor: 'pointer',
    color: blueberry,
  },
  errorText: {
    textWrap: 'nowrap',
  },
})

interface IErrorMessageLink {
  firstErrorLocation: EditorFileLocation
  firstError: StudioComponentError
}

const ErrorMessageLink: React.FC<IErrorMessageLink> = ({firstErrorLocation, firstError}) => {
  const classes = useStyles()

  const {getFileRoute} = useAppPathsContext()

  if (!firstError.location) {
    return null
  }

  const getFileRouteWithHash = (
    location: EditorFileLocation, line: number, column: number
  ) => getFileRoute(deriveEditorRouteParams(location)) +
  getHashForFile(line, column)

  return (
    <Link to={getFileRouteWithHash(firstErrorLocation,
      firstError.location.startLine,
      firstError.location.startColumn + 1)}
    >
      <span className={classes.errorText}>
        {`[Ln ${firstError.location.startColumn}, Col ${firstError.location.startLine}]`}
      </span>
    </Link>
  )
}

const ErrorMessage: React.FC = () => {
  const {errors} = useStudioComponentsContext()

  let firstErrorLocation: EditorFileLocation
  let firstError: StudioComponentError
  const numErrors = errors && Object.keys(errors).reduce((acc, key) => {
    if (errors[key].length && !firstError) {
      firstErrorLocation = deriveLocationFromKey(key)
      ;[firstError] = errors[key]
    }
    return acc + errors[key].length
  }, 0)

  if (!numErrors) {
    return null
  }

  return (
    <StaticBanner type='danger'>
      {`A total of ${numErrors} error${numErrors > 1 ? 's' : ''}`}
      <br />
      {`(${extractFilePath(firstErrorLocation)}): `}
      <br />
      {firstError.message}
      {numErrors > 1 && <><br />{`${numErrors - 1} more...`}</>}
      <ErrorMessageLink
        firstErrorLocation={firstErrorLocation}
        firstError={firstError}
      />
    </StaticBanner>
  )
}

export {ErrorMessage}
