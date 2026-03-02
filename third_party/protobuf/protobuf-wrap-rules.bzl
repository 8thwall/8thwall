load("@com_google_protobuf_3.0.0//:protobuf.bzl", cc_proto_library_old = "cc_proto_library",
                                                  csharp_proto_library_gen_old = "csharp_proto_library_gen")
load("@rules_proto//proto:defs.bzl", "proto_library")

# @rules_proto_grpc project - https://rules-proto-grpc.com/en/latest/index.html
load("@rules_proto_grpc//cpp:defs.bzl", "cc_proto_library")
load("@rules_proto_grpc//csharp:defs.bzl", "csharp_proto_compile")

PROTOBUF_VERSION = "default" # the version defined as com_protobuf_version in MODULE.bazel file (currently version 25.3-nia-v0.3)
# PROTOBUF_VERSION = '3.0.0'
PROTOBUF_LITE = False

# NOTE: The following rules should NOT override protoc and default_runtime attribute if you are using
# The builtin support for protobuf stuff (or possibly newer versions)

def cpp_proto_library_versionable(
        name,
        deps = [],
        protos = [],
        version = PROTOBUF_VERSION,
        visibility = None,
        linkstatic = False):
    """
    This function generates a versionable C++ protobuf library.

    Args:
        name (str): The name of the library.
        deps (list, optional): A list of dependencies for the library. Defaults to an empty list.
        protos (list, optional): A list of protobuf files that the library depends on. Defaults to an empty list.
        version (str, optional): The version of protobuf to use. Defaults to the global PROTOBUF_VERSION.
        visibility (list, optional): A list of labels that represent the visibility of the library. None means the library is public. Defaults to None.
        linkstatic (bool, optional): If True, the library will be linked statically. Defaults to False.
    """

    if version != "default":
        protoc = "@com_google_protobuf_" + version + "//:protoc"
        default_runtime = "@com_google_protobuf_" + version + "//:protobuf" + ("_lite" if PROTOBUF_LITE else "")

        # For normal non-lite protobuf we prefer dynlib by default
        cc_proto_library_old(
            name = name,
            deps = deps + protos,
            visibility = visibility,
            protoc = protoc,
            default_runtime = default_runtime,
            linkstatic = linkstatic,
        )

    else:
        cc_proto_library(
            name = name,
            protos = protos,
            deps = deps,
        )

def proto_library_versionable(
        name,
        srcs,
        deps = [],
        version = PROTOBUF_VERSION,
        visibility = None,
        linkstatic = False):
    """
    This function generates a versionable protobuf library.

    Args:
        name (str): The name of the library.
        srcs (list): A list of source files for the library.
        deps (list, optional): A list of dependencies for the library. Defaults to an empty list.
        version (str, optional): The version of protobuf to use. Defaults to the global PROTOBUF_VERSION.
        visibility (list, optional): A list of labels that represent the visibility of the library. None means the library is public. Defaults to None.
        linkstatic (bool, optional): If True, the library will be linked statically. Defaults to False.
    """

    # NOTE: protobuf.bzl of 3.0.0 does not define a working "proto_library(_old)"
    #       but the rules to generate are part of cc_proto_library(_old)

    # For normal non-lite protobuf we prefer dynlib by default

    if version != "default":
        protoc = "@com_google_protobuf_" + version + "//:protoc"
        default_runtime = "@com_google_protobuf_" + version + "//:protobuf" + ("_lite" if PROTOBUF_LITE else "")
        cc_proto_library_old(
            name = name,
            srcs = srcs,
            deps = deps,
            visibility = visibility,
            protoc = protoc,
            default_runtime = default_runtime,
            linkstatic = linkstatic,
        )
    else:
        proto_library(
            name = name,
            srcs = srcs,
            deps = deps,
            visibility = visibility,
        )

def csharp_proto_library_gen_versionable(
        name,
        srcs,
        deps = [],
        package = None,
        visibility = None):
    """
    This function generates a versionable C# protobuf library.

    Args:
        name (str): The name of the library.
        srcs (list): A list of source files for the library.
        deps (list, optional): A list of dependencies for the library. Defaults to an empty list.
        package (str, optional): The package name for the library. If None, the package name will be derived from the name. Defaults to None.
        visibility (list, optional): A list of labels that represent the visibility of the library. None means the library is public. Defaults to None.
    """
    # For normal non-lite protobuf we prefer dynlib by default
    
    if PROTOBUF_VERSION != "default":
        protoc = "@com_google_protobuf_" + PROTOBUF_VERSION + "//:protoc"
        csharp_proto_library_gen_old(
            name = name,
            package = package,
            srcs = srcs,
            deps = deps,
            visibility = visibility,
            protoc = protoc,
        )
    else:
        csharp_proto_compile(
            name = name,
            protos = deps,
        )

def proto_library_3_0_0(
        name,
        srcs,
        deps = [],
        visibility = None,
        linkstatic = False):
    protoc = "@com_google_protobuf_" + "3.0.0" + "//:protoc"
    default_runtime = "@com_google_protobuf_" + "3.0.0" + "//:protobuf" + ("_lite" if PROTOBUF_LITE else "")
    cc_proto_library_old(
        name = name,
        srcs = srcs,
        deps = deps,
        visibility = visibility,
        protoc = protoc,
        default_runtime = default_runtime,
        linkstatic = linkstatic,
    )

# NOTE: if you use the this rule the proto_library deps MUST be created with proto_library_3_0_0.
# The proto library created with proto_library_versionable, will not work if PROTOBUF_VERSION
# is not set specificaly to "3.0.0"
def csharp_proto_library_gen_3_0_0(
        name,
        srcs,
        deps = [],
        package = None,
        visibility = None):
    protoc = "@com_google_protobuf_" + "3.0.0" + "//:protoc"
    csharp_proto_library_gen_old(
        name = name,
        package = package,
        srcs = srcs,
        deps = deps,
        visibility = visibility,
        protoc = protoc,
    )
