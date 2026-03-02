licenses(["permissive"])  # MIT

# windows specific source files.
CPPRESTSDK_SRCS_WINDOWS = [
    "Release/src/http/client/http_client_winhttp.cpp",
    "Release/src/http/client/http_client_winrt.cpp",
    "Release/src/http/listener/http_server_httpsys.cpp",
    "Release/src/http/listener/http_server_httpsys.h",
    "Release/src/pplx/pplxwin.cpp",
    "Release/src/streams/fileio_win32.cpp",
    "Release/src/streams/fileio_winrt.cpp",
    "Release/src/websockets/client/ws_client_winrt.cpp",
]

# OSX specific source files.
CPPRESTSDK_SRCS_OSX = [
    "Release/src/pplx/pplxapple.cpp",
]

# Linux specific source files.
CPPRESTSDK_SRCS_LINUX = [
    "Release/src/pplx/pplxlinux.cpp",
]

cc_library(
    name = "cpprestsdk",
    srcs = glob(
        [
            "Release/src/**/*.cpp",
            "Release/src/**/*.h",
        ],
        exclude =
            # Exclude all platform specific file.
            # They are added back in the conditional after.
            CPPRESTSDK_SRCS_LINUX +
            CPPRESTSDK_SRCS_WINDOWS +
            CPPRESTSDK_SRCS_OSX,
    ) + select({
        "@the8thwall//bzl/conditions:osx": CPPRESTSDK_SRCS_OSX,
        "@the8thwall//bzl/conditions:linux": CPPRESTSDK_SRCS_LINUX,
        "@the8thwall//bzl/conditions:windows": CPPRESTSDK_SRCS_WINDOWS,
        "//conditions:default": ["@platforms//:incompatible"],
    }),
    hdrs =
        [
            "Release/include/cpprest/version.h",
            "Release/include/cpprest/json.h",
            "Release/include/cpprest/ws_msg.h",
            "Release/include/cpprest/ws_client.h",
            "Release/include/cpprest/asyncrt_utils.h",
            "Release/include/cpprest/base_uri.h",
            "Release/include/cpprest/details/web_utilities.h",
            "Release/include/cpprest/uri.h",
            "Release/include/cpprest/producerconsumerstream.h",
            "Release/include/cpprest/filestream.h",
            "Release/include/cpprest/details/http_helpers.h",
            "Release/include/cpprest/http_msg.h",
            "Release/include/cpprest/uri_builder.h",
            "Release/include/cpprest/rawptrstream.h",
            "Release/include/cpprest/containerstream.h",
            "Release/include/cpprest/http_compression.h",
            "Release/include/cpprest/http_headers.h",
            "Release/include/cpprest/http_listener.h",
            "Release/include/cpprest/streams.h",
            "Release/include/cpprest/astreambuf.h",
            "Release/include/cpprest/http_client.h",
            "Release/include/cpprest/details/http_constants.dat",
            "Release/include/cpprest/details/basic_types.h",
            "Release/include/cpprest/details/cpprest_compat.h",
            "Release/include/cpprest/details/nosal.h",
            "Release/include/cpprest/details/fileio.h",
            "Release/include/cpprest/details/SafeInt3.hpp",
            "Release/include/cpprest/details/http_server.h",
            "Release/include/cpprest/details/http_server_api.h",
            "Release/include/cpprest/oauth1.h",
            "Release/include/cpprest/oauth2.h",
            "Release/include/pplx/pplxtasks.h",
            "Release/include/pplx/pplx.h",
            "Release/include/pplx/pplxinterface.h",
            "Release/include/pplx/pplxcancellation_token.h",
            "Release/include/pplx/threadpool.h",
        ] + select({
            "@the8thwall//bzl/conditions:osx": [
                # The library does not comes with a pplxapple.h but uses __APPLE__ macro within the pplxlinux.h to do the right platform things.
                "Release/include/pplx/pplxlinux.h",
            ],
            "@the8thwall//bzl/conditions:linux": [
                "Release/include/pplx/pplxlinux.h",
            ],
            "@the8thwall//bzl/conditions:windows": [
                "Release/include/pplx/pplxwin.h",
            ],
            "//conditions:default": ["@platforms//:incompatible"],
        }),
    copts = [
        "-Iexternal/com_microsoft_cpprestsdk/Release/src/pch",
        # Disable http compression has it is pulling additional dependency.
        # This may be something that is not require outside of OSX.
        "-DCPPREST_EXCLUDE_WEBSOCKETS",
    ],
    includes = [
        "Release/include",
    ],
    linkopts = select({
        "@the8thwall//bzl/conditions:osx": ["-framework CoreFoundation"],
        "//conditions:default": [],
    }),
    linkstatic = 1,
    visibility = ["//visibility:public"],
    deps = [
        "@boost.asio",
        "@boost.bind",
        "@boost.system",
        "@boost.thread",
    ],
)

# Bing Request sample target
cc_binary(
    name = "BingRequest",
    srcs = ["Release/samples/BingRequest/bingrequest.cpp"],
    deps = [":cpprestsdk"],
)
