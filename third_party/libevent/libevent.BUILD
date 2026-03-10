licenses(["permissive"])  # 3-clause BSD

package(
    default_visibility = ["//visibility:public"],
)

filegroup(
    name = "windows-srcs",
    srcs = [
        "WIN32-Code/getopt.c",
        "WIN32-Code/getopt.h",
        "WIN32-Code/getopt_long.c",
        "WIN32-Code/tree.h",
        "buffer_iocp.c",
        "bufferevent_async.c",
        "event_iocp.c",
        "evthread_win32.c",
        "win32select.c",
    ],
)

filegroup(
    name = "internal-headers",
    srcs = [
        "bufferevent-internal.h",
        "changelist-internal.h",
        "compat/sys/queue.h",
        "defer-internal.h",
        "epolltable-internal.h",
        "evbuffer-internal.h",
        "event-internal.h",
        "evmap-internal.h",
        "evrpc-internal.h",
        "evsignal-internal.h",
        "evthread-internal.h",
        "ht-internal.h",
        "http-internal.h",
        "iocp-internal.h",
        "ipv6-internal.h",
        "kqueue-internal.h",
        "log-internal.h",
        "minheap-internal.h",
        "mm-internal.h",
        "openssl-compat.h",
        "ratelim-internal.h",
        "strlcpy-internal.h",
        "time-internal.h",
        "util-internal.h",
    ],
)

filegroup(
    name = "core",
    srcs = [
        "buffer.c",
        "bufferevent.c",
        "bufferevent_filter.c",
        "bufferevent_pair.c",
        "bufferevent_ratelim.c",
        "bufferevent_sock.c",
        "event.c",
        "evmap.c",
        "evthread.c",
        "evutil.c",
        "evutil_rand.c",
        "evutil_time.c",
        "listener.c",
        "log.c",
    ],
)

filegroup(
    name = "impls",
    srcs = [
        "devpoll.c",
        "epoll.c",
        "evport.c",
        "kqueue.c",
        "poll.c",
        "select.c",
        "signal.c",
        "strlcpy.c",
    ],
)

filegroup(
    name = "extras",
    srcs = [
        "evdns.c",
        "event_tagging.c",
        "evrpc.c",
        "http.c",
    ],
)

filegroup(
    name = "public-headers",
    srcs = [
        "include/evdns.h",
        "include/event.h",
        "include/event2/buffer.h",
        "include/event2/buffer_compat.h",
        "include/event2/bufferevent.h",
        "include/event2/bufferevent_compat.h",
        "include/event2/bufferevent_ssl.h",
        "include/event2/bufferevent_struct.h",
        "include/event2/dns.h",
        "include/event2/dns_compat.h",
        "include/event2/dns_struct.h",
        "include/event2/event.h",
        "include/event2/event_compat.h",
        "include/event2/event_struct.h",
        "include/event2/http.h",
        "include/event2/http_compat.h",
        "include/event2/http_struct.h",
        "include/event2/keyvalq_struct.h",
        "include/event2/listener.h",
        "include/event2/rpc.h",
        "include/event2/rpc_compat.h",
        "include/event2/rpc_struct.h",
        "include/event2/tag.h",
        "include/event2/tag_compat.h",
        "include/event2/thread.h",
        "include/event2/util.h",
        "include/event2/visibility.h",
        "include/evhttp.h",
        "include/evrpc.h",
        "include/evutil.h",
    ],
)

cc_library(
    name = "event",
    srcs = [
        ":internal-headers",
        ":core",
        ":impls",
        ":extras",
    ] + select({
      "@the8thwall//bzl/conditions:windows": [":windows-srcs"],
      "//conditions:default": [],
    }),
    hdrs = [
        # Included headers.
        ":public-headers",
    ],
    copts = [
        "-Iexternal/libevent",
        "-Iexternal/libevent/compat",
        "-Iexternal/libevent/include",
        "-fno-strict-aliasing",
        "-Wno-unneeded-internal-declaration",
        "-Wno-implicit-function-declaration",
        "-Wno-unused-function",
        "-Wno-deprecated-declarations",
        "-Wno-extended-offsetof",
        "-Wno-gnu-redeclared-enum",
        "-Wno-format-pedantic",
        "-Wno-incompatible-pointer-types",
        "-Wno-empty-translation-unit",
        "-Wno-everything",  # Needed to silence comparison between pointer and integer.
    ],
    includes = ["include"],
    linkopts = select({
      "@the8thwall//bzl/conditions:windows": [
          "-lws2_32",
          "-lshell32",
          "-ladvapi32",
      ],
      "//conditions:default": [],
    }),
    deps = [
        "@the8thwall//third_party/libevent:platform-specific-headers",
    ] + select({
        "@the8thwall//bzl/conditions:apple": [],
        "//conditions:default": ["@the8thwall//third_party/libevent:arc4random"],
    }),
)
