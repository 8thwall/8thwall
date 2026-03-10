load(":constraints.bzl", "HOST_CONSTRAINTS")

constraint_setting(
    name = "platform_setting",
)

constraint_value(
    name = "platform_constraint",
    constraint_setting = ":platform_setting",
    visibility = ["//visibility:public"],
)

platform(
    name = "host",
    visibility = ["//visibility:public"],
    constraint_values = HOST_CONSTRAINTS + [
        ":platform_constraint",
    ],
)
