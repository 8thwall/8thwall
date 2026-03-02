load("@bazel_skylib//rules:expand_template.bzl", "expand_template")
load("@bazel_skylib//rules:common_settings.bzl", "string_flag")
load("@the8thwall//bzl/assemble:nasm_assemble.bzl", "nasm_assemble")

licenses(["notice"])  # custom notice-style license, see LICENSE.md

exports_files(["LICENSE.md"])

genrule(
    name = "jsizet",
    srcs = [
    ],
    outs = [
        "jsizet.h",
    ],
    cmd = select({
        "@the8thwall//bzl/conditions:32bit": "echo '#define SIZEOF_SIZE_T 4' >> $@",
        "@the8thwall//bzl/conditions:64bit": "echo '#define SIZEOF_SIZE_T 8' >> $@",
        "@the8thwall//conditions:default": "echo '#error libjpegturbo SIZEOF_SIZE_T unset in BUILD' >> $@",
    }),
)

genrule(
    name = "jconfig-hdrs",
    srcs = [
        "@the8thwall//third_party/libjpegturbo:jconfig",
    ],
    outs = [
        "jconfig.h",
        "jconfigint.h",
        "jversion.h",
    ],
    cmd = "cp $(SRCS) $(@D)/",
)

libjpegturbo_copts = select({
    "@the8thwall//bzl/conditions:android": [
        "-O3",
        "-fPIC",
        "-w",
    ],
    "@the8thwall//bzl/conditions:windows": [],
    "//conditions:default": [
        "-O3",
        "-w",
    ],
}) + select({
    "@the8thwall//bzl/conditions:android-arm": [
        "-D__ARM_NEON__",
        "-DNEON_INTRINSICS",
        "-march=armv7-a",
        "-mfpu=neon",
        "-mfloat-abi=softfp",
        "-fprefetch-loop-arrays",
    ],
    "@the8thwall//bzl/conditions:arm64": [
        "-DNEON_INTRINSICS",
    ],
    "//conditions:default": [],
})

cc_library(
    name = "libjpeg-turbo",
    srcs = [
        "cderror.h",
        "cdjpeg.c",
        "cdjpeg.h",
        "cmyk.h",
        "jaricom.c",
        "jcapimin.c",
        "jcapistd.c",
        "jcarith.c",
        "jccoefct.c",
        "jccolor.c",
        "jcdctmgr.c",
        "jchuff.c",
        "jchuff.h",
        "jcinit.c",
        "jcmainct.c",
        "jcmarker.c",
        "jcmaster.c",
        "jcomapi.c",
        "jconfig.h",
        "jconfigint.h",
        "jcparam.c",
        "jcphuff.c",
        "jcprepct.c",
        "jcsample.c",
        "jctrans.c",
        "jdapimin.c",
        "jdapistd.c",
        "jdarith.c",
        "jdatadst.c",
        "jdatadst-tj.c",
        "jdatasrc.c",
        "jdatasrc-tj.c",
        "jdcoefct.c",
        "jdcoefct.h",
        "jdcolor.c",
        "jdct.h",
        "jddctmgr.c",
        "jdhuff.c",
        "jdhuff.h",
        "jdinput.c",
        "jdmainct.c",
        "jdmainct.h",
        "jdmarker.c",
        "jdmaster.c",
        "jdmaster.h",
        "jdmerge.c",
        "jdmerge.h",
        "jdphuff.c",
        "jdpostct.c",
        "jdsample.c",
        "jdsample.h",
        "jdtrans.c",
        "jerror.c",
        "jfdctflt.c",
        "jfdctfst.c",
        "jfdctint.c",
        "jidctflt.c",
        "jidctfst.c",
        "jidctint.c",
        "jidctred.c",
        "jinclude.h",
        "jmemmgr.c",
        "jmemnobs.c",
        "jmemsys.h",
        "jpeg_nbits_table.h",
        "jpegcomp.h",
        "jquant1.c",
        "jquant2.c",
        "jutils.c",
        "jversion.h",
        "rdbmp.c",
        "rdppm.c",
        "tjutil.h",
        "transupp.c",
        "transupp.h",
        "turbojpeg.c",
        "wrbmp.c",
        "wrppm.c",
        ":jconfig-hdrs",
        ":jsizet",
    ],
    hdrs = [
        "jccolext.c",  # should have been named .inc
        "jdcol565.c",  # should have been named .inc
        "jdcolext.c",  # should have been named .inc
        "jdmrg565.c",  # should have been named .inc
        "jdmrgext.c",  # should have been named .inc
        "jerror.h",
        "jmorecfg.h",
        "jpegint.h",
        "jpeglib.h",
        "jstdhuff.c",  # should have been named .inc
        "turbojpeg.h",
    ],
    copts = libjpegturbo_copts + [
        "-Iexternal/libjpegturbo",
        "-DBMP_SUPPORTED",
        "-DPPM_SUPPORTED",
    ],
    includes = [
        ".",
    ],
    visibility = ["//visibility:public"],
    deps = select({
        "@the8thwall//bzl/conditions:arm": [":simd_armv7a"],
        "@the8thwall//bzl/conditions:arm64": [":simd_armv8a"],
        "@the8thwall//bzl/conditions:windows": [":simd_win_x86_64"],
        "//conditions:default": [":simd_none"],
    }),
)

