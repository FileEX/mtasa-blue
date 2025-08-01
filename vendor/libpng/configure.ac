# configure.ac

# Copyright (c) 2018-2024 Cosmin Truta
# Copyright (c) 2004-2016 Glenn Randers-Pehrson

# This code is released under the libpng license.
# For conditions of distribution and use, see the disclaimer
# and license in png.h

dnl Process this file with autoconf to produce a configure script.
dnl
dnl Minor upgrades (compatible ABI): increment the package version
dnl (third field in two places below) and set the PNGLIB_RELEASE
dnl variable.
dnl
dnl Major upgrades (incompatible ABI): increment the package major
dnl version (second field, or first if desired), set the minor
dnl to 0, set PNGLIB_MAJOR below *and* follow the instructions in
dnl Makefile.am to upgrade the package name.

dnl This is here to prevent earlier autoconf from being used, it
dnl should not be necessary to regenerate configure if the time
dnl stamps are correct
AC_PREREQ([2.68])

dnl Version number stuff here:

AC_INIT([libpng],[1.6.50],[png-mng-implement@lists.sourceforge.net])
AC_CONFIG_MACRO_DIR([scripts/autoconf])

# libpng does not follow GNU file name conventions (hence 'foreign')
# color-tests requires automake 1.11 or later
# silent-rules requires automake 1.11 or later
# dist-xz requires automake 1.11 or later
# 1.12.2 fixes a security issue in 1.11.2 and 1.12.1
# 1.13 is required for parallel tests
AM_INIT_AUTOMAKE([1.13 foreign dist-xz color-tests silent-rules subdir-objects])
# The following line causes --disable-maintainer-mode to be the default to
# configure. This is necessary because libpng distributions cannot rely on the
# time stamps of the autotools generated files being correct
AM_MAINTAINER_MODE

dnl configure.ac and Makefile.am expect automake 1.11.2 or a compatible later
dnl version; aclocal.m4 will generate a failure if you use a prior version of
dnl automake, so the following is not necessary (and is not defined anyway):
dnl AM_PREREQ([1.11.2])
dnl stop configure from automagically running automake

PNGLIB_VERSION=1.6.50
PNGLIB_MAJOR=1
PNGLIB_MINOR=6
PNGLIB_RELEASE=50

dnl End of version number stuff

AC_CONFIG_SRCDIR([pngget.c])
AC_CONFIG_HEADERS([config.h])

# Check the basic programs.
AC_LANG([C])
AC_PROG_CC
AM_PROG_AS
LT_PATH_LD
AC_PROG_CPP
AC_PROG_AWK
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

dnl libtool/libtoolize; version 2.4.2 is the tested version. This or any
dnl compatible later version may be used
LT_INIT([win32-dll])
LT_PREREQ([2.4.2])

dnl Declare the AWK variable.
AC_ARG_VAR(AWK, [AWK language processor])

# Some awk implementations crash when confronted with pnglibconf.dfa.
# Run a test now, to make sure this doesn't happen.
AC_MSG_CHECKING([if awk ($AWK) works])
if ${AWK} -f ${srcdir}/scripts/options.awk out="/dev/null" version=search\
   ${srcdir}/pngconf.h ${srcdir}/scripts/pnglibconf.dfa\
   ${srcdir}/pngusr.dfa 1>&2
then
   AC_MSG_RESULT([yes])
else
   AC_MSG_FAILURE([no], 1)
fi

# This is a remnant of the old cc -E validation, where it may have been
# necessary to use a different preprocessor for .dfn files
DFNCPP="$CPP"
AC_SUBST(DFNCPP)

AC_ARG_ENABLE([tests],
   AS_HELP_STRING([--disable-tests],
      [do not build the test programs (default is to build)]),
      [enable_tests="$enableval"],
      [enable_tests=yes])

AM_CONDITIONAL([ENABLE_TESTS],
   [test "$enable_tests" != "no"])

AC_ARG_ENABLE([tools],
   AS_HELP_STRING([--disable-tools],
      [do not build the auxiliary tools (default is to build)]),
      [enable_tools="$enableval"],
      [enable_tools=yes])

