// MathicGB copyright 2012 all rights reserved. MathicGB comes with ABSOLUTELY
// NO WARRANTY and is licensed as GPL v2.0 or later - see LICENSE.txt.
#include "mathicgb/stdinc.h"
#include "mathicgb/MonoLookup.hpp"

#include "mathicgb/ModuleMonoSet.hpp"
#include "mathicgb/PolyRing.hpp"
#include "mathicgb/io-util.hpp"
#include <gtest/gtest.h>
#include <mathic.h>

using namespace mgb;

TEST(MonoLookup, RejectsAnUnknownCode) {
  auto ring = ringFromString("101 2 1\n1 1");
  ASSERT_THROW(
    MonoLookup::makeFactory(ring->monoid(), 99),
    mathic::MathicException
  );
  for (int code = 1; code <= 4; ++code)
    ASSERT_NO_THROW(MonoLookup::makeFactory(ring->monoid(), code));
}

TEST(ModuleMonoSet, RejectsAnUnknownCode) {
  auto ring = ringFromString("101 2 1\n1 1");
  ASSERT_THROW(
    ModuleMonoSet::make(ring->monoid(), 99, 1, true),
    mathic::MathicException
  );
  for (int code = 1; code <= 4; ++code)
    ASSERT_NO_THROW(ModuleMonoSet::make(ring->monoid(), code, 1, true));
}
