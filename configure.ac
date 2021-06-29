dnl AC_INIT sets up autoconf and must be first macro.
AC_INIT([mathicgb], [1.0]) # package, version, bug-report-email
AC_DEFINE_UNQUOTED(MATHICGB_VERSION, "$PACKAGE_VERSION")

# set up information about directories
AC_CONFIG_MACRO_DIR([build/autotools/m4]) # directory of extra autoconf macroes
AC_CONFIG_AUX_DIR([build/autotools]) # directory for auxiliary build tools (install-sh etc)

# check that source directory is correct
dnl  if autoconf is told the source code is in a directory that does not
dnl  contain this file then it knows that the directory is wrong.
AC_CONFIG_SRCDIR([src/mathicgb.h])

# Check availability and location of dependencies
# PKG_CHECK_MODULES([MEMTAILOR], [memtailor])
# PKG_CHECK_MODULES([MATHIC], [mathic])
MEMTAILOR_LIBS="-lmemtailor"
MATHIC_LIBS="-lmathic"

# Locate the C++ compiler.
AC_PROG_CXX
AC_LANG([C++])

# Require C++11 support
AX_CXX_COMPILE_STDCXX_11(, [mandatory])

AM_PROG_AR

dnl ----- The gtest dependency

AC_ARG_WITH([gtest], AS_HELP_STRING(
    [--with-gtest], [use gtest, which is required for running the unit tests
      with make check. Per the recommendation
      of the gtest documentation, gtest is compiled with the tests, so an
      installed gtest is not usable - you need the gtest source. GTEST_PATH
      indicates where to look for gtest and it is also where gtest
      will be downloaded to if not found or if it is found but
      the version is too small. The default path is srcdir/libs so
      that gtest needs to be at srcdir/libs/gtest/ where srcdir is the
      base of the directory being configured from.  Use
      --with-gtest=download to force a download.]),
      [],
      [with_gtest=yes]
)

AC_MSG_CHECKING([for gtest])
AS_IF([test "x$GTEST_PATH" == "x"], [GTEST_PATH="$srcdir/libs"])
AS_IF([test "x$GTEST_VERSION" == "x"], [GTEST_VERSION="1.10.0"])

