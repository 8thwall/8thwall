import type {Project} from '@repo/reality/shared/desktop/local-sync-types'

import {getDb} from './application-state'
/* eslint quotes: ["error", "single", { "allowTemplateLiterals": true }] */

const upsertLocalProject = (
  appKey: string, location: string, initialization: Project['initialization']
) => {
  const db = getDb()
  db.prepare(`\
INSERT OR REPLACE INTO projects (appKey, location, initialization, accessedAt) \
VALUES (?, ?, ?, ?)
`)
    .run(appKey, location, initialization, Date.now())
}

const deleteLocalProject = (appKey: string) => {
  const db = getDb()
  db.prepare('DELETE FROM projects WHERE appKey = ?').run(appKey)
}

const getLocalProject = (appKey: string): Project | undefined => {
  const db = getDb()
  return db.prepare<[string], Project>('SELECT * FROM projects WHERE appKey = ?').get(appKey)
}

const getLocalProjects = (): Project[] => {
  const db = getDb()
  return db.prepare<[], Project>('SELECT * FROM projects').all()
}

const bumpProjectAccessedAt = (appKey: string) => {
  const db = getDb()
  db.prepare('UPDATE projects SET accessedAt = ? WHERE appKey = ?').run(Date.now(), appKey)
}

const getLocalProjectByLocation = (location: string): Project | undefined => {
  const db = getDb()
  return db.prepare<[string], Project>('SELECT * FROM projects WHERE location = ?').get(location)
}

export {
  upsertLocalProject,
  deleteLocalProject,
  getLocalProject,
  getLocalProjects,
  bumpProjectAccessedAt,
  getLocalProjectByLocation,
}
