cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(mathicgb VERSION 1.0 LANGUAGES CXX)

option(enable_mgb "Build mgb" ON)
option(with_tbb "use TBB for multithreading" auto)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

find_package(Threads 2.1 REQUIRED QUIET)
find_package(Memtailor   REQUIRED)
find_package(Mathic      REQUIRED)
if(with_tbb)
  find_package(TBB       REQUIRED)
elseif(with_tbb STREQUAL auto)
  find_package(TBB)
  set(with_tbb ${TBB_FOUND})
endif()

add_library(memtailor STATIC IMPORTED)
set_target_properties(memtailor PROPERTIES
  IMPORTED_LOCATION             "${MEMTAILOR_LIBRARIES}"
  INTERFACE_INCLUDE_DIRECTORIES "${MEMTAILOR_INCLUDE_DIR}")

add_library(mathic STATIC IMPORTED)
set_target_properties(mathic PROPERTIES
  IMPORTED_LOCATION             "${MATHIC_LIBRARIES}"
  INTERFACE_INCLUDE_DIRECTORIES "${MATHIC_INCLUDE_DIR}")

add_compile_options(
  -DHAVE_CXX11=1
  -DPACKAGE_NAME="${PROJECT_NAME}"
  -DPACKAGE_TARNAME="${PROJECT_NAME}"
  -DPACKAGE_VERSION="${PROJECT_VERSION}"
  -DPACKAGE_STRING="${PROJECT_NAME} ${PROJECT_VERSION}"
  -DPACKAGE_BUGREPORT=""
  -DPACKAGE_URL=""
  -DPACKAGE="${PROJECT_NAME}"
  $<$<NOT:$<BOOL:${with_tbb}>>:-DMATHICGB_NO_TBB>
  -Wall -Wextra -Wno-unused-parameter -Wno-unused-value -Wno-unused-variable -Wno-sign-compare
  )

add_subdirectory(src)

set_target_properties(mathicgb PROPERTIES PUBLIC_HEADER src/mathicgb.h)

install(TARGETS mathicgb LIBRARY DESTINATION lib)
install(DIRECTORY src/mathicgb
  DESTINATION include
  FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h"
  )
install(FILES doc/mgb.1 TYPE MAN)
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
    )
  target_link_libraries(mgb mathicgb)
  install(TARGETS mgb RUNTIME DESTINATION bin)
endif()
