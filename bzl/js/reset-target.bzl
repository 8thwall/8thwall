def _reset_js_target_transition(settings, attr):
    return {
        "//bzl/js:target": "default",
    }

reset_js_target_transition = transition(
    implementation = _reset_js_target_transition,
    inputs = [],
    outputs = [
       "//bzl/js:target"
    ],
)

def _reset_js_target_impl(ctx):
    return [DefaultInfo(files = depset(ctx.files.deps))]

reset_js_target = rule(
    implementation = _reset_js_target_impl,
    attrs = {
        "deps": attr.label_list(
            cfg = reset_js_target_transition,
        ),
    },
)