AM_CONDITIONAL([ENABLE_TOOLS],
   [test "$enable_tools" != "no"])

# -Werror cannot be passed to GCC in CFLAGS because configure will fail
# (it checks the compiler with a program that generates a warning).
# Add the following option to deal with this:
AC_ARG_VAR(PNG_COPTS,
   [additional flags for the C compiler, to be used for options that would]
   [cause configure itself to fail])
AC_ARG_ENABLE(werror,
   AS_HELP_STRING([[[--enable-werror[=OPT]]]],
      [Pass -Werror or the given argument to the compiler if it is supported]),
   [test "$enable_werror" = "yes" && enable_werror="-Werror"
    if test "$enable_werror" != "no"; then
      sav_CFLAGS="$CFLAGS"
      CFLAGS="$enable_werror $CFLAGS"
      AC_MSG_CHECKING([if the compiler allows $enable_werror])
      AC_COMPILE_IFELSE(
         [AC_LANG_SOURCE([
            [int main(int argc, char **argv){]
            [return argv[argc-1][0];]
            [}]])],
         AC_MSG_RESULT(yes)
         PNG_COPTS="$PNG_COPTS $enable_werror",
         AC_MSG_RESULT(no))
      CFLAGS="$sav_CFLAGS"
    fi],)

# For GCC 5 the default mode for C is -std=gnu11 instead of -std=gnu89.
# In pngpriv.h we request just the POSIX 1003.1 and C89 APIs by defining
# _POSIX_SOURCE to 1. This is incompatible with the new default mode, so
# we test for that and force the "-std=c89" compiler option:
AC_MSG_CHECKING([if we need to force back C standard to C89])
AC_COMPILE_IFELSE(
   [AC_LANG_PROGRAM([
      [#define _POSIX_SOURCE 1]
      [#include <stdio.h>]
   ])],
   AC_MSG_RESULT(no),[
      if test "x$GCC" != "xyes"; then
         AC_MSG_ERROR(
            [Forcing back to C89 is required but the flags are only known for GCC])
      fi
   AC_MSG_RESULT(yes)
   CFLAGS="$CFLAGS -std=c89"
])

# Checks for structures and compiler characteristics.
AC_STRUCT_TM
AC_C_RESTRICT

# Checks for library functions.
AC_CHECK_FUNCS([pow], ,
  [AC_CHECK_LIB([m], [pow], , [AC_MSG_ERROR([cannot find pow])])])

# Some later POSIX 1003.1 functions are required for test programs, failure
# here is soft (the corresponding test program is not built).
AC_CHECK_FUNC([clock_gettime], , [AC_MSG_WARN([not building timepng])])
AM_CONDITIONAL([HAVE_CLOCK_GETTIME], [test "$ac_cv_func_clock_gettime" = "yes"])

AC_ARG_WITH(zlib-prefix,
   AS_HELP_STRING([[[--with-zlib-prefix]]],
      [prefix that may have been used in installed zlib]),
      [ZPREFIX=${withval}],
      [ZPREFIX='z_'])
AC_CHECK_LIB([z], [zlibVersion], ,
  [AC_CHECK_LIB([z], [${ZPREFIX}zlibVersion], ,
     [AC_MSG_ERROR([zlib not installed])])])

# The following is for pngvalid, to ensure it catches FP errors even on
# platforms that don't enable FP exceptions, the function appears in the math
# library (typically), it's not an error if it is not found.
AC_CHECK_LIB([m], [feenableexcept])
AC_CHECK_FUNCS([feenableexcept])

AC_MSG_CHECKING([if using Solaris linker])
SLD=`$LD --version 2>&1 | grep Solaris`
if test "$SLD"; then
    have_solaris_ld=yes
    AC_MSG_RESULT(yes)
else
    have_solaris_ld=no
    AC_MSG_RESULT(no)
fi
AM_CONDITIONAL(HAVE_SOLARIS_LD, test "$have_solaris_ld" = "yes")

AC_MSG_CHECKING([if libraries can be versioned])
# Special case for PE/COFF platforms: ld reports
# support for version-script, but doesn't actually
# DO anything with it.
case $host in
*cygwin* | *mingw32* | *interix* )
    have_ld_version_script=no
    AC_MSG_RESULT(no)
;;
* )

