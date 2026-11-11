mathicgb
=========

Mathicgb is a program for computing Groebner bases and signature
Groebner bases. Mathicgb is based on the fast data structures from
[mathic](https://github.com/Macaulay2/mathic).

The paper "Practical Grobner Basis Computation" describes the
algorithms in Mathicgb from a high level. It was presented at ISSAC
2012 and is available at http://arxiv.org/abs/1206.6940

The following copyright and license notice applies to all of the files in
mathicgb.

Copyright 2012 2013 Bjarke Hammersholt Roune (http://www.broune.com) and Michael Stillman

Mathicgb is licensed for use under the terms of GNU General Public License
version 2 and under any later version; the option is yours.  See the files
gpl-*.txt in this directory.

## Building with CMake

This build requires CMake 3.18 or newer and a C++17 compiler.

```sh
cmake -S . -B build-dir -GNinja -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_PREFIX=/your/install/prefix
cmake --build build-dir
ctest --test-dir build-dir --output-on-failure
cmake --install build-dir
```

Mathicgb needs [Memtailor](https://github.com/Macaulay2/memtailor) and
[Mathic](https://github.com/Macaulay2/mathic). CMake looks for each in this
order: its CMake package (installed by Memtailor 1.5 and Mathic 1.6 or later
when built with CMake), its pkg-config file, and finally its header and
library directly. Prefer the first two: only they can tell Mathicgb whether
Memtailor and Mathic were built in Debug mode, which changes their data
layout, so Mathicgb must be compiled to match. If they are in a nonstandard
location, pass `-DCMAKE_PREFIX_PATH=/their/prefix` (separate several prefixes
with semicolons, quoted; the environment variable uses colons).

`-Dwith_tbb=ON` requires TBB for multithreading, `-Dwith_tbb=OFF` builds
without it, and the default, `auto`, uses TBB if CMake finds it.

`-DBUILD_TESTING=OFF` skips the unit tests and the GoogleTest dependency.

## Using an installed Mathicgb

A CMake installation, shared or static, provides headers, pkg-config metadata,
and a relocatable CMake package under the platform's library directory. An
Autotools installation provides headers and pkg-config metadata only. In a
CMake project:

```cmake
find_package(mathicgb 1.5 CONFIG REQUIRED)
target_link_libraries(my_program PRIVATE mathicgb::mathicgb)
```

Linking to `mathicgb::mathicgb` sets up everything needed to build against
Mathicgb: it adds Mathicgb's include directory, links Memtailor, Mathic and
(if Mathicgb was built with it) TBB, and compiles your program as C++17 or
later, which Mathicgb's headers require.

`find_package(mathicgb)` in turn looks for the CMake packages of Memtailor,
Mathic and, if needed, TBB. Memtailor installs one only from version 1.5 on
and Mathic only from version 1.6 on, and each only when built with CMake.
oneTBB always installs one.

Two compile flags must match the installed library. A Debug build of Mathicgb
defines `MATHICGB_DEBUG`, which enables assertions in its headers, and a build
without TBB defines `MATHICGB_NO_TBB`, which changes Mathicgb's types. Both
the CMake package and `pkg-config --cflags mathicgb` pass these flags on, so
programs using Mathicgb match them even when they're built in Release mode.
With pkg-config you also need to request C++17 or later yourself.
