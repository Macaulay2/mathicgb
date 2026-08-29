// MathicGB copyright 2012 all rights reserved. MathicGB comes with ABSOLUTELY
// NO WARRANTY and is licensed as GPL v2.0 or later - see LICENSE.txt.
#include "mathicgb/stdinc.h"
#include "mathicgb/PrimeField.hpp"

#include <gtest/gtest.h>
#include <sstream>
#include <vector>

using namespace mgb;

namespace {
  template<class T>
  std::string toString(T&& t) {
    std::ostringstream out;
    out << static_cast<uint64>(t.value());
    return out.str();
  }
}

TEST(PrimeField, Charac) {
  const PrimeField<unsigned char> pf(11);
  ASSERT_EQ(pf.charac(), 11);
}

TEST(PrimeField, OneZero) {
  const PrimeField<unsigned char> pfChar(11);
  ASSERT_EQ("0", toString(pfChar.zero()));
  ASSERT_EQ("1", toString(pfChar.one()));
  ASSERT_TRUE(pfChar.isOne(pfChar.one()));
  ASSERT_FALSE(pfChar.isOne(pfChar.zero()));
  ASSERT_FALSE(pfChar.isZero(pfChar.one()));
  ASSERT_TRUE(pfChar.isZero(pfChar.zero()));

  const PrimeField<unsigned long> pfLong(11);
  ASSERT_EQ("0", toString(pfLong.zero()));
  ASSERT_EQ("1", toString(pfLong.one()));
  ASSERT_TRUE(pfLong.isOne(pfLong.one()));
  ASSERT_FALSE(pfLong.isOne(pfLong.zero()));
  ASSERT_FALSE(pfLong.isZero(pfLong.one()));
  ASSERT_TRUE(pfLong.isZero(pfLong.zero()));
}

TEST(PrimeField, toElement) {
  const auto max32BitUnsignedPrime = 4294967291u;
  const PrimeField<unsigned int> pf(max32BitUnsignedPrime);

  // Same number of bits (32)
  ASSERT_EQ("0", toString(pf.toElement(0)));
  ASSERT_EQ("1", toString(pf.toElement(1)));
  ASSERT_EQ("4294967290", toString(pf.toElement(-1)));

  ASSERT_EQ("0", toString(pf.toElement(max32BitUnsignedPrime)));
  ASSERT_EQ("1", toString(pf.toElement(max32BitUnsignedPrime + 1u)));
  ASSERT_EQ("4294967290", toString(pf.toElement(max32BitUnsignedPrime - 1u)));

  ASSERT_EQ("4",
    toString(pf.toElement(std::numeric_limits<unsigned int>::max())));
  ASSERT_EQ("2147483643",
    toString(pf.toElement(std::numeric_limits<int>::min())));

  // Fewer number of bits (8)
  ASSERT_EQ("127", toString(pf.toElement(std::numeric_limits<signed char>::max())));
  ASSERT_EQ("4294967163",
    toString(pf.toElement(std::numeric_limits<signed char>::min())));

  ASSERT_EQ("255",
    toString(pf.toElement(std::numeric_limits<unsigned char>::max())));
  ASSERT_EQ("0",
    toString(pf.toElement(std::numeric_limits<unsigned char>::min())));

  // More bits (64)
  ASSERT_EQ("24", toString(pf.toElement(std::numeric_limits<uint64>::max())));
  ASSERT_EQ("2147483657",
    toString(pf.toElement(std::numeric_limits<int64>::max())));
  ASSERT_EQ("2147483633",
    toString(pf.toElement(std::numeric_limits<int64>::min())));
}

TEST(PrimeField, Sum) {
  const PrimeField<unsigned char> pf2(2);
  ASSERT_EQ(pf2.zero(), pf2.sum(pf2.zero(), pf2.zero()));
  ASSERT_EQ(pf2.one(), pf2.sum(pf2.one(), pf2.zero()));
  ASSERT_EQ(pf2.one(), pf2.sum(pf2.zero(), pf2.one()));
  ASSERT_EQ(pf2.zero(), pf2.sum(pf2.one(), pf2.one()));

  const PrimeField<unsigned char> pf251(251);
  ASSERT_EQ(pf251.one(), pf251.sum(pf251.zero(), pf251.one()));
  ASSERT_EQ(pf251.toElement(-3),
    pf251.sum(pf251.toElement(-1), pf251.toElement(-2)));

  const PrimeField<unsigned char> pf101(101);
  ASSERT_EQ(pf101.toElement(100),
    pf101.sum(pf101.toElement(40), pf101.toElement(60)));
  ASSERT_EQ(pf101.toElement(9),
    pf101.sum(pf101.toElement(50), pf101.toElement(60)));
}

TEST(PrimeField, Negative) {
  const PrimeField<unsigned char> pf2(2);
  ASSERT_EQ(pf2.zero(), pf2.negative(pf2.zero()));
  ASSERT_EQ(pf2.one(), pf2.negative(pf2.one()));
  ASSERT_EQ(pf2.one(), pf2.negativeNonZero(pf2.one()));

  const PrimeField<unsigned char> pf251(251);
  ASSERT_EQ(pf251.zero(), pf251.negative(pf251.zero()));
  ASSERT_EQ(pf251.toElement(100), pf251.negative(pf251.toElement(151)));
  ASSERT_EQ(pf251.toElement(100), pf251.negativeNonZero(pf251.toElement(151)));
}

