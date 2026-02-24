def onJavascript(items, default = []):
    return select({
        "@niantic//bzl/conditions:wasm": items,
        "//conditions:default": default,
    })
