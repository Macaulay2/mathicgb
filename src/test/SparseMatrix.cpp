// MathicGB copyright 2012 all rights reserved. MathicGB comes with ABSOLUTELY
// NO WARRANTY and is licensed as GPL v2.0 or later - see LICENSE.txt.
#include "mathicgb/stdinc.h"
#include "mathicgb/SparseMatrix.hpp"

#include "mathicgb/Poly.hpp"
#include "mathicgb/PolyRing.hpp"
#include "mathicgb/io-util.hpp"
#include "mathicgb/MathicIO.hpp"
#include "mathicgb/CFile.hpp"
#include <gtest/gtest.h>
#include <cstdio>
#include <memory>
#include <string>
#include <vector>

using namespace mgb;

namespace {
  std::unique_ptr<Poly> parsePoly(const PolyRing& ring, std::string str) {
    std::istringstream in(str);
    Scanner scanner(in);
    return make_unique<Poly>(MathicIO<>().readPoly(ring, false, scanner));
  }
}

TEST(SparseMatrix, NoRows) {
  SparseMatrix mat; // test a matrix with no rows
  ASSERT_EQ(0, mat.entryCount());
  ASSERT_EQ(0, mat.rowCount());
  ASSERT_EQ(0, mat.computeColCount());
  ASSERT_EQ("matrix with no rows\n", mat.toString());
}

TEST(SparseMatrix, Simple) {
  SparseMatrix mat;

  mat.appendEntry(5, 101);
  mat.rowDone();
  ASSERT_EQ(1, mat.entryCount());
  ASSERT_EQ(1, mat.rowCount());
  ASSERT_EQ(6, mat.computeColCount());
  ASSERT_EQ(5, mat.leadCol(0));
  ASSERT_EQ(1, mat.entryCountInRow(0));
  ASSERT_EQ("0: 5#101\n", mat.toString());
  ASSERT_FALSE(mat.emptyRow(0));

  mat.rowDone(); // add a row with no entries
  ASSERT_EQ(1, mat.entryCount());
  ASSERT_EQ(2, mat.rowCount());
  ASSERT_EQ(6, mat.computeColCount());
  ASSERT_EQ(5, mat.leadCol(0));
  ASSERT_EQ(0, mat.entryCountInRow(1));
  ASSERT_EQ("0: 5#101\n1:\n", mat.toString());
  ASSERT_TRUE(mat.emptyRow(1));

  mat.appendEntry(5, 102);
  mat.appendEntry(2001, 0); // scalar zero
  mat.rowDone(); // add a row with two entries
  ASSERT_EQ(3, mat.entryCount());
  ASSERT_EQ(3, mat.rowCount());
  ASSERT_EQ(2002, mat.computeColCount());
  ASSERT_EQ(5, mat.leadCol(2));
  ASSERT_EQ(2, mat.entryCountInRow(2));
  ASSERT_EQ("0: 5#101\n1:\n2: 5#102 2001#0\n", mat.toString());
  ASSERT_FALSE(mat.emptyRow(2));
}

TEST(SparseMatrix, toRow) {
  auto ring = ringFromString("32003 6 1\n1 1 1 1 1 1");
  auto polyForMonomials = parsePoly(*ring, "a5+a4+a3+a2+a1+a0");
  std::vector<PolyRing::Monoid::ConstMonoPtr> monomials;
  for (auto it = polyForMonomials->begin(); it != polyForMonomials->end(); ++it)
    monomials.push_back(it.mono().ptr());

  SparseMatrix mat(5);
  mat.clear();
  mat.rowDone();
  mat.appendEntry(0,10);
  mat.rowDone();
  mat.appendEntry(2,20);
  mat.appendEntry(3,0);
  mat.appendEntry(4,40);
  mat.rowDone();

  Poly p(*ring);
  mat.rowToPolynomial(0, monomials, p);
  ASSERT_EQ(*parsePoly(*ring, "0"), p);
  mat.rowToPolynomial(1, monomials, p);
  ASSERT_EQ(*parsePoly(*ring, "10a5"), p);
  mat.rowToPolynomial(2, monomials, p);
  ASSERT_EQ(*parsePoly(*ring, "20a3+40a1"), p);
}

