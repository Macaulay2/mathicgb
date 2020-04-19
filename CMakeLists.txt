cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(mathicgb VERSION 1.0 LANGUAGES CXX)

option(with_tbb "use TBB for multithreading" OFF)

find_package(Threads 2.1 REQUIRED QUIET)
find_package(Memtailor   REQUIRED)
find_package(Mathic      REQUIRED)
if(with_tbb)
  find_package(TBB       REQUIRED)
else()
  find_package(TBB)
  set(with_tbb ${TBB_FOUND})
endif()

include(CheckIncludeFiles)
check_include_files("stdlib.h;stdarg.h;string.h;float.h" STDC_HEADERS)
check_include_files(sys/types.h	HAVE_SYS_TYPES_H)
check_include_files(sys/stat.h	HAVE_SYS_STAT_H)
check_include_files(stdlib.h	HAVE_STDLIB_H)
check_include_files(string.h	HAVE_STRING_H)
check_include_files(memory.h	HAVE_MEMORY_H)
check_include_files(strings.h	HAVE_STRINGS_H)
check_include_files(inttypes.h	HAVE_INTTYPES_H)
check_include_files(stdint.h	HAVE_STDINT_H)
check_include_files(unistd.h	HAVE_UNISTD_H)
check_include_files(dlfcn.h	HAVE_DLFCN_H)

add_compile_options(
  -g -O2
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
  # TODO
  -DLT_OBJDIR=".libs/"
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

target_link_libraries(mathicgb ${MATHIC_LIBRARIES} ${MEMTAILOR_LIBRARIES} Threads::Threads)
if(with_tbb)
  target_link_libraries(mathicgb ${TBB_LIBRARIES})
endif()
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
install(FILES README.md gpl-2.0.txt gpl-3.0.txt
  DESTINATION licenses/mathicgb
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
  install(TARGETS mgb RUNTIME DESTINATION bin)
endif()

option(BUILD_TESTING "Build the tests" ON)
if(BUILD_TESTING)
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
    src/test/testMain.cpp
    )

  ################################
  # add gtest testing ############
  ################################
  find_package(GTest)
  include(GoogleTest)
  gtest_discover_tests(mathicgb-gtests)
  add_test(NAME mathicgb-gtests COMMAND mathicgb-gtests)

  if(GTEST_FOUND)
    target_link_libraries(mathicgb-gtests mathicgb GTest::GTest GTest::Main)
    target_include_directories(mathicgb-gtests PRIVATE ${GTEST_INCLUDE_DIR})
  else()
    include(FetchContent)
    FetchContent_Declare(googletest
      GIT_REPOSITORY https://github.com/google/googletest.git
      GIT_TAG        release-1.10.0
      )
    FetchContent_MakeAvailable(googletest)

    target_link_libraries(mathicgb-gtests mathicgb gtest)
    target_include_directories(mathicgb-gtests PRIVATE
      ${googletest_SOURCE_DIR}/googletest/include
      ${googletest_SOURCE_DIR}/googletest/src
      ${googletest_SOURCE_DIR}/googletest
      )
  endif()
endif()