cc_library(
    name = "simd_altivec",
    srcs = [
        "jchuff.h",
        "jconfig.h",
        "jconfigint.h",
        "jdct.h",
        "jerror.h",
        "jinclude.h",
        "jmorecfg.h",
        "jpegint.h",
        "jpeglib.h",
        "jsimd.h",
        "jsimddct.h",
        "simd/jsimd.h",
        "simd/powerpc/jccolor-altivec.c",
        "simd/powerpc/jcgray-altivec.c",
        "simd/powerpc/jcsample-altivec.c",
        "simd/powerpc/jdcolor-altivec.c",
        "simd/powerpc/jdmerge-altivec.c",
        "simd/powerpc/jdsample-altivec.c",
        "simd/powerpc/jfdctfst-altivec.c",
        "simd/powerpc/jfdctint-altivec.c",
        "simd/powerpc/jidctfst-altivec.c",
        "simd/powerpc/jidctint-altivec.c",
        "simd/powerpc/jquanti-altivec.c",
        "simd/powerpc/jsimd.c",
    ],
    hdrs = [
        "simd/powerpc/jccolext-altivec.c",
        "simd/powerpc/jcgryext-altivec.c",
        "simd/powerpc/jcsample.h",
        "simd/powerpc/jdcolext-altivec.c",
        "simd/powerpc/jdmrgext-altivec.c",
        "simd/powerpc/jsimd_altivec.h",
    ],
    copts = libjpegturbo_copts,
)

SRCS_SIMD_COMMON = [
    "jchuff.h",
    "jconfig.h",
    "jconfigint.h",
    "jdct.h",
    "jerror.h",
    "jinclude.h",
    "jmorecfg.h",
    "jpegint.h",
    "jpeglib.h",
    "jsimddct.h",
    "jsimd.h",
    "simd/jsimd.h",
]

# Note, simd_x86_64 is only supported on AMD k8.
cc_library(
    name = "simd_x86_64",
    srcs = [
        "simd/x86_64/jsimd.c",
    ] + SRCS_SIMD_COMMON,
    copts = libjpegturbo_copts,
    linkstatic = 1,
    deps = select({
        "//conditions:default": [],
    }),
)

