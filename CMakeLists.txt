cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 14)
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

if(CMAKE_BUILD_TYPE STREQUAL Debug)
  add_compile_options(-DMATHICGB_DEBUG -DMATHIC_DEBUG -DMEMT_DEBUG)
endif()

add_compile_options(
  -DHAVE_CXX11=1
  -DPACKAGE_NAME="${PROJECT_NAME}"
  -DPACKAGE_TARNAME="${PROJECT_NAME}"
  -DPACKAGE_VERSION="${PROJECT_VERSION}"
  -DPACKAGE_STRING="${PROJECT_NAME} ${PROJECT_VERSION}"
  -DPACKAGE_BUGREPORT=""
  -DPACKAGE_URL=""
  -DPACKAGE="${PROJECT_NAME}"
  -DVERSION="${PROJECT_VERSION}"
  $<$<NOT:$<BOOL:${with_tbb}>>:-DMATHICGB_NO_TBB>
  )

add_library(mathicgb STATIC
  src/mathicgb/MonoArena.hpp
  src/mathicgb/Range.hpp
  src/mathicgb/mtbb.hpp
  src/mathicgb/PrimeField.hpp
  src/mathicgb/MonoMonoid.hpp
  src/mathicgb/MonoProcessor.hpp
  src/mathicgb/MonoOrder.hpp
  src/mathicgb/Unchar.hpp
  src/mathicgb/MathicIO.hpp
  src/mathicgb/NonCopyable.hpp
  src/mathicgb/StaticMonoMap.hpp
  src/mathicgb/KoszulQueue.hpp
  src/mathicgb/Poly.hpp
  src/mathicgb/ReducerHelper.hpp
  src/mathicgb/MonomialMap.hpp
  src/mathicgb/RawVector.hpp
  src/mathicgb/Atomic.hpp
  src/mathicgb/ScopeExit.hpp
  src/mathicgb/FixedSizeMonomialMap.h # change name?
  src/mathicgb/ReducerPack.hpp        src/mathicgb/ReducerPack.cpp
  src/mathicgb/ClassicGBAlg.hpp       src/mathicgb/ClassicGBAlg.cpp
  src/mathicgb/MonoLookup.hpp         src/mathicgb/MonoLookup.cpp
  src/mathicgb/SigPolyBasis.hpp       src/mathicgb/SigPolyBasis.cpp
  src/mathicgb/Basis.hpp              src/mathicgb/Basis.cpp
  src/mathicgb/ModuleMonoSet.hpp      src/mathicgb/ModuleMonoSet.cpp
  src/mathicgb/PolyBasis.hpp          src/mathicgb/PolyBasis.cpp
  src/mathicgb/PolyHashTable.hpp      src/mathicgb/PolyHashTable.cpp
  src/mathicgb/PolyRing.hpp           src/mathicgb/PolyRing.cpp
  src/mathicgb/Reducer.hpp            src/mathicgb/Reducer.cpp
  src/mathicgb/ReducerDedup.hpp       src/mathicgb/ReducerDedup.cpp
  src/mathicgb/ReducerHash.hpp        src/mathicgb/ReducerHash.cpp
  src/mathicgb/ReducerHashPack.hpp    src/mathicgb/ReducerHashPack.cpp
  src/mathicgb/ReducerNoDedup.hpp     src/mathicgb/ReducerNoDedup.cpp
  src/mathicgb/ReducerPackDedup.hpp   src/mathicgb/ReducerPackDedup.cpp
  src/mathicgb/SignatureGB.hpp        src/mathicgb/SignatureGB.cpp
  src/mathicgb/SigSPairs.hpp          src/mathicgb/SigSPairs.cpp
  src/mathicgb/SPairs.hpp             src/mathicgb/SPairs.cpp
  src/mathicgb/SigSPairQueue.hpp      src/mathicgb/SigSPairQueue.cpp
  src/mathicgb/SparseMatrix.hpp       src/mathicgb/SparseMatrix.cpp
  src/mathicgb/QuadMatrixBuilder.hpp  src/mathicgb/QuadMatrixBuilder.cpp
  src/mathicgb/TypicalReducer.hpp     src/mathicgb/TypicalReducer.cpp
  src/mathicgb/F4Reducer.hpp          src/mathicgb/F4Reducer.cpp
  src/mathicgb/F4MatrixBuilder.hpp    src/mathicgb/F4MatrixBuilder.cpp
  src/mathicgb/QuadMatrix.hpp         src/mathicgb/QuadMatrix.cpp
  src/mathicgb/F4MatrixReducer.hpp    src/mathicgb/F4MatrixReducer.cpp
  src/mathicgb/CFile.hpp              src/mathicgb/CFile.cpp
  src/mathicgb/LogDomain.hpp          src/mathicgb/LogDomain.cpp
  src/mathicgb/LogDomainSet.hpp       src/mathicgb/LogDomainSet.cpp
  src/mathicgb/F4MatrixBuilder2.hpp   src/mathicgb/F4MatrixBuilder2.cpp
  src/mathicgb/F4ProtoMatrix.hpp      src/mathicgb/F4ProtoMatrix.cpp
  src/mathicgb/F4MatrixProjection.hpp src/mathicgb/F4MatrixProjection.cpp
  src/mathicgb/io-util.hpp            src/mathicgb/io-util.cpp
  src/mathicgb/Scanner.hpp            src/mathicgb/Scanner.cpp
  src/mathicgb/stdinc.h
  src/mathicgb.cpp                    src/mathicgb.h
  )

