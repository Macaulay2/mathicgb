// MathicGB copyright 2012 all rights reserved. MathicGB comes with ABSOLUTELY
// NO WARRANTY and is licensed as GPL v2.0 or later - see LICENSE.txt.
#include "mathicgb/stdinc.h"
#include "mathicgb/CFile.hpp"

#include <gtest/gtest.h>
#include <cstdio>

using namespace mgb;

TEST(CFile, CloseIsIdempotent) {
  const char* const fileName = "CFile-test.tmp";
  {
    CFile file(fileName, "wb");
    ASSERT_TRUE(file.hasFile());
    file.close();
    ASSERT_FALSE(file.hasFile());
    file.close(); // must not close the file a second time
  } // and neither must the destructor
  ASSERT_EQ(0, std::remove(fileName));
}