# Used by only simd_x86_64, which is for AMD K8 arch.
nasm_assemble(
    name = "simd_x86_64_assemblage23",
    srcs = [
        "jconfig.h",
        "jconfigint.h",
        "simd/nasm/jcolsamp.inc",
        "simd/nasm/jdct.inc",
        "simd/nasm/jsimdcfg.inc",
        "simd/nasm/jsimdcfg.inc.h",
        "simd/nasm/jsimdext.inc",
        "simd/x86_64/jccolext-avx2.asm",
        "simd/x86_64/jccolext-sse2.asm",
        "simd/x86_64/jccolor-avx2.asm",
        "simd/x86_64/jccolor-sse2.asm",
        "simd/x86_64/jcgray-avx2.asm",
        "simd/x86_64/jcgray-sse2.asm",
        "simd/x86_64/jcgryext-avx2.asm",
        "simd/x86_64/jcgryext-sse2.asm",
        "simd/x86_64/jchuff-sse2.asm",
        "simd/x86_64/jcphuff-sse2.asm",
        "simd/x86_64/jcsample-avx2.asm",
        "simd/x86_64/jcsample-sse2.asm",
        "simd/x86_64/jdcolext-avx2.asm",
        "simd/x86_64/jdcolext-sse2.asm",
        "simd/x86_64/jdcolor-avx2.asm",
        "simd/x86_64/jdcolor-sse2.asm",
        "simd/x86_64/jdmerge-avx2.asm",
        "simd/x86_64/jdmerge-sse2.asm",
        "simd/x86_64/jdmrgext-avx2.asm",
        "simd/x86_64/jdmrgext-sse2.asm",
        "simd/x86_64/jdsample-avx2.asm",
        "simd/x86_64/jdsample-sse2.asm",
        "simd/x86_64/jfdctflt-sse.asm",
        "simd/x86_64/jfdctfst-sse2.asm",
        "simd/x86_64/jfdctint-avx2.asm",
        "simd/x86_64/jfdctint-sse2.asm",
        "simd/x86_64/jidctflt-sse2.asm",
        "simd/x86_64/jidctfst-sse2.asm",
        "simd/x86_64/jidctint-avx2.asm",
        "simd/x86_64/jidctint-sse2.asm",
        "simd/x86_64/jidctred-sse2.asm",
        "simd/x86_64/jquantf-sse2.asm",
        "simd/x86_64/jquanti-avx2.asm",
        "simd/x86_64/jquanti-sse2.asm",
        "simd/x86_64/jsimdcpu.asm",
    ],
    copts = [
        "-f elf64",
        "-DELF",
        "-DPIC",
        "-D__x86_64__",
    ],
    excludes = [
        "simd/x86_64/jccolext-avx2.asm",
        "simd/x86_64/jccolext-sse2.asm",
        "simd/x86_64/jcgryext-avx2.asm",
        "simd/x86_64/jcgryext-sse2.asm",
        "simd/x86_64/jdcolext-avx2.asm",
        "simd/x86_64/jdcolext-sse2.asm",
        "simd/x86_64/jdmrgext-avx2.asm",
        "simd/x86_64/jdmrgext-sse2.asm",
    ],
)

expand_template(
    name = "neon-compat_gen",
    out = "simd/arm/neon-compat.h",
    substitutions = {
        "#cmakedefine HAVE_VLD1_S16_X3": "#define HAVE_VLD1_S16_X3",
        "#cmakedefine HAVE_VLD1_U16_X2": "#define HAVE_VLD1_U16_X2",
        "#cmakedefine HAVE_VLD1Q_U8_X4": "#define HAVE_VLD1Q_U8_X4",
    },
    template = "simd/arm/neon-compat.h.in",
)

genrule(
    name = "neon-compat_hdr_src",
    srcs = ["simd/arm/neon-compat.h"],
    outs = ["neon-compat.h"],
    cmd = "cp $(location simd/arm/neon-compat.h) $@",
)

cc_library(
    name = "neon-compat_hdr",
    hdrs = ["neon-compat.h"],
    copts = libjpegturbo_copts,
)

SRCS_SIMD_ARM = [
    "simd/arm/jccolor-neon.c",
    "simd/arm/jcgray-neon.c",
    "simd/arm/jcphuff-neon.c",
    "simd/arm/jcsample-neon.c",
    "simd/arm/jdcolor-neon.c",
    "simd/arm/jdmerge-neon.c",
    "simd/arm/jdsample-neon.c",
    "simd/arm/jfdctfst-neon.c",
    "simd/arm/jfdctint-neon.c",
    "simd/arm/jidctfst-neon.c",
    "simd/arm/jidctint-neon.c",
    "simd/arm/jidctred-neon.c",
    "simd/arm/jquanti-neon.c",
]

