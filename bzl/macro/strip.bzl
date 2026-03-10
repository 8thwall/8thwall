def strip_if_requested(input_plugins):
    return {
        "//bzl/macro:strip-always": [plugin + ".stripped" for plugin in input_plugins],
        "//conditions:default": input_plugins,
    }

def strip_if_requested_single(input_plugin):
    return {
        "//bzl/macro:strip-always": input_plugin + ".stripped",
        "//conditions:default": input_plugin,
    }
