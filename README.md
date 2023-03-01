

SIMDString
=========================================================================================================

`SIMDString` is a [MIT-licensed](https://opensource.org/licenses/MIT) open source implementation
of a fast C++ string class designed for use in games. It is a drop-in replacement for `std::string` that
is 10-100x faster for many common operations, such as small `string::operator+=(const char*)`,
`string::string(const string&)`, `string::string(const char*)`, and `string::c_str()`.

`SIMDString` was created and optimized over a decade by Morgan McGuire (Roblox, Activision, NVIDIA),
Zander Majercik (NVIDIA, Williams College), and Corey Taylor (EA), with contributions from 
Linwan Song (Roblox), Roberto Parolin (Roblox), and Andrew Lacey (Roblox).

Games have common use cases for strings:

- Static and dynamic UI text
- Localization tags
- Parameter binding for scripting languages
- Parameter binding for shading languages
- Shader source code synthesis
- In-game chat
- Working with ASCII data files
- Logging and error handling

`SIMDString` optimizes use cases needed for these such as construction from compile-time constant `char*`,
construction from small dynamic `char*`, copying, empty string construction, and concatenation of small
strings.

The primary algorithmic optimization is embedded short strings directly within the object to avoid heap
allocation and increase cache coherence. A second algorithmic optimization is the _optional_ use of the free-list
based memory allocator from the [G3D Innovation Engine](https://casual-effects.com/g3d).
This is abstracted by the use of a `std` allocator, and the default `std` allocator or one from your
engine can be used instead.

There are many pragmatic microoptimizations to make `SIMDString` fast on many platforms. We've profiled
and tuned across x64, ARM32, ARM64, Windows, Linux, macOS, MSVC, gcc, and clang++ for a decade continuously
(and thus many processor and OS generations). We've used `SIMDString` for multiple shipping games, shipping
middleware and open source libraries, and research projects. Many seemingly good ideas were attempted and 
rejected because they did not hold up in practice robustly. That includes internal reference counting for 
shared heap buffers and early-out tests for empty string cases.

Limitations
=========================================================================================================

`inConstSegment()` must be implemented in `SIMDString.cpp` for your platform. The provided
Linux implementation will work on most non-Windows platforms but should be tested for each
before use.

`SIMDString` is implemented for 8-bit `char` strings. The internal `value_type` may be changed to
other types for wide characters and the same optimizations will apply, however we have not tested
this use case.

`SIMDString` is designed and benchmarked for the usage patterns found in games and other real-time 3D
applications. It may perform less well and even underperform `std::string` for applications with
different usage patterns.

Windows x64 was our primary platform, so while `SIMDString` was regularly tested and optimized on
other platforms, there may be more untapped performance on those.

Usage
=========================================================================================================

The top-level names bound by `SIMDString.h` are

`SIMDString&lt;INTERNAL_SIZE, alloc&gt;`
: The string class.

`inConstSegment()`
: Identifies a compile-time constant `char*` buffer.

1. The distribution has two files `SIMDString.h` and `SIMDString.cpp`. Add `SIMDString.cpp` to your
   utility library build or create a static library (do not build it as a separate DLL) and include
   `SIMDString.h` as a typical header.

2. If you do not wish to use the G3D allocator, set the macro `NO_G3D_ALLOCATOR=1`
   on any file that uses `SIMDString`.

3. Optionally define a project-specific string alias in a common header for your project,
   such as `using String = SIMDString;`. You can then easily switch
   this to `using String = std::string;` to measure the difference or quickly revert your codebase's
   string of choice on certain platforms.

4. Optionally choose the `INTERNAL_SIZE` for your application. The optimal size for the internal buffer
   differs across platforms and for any given mix of operations in a benchmark. 64 is a good default that
   does not waste too much memory when making large data structures of strings. 48 performs best for our
   internal benchmark's mixture of operations but may have inferior alignment and perform poorly on games
   that tend to have longer strings. 128 is only slightly slower and supports much larger strings.
   Note that `INTERNAL_SIZE` is not the entire size of the string when considering alignment. There is
   also a heap pointer and a `size_t` inside of the class.

5. Optionally modify `SIMDString.h` to disable `USE_SSE_MEMCPY` if you don't want SIMD optimizations
   (useful mainly when debugging/testing the string class itself on a new platform).


Alternatives
=========================================================================================================

We tested against and beat the following on performance:

- `std::string` (As of March 2022, default implementations for MSVC 2010, 2013, 2019, 2022, clang++/llvm libc++, g++, Apache)
- [`eastl::string`](https://github.com/electronicarts/EASTL) (EA)
- [`fbstring`](https://github.com/facebook/folly/blob/main/folly/docs/FBString.md) (Facebook)


Testing and Benchmarking
=========================================================================================================

With this distribution, we've included the files we used to test and benchmark SIMDString. Required dependencies 
are [GoogleTest](https://github.com/google/googletest) and [Benchmark](https://github.com/google/benchmark). 
`SIMDString/benchmarks` provides its own implementation of `main.cpp`, so `benchmark_main` should not be linked 
with these files. 

To benchmark against other string classes, register benchmarks with those string classes with the 
`REGISTER_CLASS_BENCHMARKS` macro in `benchmarks/main.cpp`. To add additional benchmarks, define the 
templated benchmark function in `benchmarks/benchmarks.h` and add the benchmark function to 
`RegisterBenchmarks` at the bottom of `benchmarks/benchmarks.h` with the `REGISTER_BENCHMARK` macro. 
Passing arguments to the benchmarks follows the instructions in the 
[Benchmark User Guide](https://github.com/google/benchmark/blob/main/docs/user_guide.md#passing-arguments). 

When running the benchmark, follow the instructions on the Benchmark user guide to 
[output to files](https://github.com/google/benchmark/blob/main/docs/user_guide.md#output-files) and to 
[run a subset of benchmarks](https://github.com/google/benchmark/blob/main/docs/user_guide.md#running-a-subset-of-benchmarks). 
Any arguments passed into `benchmarks/main.cpp` are passed directly into the Google benchmark functions. 

Talk 
=========================================================================================================
Zander Majercik (NVIDIA, Williams College) gave a talk at CPPCon announcing SIMDString which [can be viewed here](https://www.youtube.com/watch?v=fglXeSWGVDc).
The slides for this talk are available in this repo. 

Licensing
=========================================================================================================
All code under the `tests` folder is under the same MIT license as SIMDString. The `benchmarks` folder
contains code from the [LLVM-Project](https://github.com/llvm/llvm-project), and is under a separate license 
as represented by the LICENSE file in that folder. 

