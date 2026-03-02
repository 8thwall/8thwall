# Add bazel support patch

This patch adds a build file to be used with bazel build system.

# Change sqlite name patch

This patch changes the name of the sqlite library to be used with bazel build system.
This patch is incompatible with the previous patch.

# Sqlite3 includes brackets patch

This patch replaces sqlite3 includes in <> with "" to be used with bazel build system.
This solves build issues on linux platforms without having to use unsafe
compiler flags.

# Build Projdb patch

This patch adds a genrule dependency that generates the required proj.db
sqlite3 file. This patch must be applied after "Sqlite3 includes brackets patch".