TEST(SparseMatrix, ReadRejectsAnOversizedModulus) {
  const char* const fileName = "SparseMatrix-modulus-test.tmp";
  SparseMatrix mat;
  mat.appendEntry(0, 5);
  mat.rowDone();

  // write() takes a Scalar, so the only way to get an out-of-range modulus
  // into a file is to patch the field, which sits after two uint32s.
  {
    CFile file(fileName, "wb");
    mat.write(101, file.handle());
  }
  {
    CFile file(fileName, "r+b");
    const uint32 modulus = 65637; // 65536 + 101, so it truncates to 101
    ASSERT_EQ(0, std::fseek(file.handle(), 2 * sizeof(uint32), SEEK_SET));
    ASSERT_EQ(1, std::fwrite(&modulus, sizeof(modulus), 1, file.handle()));
  }

  SparseMatrix read;
  {
    CFile file(fileName, "rb");
    ASSERT_THROW(read.read(file.handle()), mathic::MathicException);
  }
  ASSERT_EQ(0, std::remove(fileName));
}

TEST(SparseMatrix, ReadRejectsACompositeModulus) {
  const char* const fileName = "SparseMatrix-composite-modulus-test.tmp";
  SparseMatrix mat;
  mat.appendEntry(0, 5);
  mat.rowDone();

  // write() does not check, so a composite modulus goes straight into the
  // file -- no patching needed, unlike the oversized case above.
  {
    CFile file(fileName, "wb");
    mat.write(100, file.handle());
  }

  SparseMatrix read;
  {
    CFile file(fileName, "rb");
    ASSERT_THROW(read.read(file.handle()), mathic::MathicException);
  }
  ASSERT_EQ(0, std::remove(fileName));
}

// Build rows of many lengths with small memory quanta, so that rows often
// outgrow their block while still being built and reserveFreeEntries has to
// move their pending entries into a new block. GCC 11 and 12 on s390x
// miscompiled that move at -O2, dropping the last pending column index while
// keeping all of the scalars (https://github.com/Macaulay2/M2/issues/2162).
TEST(SparseMatrix, RowsSpanningBlocks) {
  typedef SparseMatrix::Scalar Scalar;
  const auto check = [](const size_t quantum,
                        const std::vector<size_t>& lengths) {
    SCOPED_TRACE("memory quantum " + std::to_string(quantum));
    const auto scalarFor = [](SparseMatrix::ColIndex col) {
      return static_cast<Scalar>(1 + col % 30000);
    };
    SparseMatrix mat(quantum);
    SparseMatrix::ColIndex col = 0;
    for (const size_t len : lengths) {
      for (size_t i = 0; i < len; ++i, ++col)
        mat.appendEntry(col, scalarFor(col));
      mat.rowDone();
    }

    ASSERT_EQ(lengths.size(), mat.rowCount());
    col = 0;
    for (SparseMatrix::RowIndex row = 0; row < mat.rowCount(); ++row) {
      SCOPED_TRACE("row " + std::to_string(row));
      ASSERT_EQ(lengths[row], mat.entryCountInRow(row));
      for (auto it = mat.rowBegin(row); it != mat.rowEnd(row); ++it, ++col) {
        ASSERT_EQ(col, it.index());
        ASSERT_EQ(scalarFor(col), it.scalar());
      }
    }
  };

  std::vector<size_t> lengths;
  for (size_t len = 0; len < 60; ++len)
    lengths.push_back(len);
  for (const size_t quantum : {1, 2, 3, 7, 64})
    check(quantum, lengths);

  // With no quantum, blocks start at 2^14 entries and double, so it takes
  // long rows to outgrow one.
  lengths.insert(lengths.end(), {20000, 3, 40000, 2});
  check(0, lengths);
}