# .c files in the following list are used like .h files in that they are
# "#include"-ed in the actual .c files. So, treat them like normal headers, and
# they *should not* be compiled into individual objects.
HDRS_SIMD_ARM = [
    "simd/arm/align.h",
    "simd/arm/jchuff.h",
    "simd/arm/jcgryext-neon.c",
    "simd/arm/jdcolext-neon.c",
    "simd/arm/jdmrgext-neon.c",
]

cc_library(
    name = "simd_armv7a",
    srcs = [
        "simd/arm/aarch32/jchuff-neon.c",
        "simd/arm/aarch32/jsimd.c",
        ":jconfig-hdrs",
        ":jsizet",
    ] + SRCS_SIMD_COMMON + SRCS_SIMD_ARM,
    hdrs = [
        "simd/arm/aarch32/jccolext-neon.c",
    ] + HDRS_SIMD_ARM,
    copts = libjpegturbo_copts,
    visibility = ["//visibility:private"],
    deps = [":neon-compat_hdr"],
)

cc_library(
    name = "simd_armv8a",
    srcs = [
        "simd/arm/aarch64/jchuff-neon.c",
        "simd/arm/aarch64/jsimd.c",
        ":jconfig-hdrs",
        ":jsizet",
    ] + SRCS_SIMD_COMMON + SRCS_SIMD_ARM,
    hdrs = [
        "simd/arm/aarch64/jccolext-neon.c",
    ] + HDRS_SIMD_ARM,
    copts = libjpegturbo_copts,
    visibility = ["//visibility:private"],
    deps = [":neon-compat_hdr"],
)

cc_library(
    name = "simd_win_x86_64",
    srcs = [
        "simd/x86_64/jsimd.c",
        ":jconfig-hdrs",
        ":jsizet",
    ] + SRCS_SIMD_COMMON,
    copts = libjpegturbo_copts,
    deps = select({
        "@the8thwall//bzl/conditions:windows": [":simd-win_x86_64"],
        "//conditions:default": [],
    }),
)

nasm_assemble(
    name = "simd-win_x86_64",
    srcs = [":jconfig-hdrs"] + [
        "jconfigint.h",
        "simd/nasm/jcolsamp.inc",
        "simd/nasm/jdct.inc",
        "simd/nasm/jsimdcfg.inc",
        "simd/nasm/jsimdcfg.inc.h",
        "simd/nasm/jsimdext.inc",
        "simd/x86_64/jccolext-avx2.asm",
        "simd/x86_64/jccolext-sse2.asm",
        "simd/x86_64/jccolor-avx2.asm",
        "simd/x86_64/jccolor-sse2.asm",
        "simd/x86_64/jcgray-avx2.asm",
        "simd/x86_64/jcgray-sse2.asm",
        "simd/x86_64/jcgryext-avx2.asm",
        "simd/x86_64/jcgryext-sse2.asm",
        "simd/x86_64/jchuff-sse2.asm",
        "simd/x86_64/jcphuff-sse2.asm",
        "simd/x86_64/jcsample-avx2.asm",
        "simd/x86_64/jcsample-sse2.asm",
        "simd/x86_64/jdcolext-avx2.asm",
        "simd/x86_64/jdcolext-sse2.asm",
        "simd/x86_64/jdcolor-avx2.asm",
        "simd/x86_64/jdcolor-sse2.asm",
        "simd/x86_64/jdmerge-avx2.asm",
        "simd/x86_64/jdmerge-sse2.asm",
        "simd/x86_64/jdmrgext-avx2.asm",
        "simd/x86_64/jdmrgext-sse2.asm",
        "simd/x86_64/jdsample-avx2.asm",
        "simd/x86_64/jdsample-sse2.asm",
        "simd/x86_64/jfdctflt-sse.asm",
        "simd/x86_64/jfdctfst-sse2.asm",
        "simd/x86_64/jfdctint-avx2.asm",
        "simd/x86_64/jfdctint-sse2.asm",
        "simd/x86_64/jidctflt-sse2.asm",
        "simd/x86_64/jidctfst-sse2.asm",
        "simd/x86_64/jidctint-avx2.asm",
        "simd/x86_64/jidctint-sse2.asm",
        "simd/x86_64/jidctred-sse2.asm",
        "simd/x86_64/jquantf-sse2.asm",
        "simd/x86_64/jquanti-avx2.asm",
        "simd/x86_64/jquanti-sse2.asm",
        "simd/x86_64/jsimdcpu.asm",
    ],
    copts = [
        "-fwin64",
        "-DWIN64",
        "-D__x86_64__",
    ],
    excludes = [
        "simd/x86_64/jccolext-avx2.asm",
        "simd/x86_64/jccolext-sse2.asm",
        "simd/x86_64/jcgryext-avx2.asm",
        "simd/x86_64/jcgryext-sse2.asm",
        "simd/x86_64/jdcolext-avx2.asm",
        "simd/x86_64/jdcolext-sse2.asm",
        "simd/x86_64/jdmrgext-avx2.asm",
        "simd/x86_64/jdmrgext-sse2.asm",
    ],
)

