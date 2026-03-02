def onWindows(items, default = []):
    return select({
        "@the8thwall//bzl/conditions:windows": items,
        "//conditions:default": default,
    })
