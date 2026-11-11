# Try to find the mathic libraries
# See https://github.com/Macaulay2/mathic
#
# This file sets up mathic for CMake. Once done this will define
#  MATHIC_FOUND             - system has MATHIC lib
#  MATHIC_INCLUDE_DIR       - the MATHIC include directory
#  MATHIC_LIBRARIES         - Libraries needed to use MATHIC
#  mathic::mathic           - imported target, with mathic's ABI flags
#
# Copyright (c) 2020, Mahrud Sayrafi, <mahrud@umn.edu>
#
# Redistribution and use is allowed according to the terms of the BSD license.

# Prefer mathic's CMake config, then pkg-config; both record MATHIC_DEBUG.
find_package(mathic CONFIG QUIET)
if(TARGET mathic::mathic)
  get_target_property(MATHIC_INCLUDE_DIR mathic::mathic
    INTERFACE_INCLUDE_DIRECTORIES)
  set(MATHIC_LIBRARIES mathic::mathic)
else()
  find_package(PkgConfig QUIET)
  if(PKG_CONFIG_FOUND)
    pkg_check_modules(PC_MATHIC QUIET mathic)
  endif()

  find_path(MATHIC_INCLUDE_DIR NAMES mathic.h
    HINTS ${PC_MATHIC_INCLUDE_DIRS}
    PATHS ${INCLUDE_INSTALL_DIR} ${CMAKE_INSTALL_PREFIX}/include
    PATH_SUFFIXES mathic
    )
  find_library(MATHIC_LIBRARIES NAMES mathic
    HINTS ${PC_MATHIC_LIBRARY_DIRS}
    PATHS ${LIB_INSTALL_DIR} ${CMAKE_INSTALL_PREFIX}/lib
    )
endif()

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Mathic DEFAULT_MSG MATHIC_INCLUDE_DIR MATHIC_LIBRARIES)

mark_as_advanced(MATHIC_INCLUDE_DIR MATHIC_LIBRARIES)

if(MATHIC_FOUND AND NOT TARGET mathic::mathic)
  add_library(mathic::mathic UNKNOWN IMPORTED)
  set_target_properties(mathic::mathic PROPERTIES
    IMPORTED_LOCATION "${MATHIC_LIBRARIES}"
    INTERFACE_INCLUDE_DIRECTORIES "${MATHIC_INCLUDE_DIR}"
    INTERFACE_COMPILE_OPTIONS "${PC_MATHIC_CFLAGS_OTHER}"
    INTERFACE_LINK_LIBRARIES memtailor::memtailor)
endif()