AS_IF([test "x$with_gtest" == "xyes"],
  [AS_IF([test -f "$GTEST_PATH/src/gtest-all.cc"],
    [AC_MSG_RESULT([yes])]
    [AC_MSG_CHECKING([if gtest is version >= 1.10.0])
     tmp_CPPFLAGS="$CPPFLAGS"
     CPPFLAGS="$CPPFLAGS -I$GTEST_PATH/include -I$GTEST_PATH"
     AC_COMPILE_IFELSE(
       [AC_LANG_PROGRAM(
         [[#include <gtest/gtest.h>]],
         [[TYPED_TEST_SUITE(int, int);]])],
       [AC_MSG_RESULT([yes])
        with_gtest=yes],
       [AC_MSG_RESULT([no])
        AC_MSG_WARN([gtest version >= 1.10.0 is required; will download])
        with_gtest=download])
     CPPFLAGS="$tmp_CPPFLAGS"],
    [AC_MSG_RESULT([no])]
    [AC_MSG_WARN([$GTEST_PATH/src/gtest-all.cc not found, will download gtest])
      with_gtest=download])])

AS_IF([test "x$with_gtest" == "xdownload"],
  [ AC_MSG_RESULT([downloading])
    mkdir -p "$GTEST_PATH";
    AS_IF([test -w $GTEST_PATH], [],
      [AC_MSG_ERROR([you do not have permission to write to $GTEST_PATH for downloading gtest; try again with a different path])])
    (
      cd $GTEST_PATH;
      rm -rf release-$GTEST_VERSION.zip
      wget https://github.com/google/googletest/archive/release-$GTEST_VERSION.zip
      unzip -q release-$GTEST_VERSION.zip;
      rm release-$GTEST_VERSION.zip
      rsync -a googletest-release-$GTEST_VERSION/googletest/* .
      rm -r googletest-release-$GTEST_VERSION
    );
    if test ! -e "$GTEST_PATH/src/gtest-all.cc"; then
      AC_MSG_WARN([Failed to download or extract gtest.]);
      with_gtest="no";
    else
      AC_MSG_RESULT([Successfully downloaded and extracted gtest.]);
      with_gtest="yes";
    fi
  ],
  [test "x$with_gtest" == "xyes"], [],
  [test "x$with_gtest" == "xno"],
    [AC_MSG_RESULT([skipping])
      AC_MSG_WARN([building without gtest; you will not be able to run the unit tests with make check.])],
  [AC_MSG_ERROR([invalid value $with_gtest for with_gtest.])]
)
AS_IF([test "x$with_gtest" == "xyes"],
  [GTEST_CFLAGS="-I$GTEST_PATH/include -I$GTEST_PATH"]);
AM_CONDITIONAL(with_gtest, test "x$with_gtest" == "xyes")

dnl ----- The TBB dependency
AC_ARG_WITH([tbb], AS_HELP_STRING(
    [--with-tbb], [use TBB, which is required for multithreading. The value
      detect, which is the default, enables TBB if it can be found and
      otherwise prints a warning and continues the build without
      multithreading support. TBB is not available for Cygwin (last checked
      March 2013).]
))
AS_IF([test "x$with_tbb" == "x"], [with_tbb="detect"])
AS_IF(
  [test "x$with_tbb" == "xdetect"],
  [PKG_CHECK_MODULES([TBB], [tbb], [with_tbb="yes"], [with_tbb="no";
    AC_MSG_WARN([TBB not detected. Compiling without multithreading and without precise timing.])
  ])],
  [test "x$with_tbb" == "xyes"], [PKG_CHECK_MODULES([TBB], [tbb])],    
  [test "x$with_tbb" == "xno"], [],
  [AC_MSG_ERROR([invalid value $with_tbb for with_tbb.])]
)
AS_IF([test "x$with_tbb" == "xno"], [TBB_CFLAGS="-DMATHICGB_NO_TBB"])

dnl ----- The librt dependency
dnl On Linux TBB calls clock_gettime, which requires librt, but librt is not
dnl linked in automatically. So we need to check for that.

dnl the first AC_LINK_IFELSE causes tests for lots of C++ related things,
dnl and these print out messages. So to avoid those messages appearing
dnl after "if librt..." and before the result for that test, we do an
dnl empty AC_LINK_IFELSE. Probably there is a better way.
AC_LINK_IFELSE([AC_LANG_SOURCE([[]])], [], [])

dnl We check if -lrt is necessary. We need librt if we are building with TBB,
dnl if linking and compiling works when linking with librt but it doesn't
dnl without linking with librt.
AS_IF([test "x$with_tbb" == "xyes"],
  [AC_MSG_CHECKING([if librt is needed to support TBB on this platform]);
  oldLIBS=$LIBS;
  LIBS="$TBB_LIBS -lrt $LIBS";
  oldCFLAGS=$CFLAGS;
  CFLAGS="$CFLAGS $TBB_CFLAGS";
  AC_LINK_IFELSE(
    [AC_LANG_PROGRAM(
      [[#]include [<]tbb/tbb.h[>]], [[tbb::tick_count::now();]]
    )],
    [LIBS=$oldLibs; AC_LINK_IFELSE(
      [AC_LANG_PROGRAM(
        [[#]include [<]tbb/tbb.h[>]], [[tbb::tick_count::now();]]
      )],
      [AC_MSG_RESULT([no])],
      [AC_MSG_RESULT([yes]); RT_LIBS="-lrt"]
    )],
    [AC_MSG_RESULT([no])]
  )];
  LIBS=$oldLIBS;
  CFLAGS=$oldCFLAGS;
)

DEPS_CFLAGS="$MEMTAILOR_CFLAGS $MATHIC_CFLAGS $TBB_CFLAGS $GTEST_CFLAGS"
DEPS_LIBS="$MEMTAILOR_LIBS $MATHIC_LIBS $TBB_LIBS $RT_LIBS"

AC_SUBST(DEPS_CFLAGS)
AC_SUBST(DEPS_LIBS)

# Enable optional maintainer mode (off by default)
dnl AM_MAINTAINER_MODE turns off automatic reconstruction of the build
dnl files if the source build files have changed. A developer will want
dnl those automatic reconstructions to happen so that changes to the
dnl build system are actually carried out. However, a user might not
dnl have the tools required to reconfigure and the need for
dnl reconstruction might be spurious if the last-modified date is set
dnl incorrectly on the build files.
dnl
dnl Passing the option [enable] to AM_MAINTAINER_MODE makes the
dnl non-reconstruction feature available, but only when turned on by
dnl passing the option –disable-maintainer-mode. This option is
dnl apparently useful to some package distributors.
AM_MAINTAINER_MODE([enable])

# Set up Automake
dnl foreign: do not create the GNU-specific file COPYING and do not complain
dnl   that GNU-specific files like NEWS, README, AUTHORS and ChangeLog are
dnl   missing.
dnl -Wall: set Automake to emit all warnings it can. Is NOT RELATED to setting
dnl   warnings for other tools. For example, it wil not make the compiler
dnl   get a -Wall option.
dnl subdir-objects: Put object files in a directory structure based on
dnl   the directory structure of the source files. This way, two source
dnl   files with the same name in different directories do not conflict.
AM_INIT_AUTOMAKE([foreign subdir-objects -Wall])

# if --enable-silent-rules is passed to ./configure or if V=0 is passed
# to make, then the compilation output will be much less verbose making
# it possible to spot warnings and errors as they go by.
AM_SILENT_RULES()

# Set up the $(LN_S) macro, which creates symbolic links
AC_PROG_LN_S

# set output variable INSTALL to the name of a BSD-compatible install program.
# Requires install-sh to be present as a fallback, even on systems where
# the fallback is not used.
AC_PROG_INSTALL

# Set up LibTool
LT_INIT([disable-shared])

dnl Set the version for the library -- this concerns compatibility of the
dnl source and binary interface of the library and is not the same as the
dnl version of the project.
AC_SUBST([MATHICGB_SO_VERSION], [0:0:0])

dnl Set up AC_OUTPUT to create each file by copying an input file
dnl while substituting the output variable values.
AC_CONFIG_FILES([Makefile
  build/autotools/mathicgb.pc:build/autotools/mathicgb.pc.in])

dnl Macro that is required to be at the end of any Autoconf script.
dnl Creates config.status and launches it.
AC_OUTPUT