cc_library(
    name = "simd_none",
    srcs = [
        "jchuff.h",
        "jconfig.h",
        "jconfigint.h",
        "jdct.h",
        "jerror.h",
        "jinclude.h",
        "jmorecfg.h",
        "jpegint.h",
        "jpeglib.h",
        "jsimd.h",
        "jsimd_none.c",
        "jsimddct.h",
        "jsizet.h",
    ],
    copts = libjpegturbo_copts,
)

expand_template(
    name = "jconfig_win",
    out = "jconfig_win.h",
    substitutions = {
        "@JPEG_LIB_VERSION@": "62",
        "@VERSION@": "2.1.4",
        "@LIBJPEG_TURBO_VERSION_NUMBER@": "2001004",
        "@BITS_IN_JSAMPLE@": "8",
        "#cmakedefine C_ARITH_CODING_SUPPORTED": "#define C_ARITH_CODING_SUPPORTED",
        "#cmakedefine D_ARITH_CODING_SUPPORTED": "#define D_ARITH_CODING_SUPPORTED",
        "#cmakedefine MEM_SRCDST_SUPPORTED": "#define MEM_SRCDST_SUPPORTED",
        "#cmakedefine WITH_SIMD": "",
    },
    template = "win/jconfig.h.in",
)

JCONFIG_NOWIN_COMMON_SUBSTITUTIONS = {
    "@JPEG_LIB_VERSION@": "62",
    "@VERSION@": "2.1.4",
    "@LIBJPEG_TURBO_VERSION_NUMBER@": "2001004",
    "#cmakedefine C_ARITH_CODING_SUPPORTED 1": "#define C_ARITH_CODING_SUPPORTED 1",
    "#cmakedefine D_ARITH_CODING_SUPPORTED 1": "#define D_ARITH_CODING_SUPPORTED 1",
    "#cmakedefine MEM_SRCDST_SUPPORTED 1": "#define MEM_SRCDST_SUPPORTED 1",
    "@BITS_IN_JSAMPLE@": "8",
    "#cmakedefine HAVE_LOCALE_H 1": "#define HAVE_LOCALE_H 1",
    "#cmakedefine HAVE_STDDEF_H 1": "#define HAVE_STDDEF_H 1",
    "#cmakedefine HAVE_STDLIB_H 1": "#define HAVE_STDLIB_H 1",
    "#cmakedefine NEED_SYS_TYPES_H 1": "#define NEED_SYS_TYPES_H 1",
    "#cmakedefine NEED_BSD_STRINGS 1": "",
    "#cmakedefine HAVE_UNSIGNED_CHAR 1": "#define HAVE_UNSIGNED_CHAR 1",
    "#cmakedefine HAVE_UNSIGNED_SHORT 1": "#define HAVE_UNSIGNED_SHORT 1",
    "#cmakedefine INCOMPLETE_TYPES_BROKEN 1": "",
    "#cmakedefine RIGHT_SHIFT_IS_UNSIGNED 1": "",
    "#cmakedefine __CHAR_UNSIGNED__ 1": "",
    "#undef const": "",
    "#undef size_t": "",
}

JCONFIG_NOWIN_SIMD_SUBSTITUTIONS = {
    "#cmakedefine WITH_SIMD 1": "#define WITH_SIMD 1",
}

