import Database, {Database as SqliteDatabase} from 'better-sqlite3'
import path from 'path'
import {mkdirSync} from 'fs'
import {NODE_MODULES_PATH} from './resources'

const BETTER_SQLITE3_NODE_PATH = 'better-sqlite3/build/Release/better_sqlite3.node'

let db: SqliteDatabase

const initDb = (location: string) => {
  const dbPath = path.join(location, 'application-state.db')

  mkdirSync(location, {recursive: true})

  db = new Database(dbPath, {
    readonly: false,
    nativeBinding: path.resolve(NODE_MODULES_PATH, BETTER_SQLITE3_NODE_PATH),
  })

  db.exec(`
    CREATE TABLE IF NOT EXISTS projects (
      appKey TEXT PRIMARY KEY,
      location TEXT,
      initialization TEXT
    )
  `)

  try {
    // Add 'initialization' column exists for migration
    db.exec('ALTER TABLE projects ADD COLUMN initialization TEXT')
    db.exec('UPDATE projects SET initialization = \'done\' WHERE initialization IS NULL')
  } catch {
    // ignore, initialization column already exists
  }

  try {
    db.exec('ALTER TABLE projects ADD COLUMN accessedAt INTEGER')
  } catch {
    // ignore, accessedAt column already exists
  }

  db.exec(`
    CREATE TABLE IF NOT EXISTS preferences (
      key TEXT PRIMARY KEY,
      value TEXT
    )
  `)

  // delete projects without an appKey
  db.exec(`
    DELETE FROM projects WHERE appKey IS NULL OR appKey = '' or appKey = 'null'
  `)
}

const getDb = () => {
  if (!db) {
    throw new Error('Database not initialized')
  }
  return db
}

export {
  initDb,
  getDb,
}
