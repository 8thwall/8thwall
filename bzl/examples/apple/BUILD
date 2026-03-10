load("//bzl/apple:lipo.bzl", "lipo_binary")

lipo_binary(
    name = "lipo-example",
    # Example creates a multi-arch binary with only 1 architecture. In
    # practical use, this rule is paired with 1:2+ transitions that package
    # multiple architectures together.
    binaries = [
        "//bzl/examples:hello",
    ],
    target_compatible_with = select({
        "//bzl/conditions:apple": [],
        "//conditions:default": ["@platforms//:incompatible"],
    }),
)