target_link_libraries(mathicgb
  $<$<BOOL:${with_tbb}>:${TBB_LIBRARIES}>
  ${MEMTAILOR_LIBRARIES}
  ${MATHIC_LIBRARIES}
  Threads::Threads
  )
target_include_directories(mathicgb
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
    ${MEMTAILOR_INCLUDE_DIR}
    ${MATHIC_INCLUDE_DIR}
  )
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

include(CTest)
if(BUILD_TESTING)
  add_executable(mathicgb-gtests
    src/test/F4MatrixBuilder.cpp
    src/test/F4MatrixReducer.cpp
    src/test/MathicIO.cpp
    src/test/MonoMonoid.cpp
    src/test/PrimeField.cpp
    src/test/QuadMatrixBuilder.cpp
    src/test/Range.cpp
    src/test/Scanner.cpp
    src/test/SparseMatrix.cpp
    src/test/gb-test.cpp
    src/test/ideals.hpp src/test/ideals.cpp
    src/test/mathicgb.cpp
    src/test/poly-test.cpp
    src/test/testMain.cpp
    )

  ################################
  # add gtest testing ############
  ################################
  find_package(GTest)
  include(GoogleTest)
  gtest_discover_tests(mathicgb-gtests)

  # Check for recent version of GoogleTest
  include(CheckFunctionExists)
  set(CMAKE_REQUIRED_INCLUDES "${GTEST_INCLUDE_DIRS}")
  check_function_exists(TYPED_TEST_SUITE GTEST_VERSION_OK)

  if(GTEST_FOUND AND GTEST_VERSION_OK)
    target_link_libraries(mathicgb-gtests mathicgb GTest::GTest GTest::Main)
    target_include_directories(mathicgb-gtests PRIVATE ${GTEST_INCLUDE_DIR})
  else()
    include(FetchContent)
    FetchContent_Declare(googletest
      GIT_REPOSITORY https://github.com/google/googletest.git
      GIT_TAG        release-1.10.0
      )
    set(BUILD_GMOCK OFF CACHE STRING "...")
    FetchContent_MakeAvailable(googletest)

    target_link_libraries(mathicgb-gtests mathicgb gtest)
    target_include_directories(mathicgb-gtests PRIVATE
      ${googletest_SOURCE_DIR}/googletest/include
      ${googletest_SOURCE_DIR}/googletest/src
      ${googletest_SOURCE_DIR}/googletest
      )
  endif()
endif()