if test "$have_solaris_ld" = "yes"; then
    GLD=`$LD --help < /dev/null 2>&1 | grep 'M mapfile'`
else
    GLD=`$LD --help < /dev/null 2>/dev/null | grep version-script`
fi

if test "$GLD"; then
    have_ld_version_script=yes
    AC_MSG_RESULT(yes)
else
    have_ld_version_script=no
    AC_MSG_RESULT(no)
    AC_MSG_WARN(*** You have not enabled versioned symbols.)
fi
;;
esac

AM_CONDITIONAL(HAVE_LD_VERSION_SCRIPT, test "$have_ld_version_script" = "yes")

if test "$have_ld_version_script" = "yes"; then
    AC_MSG_CHECKING([for symbol prefix])
    SYMBOL_PREFIX=`echo "PREFIX=__USER_LABEL_PREFIX__" \
                  | ${CPP:-${CC:-gcc} -E} - 2>&1 \
                  | ${EGREP:-grep} "^PREFIX=" \
                  | ${SED:-sed} -e "s:^PREFIX=::" -e "s:__USER_LABEL_PREFIX__::"`
    AC_SUBST(SYMBOL_PREFIX)
    AC_MSG_RESULT($SYMBOL_PREFIX)
fi

# Substitutions for .in files
AC_SUBST(PNGLIB_VERSION)
AC_SUBST(PNGLIB_MAJOR)
AC_SUBST(PNGLIB_MINOR)
AC_SUBST(PNGLIB_RELEASE)

# Additional arguments (and substitutions)
# Allow the pkg-config directory to be set
AC_ARG_WITH(pkgconfigdir,
   AS_HELP_STRING([[[--with-pkgconfigdir]]],
      [Use the specified pkgconfig dir (default is libdir/pkgconfig)]),
   [pkgconfigdir=${withval}],
   [pkgconfigdir='${libdir}/pkgconfig'])

AC_SUBST([pkgconfigdir])
AC_MSG_NOTICE([[pkgconfig directory is ${pkgconfigdir}]])

# Make the *-config binary config scripts optional
AC_ARG_WITH(binconfigs,
   AS_HELP_STRING([[[--with-binconfigs]]],
      [Generate shell libpng-config scripts as well as pkg-config data]
      [@<:@default=yes@:>@]),
   [if test "${withval}" = no; then
      binconfigs=
      AC_MSG_NOTICE([[libpng-config scripts will not be built]])
    else
      binconfigs='${binconfigs}'
    fi],
   [binconfigs='${binconfigs}'])
AC_SUBST([binconfigs])

# Support for prefixes to the API function names; this will generate defines
# at the start of the build to rename exported library functions
AC_ARG_WITH(libpng-prefix,
   AS_HELP_STRING([[[--with-libpng-prefix]]],
      [prefix libpng exported function (API) names with the given value]),
   [if test "${withval:-no}" != "no"; then
      AC_SUBST([PNG_PREFIX], [${withval}])
    fi])
AM_CONDITIONAL([DO_PNG_PREFIX], [test "${with_libpng_prefix:-no}" != "no"])

