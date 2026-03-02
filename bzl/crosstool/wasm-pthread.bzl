def emoptsAllowMemoryGrowth():
    return select({
        # non-wasm thread runs slower if we allow memory growth in pthread
        "@the8thwall//bzl/conditions:wasm-pthread": ["ALLOW_MEMORY_GROWTH=0"],
        "//conditions:default": ["ALLOW_MEMORY_GROWTH=1"],
    })
