"""
Module for handling node source transition logic in Bazel.
"""

# buildifier: disable=unused-variable
def _node_source_transition_impl(settings, attr):
    # NOTE(lreyna): This is a temporary workaround since building node from source for linux
    # is not supported yet.
    if attr.generator_function == "docker_image" or attr.generator_function == "node_package":
        return {}
    else:
        return {"//bzl/node:source-built": True}

node_source_transition = transition(
    implementation = _node_source_transition_impl,
    inputs = [],
    outputs = ["//bzl/node:source-built"],
)