# Control over what links are made for installed files.  Versioned files are
# always installed, when the following options are turned on corresponding
# unversioned links are also created (normally as symbolic links):
AC_ARG_ENABLE([unversioned-links],
   AS_HELP_STRING([[[--enable-unversioned-links]]],
      [Installed libpng header files are placed in a versioned subdirectory]
      [and installed libpng library (including DLL) files are versioned.]
      [If this option is enabled unversioned links will be created pointing to]
      [the corresponding installed files.  If you use libpng.pc or]
      [libpng-config for all builds you do not need these links, but if you]
      [compile programs directly they will typically #include <png.h> and]
      [link with -lpng; in that case you need the links.]
      [The links can be installed manually using 'make install-header-links']
      [and 'make install-library-links' and can be removed using the]
      [corresponding uninstall- targets.  If you do enable this option every]
      [libpng 'make install' will recreate the links to point to the just]
      [installed version of libpng.  The default is to create the links;]
      [use --disable-unversioned-links to change this]))

# The AM_CONDITIONAL test is written so that the default is enabled;
# --disable-unversioned-links must be given to turn the option off.
AM_CONDITIONAL([DO_INSTALL_LINKS],[test "$enable_unversioned_links" != "no"])

AC_ARG_ENABLE([unversioned-libpng-pc],
   AS_HELP_STRING([[[--enable-unversioned-libpng-pc]]],
      [Install the configuration file 'libpng.pc' as a link to the versioned]
      [version.  This is done by default - use --disable-unversioned-libpng-pc]
      [to change this.]))
AM_CONDITIONAL([DO_INSTALL_LIBPNG_PC],
   [test "$enable_unversioned_libpng_pc" != "no"])

AC_ARG_ENABLE([unversioned-libpng-config],
   AS_HELP_STRING([[[--enable-unversioned-libpng-config]]],
      [Install the configuration file 'libpng-config' as a link to the]
      [versioned version.  This is done by default - use]
      [--disable-unversioned-libpng-config to change this.]))
AM_CONDITIONAL([DO_INSTALL_LIBPNG_CONFIG],
   [test "$enable_unversioned_libpng_config" != "no"])

# HOST-SPECIFIC OPTIONS
# =====================
#
# DEFAULT
# =======
#
AC_ARG_ENABLE([hardware-optimizations],
   AS_HELP_STRING([[[--enable-hardware-optimizations]]],
      [Enable hardware optimizations: =no/off, yes/on.]),
   [case "$enableval" in
      no|off)
         # disable hardware optimization on all systems:
         enable_arm_neon=no
         AC_DEFINE([PNG_ARM_NEON_OPT], [0],
           [Disable ARM_NEON optimizations])
         enable_mips_msa=no
         AC_DEFINE([PNG_MIPS_MSA_OPT], [0],
           [Disable MIPS_MSA optimizations])
         enable_mips_mmi=no
         AC_DEFINE([PNG_MIPS_MMI_OPT], [0],
           [Disable MIPS_MMI optimizations])
         enable_powerpc_vsx=no
         AC_DEFINE([PNG_POWERPC_VSX_OPT], [0],
           [Disable POWERPC VSX optimizations])
         enable_intel_sse=no
         AC_DEFINE([PNG_INTEL_SSE_OPT], [0],
           [Disable INTEL_SSE optimizations])
         enable_loongarch_lsx=no
         AC_DEFINE([PNG_LOONGARCH_LSX_OPT], [0],
           [Disable LOONGARCH_LSX optimizations])
         enable_riscv_rvv=no
         AC_DEFINE([PNG_RISCV_RVV_OPT], [0],
           [Disable RISC-V Vector optimizations])
         ;;
      *)
         # allow enabling hardware optimization on any system:
         case "$host_cpu" in
            arm*|aarch64*)
              enable_arm_neon=yes
              AC_DEFINE([PNG_ARM_NEON_OPT], [2],
                [Enable ARM_NEON optimizations])
              ;;
            mipsel*|mips64el*)
              enable_mips_mmi=yes
              enable_mips_msa=yes
              AC_DEFINE([PNG_MIPS_MMI_OPT], [1],
                [Enable MIPS_MMI optimizations])
              AC_DEFINE([PNG_MIPS_MSA_OPT], [2],
                [Enable MIPS_MSA optimizations])
              ;;
            i?86|x86_64)
              enable_intel_sse=yes
              AC_DEFINE([PNG_INTEL_SSE_OPT], [1],
                [Enable Intel SSE optimizations])
              ;;
            powerpc*|ppc64*)
              enable_powerpc_vsx=yes
              AC_DEFINE([PNG_POWERPC_VSX_OPT], [2],
                [Enable POWERPC VSX optimizations])
              ;;
            loongarch*)
              enable_loongarch_lsx=yes
              AC_DEFINE([PNG_LOONGARCH_LSX_OPT], [1],
                [Enable LOONGARCH_LSX optimizations])
              ;;
            riscv64)
              enable_riscv_rvv=yes
              AC_DEFINE([PNG_RISCV_RVV_OPT], [2],
                [Enable RISC-V Vector optimizations])
              ;;
         esac
         ;;
   esac])

