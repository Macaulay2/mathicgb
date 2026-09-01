// MathicGB copyright 2012 all rights reserved. MathicGB comes with ABSOLUTELY
// NO WARRANTY and is licensed as GPL v2.0 or later - see LICENSE.txt.
#ifndef MATHICGB_ATOMIC_GUARD
#define MATHICGB_ATOMIC_GUARD

#include <atomic>

MATHICGB_NAMESPACE_BEGIN

/// This class is equivalent to std::atomic<T>. Some functions from the
/// interface of std::atomic are missing - add them as necessary. Do not add
/// operator= and operator T() --- it is better to make the code explicit
/// about when and how loading and storing of atomic variables occurs.
///
/// We force all the functions to be inline because they can contain switches
/// on the value of std::memory_order. This will usually be a compile-time
/// constant parameter so that after inlining the switch will disappear. Yet
/// the code size of the switch may make some compilers avoid the inline.
template<class T>
class Atomic {
public:
  Atomic(): mValue() {}
  Atomic(T value): mValue(value) {}

  MATHICGB_INLINE
  T load(const std::memory_order order = std::memory_order_seq_cst) const {
    MATHICGB_ASSERT(debugAligned());
    return mValue.load(order);
  }

  MATHICGB_INLINE
  void store(
    const T value,
    const std::memory_order order = std::memory_order_seq_cst
  ) {
    MATHICGB_ASSERT(debugAligned());
    mValue.store(value, order);
  }

private:
  Atomic(const Atomic<T>&); // not available
  void operator=(const Atomic<T>&); // not available

  bool debugAligned() const {
    return reinterpret_cast<size_t>(&mValue) % sizeof(T) == 0;
  }

  std::atomic<T> mValue;
};

MATHICGB_NAMESPACE_END

#endif
