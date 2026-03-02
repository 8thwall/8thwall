def _exec_transition_impl(ctx):
    return [ctx.attr.actual[DefaultInfo]]

# Convert transition a target dependency to a exec dependency.
exec_transition = rule(
    implementation = _exec_transition_impl,
    attrs = {
        "actual": attr.label(cfg = "exec"),
    },
)