# ARM NEON
# ========

AC_ARG_ENABLE([arm-neon],
   AS_HELP_STRING([[[--enable-arm-neon]]],
      [Enable ARM NEON optimizations: =no/off, check, api, yes/on.]
      [no/off: disable the optimizations;]
      [check: use internal checking code (deprecated and poorly supported);]
      [api: disable by default, enable by a call to png_set_option;]
      [yes/on: turn on unconditionally.]
      [If not specified: determined by the compiler.]),
   [case "$enableval" in
      no|off)
         # disable the default enabling on __ARM_NEON__ systems:
         AC_DEFINE([PNG_ARM_NEON_OPT], [0],
                   [Disable ARM Neon optimizations])
         # Prevent inclusion of the assembler files below:
         enable_arm_neon=no ;;
      check)
         AC_DEFINE([PNG_ARM_NEON_CHECK_SUPPORTED], [],
                   [Check for ARM Neon support at run-time]);;
      api)
         AC_DEFINE([PNG_ARM_NEON_API_SUPPORTED], [],
                   [Turn on ARM Neon optimizations at run-time]);;
      yes|on)
         AC_DEFINE([PNG_ARM_NEON_OPT], [2],
                   [Enable ARM Neon optimizations])
         AC_MSG_WARN([--enable-arm-neon: please specify 'check' or 'api';]
            [if you want the optimizations unconditionally,]
            [pass '-mfpu=neon' to the compiler.]);;
      *)
         AC_MSG_ERROR([--enable-arm-neon=${enable_arm_neon}:]
                      [invalid argument])
   esac])

# Add ARM-specific files to all builds where $host_cpu is arm ('arm*') or
# where ARM optimizations were explicitly requested. (This allows a fallback
# if a future host CPU does not match 'arm*'.)

AM_CONDITIONAL([PNG_ARM_NEON],
   [test "$enable_arm_neon" != 'no' &&
    case "$host_cpu" in
      arm*|aarch64*) : ;;
      *)    test "$enable_arm_neon" != '' ;;
    esac])

# MIPS MSA
# ========

AC_ARG_ENABLE([mips-msa],
   AS_HELP_STRING([[[--enable-mips-msa]]],
      [Enable MIPS MSA optimizations: =no/off, check, api, yes/on.]
      [no/off: disable the optimizations;]
      [check: use internal checking code (deprecated and poorly supported);]
      [api: disable by default, enable by a call to png_set_option;]
      [yes/on: turn on unconditionally.]
      [If not specified: determined by the compiler.]),
   [case "$enableval" in
      no|off)
         # disable the default enabling on __mips_msa systems:
         AC_DEFINE([PNG_MIPS_MSA_OPT], [0],
                   [Disable MIPS MSA optimizations])
         # Prevent inclusion of the assembler files below:
         enable_mips_msa=no ;;
      check)
         AC_DEFINE([PNG_MIPS_MSA_CHECK_SUPPORTED], [],
                   [Check for MIPS MSA support at run-time]);;
      api)
         AC_DEFINE([PNG_MIPS_MSA_API_SUPPORTED], [],
                   [Turn on MIPS MSA optimizations at run-time]);;
      yes|on)
         AC_DEFINE([PNG_MIPS_MSA_OPT], [2],
                   [Enable MIPS MSA optimizations])
         AC_MSG_WARN([--enable-mips-msa: please specify 'check' or 'api';]
            [if you want the optimizations unconditionally,]
            [pass '-mmsa -mfp64' to the compiler.]);;
      *)
         AC_MSG_ERROR([--enable-mips-msa=${enable_mips_msa}:]
                      [invalid argument])
   esac])

# Add MIPS-specific files to all builds where $host_cpu is mips ('mips*') or
# where MIPS optimizations were explicitly requested. (This allows a fallback
# if a future host CPU does not match 'mips*'.)

