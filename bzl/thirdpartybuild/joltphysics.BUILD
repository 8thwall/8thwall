licenses(["permissive"])  # MIT

cc_library(
    name = "joltphysics",
    srcs = glob([
        # Core engine
        "Jolt/*.cpp",
        "Jolt/Core/*.cpp",
        "Jolt/Math/*.cpp",
        "Jolt/Geometry/*.cpp",
        "Jolt/ObjectStream/*.cpp",

        # Utility modules
        "Jolt/AABBTree/*.cpp",
        "Jolt/Skeleton/*.cpp",
        "Jolt/TriangleSplitter/*.cpp",

        # Physics engine
        "Jolt/Physics/*.cpp",
        "Jolt/Physics/Body/*.cpp",
        "Jolt/Physics/Character/*.cpp",
        "Jolt/Physics/Collision/*.cpp",
        "Jolt/Physics/Collision/BroadPhase/*.cpp",
        "Jolt/Physics/Collision/Shape/*.cpp",
        "Jolt/Physics/Constraints/*.cpp",
        "Jolt/Physics/Ragdoll/*.cpp",
        "Jolt/Physics/SoftBody/*.cpp",
        "Jolt/Physics/Vehicle/*.cpp",
    ]),
    hdrs = glob([
        # Core headers
        "Jolt/*.h",
        "Jolt/Core/*.h",
        "Jolt/Core/*.inl",
        "Jolt/Math/*.h",
        "Jolt/Math/*.inl",
        "Jolt/Geometry/*.h",
        "Jolt/ObjectStream/*.h",

        # Utility headers
        "Jolt/AABBTree/*.h",
        "Jolt/AABBTree/*/*.h",
        "Jolt/Skeleton/*.h",
        "Jolt/TriangleSplitter/*.h",

        # Physics headers
        "Jolt/Physics/*.h",
        "Jolt/Physics/Body/*.h",
        "Jolt/Physics/Body/*.inl",
        "Jolt/Physics/Character/*.h",
        "Jolt/Physics/Collision/*.h",
        "Jolt/Physics/Collision/BroadPhase/*.h",
        "Jolt/Physics/Collision/Shape/*.h",
        "Jolt/Physics/Constraints/*.h",
        "Jolt/Physics/Constraints/ConstraintPart/*.h",
        "Jolt/Physics/Ragdoll/*.h",
        "Jolt/Physics/SoftBody/*.h",
        "Jolt/Physics/Vehicle/*.h",
    ]),
    copts = [
        "-Iexternal/joltphysics",
    ],
    includes = [
        ".",
    ],
    strip_include_prefix = "Jolt",
    visibility = ["//visibility:public"],
)
