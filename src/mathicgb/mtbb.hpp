// MathicGB copyright 2012 all rights reserved. MathicGB comes with ABSOLUTELY
// NO WARRANTY and is licensed as GPL v2.0 or later - see LICENSE.txt.
#ifndef MATHICGB_M_TBB_GUARD
#define MATHICGB_M_TBB_GUARD

/// A compatibility layer for tbb. If we are compiling with tbb present, then
/// these classes will simply be the same classes as in tbb. However, if we
/// are compiling without tbb (so without parallelism), then these classes will
/// be trivial non-parallel implementations that allows MathicGB to work
/// without tbb being present. TBB doesn't work on Cygwin, so that is at least
/// one good reason to have this compatibility layer. This only works if all
/// uses of tbb go through mtbb, so make sure to do that.

#ifndef WITH_TBB
#ifndef MATHICGB_NO_TBB
#define WITH_TBB 1
#endif
#endif

#ifdef WITH_TBB
#include <tbb/tbb.h>
#include <thread>
#include <mutex>

namespace mtbb {
  using ::tbb::task_arena;
  using ::tbb::enumerable_thread_specific;
  using ::tbb::queuing_mutex;
  using ::tbb::null_mutex;
  using ::tbb::parallel_sort;
  using ::tbb::parallel_for;
  using ::tbb::blocked_range;
  using ::tbb::tick_count;
  using ::std::mutex;
  using lock_guard = ::std::lock_guard<std::mutex>;

  template<class Key, class T, class Hash, class KeyEqual>
  using unordered_map = ::tbb::concurrent_unordered_map<Key, T, Hash, KeyEqual>;

  template<typename T>  
#if TBB_VERSION_MAJOR >= 2021
  using feeder = ::tbb::feeder<T>;
#else
  using feeder = ::tbb::parallel_do_feeder<T>;
#endif
  
  template<typename T1, typename T2>
  static inline void parallel_for_each(T1 a, T1 b, T2 c)
  {
#if TBB_VERSION_MAJOR >= 2021    
    tbb::parallel_for_each(a,b,c);
#else
    tbb::parallel_do(a,b,c);
#endif    
  }

  inline int numThreads(int nthreads)
  {
    return (nthreads != 0 ? nthreads :
#if TBB_VERSION_MAJOR >= 2021    
            tbb::info::default_concurrency() 
#else
            tbb::task_scheduler_init::default_num_threads()
#endif
    );
  }
}

#else // TBB not present

// below is an interface to serial versions of the above code.
#include <unordered_map>
#include <functional>
#include <vector>
#include <ctime>
#include <algorithm>
#include <memory>
#include <chrono>
#include <cassert>

namespace mtbb {

  class task_arena {
  public:
    task_arena(int) {}
    static const int automatic = 1;
    template<typename F>
    auto execute(const F& f) -> decltype(f()) {
        return f();
    }
  };

  inline int numThreads(int nthreads)
  {
    return 1;
  }
  
  class mutex {
  public:
    mutex(): mLocked(false) {}

    void lock() {
      assert(!mLocked); // deadlock
      mLocked = true;
    }

    bool try_lock() {
      if (mLocked)
        return false;
      lock();
      return true;
    }

    void unlock() {
      assert(mLocked);
      mLocked = false;
    }

    class scoped_lock {
    public:
      scoped_lock(): mMutex(nullptr) {}
      scoped_lock(mutex& m): mMutex(&m) {mMutex->lock();}
      ~scoped_lock() {
        if (mMutex != nullptr)
          release();
      }
      
      void acquire(mutex& m) {
        assert(mMutex == nullptr);
        mMutex = &m;
      }
      
      bool try_acquire(mutex& m) {
        assert(mMutex == nullptr);
        if (!m.try_lock())
          return false;
        mMutex = &m;
        return true;
      }
      
      void release() {
        assert(mMutex != nullptr);
        mMutex->unlock();
        mMutex = nullptr;
      }
      
    private:
      mutex* mMutex;
    };
    
  private:
    bool mLocked;
  };

  using lock_guard = mutex::scoped_lock;

  using null_mutex = mutex;
  using queuing_mutex = mutex;

  template<class Key, class T, class Hash, class KeyEqual>
  using unordered_map = ::std::unordered_map<Key, T, Hash, KeyEqual>;
  
  template<class T>
  class enumerable_thread_specific {
  public:

    template<class Op>
    enumerable_thread_specific(Op&& creater): mCreater(creater) {}

    bool empty() const {return mObj.get() == nullptr;}

    using reference = T&;
      
    T& local() {
      if (empty())
        mObj = std::make_unique<T>(mCreater());
      assert(!empty());
      return *mObj;
    }

    T* begin() {
      if (empty())
        return nullptr;
      else
        return mObj.get();
    }

    T* end() {
      if (empty())
        return nullptr;
      else
        return begin() + 1;
    }

    void clear() {
      mObj.reset(nullptr);
    }

  private:
    std::function<T()> mCreater;
    std::unique_ptr<T> mObj;
  };
  
  template<class Value>
  class blocked_range {
  public:
    typedef size_t size_type;
    typedef Value const_iterator;

    blocked_range(Value begin, Value end, size_t grainSize = 1):
      mBegin(begin), mEnd(end), mGrainSize(grainSize) {}

    size_type size() const {return end() - begin();}
    bool empty() const {return mBegin == mEnd;}
    size_type grainsize() const {return mGrainSize;}
    bool is_divisible() const {return false;}

    const_iterator begin() const {return mBegin;}
    const_iterator end() const {return mEnd;}

  private:
    const_iterator mBegin;
    const_iterator mEnd;
    size_type mGrainSize;
  };
    
  template<class Range, class Func>
  void parallel_for(Range&& range, Func&& f) {
    f(range);
  }

  template<class Index, class Func>
  void parallel_for(Index begin, Index end, Index step, Func&& f) {
    for (auto i = begin; i < end; i += step)
      f(i);
  }

  template<class T>
  class feeder {
  public:
    feeder(std::vector<T>& tasks): mTasks(tasks) {}

    template<class TT>
    void add(TT&& t) {mTasks.push_back(std::forward<TT>(t));}

  private:
    std::vector<T>& mTasks;
  };

  template<class InputIterator, class Body>
  void parallel_for_each(InputIterator begin, InputIterator end, Body body) {
    typedef typename std::remove_reference<decltype(*begin)>::type Task;
    std::vector<Task> tasks;
    feeder<Task> feeder(tasks);
    for (; begin != end; ++begin) {
      tasks.push_back(*begin);
      while (!tasks.empty()) {
        auto task = std::move(tasks.back());
        tasks.pop_back();
        body(task, feeder);
      }
    }
  }

  template<class It, class Pred>
  void parallel_sort(It begin, It end, Pred&& pred) {
    std::sort(begin, end, pred);
  }

  class tick_count {
  private:
    // This really should be std::chrono::steady_clock, but GCC 4.5.3 doesn't
    // have that.
    typedef std::chrono::system_clock clock;

  public:
    tick_count(): mTime() {}

    static tick_count now() {
      tick_count t;
      t.mTime = clock::now();
      return t;
    }

    class interval_t {
    public:
      interval_t(double seconds): mSeconds(seconds) {}

      double seconds() const {return mSeconds;}

    private:
      const double mSeconds;
    };

    interval_t operator-(const tick_count t) const {
      typedef std::chrono::duration<double> SecondDuration;
      const auto duration =
        std::chrono::duration_cast<SecondDuration>(mTime - t.mTime);
      return duration.count();
    }

  private:
    clock::time_point mTime;
  };
}

#endif
#endif