AM_CONDITIONAL([PNG_MIPS_MSA],
   [test "$enable_mips_msa" != 'no' &&
    case "$host_cpu" in
      mipsel*|mips64el*) : ;;
    esac])

# MIPS MMI
# ========

AC_ARG_ENABLE([mips-mmi],
   AS_HELP_STRING([[[--enable-mips-mmi]]],
      [Enable MIPS MMI optimizations: =no/off, check, api, yes/on.]
      [no/off: disable the optimizations;]
      [check: use internal checking code (deprecated and poorly supported);]
      [api: disable by default, enable by a call to png_set_option;]
      [yes/on: turn on unconditionally.]
      [If not specified: determined by the compiler.]),
   [case "$enableval" in
      no|off)
         # disable the default enabling on __mips_mmi systems:
         AC_DEFINE([PNG_MIPS_MMI_OPT], [0],
                   [Disable MIPS MMI optimizations])
         # Prevent inclusion of the assembler files below:
         enable_mips_mmi=no;;
      check)
         AC_DEFINE([PNG_MIPS_MMI_CHECK_SUPPORTED], [],
                   [Check for MIPS MMI support at run-time]);;
      api)
         AC_DEFINE([PNG_MIPS_MMI_API_SUPPORTED], [],
                   [Turn on MIPS MMI optimizations at run-time]);;
      yes|on)
         AC_DEFINE([PNG_MIPS_MMI_OPT], [1],
                   [Enable MIPS MMI optimizations])
         AC_MSG_WARN([--enable-mips-mmi: please specify 'check' or 'api';]
            [if you want the optimizations unconditionally]
            [pass '-mloongson-mmi -march=loongson3a' to the compiler.]);;
      *)
         AC_MSG_ERROR([--enable-mips-mmi=${enable_mips_mmi}:]
                      [invalid argument])
   esac])

# Add MIPS specific files to all builds where the host_cpu is mips ('mips*') or
# where MIPS optimizations were explicitly requested. (This allows a fallback
# if a future host CPU does not match 'mips*'.)

AM_CONDITIONAL([PNG_MIPS_MMI],
   [test "$enable_mips_mmi" != 'no' &&
    case "$host_cpu" in
      mipsel*|mips64el*) : ;;
    esac])

# INTEL SSE
# =========

AC_ARG_ENABLE([intel-sse],
   AS_HELP_STRING([[[--enable-intel-sse]]],
      [Enable Intel SSE optimizations: =no/off, yes/on.]
      [no/off: disable the optimizations;]
      [yes/on: enable the optimizations.]
      [If not specified: determined by the compiler.]),
   [case "$enableval" in
      no|off)
         # disable the default enabling:
         AC_DEFINE([PNG_INTEL_SSE_OPT], [0],
                   [Disable Intel SSE optimizations])
         # Prevent inclusion of the assembler files below:
         enable_intel_sse=no ;;
      yes|on)
         AC_DEFINE([PNG_INTEL_SSE_OPT], [1],
                   [Enable Intel SSE optimizations]);;
      *)
         AC_MSG_ERROR([--enable-intel-sse=${enable_intel_sse}:]
                      [invalid argument])
   esac])

# Add Intel-specific files to all builds where $host_cpu is Intel ('x86*') or
# where Intel optimizations were explicitly requested. (This allows a fallback
# if a future host CPU does not match 'x86*'.)
AM_CONDITIONAL([PNG_INTEL_SSE],
   [test "$enable_intel_sse" != 'no' &&
    case "$host_cpu" in
      i?86|x86_64) : ;;
      *)    test "$enable_intel_sse" != '' ;;
    esac])

# POWERPC VSX
# ===========

