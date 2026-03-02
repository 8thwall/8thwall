licenses(["permissive"])  # Modified BSD

cc_library(
    name = "cblas-hdrs",
    hdrs = [
        "BLAS/WRAP/cblas.h",
    ],
    includes = [
        "BLAS/WRAP",
    ],
    visibility = ["//visibility:private"],
)

cc_library(
    name = "clapack-common-hdrs",
    hdrs = [
        "INCLUDE/blaswrap.h",
        "INCLUDE/f2c.h",
    ],
    includes = [
        "INCLUDE",
    ],
    visibility = ["//visibility:private"],
)

cc_library(
    name = "f2c",
    srcs = [
        "F2CLIBS/libf2c/c_abs.c",
        "F2CLIBS/libf2c/c_div.c",
        "F2CLIBS/libf2c/cabs.c",
        "F2CLIBS/libf2c/close.c",
        "F2CLIBS/libf2c/d_cnjg.c",
        "F2CLIBS/libf2c/d_imag.c",
        "F2CLIBS/libf2c/d_sign.c",
        "F2CLIBS/libf2c/endfile.c",
        "F2CLIBS/libf2c/err.c",
        "F2CLIBS/libf2c/exit_.c",
        "F2CLIBS/libf2c/f77_aloc.c",
        "F2CLIBS/libf2c/fio.h",
        "F2CLIBS/libf2c/fmt.h",
        "F2CLIBS/libf2c/i_nint.c",
        "F2CLIBS/libf2c/open.c",
        "F2CLIBS/libf2c/pow_dd.c",
        "F2CLIBS/libf2c/pow_ii.c",
        "F2CLIBS/libf2c/pow_ri.c",
        "F2CLIBS/libf2c/r_cnjg.c",
        "F2CLIBS/libf2c/r_imag.c",
        "F2CLIBS/libf2c/r_lg10.c",
        "F2CLIBS/libf2c/r_sign.c",
        "F2CLIBS/libf2c/s_cat.c",
        "F2CLIBS/libf2c/s_cmp.c",
        "F2CLIBS/libf2c/s_copy.c",
        "F2CLIBS/libf2c/sig_die.c",
        "F2CLIBS/libf2c/sysdep1.h",
        "F2CLIBS/libf2c/util.c",
        "F2CLIBS/libf2c/z_abs.c",
        "F2CLIBS/libf2c/z_div.c",
    ],
    copts = [
        "-Wno-format",
        "-Wno-logical-op-parentheses",
        "-Wno-parentheses",
        "-Wno-shift-op-parentheses",
        "-Wno-sometimes-uninitialized",
        "-Wno-implicit-function-declaration",
        "-Wno-unused-variable",
        "-Wno-unused-but-set-variable",
    ] + select({
        "@the8thwall//bzl/conditions:windows": [
            "-DNO_TRUNCATE",
            "-DMSDOS",
        ],
        "//conditions:default": [],
    }),
    deps = [
        ":clapack-common-hdrs",
    ],
)

cc_library(
    name = "cblas",
    srcs = glob([
        "BLAS/SRC/*.c",
    ]) + [
        "BLAS/WRAP/cblasimpl.c",
        "BLAS/WRAP/cblaswr.c",
        "BLAS/WRAP/fblaswr.h",
    ],
    copts = [
        "-Wno-absolute-value",
        "-Wno-format",
        "-Wno-incompatible-pointer-types-discards-qualifiers",
        "-Wno-logical-op-parentheses",
        "-Wno-unused-but-set-variable",
    ] + select({
        "@the8thwall//bzl/conditions:windows": [
            "-DNO_TRUNCATE",
            "-DMSDOS",
        ],
        "//conditions:default": [],
    }),
    # Keep visibility limited to //c8:clapack to prevent leaking f2c.h stuff.
    visibility = ["//c8:__pkg__"],
    deps = [
        ":cblas-hdrs",
        ":clapack-common-hdrs",
        ":f2c",
        "//c8:c8-log",
    ],
)

cc_library(
    name = "clapack-hdrs",
    hdrs = [
        "INCLUDE/clapack.h",
    ],
    includes = [
        "INCLUDE",
    ],
    visibility = ["//visibility:private"],
)

# Add the subset of clapack methods that we use.
cc_library(
    name = "clapack",
    srcs = [
        "INSTALL/slamch.c",
        "SRC/ieeeck.c",
        "SRC/ilaenv.c",
        "SRC/iparmq.c",
        "SRC/sbdsdc.c",
        "SRC/sbdsqr.c",
        "SRC/sgebd2.c",
        "SRC/sgebrd.c",
        "SRC/sgelq2.c",
        "SRC/sgelqf.c",
        "SRC/sgeqr2.c",
        "SRC/sgeqrf.c",
        "SRC/sgesdd.c",
        "SRC/sgetc2.c",
        "SRC/sgetf2.c",
        "SRC/sgetrf.c",
        "SRC/sgetri.c",
        "SRC/sgetrs.c",
        "SRC/slabad.c",
        "SRC/slabrd.c",
        "SRC/slacpy.c",
        "SRC/slaed5.c",
        "SRC/slaed6.c",
        "SRC/slamrg.c",
        "SRC/slange.c",
        "SRC/slanst.c",
        "SRC/slapy2.c",
        "SRC/slarf.c",
        "SRC/slarfb.c",
        "SRC/slarfg.c",
        "SRC/slarft.c",
        "SRC/slartg.c",
        "SRC/slas2.c",
        "SRC/slascl.c",
        "SRC/slasd0.c",
        "SRC/slasd1.c",
        "SRC/slasd2.c",
        "SRC/slasd3.c",
        "SRC/slasd4.c",
        "SRC/slasd5.c",
        "SRC/slasd6.c",
        "SRC/slasd7.c",
        "SRC/slasd8.c",
        "SRC/slasda.c",
        "SRC/slasdq.c",
        "SRC/slasdt.c",
        "SRC/slaset.c",
        "SRC/slasq1.c",
        "SRC/slasq2.c",
        "SRC/slasq5.c",
        "SRC/slasq6.c",
        "SRC/slasr.c",
        "SRC/slasrt.c",
        "SRC/slassq.c",
        "SRC/slasv2.c",
        "SRC/slaswp.c",
        "SRC/slazq3.c",
        "SRC/slazq4.c",
        "SRC/sorg2r.c",
        "SRC/sorgbr.c",
        "SRC/sorgl2.c",
        "SRC/sorglq.c",
        "SRC/sorgqr.c",
        "SRC/sorm2r.c",
        "SRC/sormbr.c",
        "SRC/sorml2.c",
        "SRC/sormlq.c",
        "SRC/sormqr.c",
        "SRC/strti2.c",
        "SRC/strtri.c",
    ],
    copts = [
        "-Wno-format",
        "-Wno-logical-op-parentheses",
        "-Wno-parentheses",
        "-Wno-shift-op-parentheses",
        "-Wno-sometimes-uninitialized",
        "-Wno-unused-variable",
        "-Wno-unused-but-set-variable",
    ] + select({
        "@the8thwall//bzl/conditions:windows": [
            "-DNO_TRUNCATE",
            "-DMSDOS",
        ],
        "//conditions:default": [],
    }),
    # Keep visibility limited to //c8:clapack to prevent leaking f2c.h stuff.
    visibility = ["//c8:__pkg__"],
    deps = [
        ":cblas",
        ":clapack-common-hdrs",
        ":clapack-hdrs",
        ":f2c",
    ],
)