JCONFIG_NOWIN_NOSIMD_SUBSTITUTIONS = {
    "#cmakedefine WITH_SIMD 1": "",
}

JCONFIG_NOWIN_SIMD_SUBSTITUTIONS.update(JCONFIG_NOWIN_COMMON_SUBSTITUTIONS)

JCONFIG_NOWIN_NOSIMD_SUBSTITUTIONS.update(JCONFIG_NOWIN_COMMON_SUBSTITUTIONS)

expand_template(
    name = "jconfig_nowin_nosimd",
    out = "jconfig_nowin_nosimd.h",
    substitutions = JCONFIG_NOWIN_NOSIMD_SUBSTITUTIONS,
    template = "jconfig.h.in",
)

expand_template(
    name = "jconfig_nowin_simd",
    out = "jconfig_nowin_simd.h",
    substitutions = JCONFIG_NOWIN_SIMD_SUBSTITUTIONS,
    template = "jconfig.h.in",
)

JCONFIGINT_COMMON_SUBSTITUTIONS = {
    "@BUILD@": "20221022",
    "@VERSION@": "2.1.4",
    "@CMAKE_PROJECT_NAME@": "libjpeg-turbo",
    "#undef inline": "",
    "#cmakedefine HAVE_INTRIN_H": "",
}

JCONFIGINT_NOWIN_SUBSTITUTIONS = {
    "#cmakedefine HAVE_BUILTIN_CTZL": "#define HAVE_BUILTIN_CTZL",
    "@INLINE@": "inline __attribute__((always_inline))",
    "#define SIZEOF_SIZE_T  @SIZE_T@": "#if (__WORDSIZE==64 && !defined(__native_client__))\n" +
                                       "#define SIZEOF_SIZE_T 8\n" +
                                       "#else\n" +
                                       "#define SIZEOF_SIZE_T 4\n" +
                                       "#endif\n",
}

JCONFIGINT_WIN_SUBSTITUTIONS = {
    "#cmakedefine HAVE_BUILTIN_CTZL": "",
    "#define INLINE  @INLINE@": "#if defined(__GNUC__)\n" +
                                "#define INLINE inline __attribute__((always_inline))\n" +
                                "#elif defined(_MSC_VER)\n" +
                                "#define INLINE __forceinline\n" +
                                "#else\n" +
                                "#define INLINE\n" +
                                "#endif\n",
    "#define SIZEOF_SIZE_T  @SIZE_T@": "#if (__WORDSIZE==64)\n" +
                                       "#define SIZEOF_SIZE_T 8\n" +
                                       "#else\n" +
                                       "#define SIZEOF_SIZE_T 4\n" +
                                       "#endif\n",
}

JCONFIGINT_NOWIN_SUBSTITUTIONS.update(JCONFIGINT_COMMON_SUBSTITUTIONS)

JCONFIGINT_WIN_SUBSTITUTIONS.update(JCONFIGINT_COMMON_SUBSTITUTIONS)

expand_template(
    name = "jconfigint_nowin",
    out = "jconfigint_nowin.h",
    substitutions = JCONFIGINT_NOWIN_SUBSTITUTIONS,
    template = "jconfigint.h.in",
)

expand_template(
    name = "jconfigint_win",
    out = "jconfigint_win.h",
    substitutions = JCONFIGINT_WIN_SUBSTITUTIONS,
    template = "jconfigint.h.in",
)

