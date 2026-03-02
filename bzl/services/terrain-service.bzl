"""
This module provides support for the terrain service verifying the presense of tools in the system PATH if the platform host is supported
and generating BUILD files with constraints and a .bzl file with the paths of the tools found
"""

def monitor_tools_in_path(repository_ctx, list_of_tools_to_monitor):
    """Monitor a list of Tool in the system PATH

    Args:
        repository_ctx (_type_): _description_
        list_of_tools_to_monitor (_type_): _description_
    """

    platform_os = repository_ctx.os.name.lower()

    # monitoring system paths tools for changes
    system_paths = repository_ctx.getenv("PATH")
    path_separator = "/"
    if not "win" in platform_os and not "nt" in platform_os:
        system_paths = system_paths.split(":")
    else:
        system_paths = system_paths.split(";")
        path_separator = "\\"

    for system_path in system_paths:
        for tool in list_of_tools_to_monitor:
            repository_ctx.watch(system_path + path_separator + tool)

def _terrain_service_support_impl(repository_ctx):
    
    # Monitoring if any of this tools shows up between builds
    monitor_tools_in_path(repository_ctx, ["pdal", "projinfo", "laszip", "laszip64"])
    
    # Check if 'pdal' exists in the system PATH
    pdal_path = repository_ctx.which("pdal")
    projinfo_path = repository_ctx.which("projinfo")
    laszip_path = repository_ctx.which("laszip")
    
    # Acconding to the documentation on 64 bit architecture the laszip executable is named laszip64
    # Which was confirmed trying to compile locally, so in that case we also check if laszip64 is in path
    if laszip_path == None:
        platform_cpu = repository_ctx.os.arch.lower()
        if "64" in platform_cpu:
            laszip_path = repository_ctx.which("laszip64")
    
    pdal_build_default_constraint_value = ":has_pdal" if pdal_path != None else ":has_not_pdal"
    projinfo_build_default_constraint_value = ":has_projinfo" if projinfo_path != None else ":has_not_projinfo"
    laszip_build_default_constraint_value = ":has_laszip" if laszip_path != None else ":has_not_laszip"

    pdal_bin_paths_str = "PDAL_BIN_HOST_PATH=\"%s\"" % (pdal_path if pdal_path != None else "")
    projinfo_bin_paths_str = "PROJINFO_BIN_HOST_PATH=\"%s\"" % (projinfo_path if projinfo_path != None else "")
    laszip_bin_paths_str = "LASZIP_BIN_HOST_PATH=\"%s\"" % (laszip_path if laszip_path != None else "")


    # Generate BUILD file with constraints
    repository_ctx.file(        
        "BUILD.bazel",
        """
load("@bazel_skylib//lib:selects.bzl", "selects")

constraint_setting(
    name = "pdal_available",
    default_constraint_value = "{pdal_default}",
)

constraint_setting(
    name = "projinfo_available",
    default_constraint_value = "{projinfo_default}",
)

constraint_setting(
    name = "laszip_available",
    default_constraint_value = "{laszip_default}",
)

constraint_value(
    name = "has_pdal",
    constraint_setting = ":pdal_available",   
    visibility = ["//visibility:public"],
)

constraint_value(
    name = "has_not_pdal",
    constraint_setting = ":pdal_available",
    visibility = ["//visibility:public"],
)

constraint_value(
    name = "has_projinfo",
    constraint_setting = ":projinfo_available",
    visibility = ["//visibility:public"],
)

constraint_value(
    name = "has_not_projinfo",
    constraint_setting = ":projinfo_available",
    visibility = ["//visibility:public"],
)

constraint_value(
    name = "has_laszip",
    constraint_setting = ":laszip_available",
    visibility = ["//visibility:public"],
)

constraint_value(
    name = "has_not_laszip",
    constraint_setting = ":laszip_available",
    visibility = ["//visibility:public"],
)

selects.config_setting_group(
    name = "linux_platform_supported",
    match_all = [
        "@platforms//cpu:x86_64",
        "@platforms//os:linux",
    ],
)

selects.config_setting_group(
    name = "macos_platform_supported",
    match_all = [
        "@platforms//os:osx",
    ],
)

selects.config_setting_group(
    name = "platform_supported",
    match_any = [
        ":linux_platform_supported",
        ":macos_platform_supported",
    ],
    visibility = ["//visibility:public"],
)

selects.config_setting_group(
    name = "all_tools_supported",
    match_all = [
        ":has_pdal",
        ":has_projinfo",
        ":has_laszip",
    ],
    visibility = ["//visibility:public"],
)

selects.config_setting_group(
    name = "fully_supported",
    match_all = [
        ":platform_supported",
        ":all_tools_supported",
    ],
    visibility = ["//visibility:public"],
)

        """.format(
            pdal_default = pdal_build_default_constraint_value,
            projinfo_default = projinfo_build_default_constraint_value,
            laszip_default = laszip_build_default_constraint_value,
        ),
        executable = False,
    )

    # Generate a .bzl file with the paths of the tools found - path will be empty if not found
    repository_ctx.file(
        
        "tools_paths_found.bzl",
        """
{pdal_bin_paths}
{projinfo_bin_paths}
{laszip_bin_paths}
        """.format(
            pdal_bin_paths = pdal_bin_paths_str,
            projinfo_bin_paths = projinfo_bin_paths_str,
            laszip_bin_paths = laszip_bin_paths_str,
        ),
        executable = False,
    )
        
    pdal_path = pdal_path if pdal_path != None else "/dummy/for_watch/pdal_not_found"
    projinfo_path = projinfo_path if projinfo_path != None else "/dummy/for_watch/projinfo_not_found"
    laszip_path = laszip_path if laszip_path != None else "/dummy/for_watch/laszip_not_found"

    repository_ctx.watch(pdal_path)
    repository_ctx.watch(projinfo_path)
    repository_ctx.watch(laszip_path)


terrain_service_support = repository_rule(
    implementation = _terrain_service_support_impl,
    attrs = {},
    doc = "A repository rule that checks for the some tool and sets a constraint if available", 
)