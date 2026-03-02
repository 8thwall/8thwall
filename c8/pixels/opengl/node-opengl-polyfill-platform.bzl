_POLYFILL_PLATFORM_LABEL = "//c8/pixels/opengl:node-opengl-polyfill-platform"

def _node_opengl_polyfill_transition_impl(settings, _):
    platform = settings.get(_POLYFILL_PLATFORM_LABEL)

    if not platform:
        fail("Could not find {}".format(_POLYFILL_PLATFORM_LABEL))

    return {
        "//command_line_option:platforms": [platform],
        "//command_line_option:linkopt": [],
        "//command_line_option:copt": [],
    }

_node_opengl_polyfill_transition = transition(
    implementation = _node_opengl_polyfill_transition_impl,
    inputs = [_POLYFILL_PLATFORM_LABEL],
    outputs = ["//command_line_option:platforms", "//command_line_option:linkopt", "//command_line_option:copt"],
)

def _node_opengl_polyfill_impl(ctx):
    return [ctx.attr.actual[DefaultInfo]]

# Transition to the node opengl polyfill platform.
node_opengl_polyfill = rule(
    implementation = _node_opengl_polyfill_impl,
    cfg = _node_opengl_polyfill_transition,
    attrs = {
        "actual": attr.label(
            mandatory = True,
        ),
    },
)
