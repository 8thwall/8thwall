licenses(["permissive"])  # zlib

cc_library(
    name = "bullet3",
    srcs = glob(
        include = [
            "src/*.cpp",
            "src/**/*.cpp",
            "src/*.h",
            "src/**/*.h",
            "src/*.hpp",
            "src/**/*.hpp",
        ],
        exclude = [
            "**/Bullet3OpenCL/**",
            "**/*All.cpp",
            "src/btBulletCollisionCommon.h",
            "src/btBulletDynamicsCommon.h",
        ],
    ),
    hdrs = [
        "src/btBulletCollisionCommon.h",
        "src/btBulletDynamicsCommon.h",
    ],
    copts = [
        "-Iexternal/bullet3/src",
        "-Iexternal/bullet3/src/Bullet3Collision/BroadPhaseCollision",
    ],
    includes = [
        "src",
    ],
    strip_include_prefix = "src",
    visibility = ["//visibility:public"],
)