AC_ARG_ENABLE([powerpc-vsx],
AS_HELP_STRING([[[--enable-powerpc-vsx]]],
      [Enable POWERPC VSX optimizations: =no/off, check, api, yes/on.]
      [no/off: disable the optimizations;]
      [check: use internal checking code;]
      [api: disable by default, enable by a call to png_set_option;]
      [yes/on: turn on unconditionally.]
      [If not specified: determined by the compiler.]),
   [case "$enableval" in
      no|off)
         # disable the default enabling on __ppc64__ systems:
         AC_DEFINE([PNG_POWERPC_VSX_OPT], [0],
                   [Disable POWERPC VSX optimizations])
         # Prevent inclusion of the platform-specific files below:
         enable_powerpc_vsx=no ;;
      check)
         AC_DEFINE([PNG_POWERPC_VSX_CHECK_SUPPORTED], [],
                   [Check for POWERPC VSX support at run-time])
         AC_MSG_WARN([--enable-powerpc-vsx: please see contrib/powerpc/README]
                     [for the list of supported systems.]);;
      api)
         AC_DEFINE([PNG_POWERPC_VSX_API_SUPPORTED], [],
                   [Turn on POWERPC VSX optimizations at run-time]);;
      yes|on)
         AC_DEFINE([PNG_POWERPC_VSX_OPT], [2],
                   [Enable POWERPC VSX optimizations])
         AC_MSG_WARN([--enable-powerpc-vsx: please specify 'check' or 'api';]
            [if you want the optimizations unconditionally,]
            [pass '-maltivec -mvsx' or '-mcpu=power8' to the compiler.]);;
      *)
         AC_MSG_ERROR([--enable-powerpc-vsx=${enable_powerpc_vsx}:]
                      [invalid argument])
   esac])

# Add PowerPC-specific files to all builds where $host_cpu is powerpc
# ('powerpc*') or where PowerPC optimizations were explicitly requested.
# (This allows a fallback if a future host CPU does not match 'powerpc*'.)

AM_CONDITIONAL([PNG_POWERPC_VSX],
   [test "$enable_powerpc_vsx" != 'no' &&
    case "$host_cpu" in
      powerpc*|ppc64*) : ;;
    esac])

# LOONGARCH LSX
# =============

AC_ARG_ENABLE([loongarch-lsx],
   AS_HELP_STRING([[[--enable-loongarch-lsx]]],
      [Enable LOONGARCH LSX optimizations: =no/off, yes/on:]
      [no/off: disable the optimizations;]
      [yes/on: turn on unconditionally.]
      [If not specified: determined by the compiler.]),
   [case "$enableval" in
      no|off)
         # disable the default enabling on __loongarch_simd systems:
         AC_DEFINE([PNG_LOONGARCH_LSX_OPT], [0],
                   [Disable LOONGARCH LSX optimizations])
         # Prevent inclusion of the assembler files below:
         enable_loongarch_lsx=no;;
      yes|on)
         AC_DEFINE([PNG_LOONGARCH_LSX_OPT], [1],
                   [Enable LOONGARCH LSX optimizations])
         ;;
      *)
         AC_MSG_ERROR([--enable-loongarch-lsx=${enable_loongarch_lsx}:]
                      [invalid argument])
   esac])

if test "$enable_loongarch_lsx" != "no" &&
   case "$host_cpu" in
      loongarch*) : ;;
      *)    test "$enable_loongarch_lsx" != '' ;;
   esac
then
   compiler_support_loongarch_lsx=no
   AC_MSG_CHECKING(whether to use LoongArch LSX intrinsics)
   save_CFLAGS="$CFLAGS"
   LSX_CFLAGS="${LSX_CFLAGS:-"-mlsx"}"
   CFLAGS="$CFLAGS $LSX_CFLAGS"
   AC_COMPILE_IFELSE([AC_LANG_SOURCE([[
#include <lsxintrin.h>
int main(){
   __m128i a, b, c;
   a = __lsx_vadd_w(b, c);
   return 0;
}]])],compiler_support_loongarch_lsx=yes)
   CFLAGS="$save_CFLAGS"
   AC_MSG_RESULT($compiler_support_loongarch_lsx)
   if test "$compiler_support_loongarch_lsx" = "yes"; then
      AC_DEFINE([PNG_LOONGARCH_LSX_OPT], [1],
                [Enable LOONGARCH LSX optimizations])
   else
      AC_MSG_WARN([Compiler does not support loongarch LSX.])
   fi
fi

