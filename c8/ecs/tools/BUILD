load("//bzl/js:js.bzl", "js_cli", "js_library", "js_test")

js_cli(
    name = "fix-duplication",
    esnext = True,
    main = [
        "fix-duplication.ts",
    ],
)

js_cli(
    name = "generate-asm-types",
    main = [
        "generate-asm-types.ts",
    ],
    deps = [
        "//bzl/js/codegen:generate-ts-from-cpp",
    ],
)

genrule(
    name = "asm-types-generated",
    srcs = [
        "//c8/ecs:ecs-js.cc",
    ],
    outs = ["asm-types.ts"],
    cmd = "$(location :generate-asm-types) $(SRCS) > $@",
    tools = ["generate-asm-types"],
    visibility = ["//c8/ecs/gen:__pkg__"],
)

js_cli(
    name = "generate-ecs-definition",
    main = [
        "generate-ecs-definition.ts",
    ],
)

genrule(
    name = "ecs-definition-generated",
    srcs = [
        "//c8/ecs/src/runtime:test-runtime-types",
    ],
    outs = ["ecs-definition-generated.ts"],
    cmd = "$(location :generate-ecs-definition) $(SRCS) > $@",
    tools = [":generate-ecs-definition"],
    visibility = ["//c8/ecs/gen:__pkg__"],
)

genrule(
    name = "generated-features-list-gen",
    outs = ["generated-features-list.ts"],
    cmd = "$(location :generate-features-list) > $@",
    tools = ["generate-features-list"],
)

js_library(
    name = "generated-features-list",
    srcs = [
        ":generated-features-list-gen",
    ],
    deps = [
        "//c8/ecs/src/shared/features",
    ],
)

js_cli(
    name = "generate-features-list",
    data = [
        "//c8/ecs/src/shared/features:features-files",
    ],
    main = [
        "generate-features-list.ts",
    ],
)

js_cli(
    name = "generate-metadata",
    data = [
        "//c8/ecs/src/runtime:animation.ts",
        "//c8/ecs/src/runtime:fly-controller.ts",
        "//c8/ecs/src/runtime:orbit-controls.ts",
        "//c8/ecs/src/runtime:particles.ts",
        "//c8/ecs/src/runtime/xr:xr-face-components.ts",
    ],
    esnext = 1,
    main = [
        "generate-metadata.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//c8/ecs/src/shared:parse-component-ast",
        "//c8/ecs/src/shared:runtime-version",
        "//c8/ecs/src/shared/features:edition",
        "//c8/ecs/tools:generated-features-list",
    ],
)

sh_binary(
    name = "finalize-bundle",
    srcs = ["finalize-bundle.sh"],
    data = [
        "//c8/ecs:bundle-without-metadata",
        "//c8/ecs/tools:generate-metadata",
    ],
)

js_test(
    name = "features-test",
    size = "small",
    main = [
        "features-test.ts",
    ],
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/shared/features:edition",
        "//c8/ecs/tools:generated-features-list",
    ],
)
