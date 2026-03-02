licenses(["permissive"])  # MIT

genrule(
    name = "lws_config_hdrs",
    srcs = ["@the8thwall//third_party/libwebsockets:lws_configs"],
    outs = [
        "lws_config.h",
        "lws_config_private.h",
    ],
    cmd = "cp $(SRCS) $(@D)/",
)

cc_library(
    name = "libwebsockets",
    srcs = glob([
               "lib/core/*.c",
               "lib/core/*.h",
               "lib/event-libs/*.h",
               "lib/event-libs/poll/*.c",
               "lib/event-libs/poll/*.h",
               "lib/system/*.c",
               # this header needed as referenced from lib/core/private-lib-core.h
               # regardless LWS_WITH_SYS_METRICS is on/off. maybe LWS bug?
               "lib/system/metrics/*.h",
           ]) + select({
               # lib/system/smd/ uses pthread and only works on unix-like platform
               "@platforms//os:windows": [],
               "//conditions:default": glob([
                   "lib/system/smd/*.c",
                   "lib/system/smd/*.h",
               ]),
           }) +
           # Core net
           glob(
               [
                   "lib/core-net/*.c",
                   "lib/core-net/*.h",
                   "lib/core-net/client/*.c",
                   "lib/core-net/client/*.h",
               ],
               exclude = [
                   "lib/core-net/socks5-client.c",  # LWS_WITH_SOCKS5
                   "lib/core-net/route.c",  # LWS_WITH_NETLINK
                   "lib/core-net/lws-dsh.c",  # LWS_WITH_LWS_DSH
                   "lib/core-net/sequencer.c",  # LWS_WITH_SEQUENCER
               ],
           ) +
           # Roles
           glob(
               [
                   "lib/roles/*.h",
                   "lib/roles/h1/*.h",
                   "lib/roles/h1/*.c",
                   "lib/roles/h2/*.h",
                   "lib/roles/h2/*.c",
                   "lib/roles/http/*.h",
                   "lib/roles/http/*.c",
                   "lib/roles/http/**/*.h",
                   "lib/roles/http/**/*.c",
                   "lib/roles/ws/*.h",
                   "lib/roles/ws/*.c",
                   "lib/roles/ws/**/*.h",
                   "lib/roles/ws/**/*.c",
                   "lib/roles/raw-skt/*.c",
                   "lib/roles/raw-file/*.c",
                   "lib/roles/pipe/*.c",
                   "lib/roles/listen/*.c",
               ],
               exclude = [
                   "lib/roles/ws/ext/extension.c",  # !LWS_WITHOUT_EXTENSIONS
                   "lib/roles/ws/ext/extension-permessage-deflate.c",  # !LWS_WITHOUT_EXTENSIONS
                   "lib/roles/http/server/ranges.c",  # LWS_WITH_RANGES
                   "lib/roles/http/server/access-log.c",  # LWS_WITH_ACCESS_LOG
                   "lib/roles/http/compression/deflate/deflate.h",  # !LWS_WITHOUT_EXTENSIONS
                   "lib/roles/http/compression/deflate/deflate.c",  # !LWS_WITHOUT_EXTENSIONS
                   "lib/roles/http/compression/stream.c",  # LWS_WITH_HTTP_STREAM_COMPRESSION
                   "lib/roles/http/compression/brotli/brotli.c",  # LWS_WITH_HTTP_STREAM_COMPRESSION
                   "lib/roles/h2/minihuf.c",
                   "lib/roles/http/minilex.c",
               ],
           ) +
           # TLS
           glob(
               [
                   "lib/tls/*.c",
                   "lib/tls/*.h",
                   "lib/tls/openssl/*.c",
                   "lib/tls/openssl/*.h",
               ],
               exclude = [
                   "lib/tls/tls-jit-trust.c",
                   # Exclude followings as LWS_WITH_GENCRYPTO is not enabled
                   "lib/tls/openssl/lws-genhash.c",
                   "lib/tls/openssl/lws-genrsa.c",
                   "lib/tls/openssl/lws-genaes.c",
                   "lib/tls/lws-genec-common.c",
                   "lib/tls/openssl/lws-genec.c",
                   "lib/tls/openssl/lws-gencrypto.c",
               ],
           ) +
           # Platform
           select({
               "@platforms//os:windows": [
                   "lib/plat/windows/private-lib-plat-windows.h",
                   "lib/plat/windows/windows-fds.c",
                   "lib/plat/windows/windows-file.c",
                   "lib/plat/windows/windows-init.c",
                   "lib/plat/windows/windows-misc.c",
                   "lib/plat/windows/windows-pipe.c",
                   "lib/plat/windows/windows-resolv.c",
                   "lib/plat/windows/windows-service.c",
                   "lib/plat/windows/windows-sockets.c",
               ],
               "//conditions:default": [
                   "lib/plat/unix/private-lib-plat-unix.h",
                   "lib/plat/unix/unix-caps.c",
                   "lib/plat/unix/unix-misc.c",
                   "lib/plat/unix/unix-file.c",
                   "lib/plat/unix/unix-init.c",
                   "lib/plat/unix/unix-pipe.c",
                   "lib/plat/unix/unix-service.c",
                   "lib/plat/unix/unix-sockets.c",
                   "lib/plat/unix/unix-fds.c",
                   "lib/plat/unix/unix-resolv.c",
                   # TODO: android build fine w/o this. is this needed??
                   #plat/unix/android/android-resolv.c
               ],
           }) +
           # Misc
           [
               "lib/misc/base64-decode.c",
               "lib/misc/prng.c",
               "lib/misc/lws-ring.c",
               "lib/misc/dir.c",
               "lib/misc/sha-1.c",
               "lib/misc/cache-ttl/file.c",
               "lib/misc/cache-ttl/heap.c",
               "lib/misc/cache-ttl/lws-cache-ttl.c",
               "lib/misc/cache-ttl/private-lib-misc-cache-ttl.h",
           ] +
           # Android specific sources
           select({
               "@the8thwall//bzl/conditions:android": [
                   "lib/misc/getifaddrs.c",
                   "lib/misc/getifaddrs.h",
               ],
               "//conditions:default": [],
           }) +
           # Windows specific sources
           select({
               "@platforms//os:windows": [
                   "win32port/dirent/dirent-win32.h",
                   "win32port/win32helpers/getopt_long.c",
                   "win32port/win32helpers/getopt.c",
                   "win32port/win32helpers/getopt.h",
                   "win32port/win32helpers/gettimeofday.c",
                   "win32port/win32helpers/gettimeofday.h",
               ],
               "//conditions:default": [],
           }) + [
        ":lws_config_hdrs",
    ],
    hdrs = glob([
        "include/*.h",
        "include/libwebsockets/*.h",
        "include/libwebsockets/**/*.h",
    ]),
    copts = [
        # TODO: Revisit if/how we can avoid -I options
        "-Iexternal/libwebsockets/include",
        "-Iexternal/libwebsockets/lib/core",
        "-Iexternal/libwebsockets/lib/core-net",
        "-Iexternal/libwebsockets/lib/tls",
        "-Iexternal/libwebsockets/lib/event-libs",
        "-Iexternal/libwebsockets/lib/system/metrics",
        "-Iexternal/libwebsockets/lib/roles",
        "-Iexternal/libwebsockets/lib/roles/h1",
        "-Iexternal/libwebsockets/lib/roles/h2",
        "-Iexternal/libwebsockets/lib/roles/http",
        "-Iexternal/libwebsockets/lib/roles/ws",
        "-Iexternal/libwebsockets/lib/misc/cache-ttl",
        "-Iexternal/libwebsockets/lib",
    ] +
    # Platform dependent options
    select({
        "@the8thwall//bzl/conditions:windows": [
            "-Iexternal/libwebsockets/lib/plat/windows",
            "-Iexternal/libwebsockets/win32port/dirent",
            "-Iexternal/libwebsockets/win32port/win32helpers",
        ],
        "//conditions:default": [
            "-Iexternal/libwebsockets/lib/system/smd",
            "-Iexternal/libwebsockets/lib/plat/unix",
        ],
    }),
    includes = [
        "include",
    ],
    linkopts = select({
        "@the8thwall//bzl/conditions:windows": [
            "-lws2_32",
            "-luserenv",
            "-lpsapi",
            "-liphlpapi",
            "-lcrypt32",
        ],
        "//conditions:default": [],
    }),
    visibility = ["//visibility:public"],
    deps = [
        "@boringssl//:crypto",
        "@boringssl//:ssl",
        "@zlib",
    ],
)
