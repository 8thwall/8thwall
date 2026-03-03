licenses(["permissive"])  # Apache 2.0

load("@the8thwall//bzl/windows:select.bzl", "onWindows")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "mdnssd",
    srcs = [
        "mDNSShared/CommonServices.h",
        "mDNSShared/DebugServices.h",
        "mDNSShared/dnssd_clientlib.c",
        "mDNSShared/dnssd_clientstub.c",
        "mDNSShared/dnssd_ipc.c",
        "mDNSShared/dnssd_ipc.h",
    ] + onWindows(["mDNSWindows/DLL/dllmain.c"]),
    hdrs = glob([
        "mDNSShared/dns_sd.h",
    ]),
    copts = [
        "-Iexternal/androidmdnsresponder/mDNSCore",
        "-Iexternal/androidmdnsresponder/mDNSWindows",
        "-DHAVE_IPV6",
        "-DNOT_HAVE_SA_LEN",
        "-DPLATFORM_NO_RLIMIT",
        "-DTARGET_OS_WIN32",
        "-DUSE_TCP_LOOPBACK",  # Maybe needed?
        "-DMDNS_DEBUGMSGS=0",
        "-D_WINSOCKAPI_",
        "-D_WINSOCK_DEPRECATED_NO_WARNINGS",
        "-fno-strict-aliasing",
        "-fwrapv",
        "-W",
        "-Wall",
        "-Wextra",
        "-Wno-array-bounds",
        "-Wno-pointer-sign",
        "-Wno-unused",
        "-Wno-unused-parameter",
        "-Werror=implicit-function-declaration",
        "-Wno-pedantic",
    ],
    includes = ["mDNSShared"],
    linkopts = onWindows(["-lWs2_32"]),
    visibility = ["//visibility:public"],
    deps = [],
)
