# NOTE: We have not tested the build for every assimp format type in this
# bazelized version of assimp.
#
# For any issues building/using particular formats, please reach out to
# @peternguyen and @dknoblauch.
ALL_ASSIMP_FORMAT_TYPES = [
    "3D",
    "3DS",
    "3MF",
    "AC",
    "AMF",
    "ASE",
    "ASSBIN",
    "ASSJSON",
    "ASSXML",
    "B3D",
    "BLEND",
    "BVH",
    "C4D",  # Non-free
    "COB",
    "COLLADA",
    "CSM",
    "DXF",
    "FBX",
    "GLTF",
    "HMP",
    "IFC",
    "IQM",
    "IRR",
    "IRRMESH",
    "LWO",
    "LWS",
    "M3D",
    "MD2",
    "MD3",
    "MD5",
    "MDC",
    "MDL",
    "MMD",
    "MS3D",
    "NDO",
    "NFF",
    "OBJ",
    "OFF",
    "OGRE",
    "OPENGEX",
    "PBRT",
    "PLY",
    "Q3BSP",
    "Q3D",
    "RAW",
    "SIB",
    "SMD",
    "STEP",
    "STL",
    "TERRAGEN",
    "X",
    "X3D",
    "XGL",
]

def _gen_assimp_revision_file_impl(ctx):
    GIT_COMMIT_HASH_PATTERN = "@GIT_COMMIT_HASH@"
    GIT_BRANCH_PATTERN = "@GIT_BRANCH@"
    MAJOR_VERSION_PATTERN = "@ASSIMP_VERSION_MAJOR@"
    MINOR_VERSION_PATTERN = "@ASSIMP_VERSION_MINOR@"
    PATCH_VERSION_PATTERN = "@ASSIMP_VERSION_PATCH@"
    PACKAGE_VERSION_PATTERN = "@ASSIMP_PACKAGE_VERSION@"

    ctx.actions.expand_template(
        template = ctx.file.src,
        output = ctx.outputs.out,
        substitutions = {
            GIT_COMMIT_HASH_PATTERN: "0",
            GIT_BRANCH_PATTERN: "",
            MAJOR_VERSION_PATTERN: "0",
            MINOR_VERSION_PATTERN: "0",
            PATCH_VERSION_PATTERN: "0",
            PACKAGE_VERSION_PATTERN: "0",
        },
    )

_gen_assimp_revision_file = rule(
    implementation = _gen_assimp_revision_file_impl,
    attrs = {
        # https://github.com/assimp/assimp/blob/v5.2.2/revision.h.in
        "src": attr.label(allow_single_file = True),
        "out": attr.output(mandatory = True),
    },
)

def assimp_cc_library(name, formats, other_defines = []):
    [
        fail("Unrecognized assimp format specifier provided: '{}'".format(f))
        for f in formats
        if f not in ALL_ASSIMP_FORMAT_TYPES
    ]
    ASSIMP_COMPILE_DEFINES = [
        "RAPIDJSON_HAS_STDSTRING",
        "RAPIDJSON_NOMEMBERITERATORCLASS",
        "ASSIMP_BUILD_NO_OWN_ZLIB",
    ]
    for assimp_format in ALL_ASSIMP_FORMAT_TYPES:
        if assimp_format not in formats:
            ASSIMP_COMPILE_DEFINES.append("ASSIMP_BUILD_NO_{}_IMPORTER".format(assimp_format))
            ASSIMP_COMPILE_DEFINES.append("ASSIMP_BUILD_NO_{}_EXPORTER".format(assimp_format))

    native.genrule(
        name = "config",
        srcs = ["include/assimp/config.h.in"],
        outs = ["include/assimp/config.h"],
        cmd = "cat $(location include/assimp/config.h.in) >> $@",
    )
    native.cc_library(
        name = "config_hdrs",
        hdrs = [
            ":config",
        ],
    )

    _gen_assimp_revision_file(
        name = "assimp_revision",
        src = "revision.h.in",
        out = "revision.h",
    )

    native.cc_library(
        name = name,
        srcs = native.glob([
            "code/**/*.h",
            "code/**/*.hpp",
            "code/**/*.inl",
            "code/**/*.cpp",
            "contrib/Open3DGC/**/.cpp",
            "contrib/Open3DGC/**/.h",
            "contrib/Open3DGC/**/.inl",
            "contrib/clipper/*.cpp",
            "contrib/clipper/*.hpp",
            "contrib/openddlparser/**/*.cpp",
            "contrib/openddlparser/**/*.h",
            "contrib/poly2tri/**/*.cc",
            "contrib/poly2tri/**/*.h",
            "contrib/pugixml/src/*.hpp",
            "contrib/rapidjson/**/*.h",
            "contrib/rapidjson/*.h",
            "contrib/stb/stb_image.h",
            "contrib/unzip/*.c",
            "contrib/unzip/*.h",
            "contrib/utf8cpp/**/*.h",
        ]),
        hdrs = native.glob([
            "include/assimp/*.h",
            "include/assimp/*.inl",
            "include/assimp/*.hpp",
            "include/assimp/**/*.h",
        ]) + [":assimp_revision"],
        textual_hdrs = [
            "contrib/pugixml/src/pugixml.cpp",
        ],
        includes = [
            ".",
            "code",
            "include",
            "contrib",
            "contrib/unzip",
            "contrib/pugixml/src",
            "contrib/rapidjson/include",
            "contrib/openddlparser/include",
        ],
        deps = [
            "@zlib",
            ":config_hdrs",
        ],
        local_defines = ASSIMP_COMPILE_DEFINES,
        defines = other_defines,
    )
