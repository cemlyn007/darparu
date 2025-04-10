"""
Originally copied from https://github.com/cartographer-project/cartographer/blob/master/bazel/third_party/freetype2.BUILD.
"""

load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "freetype2",
    srcs = glob([
        "src/base/*.c",
        "src/cache/*.c",
        "src/cff/*.c",
        "src/sfnt/*.c",
        "src/gxvalid/*.c",
        "src/raster/*.c",
        "src/psnames/*.c",
        "src/truetype/*.c",
        "src/psaux/*.c",
        "src/otvalid/*.c",
        "src/cid/*.c",
        "src/pshinter/*.c",
        "src/pfr/*.c",
        "src/pcf/*.c",
        # "src/lzw/*.c",
        # "src/gzip/*.c",
        "src/autofit/*.c",
    ]) + [
        "builds/unix/ftsystem.c",
        "src/bdf/bdf.c",
        "src/bzip2/ftbzip2.c",
        "src/smooth/smooth.c",
        "src/type1/type1.c",
        "src/type42/type42.c",
        "src/winfonts/winfnt.c",
        "src/type1/t1afm.c",
        "src/type42/t42drivr.c",
        "include/freetype/fterrors.h",  # TODO
        "src/type42/t42objs.c",
        "src/type42/t42parse.c",
        "src/smooth/ftgrays.c",
        "src/type1/t1driver.c",
        "src/bdf/bdflib.c",
    ] + glob([
        "src/**/*.h",
        "include/freetype/internal/**/*.h",
    ]),
    hdrs = [
        "include/freetype/config/ftmodule.h",
        "src/type42/t42drivr.c",  # TODO
        "include/freetype/config/integer-types.h",
        "include/freetype/config/public-macros.h",
        "include/freetype/config/mac-support.h",
        "src/type42/t42objs.c",  # TODO
        "src/type42/t42parse.c",  # TODO
    ] + glob([
        "include/freetype/config/ftconfig.h",
        # freetype/ftgxval.h
        "include/freetype/*.h",
        "include/ft2build.h",
        "include/freetype/config/ftheader.h",
        "include/freetype/config/ftoption.h",
        "include/freetype/config/ftstdlib.h",
        "src/type1/*.c",
        "src/base/*.c",
        "src/bdf/*.c",
        "src/smooth/*.c",
        "src/cache/*.c",
        "src/cff/*.c",
        "src/sfnt/*.c",
        "src/gxvalid/*.c",
        "src/raster/*.c",
        "src/psnames/*.c",
        "src/truetype/*.c",
        "src/psaux/*.c",
        "src/otvalid/*.c",
        "src/cid/*.c",
        "src/pshinter/*.c",
        "src/pfr/*.c",
        "src/pcf/*.c",
        # "src/lzw/*.c",
        # "src/lzw/*.h",
        # "src/gzip/*.c",
        # "src/gzip/*.h",
        "src/autofit/*.c",
    ]),
    copts = [
        "-Wno-covered-switch-default",
        "-Wno-implicit-function-declaration",
        "-DFT2_BUILD_LIBRARY",
        "-DFT_CONFIG_MODULES_H=<ftmodule.h>",
        "-DHAVE_UNISTD_H=1",
        "-DHAVE_FCNTL_H=1",
        "-DHAVE_STDINT_H=1",
        # Remove the error definitions that don't work
        # "-DFT_ERRORDEF_(e,v,s)=e=v,",
        # "-DFT_NOERRORDEF_(e,v,s)=e=v,",
        "-Ifreetype/config",
        "-iquote=src/base",
        "-iquote=src/sfnt",
        "-iquote=src/psaux",
        "-iquote=include/freetype",
        "-iquote=src/raster",
        "-iquote=src/autofit",
        "-iquote=src/pcf",
    ],
    defines = [
        "FT_FREETYPE_H=<freetype/freetype.h>",
        "FT_CONFIG_CONFIG_H=<ftconfig.h>",
        "FT_ERROR_DEFINITIONS_H=<fterrors.h>",
        "FT_CONFIG_OPTION_SYSTEM_ZLIB",
        "FT_CONFIG_OPTION_NO_LZW",  # Replace USE_LZW with NO_LZW to disable LZW support
        "FT_CONFIG_OPTION_NO_GZIP",  # Add this line to disable internal gzip implementation
        # "FT_ERR_PREFIX=FT_Err_",
        # Remove the error definitions that don't work
        # "-DFT_ERRORDEF_(e,v,s)=e=v,",
        # "-DFT_NOERRORDEF_(e,v,s)=e=v,",
    ],
    includes = [
        "include",
        "include/freetype",
        "include/freetype/config",
        "src/base",
    ],
    local_defines = [
        "MACOS",
    ],
    visibility = ["//visibility:public"],
    deps = [],
)
