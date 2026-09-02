// MathicGB copyright 2012 all rights reserved. MathicGB comes with ABSOLUTELY
// NO WARRANTY and is licensed as GPL v2.0 or later - see LICENSE.txt.
#include "mathicgb/stdinc.h"
#include "mathicgb/QuadMatrix.hpp"

#include "mathicgb/CFile.hpp"
#include <gtest/gtest.h>
#include <cstdio>

using namespace mgb;

TEST(QuadMatrix, ReadWritesAllFourSubmatrices) {
  const SparseMatrix::Scalar modulus = 101;

  // A distinct entry per quadrant, so that an empty quadrant and a quadrant
  // holding another's data both show up.
  QuadMatrix in;
  in.topLeft.appendEntry(0, 11);
  in.topLeft.rowDone();
  in.topRight.appendEntry(1, 22);
  in.topRight.rowDone();
  in.bottomLeft.appendEntry(2, 33);
  in.bottomLeft.rowDone();
  in.bottomRight.appendEntry(3, 44);
  in.bottomRight.rowDone();

  const char* const fileName = "QuadMatrix-test.tmp";
  {
    CFile file(fileName, "wb");
    in.write(modulus, file.handle());
  }

  QuadMatrix out;
  {
    CFile file(fileName, "rb");
    ASSERT_EQ(modulus, out.read(file.handle()));
  }
  ASSERT_EQ(0, std::remove(fileName));

  ASSERT_EQ(in.topLeft.toString(), out.topLeft.toString());
  ASSERT_EQ(in.topRight.toString(), out.topRight.toString());
  ASSERT_EQ(in.bottomLeft.toString(), out.bottomLeft.toString());
  ASSERT_EQ(in.bottomRight.toString(), out.bottomRight.toString());
}
