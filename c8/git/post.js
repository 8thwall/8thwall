/* eslint-disable camelcase,no-bitwise */
/* global FS */
const chmod_original = FS.chmod

// Ensure that read/write permission in always given. This is necessary when we reconcile the
// in-memory fs with indexedDB for files which libgit2 marks as read-only, namely
// .git/objects/* files.
FS.chmod = (path, mode, dontFollow) => {
  const modeWithRwx = mode | 0o644
  return chmod_original(path, modeWithRwx, dontFollow)
}

const storeLocalEntry_original = FS.filesystems.IDBFS.storeLocalEntry

// Fix this to ensure that permissions are updated on existing files before writing to them.
// This is necessary because we were missing a chmod path that always set full permissions
// for files in .git directory.
FS.filesystems.IDBFS.storeLocalEntry = (path, entry, callback) => {
  try {
    // Give user write access to the file if it exists.
    FS.chmod(path, entry.mode)
  } catch (err) {
    // Doesn't exist, this is okay.
  }

  storeLocalEntry_original(path, entry, callback)
}