# Add LoongArch specific files to all builds where the host_cpu is loongarch
# ('loongarch*') or where LoongArch optimizations were explicitly requested.
# (This allows a fallback if a future host CPU does not match 'loongarch*'.)

AM_CONDITIONAL([PNG_LOONGARCH_LSX],
   [test "$enable_loongarch_lsx" != "no" &&
    test "$compiler_support_loongarch_lsx" = "yes" &&
    case "$host_cpu" in
      loongarch*) : ;;
      *)    test "$enable_loongarch_lsx" != '' ;;
    esac])

# RISC-V
# ======
#
# RISC-V Vector support.

AC_ARG_ENABLE([riscv-rvv],
AS_HELP_STRING([[[--enable-riscv-rvv]]],
      [Enable RISC-V Vector optimizations: =no/off, check, api, yes/on:]
      [no/off: disable the optimizations; check: use internal checking code]
      [api: disable by default, enable by a call to png_set_option]
      [yes/on: turn on. If not specified: determined by the compiler.]),
   [case "$enableval" in
      no|off)
         # disable the default enabling on __riscv systems:
         AC_DEFINE([PNG_RISCV_RVV_OPT], [0],
                   [Disable RISC-V Vector optimizations])
         # Prevent inclusion of the platform-specific files below:
         enable_riscv_rvv=no ;;
      yes|on)
         AC_DEFINE([PNG_RISCV_RVV_OPT], [2],
                   [Enable RISC-V Vector optimizations])
         AC_MSG_WARN([--enable-riscv-rvv:]
            [if you want the optimizations pass e.g. '-march=rv64gv1p0' to the compiler.]);;
      *)
         AC_MSG_ERROR([--enable-riscv-rvv=${enable_riscv_rvv}: invalid value])
   esac])

if test "$enable_riscv_rvv" != "no" &&
   case "$host_cpu" in
      riscv64) : ;;
      *)    test "$enable_riscv_rvv" != '' ;;
   esac
then
   compiler_support_riscv_rvv=no
   AC_MSG_CHECKING(whether to use RISC-V RVV intrinsics)

   AC_COMPILE_IFELSE([AC_LANG_SOURCE([[
#include <riscv_vector.h>
#include <asm/hwcap.h>
#ifndef COMPAT_HWCAP_ISA_V /* added in linux-6.5 */
#error "COMPAT_HWCAP_ISA_V is not available"
#endif
int main() {
   const float src[] = { 0.0f, 0.0f, 0.0f, 0.0f };
   uint64_t ptr[2] = {0x0908060504020100, 0xFFFFFFFF0E0D0C0A};
   vuint8m1_t a = __riscv_vreinterpret_v_u64m1_u8m1(__riscv_vle64_v_u64m1(ptr, 2));
   vfloat32m1_t val = __riscv_vle32_v_f32m1((const float*)(src), 4);
   return (int)__riscv_vfmv_f_s_f32m1_f32(val);
}]])],compiler_support_riscv_rvv=yes)
   AC_MSG_RESULT($compiler_support_riscv_rvv)
   if test "$compiler_support_riscv_rvv" = "yes"; then
      AC_DEFINE([PNG_RISCV_RVV_OPT], [1],
                [Enable RISCV RVV optimizations])
   else
      AC_MSG_WARN([Compiler does not support riscv rvv.])
   fi
fi

# Add RISC-V-specific files to all builds where $host_cpu is riscv ('riscv64')
# or where RISC-V optimizations were explicitly requested (this allows a fallback
# if a future host CPU does not match 'riscv64')

AM_CONDITIONAL([PNG_RISCV_RVV],
   [test "$enable_riscv_rvv" != "no" &&
    test "$compiler_support_riscv_rvv" = "yes" &&
    case "$host_cpu" in
      riscv64) : ;;
      *)    test "$compiler_support_riscv_rvv" != '' ;;
    esac])

AC_MSG_NOTICE([[Extra options for compiler: $PNG_COPTS]])

# Config files, substituting as above
AC_CONFIG_FILES([Makefile libpng.pc:libpng.pc.in])
AC_CONFIG_FILES([libpng-config:libpng-config.in],
   [chmod +x libpng-config])

AC_OUTPUT
