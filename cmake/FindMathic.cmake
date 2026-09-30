# Prefer the upstream config; pkg-config also carries the dependency's ABI flags.
# Copyright (c) 2020, Mahrud Sayrafi, <mahrud@umn.edu>
# Redistribution and use is allowed according to the terms of the BSD license.
if(NOT TARGET mathic::mathic)
  find_package(mathic CONFIG QUIET)
endif()
if(NOT TARGET mathic::mathic)
  find_package(PkgConfig QUIET)
  if(PKG_CONFIG_FOUND)
    pkg_check_modules(MATHIC QUIET IMPORTED_TARGET mathic)
    if(TARGET PkgConfig::MATHIC)
      add_library(mathic::mathic INTERFACE IMPORTED)
      set_property(TARGET mathic::mathic PROPERTY
        INTERFACE_LINK_LIBRARIES PkgConfig::MATHIC)
    endif()
  endif()
endif()
# Older installations may provide only headers and a library.
if(NOT TARGET mathic::mathic)
  find_path(MATHIC_INCLUDE_DIR NAMES mathic.h
    PATHS ${INCLUDE_INSTALL_DIR} ${CMAKE_INSTALL_PREFIX}/include
    PATH_SUFFIXES mathic)
  find_library(MATHIC_LIBRARY NAMES mathic
    PATHS ${LIB_INSTALL_DIR} ${CMAKE_INSTALL_PREFIX}/lib)
  if(MATHIC_INCLUDE_DIR AND MATHIC_LIBRARY)
    add_library(mathic::mathic UNKNOWN IMPORTED)
    set_target_properties(mathic::mathic PROPERTIES
      IMPORTED_LOCATION "${MATHIC_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${MATHIC_INCLUDE_DIR}"
      INTERFACE_LINK_LIBRARIES memtailor::memtailor)
  endif()
  mark_as_advanced(MATHIC_INCLUDE_DIR MATHIC_LIBRARY)
endif()
set(MATHIC_FOUND FALSE)
if(TARGET mathic::mathic)
  set(MATHIC_FOUND TRUE)
  set(MATHIC_LIBRARIES mathic::mathic)
endif()
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Mathic DEFAULT_MSG MATHIC_FOUND)