TEST(PrimeField, Difference) {
  const PrimeField<unsigned char> pf2(2);
  ASSERT_EQ(pf2.zero(), pf2.difference(pf2.zero(), pf2.zero()));
  ASSERT_EQ(pf2.one(), pf2.difference(pf2.one(), pf2.zero()));
  ASSERT_EQ(pf2.one(), pf2.difference(pf2.zero(), pf2.one()));
  ASSERT_EQ(pf2.zero(), pf2.difference(pf2.one(), pf2.one()));

  const PrimeField<unsigned char> pf251(251);
  ASSERT_EQ(pf251.one(), pf251.difference(pf251.one(), pf251.zero()));
  ASSERT_EQ(pf251.toElement(-3),
    pf251.difference(pf251.toElement(1), pf251.toElement(4)));

  const PrimeField<unsigned char> pf101(101);
  ASSERT_EQ(pf101.toElement(20),
    pf101.difference(pf101.toElement(60), pf101.toElement(40)));
  ASSERT_EQ(pf101.toElement(-20),
    pf101.difference(pf101.toElement(40), pf101.toElement(60)));
}

TEST(PrimeField, Product) {
  const PrimeField<unsigned char> pf2(2);
  ASSERT_EQ(pf2.zero(), pf2.product(pf2.zero(), pf2.zero()));
  ASSERT_EQ(pf2.zero(), pf2.product(pf2.one(), pf2.zero()));
  ASSERT_EQ(pf2.zero(), pf2.product(pf2.zero(), pf2.one()));
  ASSERT_EQ(pf2.one(), pf2.product(pf2.one(), pf2.one()));

  const PrimeField<unsigned char> pf251(251);
  ASSERT_EQ(pf251.zero(), pf251.product(pf251.one(), pf251.zero()));
  ASSERT_EQ(pf251.one(), pf251.product(pf251.one(), pf251.one()));
  ASSERT_EQ(pf251.one(),
    pf251.product(pf251.toElement(-1), pf251.toElement(-1)));
  ASSERT_EQ(pf251.one(),
    pf251.product(pf251.toElement(2), pf251.toElement(126)));

  const PrimeField<unsigned char> pf101(101);
  ASSERT_EQ(pf101.toElement(20),
    pf101.product(pf101.toElement(5), pf101.toElement(4)));
  ASSERT_EQ(pf101.toElement(-20),
    pf101.product(pf101.toElement(-2), pf101.toElement(10)));

  const PrimeField<uint16> pf16(65521);
  ASSERT_EQ(pf16.toElement(-20),
    pf16.product(pf16.toElement(-2), pf16.toElement(10)));

  const PrimeField<uint32> pf32(4294967291u);
  ASSERT_EQ(pf32.toElement(-20),
    pf32.product(pf32.toElement(-2), pf32.toElement(10)));
}

TEST(PrimeField, Inverse) {
  const PrimeField<unsigned char> pf2(2);
  ASSERT_EQ(pf2.one(), pf2.inverse(pf2.one()));

  const PrimeField<unsigned char> pf251(251);
  ASSERT_EQ(pf251.one(), pf2.inverse(pf251.one()));
  ASSERT_EQ(pf251.toElement(-1), pf251.inverse(pf251.toElement(-1)));
  ASSERT_EQ(pf251.toElement(235), pf251.inverse(pf251.toElement(47)));

  const PrimeField<uint16> pf16(65521);
  ASSERT_EQ(pf16.one(), pf16.inverse(pf16.one()));
  ASSERT_EQ(pf16.toElement(-1), pf16.inverse(pf16.toElement(-1)));
  ASSERT_EQ(pf16.toElement(43216), pf16.inverse(pf16.toElement(47)));

  const PrimeField<uint32> pf32(4294967291u);
  ASSERT_EQ(pf32.one(), pf32.inverse(pf32.one()));
  ASSERT_EQ(pf32.toElement(-1), pf32.inverse(pf32.toElement(-1)));
  ASSERT_EQ(pf32.toElement(3015615332u), pf32.inverse(pf32.toElement(47)));
}

