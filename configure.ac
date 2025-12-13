# Note: also update version in CMakeLists.txt
dnl AC_INIT sets up autoconf and must be first macro.
AC_INIT([mathicgb], [1.1]) # package, version, bug-report-email

# set up information about directories
AC_CONFIG_MACRO_DIR([build/autotools/m4]) # directory of extra autoconf macroes
AC_CONFIG_AUX_DIR([build/autotools]) # directory for auxiliary build tools (install-sh etc)

# check that source directory is correct
dnl  if autoconf is told the source code is in a directory that does not
dnl  contain this file then it knows that the directory is wrong.
AC_CONFIG_SRCDIR([src/mathicgb.h])

# Locate the C++ compiler.
AC_PROG_CXX
AC_LANG([C++])

# Require C++11 support
AX_CXX_COMPILE_STDCXX_11(, [mandatory])

AM_PROG_AR

# Check availability and location of dependencies
AC_LANG([C++])
AC_DEFUN([NO_MEMTAILOR_ERROR], AC_MSG_ERROR([memtailor is required]))
AC_CHECK_HEADER([memtailor.h],, [NO_MEMTAILOR_ERROR])
AC_SEARCH_LIBS([libmemtailorIsPresent], [memtailor],, [NO_MEMTAILOR_ERROR])

AC_LANG([C++])
AC_DEFUN([NO_MATHIC_ERROR], AC_MSG_ERROR([mathic is required]))
AC_CHECK_HEADER([mathic.h],, [NO_MATHIC_ERROR])
AC_SEARCH_LIBS([libmathicIsPresent], [mathic],, [NO_MATHIC_ERROR])

dnl ----- The gtest dependency

AC_ARG_WITH([gtest],
    [AS_HELP_STRING([--with-gtest],
        [use gtest, which is required for running the unit tests
         with make check [default=yes].])],,
    [with_gtest=yes])

AC_DEFUN([NO_GTEST_ERROR],
    [AC_MSG_ERROR([gtest not found; try again using --without-gtest])])

AS_IF([test "x$with_gtest" != "xno"],
     [AC_LANG([C++])
      AC_CHECK_HEADER([gtest/gtest.h],
         [AC_MSG_CHECKING([for library containing testing::InitGoogleTest])
          SAVELIBS=$LIBS
          LIBS="$LIBS -lgtest -pthread"
          AC_LINK_IFELSE(
              [AC_LANG_PROGRAM([
                  #include <gtest/gtest.h>
                  ], [
                  testing::InitGoogleTest()])],
              [AC_MSG_RESULT([-lgtest])],
              [AC_MSG_RESULT([no])
               NO_GTEST_ERROR])
          LIBS=$SAVELIBS],
         [NO_GTEST_ERROR])])

AM_CONDITIONAL([with_gtest], [test "x$with_gtest" != "xno"])

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
  [PKG_CHECK_MODULES([TBB], [tbb], [with_tbb="yes"],
    [PKG_CHECK_MODULES([TBB], [tbb32], [with_tbb="yes"], [with_tbb="no";
      AC_MSG_WARN([TBB not detected. Compiling without multithreading and without precise timing.])])
  ])],
  [test "x$with_tbb" == "xyes"], [PKG_CHECK_MODULES([TBB], [tbb],,
    [PKG_CHECK_MODULES([TBB], [tbb32])])],
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

DEPS_CFLAGS="$TBB_CFLAGS"
DEPS_LIBS="$TBB_LIBS $RT_LIBS"

AC_SUBST(DEPS_CFLAGS)
AC_SUBST(DEPS_LIBS)

AC_ARG_ENABLE([cli],
    AS_HELP_STRING([--disable-cli],
	[disable building the command line interface]),
    [case "${enableval}" in
	yes) cli=true ;;
	no)  cli=false ;;
	*) AC_MSG_ERROR([bad value "${enableval}" for --enable-cli]) ;;
     esac],
    [cli=true])
AM_CONDITIONAL([CLI], [test x$cli = xtrue])

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
AC_SUBST([MATHICGB_SO_VERSION], [0:1:0])

dnl Set up AC_OUTPUT to create each file by copying an input file
dnl while substituting the output variable values.
AC_CONFIG_FILES([Makefile
  build/autotools/mathicgb.pc:build/autotools/mathicgb.pc.in])

dnl Macro that is required to be at the end of any Autoconf script.
dnl Creates config.status and launches it.
AC_OUTPUT
