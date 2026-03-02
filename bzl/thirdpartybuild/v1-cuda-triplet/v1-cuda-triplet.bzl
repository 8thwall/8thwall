def _cuda_prebuilt_static_library(name, suffix, path_prefix, hdrs, includes):
    static_archive_name = "lib{}{}.a".format(name, suffix)
    archive_path_glob = "{}/{}".format(path_prefix, static_archive_name)
    static_srcs = native.glob([archive_path_glob])

    native.cc_library(
        name = name + suffix,
        srcs = static_srcs if static_srcs else [],
        hdrs = hdrs,
        includes = includes,
    )

def _cuda_prebuilt_shared_library(name, path_prefix, hdrs, includes):
    unversioned_so_name = "lib{}.so".format(name)
    so_basename_glob = "{}*".format(unversioned_so_name)
    so_path_glob = "{}/{}".format(path_prefix, so_basename_glob)

    native.cc_library(
        name = name,
        srcs = native.glob([
            so_path_glob,
        ]),
        hdrs = hdrs,
        includes = includes,
    )

def cuda_prebuilt_library(name, path_prefix, hdrs, includes):
    """Automatically wraps a cuda prebuilt libraries.

    For a "name" it will wrap libname.so.* under a :name bazel target
    if libname_static.a or libname_static_nocallback.a are available, those will be
    also wrapper respectively in :name_static and :name_static_nocallback bazel targets
    Args:
        name (str): name of the library
        path_prefix (path): prefix of the path where to search the library
        hdrs (list): headers for this library
        includes (list): include for this library
    """

    # Shared libraries
    _cuda_prebuilt_shared_library(name, path_prefix, hdrs, includes)

    # Static libraries
    _cuda_prebuilt_static_library(name, "_static", path_prefix, hdrs, includes)
    _cuda_prebuilt_static_library(name, "_static_nocallback", path_prefix, hdrs, includes)
