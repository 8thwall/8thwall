def _wasm_platforms_transition_impl(settings, attr):
    return {"//command_line_option:platforms": settings["//bzl/wasm:wasm-platforms"]}

wasm_platforms_transition = transition(
    implementation = _wasm_platforms_transition_impl,
    inputs = ["//bzl/wasm:wasm-platforms"],
    outputs = ["//command_line_option:platforms"],
)