TEST(PrimeField, Quotient) {
  const PrimeField<unsigned char> pf2(2);
  ASSERT_EQ(pf2.one(), pf2.quotient(pf2.one(), pf2.one()));
  ASSERT_EQ(pf2.zero(), pf2.quotient(pf2.zero(), pf2.one()));

  const PrimeField<unsigned char> pf251(251);
  ASSERT_EQ(pf251.one(), pf2.quotient(pf251.one(), pf251.one()));
  ASSERT_EQ
    (pf251.toElement(-1), pf251.quotient(pf251.one(), pf251.toElement(-1)));
  ASSERT_EQ
    (pf251.toElement(-1), pf251.quotient(pf251.toElement(-1), pf251.one()));
  ASSERT_EQ(
    pf251.one(),
    pf251.quotient(pf251.toElement(-1), pf251.toElement(-1))
  );
  ASSERT_EQ(
    pf251.toElement(203),
    pf251.quotient(pf251.toElement(3), pf251.toElement(47))
  );

  const PrimeField<uint16> pf16(65521);
  ASSERT_EQ(
    pf16.toElement(20911),
    pf16.quotient(pf16.toElement(2), pf16.toElement(47))
  );

  const PrimeField<uint32> pf32(4294967291u);
  ASSERT_EQ(
    pf32.toElement(3015615332u),
    pf32.quotient(pf32.toElement(1), pf32.toElement(47))
  );
}

TEST(PrimeField, PlusOne) {
  const PrimeField<unsigned char> pf2(2);
  ASSERT_EQ(pf2.zero(), pf2.plusOne(pf2.one()));
  ASSERT_EQ(pf2.one(), pf2.plusOne(pf2.zero()));

  const PrimeField<unsigned char> pf251(251);
  ASSERT_EQ(pf251.one(), pf251.plusOne(pf251.zero()));
  ASSERT_EQ(pf251.zero(), pf251.plusOne(pf251.toElement(-1)));
  ASSERT_EQ(pf251.toElement(250), pf251.plusOne(pf251.toElement(249)));

  const PrimeField<uint16> pf16(65521);
  ASSERT_EQ(pf16.toElement(20911), pf16.plusOne(pf16.toElement(20910)));

  const PrimeField<uint32> pf32(4294967291u);
  ASSERT_EQ
    (pf32.toElement(3015615332u), pf32.plusOne(pf32.toElement(3015615331u)));
}

TEST(PrimeField, MaxCharacteristic) {
  // Limited by what the type can store.
  ASSERT_EQ(255u, PrimeField<uint8>::maxCharacteristic());
  ASSERT_EQ(65535u, PrimeField<uint16>::maxCharacteristic());
  ASSERT_EQ(4294967295u, PrimeField<uint32>::maxCharacteristic());

  // Limited by the product of two elements having to fit in 64 bits.
  ASSERT_EQ(static_cast<uint64>(1) << 32, PrimeField<uint64>::maxCharacteristic());
  ASSERT_EQ(
    static_cast<unsigned long>(1) << 32,
    PrimeField<unsigned long>::maxCharacteristic()
  );

  // It has to be usable as a constant, since the constructor compares
  // against it on paths that makeField() reaches.
  static_assert(PrimeField<uint16>::maxCharacteristic() == 65535, "");
}

TEST(PrimeField, CharacteristicTooLarge) {
  typedef PrimeField<unsigned long> PF;
  const auto max = PF::maxCharacteristic();

  ASSERT_NO_THROW((void)PF(max));
  ASSERT_NO_THROW((void)PF(4294967291u)); // largest 32 bit prime
  ASSERT_THROW((void)PF(max + 1), mathic::MathicException);
  ASSERT_THROW((void)PF((static_cast<unsigned long>(1) << 40) + 15),
    mathic::MathicException);
}

TEST(PrimeField, IsPrime) {
  ASSERT_FALSE(isPrime(0));
  ASSERT_FALSE(isPrime(1));
  ASSERT_TRUE(isPrime(2));
  ASSERT_TRUE(isPrime(3));
  ASSERT_FALSE(isPrime(4));

  // The bases the test uses. Without special casing these it reports them
  // composite, as the base is then zero modulo n.
  ASSERT_TRUE(isPrime(7));
  ASSERT_TRUE(isPrime(61));

  ASSERT_TRUE(isPrime(101));
  ASSERT_TRUE(isPrime(32003));      // used by the ideals in the test suite
  ASSERT_TRUE(isPrime(65521));      // largest 16 bit prime
  ASSERT_FALSE(isPrime(65535));
  ASSERT_TRUE(isPrime(10000019));
  ASSERT_TRUE(isPrime(2147483647));  // 2^31 - 1
  ASSERT_TRUE(isPrime(4294967291u)); // largest 32 bit prime
  ASSERT_FALSE(isPrime(4294967295u));  // 3 * 5 * 17 * 257 * 65537
  ASSERT_FALSE(isPrime(static_cast<uint64>(1) << 32));

  // A strong pseudoprime to the bases 2, 3, 5 and 7, so a more obvious
  // choice of bases would report it prime.
  ASSERT_FALSE(isPrime(3215031751u));

  // Agree with a sieve on a range large enough to catch a systematic error.
  const unsigned int sieveMax = 100000;
  std::vector<bool> sieve(sieveMax + 1, true);
  sieve[0] = sieve[1] = false;
  for (unsigned long i = 2; i * i <= sieveMax; ++i)
    if (sieve[i])
      for (unsigned long j = i * i; j <= sieveMax; j += i)
        sieve[j] = false;
  for (unsigned int n = 0; n <= sieveMax; ++n)
    ASSERT_EQ(sieve[n], isPrime(n)) << "disagreed with the sieve at n = " << n;
}
