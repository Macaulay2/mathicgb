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

## Building and using the CMake package

This build requires CMake 3.18 or newer and a C++17 compiler.

```sh
cmake -S . -B build-dir -GNinja -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_PREFIX=/your/install/prefix
cmake --build build-dir
ctest --test-dir build-dir --output-on-failure
cmake --install build-dir
```

`BUILD_TESTING=OFF` disables the GoogleTest dependency and unit tests.
Both shared and static installations provide headers, pkg-config metadata,
and a relocatable CMake package under the platform's library directory:

```cmake
find_package(mathicgb 1.4 CONFIG REQUIRED)
target_link_libraries(my_program PRIVATE mathicgb::mathicgb)
```

The target supplies C++17, include paths and transitive dependencies. Dependency
CMake configs are preferred; pkg-config metadata is supported for existing
system installations. Older Homebrew CMake bottles may lack both metadata
formats; build the dependency with its released Autotools build, or install a
CMake package that supplies the config. Bare header/library discovery is insufficient to propagate
dependencies' ABI flags. The dependency find modules are shipped with the config.
Use a quoted semicolon-separated `-DCMAKE_PREFIX_PATH='/prefix/one;/prefix/two'`
for nonstandard prefixes; the Unix environment variable uses colons instead.

The installed package exports `MATHICGB_DEBUG` if the library was built in
Debug mode, even when the consumer uses Release. It inherits dependencies' own
ABI flags instead of forcing their Debug layouts. `pkg-config --cflags mathicgb`
also reports the installed library's Debug flag; pkg-config consumers should
select C++17 or later explicitly.

`with_tbb` retains its ON/OFF/auto behavior. Both metadata formats propagate
`MATHICGB_NO_TBB` for a build without TBB, or the TBB dependency when enabled.
