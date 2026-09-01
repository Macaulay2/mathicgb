// MathicGB copyright 2012 all rights reserved. MathicGB comes with ABSOLUTELY
// NO WARRANTY and is licensed as GPL v2.0 or later - see LICENSE.txt.
#ifndef MATHICGB_ATOMIC_GUARD
#define MATHICGB_ATOMIC_GUARD

// We need this include for std::memory_order even if we are not
// using std::atomic.
#include <atomic>

MATHICGB_NAMESPACE_BEGIN

namespace AtomicInternal {
#ifdef MATHICGB_USE_FAKE_ATOMIC
  // This class has the same interface as the actual custom atomic
  // class but it does absolutely no synchronization and it does not
  // constrain compiler optimizations in any way. The purpose of this class
  // is to enable it while running only a single thread to determine the
  // overhead imposed by the atomic operations.
  template<class T>
  class FakeAtomic {
  public:
    FakeAtomic(): mValue() {}
    FakeAtomic(T value): mValue(value) {}
    T load(const std::memory_order) const {return mValue;}
    void store(const T value, const std::memory_order order) {mValue = value;}

  private:
    T mValue;
  };

  template<class T, size_t size>
  struct ChooseAtomic {
    typedef FakeAtomic<T> type;
  };

#else
  /// Class for deciding which implementation of atomic to use. The default is
  /// to use std::atomic which is a fine choice if std::atomic is implemented
  /// in a reasonable way by the standard library implementation you are using.
  template<class T, size_t size>
  struct ChooseAtomic {
    typedef std::atomic<T> type;
  };
#endif
}

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

  typename AtomicInternal::ChooseAtomic<T, sizeof(T)>::type mValue;
};

MATHICGB_NAMESPACE_END

#endif
