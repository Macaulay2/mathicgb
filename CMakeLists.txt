cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(mathicgb VERSION 1.4 LANGUAGES CXX)

option(enable_mgb "Build mgb" ON)

# ON requires TBB, OFF builds without it, and auto uses it if it can be found.
set(with_tbb auto CACHE STRING
  "use TBB for multithreading: ON, OFF, or auto to use it if it is found")
set_property(CACHE with_tbb PROPERTY STRINGS auto ON OFF)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

find_package(Threads 2.1 REQUIRED QUIET)
find_package(Memtailor   REQUIRED)
find_package(Mathic      REQUIRED)
# The auto case has to be tested first, as if(with_tbb) is true for "auto".
# Every branch resolves with_tbb to a plain boolean, which shadows the cache
# entry for the generator expressions in src/CMakeLists.txt.
if(with_tbb STREQUAL "auto")
  find_package(TBB)
  if(TBB_FOUND)
    set(with_tbb ON)
  else()
    set(with_tbb OFF)
    message(WARNING "TBB not detected.  Compiling without multithreading "
      "and without precise timing.")
  endif()
elseif(with_tbb)
  find_package(TBB       REQUIRED)
  set(with_tbb ON)
else()
  set(with_tbb OFF)
endif()
message(STATUS "Multithreading with TBB: ${with_tbb}")

add_library(memtailor STATIC IMPORTED)
set_target_properties(memtailor PROPERTIES
  IMPORTED_LOCATION             "${MEMTAILOR_LIBRARIES}"
  INTERFACE_INCLUDE_DIRECTORIES "${MEMTAILOR_INCLUDE_DIR}")

add_library(mathic STATIC IMPORTED)
set_target_properties(mathic PROPERTIES
  IMPORTED_LOCATION             "${MATHIC_LIBRARIES}"
  INTERFACE_INCLUDE_DIRECTORIES "${MATHIC_INCLUDE_DIR}")

include(GNUInstallDirs)

add_subdirectory(src)

set_target_properties(mathicgb PROPERTIES PUBLIC_HEADER src/mathicgb.h)

install(TARGETS mathicgb LIBRARY DESTINATION lib)
install(DIRECTORY src/mathicgb
  DESTINATION include
  FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h"
  )
install(FILES doc/mgb.1 DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
install(FILES README.md gpl-2.0.txt gpl-3.0.txt
  DESTINATION licenses/mathicgb
  )

if(enable_mgb)
  add_executable(mgb
    src/cli/GBMain.cpp
    src/cli/CommonParams.hpp   src/cli/CommonParams.cpp
    src/cli/GBAction.hpp       src/cli/GBAction.cpp
    src/cli/GBCommonParams.hpp src/cli/GBCommonParams.cpp
    src/cli/MatrixAction.hpp   src/cli/MatrixAction.cpp
    src/cli/SigGBAction.hpp    src/cli/SigGBAction.cpp
    src/cli/HelpAction.hpp     src/cli/HelpAction.cpp
    src/cli/VersionAction.hpp  src/cli/VersionAction.cpp
    )
  target_link_libraries(mgb mathicgb)
  install(TARGETS mgb RUNTIME DESTINATION bin)
endif()
