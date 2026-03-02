load(":cpu-microarch-allowed_target.bzl", "ALLOWED_MARCH_TARGETS")

package(default_visibility = ["//visibility:public"])

# This files specify compatible micro architecture that are passed to the cc toolchain via -march= . This is a custom set of sometimes proprietary architecture that have been tested to be compatible with our current toolchain.

constraint_setting(
    name = "cpu-microarch-flavor-setting",
    default_constraint_value = ":not-specified",
)

# This constraint does specify to the compiler any specific target micro architecture. This is used as a default value.
constraint_value(
    name = "not-specified",
    constraint_setting = ":cpu-microarch-flavor-setting",
)

# Create constraint_value for each targeted micro archetecture.
[constraint_value(
    name = cpu_microarch,
    constraint_setting = ":cpu-microarch-flavor-setting",
) for cpu_microarch in ALLOWED_MARCH_TARGETS]
