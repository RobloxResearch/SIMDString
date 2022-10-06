//#define TEST_EASTL
//#define TEST_FOLLY
#define TEST_G3D_ALLOC


#include "SIMDString.h"
#include "benchmarks.h"

#ifdef TEST_EASTL
#   include "EASTL/string.h"
#endif

#ifdef TEST_FBSTRING
#   include "folly/FBString.h"
#endif

#ifdef TEST_G3D_ALLOC
#   include <G3D/G3D.h>
#endif


int main(int argc, const char* argv[]) {
    // __VA_ARGS_ is necessary because type templating messes up Macro argument parsing
#   define REGISTER_CLASS_BENCHMARKS(...) RegisterBenchmarks<__VA_ARGS__>(#__VA_ARGS__)

    // Register benchmarks for each class
    REGISTER_CLASS_BENCHMARKS(std::string);
    REGISTER_CLASS_BENCHMARKS(SIMDString<64, ::std::allocator<char>>);

#   ifdef TEST_G3D_ALLOC
    REGISTER_CLASS_BENCHMARKS(SIMDString<64, G3D::g3d_allocator<char>>); 
#   endif

#   ifdef TEST_EASTL
    REGISTER_CLASS_BENCHMARKS(eastl::string); 
#   endif

#   ifdef TEST_FBSTRING
    REGISTER_CLASS_BENCHMARKS(folly::fbstring); 
#   endif 

#   undef REGISTER_CLASS_BENCHMARKS

    // Run benchmarks
    ::benchmark::Initialize(&argc, argv);
    if (::benchmark::ReportUnrecognizedArguments(argc, argv)) return 1;
    ::benchmark::RunSpecifiedBenchmarks();
    ::benchmark::Shutdown();

    return 0;
}
