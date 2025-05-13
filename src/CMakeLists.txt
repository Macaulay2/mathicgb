if(CMAKE_BUILD_TYPE STREQUAL Debug)
  add_compile_options(-DMATHICGB_DEBUG -DMATHIC_DEBUG -DMEMT_DEBUG)
endif()

add_library(mathicgb STATIC
  mathicgb.h
  mathicgb.cpp
  mathicgb/MonoArena.hpp
  mathicgb/Range.hpp
  mathicgb/mtbb.hpp
  mathicgb/PrimeField.hpp
  mathicgb/MonoMonoid.hpp
  mathicgb/MonoProcessor.hpp
  mathicgb/MonoOrder.hpp
  mathicgb/Unchar.hpp
  mathicgb/MathicIO.hpp
  mathicgb/NonCopyable.hpp
  mathicgb/StaticMonoMap.hpp
  mathicgb/KoszulQueue.hpp
  mathicgb/Poly.hpp
  mathicgb/ReducerHelper.hpp
  mathicgb/MonomialMap.hpp
  mathicgb/RawVector.hpp
  mathicgb/Atomic.hpp
  mathicgb/ScopeExit.hpp
  mathicgb/FixedSizeMonomialMap.h # change name?
  mathicgb/ReducerPack.hpp        mathicgb/ReducerPack.cpp
  mathicgb/ClassicGBAlg.hpp       mathicgb/ClassicGBAlg.cpp
  mathicgb/MonoLookup.hpp         mathicgb/MonoLookup.cpp
  mathicgb/SigPolyBasis.hpp       mathicgb/SigPolyBasis.cpp
  mathicgb/Basis.hpp              mathicgb/Basis.cpp
  mathicgb/ModuleMonoSet.hpp      mathicgb/ModuleMonoSet.cpp
  mathicgb/PolyBasis.hpp          mathicgb/PolyBasis.cpp
  mathicgb/PolyHashTable.hpp      mathicgb/PolyHashTable.cpp
  mathicgb/PolyRing.hpp           mathicgb/PolyRing.cpp
  mathicgb/Reducer.hpp            mathicgb/Reducer.cpp
  mathicgb/ReducerDedup.hpp       mathicgb/ReducerDedup.cpp
  mathicgb/ReducerHash.hpp        mathicgb/ReducerHash.cpp
  mathicgb/ReducerHashPack.hpp    mathicgb/ReducerHashPack.cpp
  mathicgb/ReducerNoDedup.hpp     mathicgb/ReducerNoDedup.cpp
  mathicgb/ReducerPackDedup.hpp   mathicgb/ReducerPackDedup.cpp
  mathicgb/SignatureGB.hpp        mathicgb/SignatureGB.cpp
  mathicgb/SigSPairs.hpp          mathicgb/SigSPairs.cpp
  mathicgb/SPairs.hpp             mathicgb/SPairs.cpp
  mathicgb/SigSPairQueue.hpp      mathicgb/SigSPairQueue.cpp
  mathicgb/SparseMatrix.hpp       mathicgb/SparseMatrix.cpp
  mathicgb/QuadMatrixBuilder.hpp  mathicgb/QuadMatrixBuilder.cpp
  mathicgb/TypicalReducer.hpp     mathicgb/TypicalReducer.cpp
  mathicgb/F4Reducer.hpp          mathicgb/F4Reducer.cpp
  mathicgb/F4MatrixBuilder.hpp    mathicgb/F4MatrixBuilder.cpp
  mathicgb/QuadMatrix.hpp         mathicgb/QuadMatrix.cpp
  mathicgb/F4MatrixReducer.hpp    mathicgb/F4MatrixReducer.cpp
  mathicgb/CFile.hpp              mathicgb/CFile.cpp
  mathicgb/LogDomain.hpp          mathicgb/LogDomain.cpp
  mathicgb/LogDomainSet.hpp       mathicgb/LogDomainSet.cpp
  mathicgb/F4MatrixBuilder2.hpp   mathicgb/F4MatrixBuilder2.cpp
  mathicgb/F4ProtoMatrix.hpp      mathicgb/F4ProtoMatrix.cpp
  mathicgb/F4MatrixProjection.hpp mathicgb/F4MatrixProjection.cpp
  mathicgb/io-util.hpp            mathicgb/io-util.cpp
  mathicgb/Scanner.hpp            mathicgb/Scanner.cpp
  mathicgb/stdinc.h
  )

target_compile_options(mathicgb PUBLIC
  $<$<NOT:$<BOOL:${with_tbb}>>:-DMATHICGB_NO_TBB>
  )

target_link_libraries(mathicgb
  memtailor mathic Threads::Threads
  $<$<BOOL:${with_tbb}>:TBB::tbb>
  )

target_include_directories(mathicgb PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>
  )

include(CTest)
if(BUILD_TESTING)
  add_executable(mathicgb-unit-tests
    test/F4MatrixBuilder.cpp
    test/F4MatrixReducer.cpp
    test/MathicIO.cpp
    test/MonoMonoid.cpp
    test/PrimeField.cpp
    test/QuadMatrixBuilder.cpp
    test/Range.cpp
    test/Scanner.cpp
    test/SparseMatrix.cpp
    test/gb-test.cpp
    test/ideals.hpp test/ideals.cpp
    test/mathicgb.cpp
    test/poly-test.cpp
    test/testMain.cpp
    )

  ################################
  # add gtest testing ############
  ################################
  if(NOT GTEST_FOUND)
    find_package(GTest)
  endif()
  include(GoogleTest)
  gtest_discover_tests(mathicgb-unit-tests TEST_PREFIX unit-tests:)

  if(GTEST_FOUND)
    target_link_libraries(mathicgb-unit-tests mathicgb GTest::GTest GTest::Main)
  else()
    include(FetchContent)
    FetchContent_Declare(googletest
      GIT_REPOSITORY https://github.com/google/googletest.git
      GIT_TAG        v1.16.0
      )
    FetchContent_MakeAvailable(googletest)

    target_link_libraries(mathicgb-unit-tests mathicgb gtest)
    target_include_directories(mathicgb-unit-tests PRIVATE
      ${googletest_SOURCE_DIR}/googletest/include
      ${googletest_SOURCE_DIR}/googletest/src
      ${googletest_SOURCE_DIR}/googletest
      )
  endif()
endif()
