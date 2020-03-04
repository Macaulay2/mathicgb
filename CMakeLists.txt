cmake_minimum_required(VERSION 3.12)

project(mathicgb VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(VERSION="1.0")

find_package(TBB REQUIRED)

find_package(Threads 2.1 REQUIRED QUIET)
find_package(PkgConfig REQUIRED QUIET)
find_package(Memtailor REQUIRED memtailor)
find_package(Mathic    REQUIRED mathic)

add_library(mathicgb
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

target_link_libraries(mathicgb ${MATHIC_LIBRARIES} ${MEMTAILOR_LIBRARIES} Threads::Threads TBB)
target_include_directories(mathicgb
  PUBLIC
    src
    ${MATHIC_INCLUDE_DIR}
    ${MEMTAILOR_INCLUDE_DIR}
  )

install(TARGETS mathicgb ARCHIVE DESTINATION lib)
install(FILES src/mathicgb.h DESTINATION include)
install(DIRECTORY src/mathicgb/
  DESTINATION include/mathicgb
  FILES_MATCHING PATTERN "*.h"
#  PATTERN stdinc.h EXCLUDE
  )

option(enable_mgb "Build mgb" OFF)
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
endif()

option(PACKAGE_TESTS "Build the tests" ON)
if(PACKAGE_TESTS)
  enable_testing()
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
    src/test/gtestInclude.cpp
    src/test/testMain.cpp
    )

  target_link_libraries(mathicgb-gtests mathicgb)

################################
# add gtest testing ############
################################

include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.8.0
)

FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
  FetchContent_Populate(googletest)
endif()

target_include_directories(mathicgb-gtests
  PRIVATE
  ${googletest_SOURCE_DIR}/googletest/include
  ${googletest_SOURCE_DIR}/googletest/src
  ${googletest_SOURCE_DIR}/googletest
  )

add_test(NAME mathicgb-gtests
      COMMAND mathicgb-gtests
  )
endif()
