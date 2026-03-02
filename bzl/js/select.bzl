def onJavascript(items, default = []):
    return select({
        "@the8thwall//bzl/conditions:wasm": items,
        "//conditions:default": default,
    })