# jiminy cricket the way this file is generated is completely outrageous
genrule(
    name = "configure_simd",
    outs = ["simd/jsimdcfg.inc"],
    cmd = "cat <<'EOF' >$@\n" +
          "%define DCTSIZE 8\n" +
          "%define DCTSIZE2 64\n" +
          "%define RGB_RED 0\n" +
          "%define RGB_GREEN 1\n" +
          "%define RGB_BLUE 2\n" +
          "%define RGB_PIXELSIZE 3\n" +
          "%define EXT_RGB_RED 0\n" +
          "%define EXT_RGB_GREEN 1\n" +
          "%define EXT_RGB_BLUE 2\n" +
          "%define EXT_RGB_PIXELSIZE 3\n" +
          "%define EXT_RGBX_RED 0\n" +
          "%define EXT_RGBX_GREEN 1\n" +
          "%define EXT_RGBX_BLUE 2\n" +
          "%define EXT_RGBX_PIXELSIZE 4\n" +
          "%define EXT_BGR_RED 2\n" +
          "%define EXT_BGR_GREEN 1\n" +
          "%define EXT_BGR_BLUE 0\n" +
          "%define EXT_BGR_PIXELSIZE 3\n" +
          "%define EXT_BGRX_RED 2\n" +
          "%define EXT_BGRX_GREEN 1\n" +
          "%define EXT_BGRX_BLUE 0\n" +
          "%define EXT_BGRX_PIXELSIZE 4\n" +
          "%define EXT_XBGR_RED 3\n" +
          "%define EXT_XBGR_GREEN 2\n" +
          "%define EXT_XBGR_BLUE 1\n" +
          "%define EXT_XBGR_PIXELSIZE 4\n" +
          "%define EXT_XRGB_RED 1\n" +
          "%define EXT_XRGB_GREEN 2\n" +
          "%define EXT_XRGB_BLUE 3\n" +
          "%define EXT_XRGB_PIXELSIZE 4\n" +
          "%define RGBX_FILLER_0XFF 1\n" +
          "%define JSAMPLE byte ; unsigned char\n" +
          "%define SIZEOF_JSAMPLE SIZEOF_BYTE ; sizeof(JSAMPLE)\n" +
          "%define CENTERJSAMPLE 128\n" +
          "%define JCOEF word ; short\n" +
          "%define SIZEOF_JCOEF SIZEOF_WORD ; sizeof(JCOEF)\n" +
          "%define JDIMENSION dword ; unsigned int\n" +
          "%define SIZEOF_JDIMENSION SIZEOF_DWORD ; sizeof(JDIMENSION)\n" +
          "%define JSAMPROW POINTER ; JSAMPLE * (jpeglib.h)\n" +
          "%define JSAMPARRAY POINTER ; JSAMPROW * (jpeglib.h)\n" +
          "%define JSAMPIMAGE POINTER ; JSAMPARRAY * (jpeglib.h)\n" +
          "%define JCOEFPTR POINTER ; JCOEF * (jpeglib.h)\n" +
          "%define SIZEOF_JSAMPROW SIZEOF_POINTER ; sizeof(JSAMPROW)\n" +
          "%define SIZEOF_JSAMPARRAY SIZEOF_POINTER ; sizeof(JSAMPARRAY)\n" +
          "%define SIZEOF_JSAMPIMAGE SIZEOF_POINTER ; sizeof(JSAMPIMAGE)\n" +
          "%define SIZEOF_JCOEFPTR SIZEOF_POINTER ; sizeof(JCOEFPTR)\n" +
          "%define DCTELEM word ; short\n" +
          "%define SIZEOF_DCTELEM SIZEOF_WORD ; sizeof(DCTELEM)\n" +
          "%define float FP32 ; float\n" +
          "%define SIZEOF_FAST_FLOAT SIZEOF_FP32 ; sizeof(float)\n" +
          "%define ISLOW_MULT_TYPE word ; must be short\n" +
          "%define SIZEOF_ISLOW_MULT_TYPE SIZEOF_WORD ; sizeof(ISLOW_MULT_TYPE)\n" +
          "%define IFAST_MULT_TYPE word ; must be short\n" +
          "%define SIZEOF_IFAST_MULT_TYPE SIZEOF_WORD ; sizeof(IFAST_MULT_TYPE)\n" +
          "%define IFAST_SCALE_BITS 2 ; fractional bits in scale factors\n" +
          "%define FLOAT_MULT_TYPE FP32 ; must be float\n" +
          "%define SIZEOF_FLOAT_MULT_TYPE SIZEOF_FP32 ; sizeof(FLOAT_MULT_TYPE)\n" +
          "%define JSIMD_NONE 0x00\n" +
          "%define JSIMD_MMX 0x01\n" +
          "%define JSIMD_3DNOW 0x02\n" +
          "%define JSIMD_SSE 0x04\n" +
          "%define JSIMD_SSE2 0x08\n" +
          "EOF",
)
