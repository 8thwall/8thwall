licenses(["permissive"])  # MIT

package(default_visibility = ["//visibility:public"])

genrule(
    name = "draco_features",
    outs = ["draco/draco_features.h"],
    cmd = """
      echo '// GENERATED FILE -- DO NOT EDIT' >> $@

      echo '#ifndef DRACO_FEATURES_H_' >> $@
      echo '#define DRACO_FEATURES_H_' >> $@

      echo '#define DRACO_POINT_CLOUD_COMPRESSION_SUPPORTED' >> $@
      echo '#define DRACO_MESH_COMPRESSION_SUPPORTED' >> $@
      echo '#define DRACO_NORMAL_ENCODING_SUPPORTED' >> $@
      echo '#define DRACO_STANDARD_EDGEBREAKER_SUPPORTED' >> $@
      echo '#define DRACO_PREDICTIVE_EDGEBREAKER_SUPPORTED' >> $@
      echo '#define DRACO_ATTRIBUTE_INDICES_DEDUPLICATION_SUPPORTED' >> $@
      echo '#define DRACO_ATTRIBUTE_VALUES_DEDUPLICATION_SUPPORTED' >> $@

      echo '#endif  // DRACO_FEATURES_H_' >> $@
    """,
)

cc_library(
    name = "draco",
    srcs = glob(
        # We are not including 'javascript/emscripten/', 'unity/' or 'maya/'
        [
            "src/draco/core/*.cc",
            "src/draco/animation/**/*.cc",
            "src/draco/attributes/**/*.cc",
            "src/draco/compression/**/*.cc",
            "src/draco/core/**/*.cc",
            "src/draco/io/**/*.cc",
            "src/draco/material/**/*.cc",
            "src/draco/mesh/**/*.cc",
            "src/draco/metadata/**/*.cc",
            "src/draco/point_cloud/**/*.cc",
            "src/draco/scene/**/*.cc",
            "src/draco/texture/**/*.cc",
        ],
        exclude = [
            # We will not include the test files.
            "src/draco/**/*_test*.cc",
        ],
    ) + [
        ":draco_features",
    ],
    hdrs = glob(
        [
            "src/draco/core/*.h",
            "src/draco/animation/**/*.h",
            "src/draco/attributes/**/*.h",
            "src/draco/compression/**/*.h",
            "src/draco/io/**/*.h",
            "src/draco/material/**/*.h",
            "src/draco/mesh/**/*.h",
            "src/draco/metadata/**/*.h",
            "src/draco/point_cloud/**/*.h",
            "src/draco/scene/**/*.h",
            "src/draco/texture/**/*.h",
        ],
        exclude = [
            "src/draco/**/*_test*.h",
        ],
    ),
    copts = [
        # Needed this one for loading in bunny.drc.
        "-DDRACO_BACKWARDS_COMPATIBILITY_SUPPORTED",
        # These are on by default in the cmake build on mac.
        "-DDRACO_MESH_COMPRESSION",
        "-DDRACO_POINT_CLOUD_COMPRESSION",
        "-DDRACO_PREDICTIVE_EDGEBREAKER",
        "-DDRACO_STANDARD_EDGEBREAKER",
    ],
    includes = ["src"],
    visibility = ["//visibility:public"],
)
