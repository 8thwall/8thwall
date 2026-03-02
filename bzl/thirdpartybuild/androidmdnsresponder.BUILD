licenses(["permissive"])  # Apache 2.0

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "mdnssd",
    srcs = [
        "mDNSShared/dnssd_clientlib.c",
        "mDNSShared/dnssd_clientstub.c",
        "mDNSShared/dnssd_ipc.c",
        "mDNSShared/dnssd_ipc.h",
    ],
    hdrs = glob([
        "mDNSShared/dns_sd.h",
    ]),
    copts = [
        "-Iexternal/androidmdnsresponder/mDNSCore",
        "-Iexternal/androidmdnsresponder/mDNSPosix",
        "-D_GNU_SOURCE",
        "-DHAVE_IPV6",
        "-DHAVE_LINUX",
        "-DNOT_HAVE_SA_LEN",
        "-DPLATFORM_NO_RLIMIT",
        "-DTARGET_OS_LINUX",
        "-DUSES_NETLINK",
        "-DMDNS_DEBUGMSGS=0",
        "-DMDNS_UDS_SERVERPATH=\\\"/dev/socket/mdnsd\\\"",
        "-DMDNS_USERNAME=\\\"mdnsr\\\"",
        "-fno-strict-aliasing",
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
    linkopts = [
        "-llog",
    ],
    visibility = ["//visibility:public"],
    deps = [
    ],
)
