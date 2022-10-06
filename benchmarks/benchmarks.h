#ifndef SIMDSTRING_BENCHMARK_H
#define SIMDSTRING_BENCHMARK_H

#include <benchmark/benchmark.h>
#include <sstream>

////////////////////////////////////////////////////////////////////////////////////////
// SIMDString benchmarks contains modified code from LLVM string benchmarks
////////////////////////////////////////////////////////////////////////////////////////
// https://github.com/llvm/llvm-project/blob/main/libcxx/benchmarks/string.bench.cpp
////////////////////////////////////////////////////////////////////////////////////////

constexpr std::size_t MAX_STRING_LEN = 1 << 21;
#define CONST_C_STR "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Cras finibus pellentesque magna vel ultrices. Nulla nisl ipsum, dapibus ac ullamcorper vel, lacinia at massa. Pellentesque eget ipsum volutpat."
#define CONST_C_STR_SIZE 201

////////////////////////////////////////////////////////////////////////////////////////
// Constructor Benchmark Definitions
template<class Str>
static void BM_CtorDefault(benchmark::State& state)
{
    for (auto _ : state){
        Str Default;
        benchmark::DoNotOptimize(Default);
    }
}

template<class Str>
static void BM_Ctor(benchmark::State& state)
{
    for (auto _ : state)
    {
        Str s1(state.range(0), '*');
        benchmark::DoNotOptimize(s1);
    }
}

template<class Str>
void BM_ConstCStrConstruct(benchmark::State& state)
{    
    for (auto _ : state){
        Str s1(CONST_C_STR);
        benchmark::DoNotOptimize(s1);
    }
}

template<class Str>
static void BM_CopyConstruct(benchmark::State& state)
{
    Str s1(state.range(0), '-');
    for (auto _ : state)
    {
        Str s2(s1);
        benchmark::DoNotOptimize(s2);
    }
}

template<class Str>
static void BM_CstrConstruct(benchmark::State& state)
{
    char* tmp = new char[state.range(0) + 1];
    memset(tmp, '*', state.range(0));
    tmp[state.range(0)] = '\0';
    for (auto _ : state){
        Str s1(tmp);
        benchmark::DoNotOptimize(s1);
    }
    delete[] tmp;
}

template<class Str>
static void BM_ConstCstrCopyConstruct(benchmark::State& state)
{
    Str s1(CONST_C_STR);
    for (auto _ : state){
        Str s2(s1);
        benchmark::DoNotOptimize(s2);
    }
}

////////////////////////////////////////////////////////////////////////////////////////
// Assignment Operator Benchmark Definitions
template<class Str>
static void BM_Assign(benchmark::State& state)
{
    Str s1(state.range(0), '*');
    Str s2(state.range(0), '-');
    for (auto _ : state){
        benchmark::DoNotOptimize(s2 = s1);
    }
}

template<class Str>
static void BM_AssignCstr(benchmark::State& state)
{
    char* tmp = new char[state.range(0) + 1];
    memset(tmp, '*', state.range(0));
    tmp[state.range(0)] = '\0';
    Str s1(state.range(0), '*');
    for (auto _ : state){
        benchmark::DoNotOptimize(s1 = tmp);
    }
    delete[] tmp;
}

template<class Str>
void BM_AssignConstCstr(benchmark::State& state)
{    
    Str s1;
    for (auto _ : state){
        benchmark::DoNotOptimize(s1 = CONST_C_STR);
    }
}

////////////////////////////////////////////////////////////////////////////////////////
// Append Benchmark Definitions
template<class Str>
static void BM_AppendToEmpty(benchmark::State& state)
{
    Str s1;
    Str s2(state.range(0), '*');
    for (auto _ : state)
        benchmark::DoNotOptimize(s1.append(s2));
}

template<class Str>
static void BM_Append(benchmark::State& state)
{
    Str s1(state.range(0), '-');
    Str s2(state.range(0), '*');
    for (auto _ : state)
        benchmark::DoNotOptimize(s1.append(s2));
}

template<class Str>
static void BM_AppendOperator(benchmark::State& state)
{
    Str s1(state.range(0), '-');
    Str s2(state.range(0), '*');
    for (auto _ : state)
        benchmark::DoNotOptimize(s1 += s2);
}

template<class Str>
static void BM_AppendOperatorTwice(benchmark::State& state)
{
    Str s1(state.range(0), '-');
    Str s2(state.range(0), '*');
    for (auto _ : state)
        benchmark::DoNotOptimize((s1 += s2) += s2);
}

template<class Str>
static void BM_ConstCstrAppend(benchmark::State& state)
{
    Str s1(CONST_C_STR);
    Str s2(state.range(0), '-');
    for (auto _ : state)
        benchmark::DoNotOptimize(s1.append(s2));
}

template<class Str>
static void BM_CstrAppend(benchmark::State& state)
{
    Str s1(state.range(0), '-');
    for (auto _ : state)
        benchmark::DoNotOptimize(s1.append(CONST_C_STR));
}

template<class Str>
static void BM_CstrAppendOperator(benchmark::State& state)
{
    Str s1(state.range(0), '-');
    for (auto _ : state)
        benchmark::DoNotOptimize(s1 += CONST_C_STR);
}

template<class Str>
static void BM_CstrAppendOperatorTwice(benchmark::State& state)
{
    Str s1(state.range(0), '-');
    for (auto _ : state)
        benchmark::DoNotOptimize((s1 += CONST_C_STR) += CONST_C_STR);
}

template<class Str>
static void BM_PushBack(benchmark::State& state)
{
    Str s1;
    for (auto _ : state)
    {
        auto len = state.range(0);
        while (--len)
        {
            s1.push_back('*');
            benchmark::DoNotOptimize(s1);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////
// Insert Benchmark Definitions
template<class Str>
static void BM_InsertToEmpty(benchmark::State& state)
{
    Str s1;
    Str s2(state.range(0), '*');
    for (auto _ : state)
        benchmark::DoNotOptimize(s1.insert(0, s2));
}

template<class Str>
static void BM_Insert(benchmark::State& state)
{
    Str s1(state.range(0), '-');
    Str s2(state.range(0), '*');
    for (auto _ : state)
        benchmark::DoNotOptimize(s1.insert(state.range(0)/2, s2));
}

template<class Str>
static void BM_CstrInsert(benchmark::State& state)
{
    Str s1(state.range(0), '-');
    for (auto _ : state)
        benchmark::DoNotOptimize(s1.insert(state.range(0)/2, CONST_C_STR));
}

template<class Str>
static void BM_ConstCstrInsert(benchmark::State& state)
{
    Str s1(CONST_C_STR);
    Str s2(state.range(0), '-');
    for (auto _ : state)
        benchmark::DoNotOptimize(s1.insert(CONST_C_STR_SIZE/2, s2));
}

////////////////////////////////////////////////////////////////////////////////////////
// Concat Benchmark Definitions
template<class Str>
static void BM_ConcatToEmpty(benchmark::State& state)
{
    Str s1;
    Str s2(state.range(0), '*');
    for (auto _ : state)
        benchmark::DoNotOptimize(s1 + s2);
}

template<class Str>
static void BM_Concat(benchmark::State& state)
{
    Str s1(state.range(0), '-');
    Str s2(state.range(0), '*');
    for (auto _ : state)
        benchmark::DoNotOptimize(s1 + s2);
}

template<class Str>
static void BM_ConcatTwice(benchmark::State& state)
{
    Str s1(state.range(0), '-');
    Str s2(state.range(0), '*');
    for (auto _ : state)
        benchmark::DoNotOptimize(s1 + s2 + s2);
}

template<class Str>
static void BM_ConstCstrConcat(benchmark::State& state)
{
    Str s1(CONST_C_STR);
    Str s2(state.range(0), '-');
    for (auto _ : state)
        benchmark::DoNotOptimize(s1 + s2);
}

template<class Str>
static void BM_CstrConcat(benchmark::State& state)
{
    Str s1(state.range(0), '-');
    for (auto _ : state)
        benchmark::DoNotOptimize(s1 + CONST_C_STR);
}

template<class Str>
static void BM_CstrConcatTwice(benchmark::State& state)
{
    Str s1(state.range(0), '-');
    for (auto _ : state)
        benchmark::DoNotOptimize(s1 + CONST_C_STR + CONST_C_STR);
}

////////////////////////////////////////////////////////////////////////////////////////
// Find Benchmark Definitions

// Benchmark when there is no match.
template<class Str>
static void BM_FindNoMatch(benchmark::State &state) {
 Str s1(state.range(0), '-');
 Str s2(8, '*');
 for (auto _ : state)
   benchmark::DoNotOptimize(s1.find(s2));
}

// Benchmark when the string matches first time.
template<class Str>
static void BM_FindAllMatch(benchmark::State &state) {
 Str s1(MAX_STRING_LEN, '-');
 Str s2(state.range(0), '-');
 for (auto _ : state)
   benchmark::DoNotOptimize(s1.find(s2));
}
// Benchmark when the string matches somewhere in the end.
template<class Str>
static void BM_FindMatch1(benchmark::State &state) {
  Str s1(MAX_STRING_LEN / 2, '*');
  s1 += Str(state.range(0), '-');
  Str s2(state.range(0), '-');
  for (auto _ : state)
    benchmark::DoNotOptimize(s1.find(s2));
}

// Benchmark when the string matches somewhere from middle to the end.
template<class Str>
static void BM_FindMatch2(benchmark::State &state) {
  Str s1(MAX_STRING_LEN / 2, '*');
  s1 += Str(state.range(0), '-');
  s1 += Str(state.range(0), '*');
  Str s2(state.range(0), '-');
  for (auto _ : state)
    benchmark::DoNotOptimize(s1.find(s2));
}

////////////////////////////////////////////////////////////////////////////////////////
// RFind Benchmark Definitions

// Benchmark when there is no match.
template<class Str>
static void BM_RFindNoMatch(benchmark::State &state) {
 Str s1(state.range(0), '-');
 Str s2(8, '*');
 for (auto _ : state)
   benchmark::DoNotOptimize(s1.rfind(s2));
}

// Benchmark when the string matches first time.
template<class Str>
static void BM_RFindAllMatch(benchmark::State &state) {
 Str s1(MAX_STRING_LEN, '-');
 Str s2(state.range(0), '-');
 for (auto _ : state)
   benchmark::DoNotOptimize(s1.rfind(s2));
}

// Benchmark when the string matches somewhere in the end.
template<class Str>
static void BM_RFindMatch1(benchmark::State &state) {
  Str s1(state.range(0), '-');
  s1 += Str(MAX_STRING_LEN / 2, '*');
  Str s2(state.range(0), '-');
  for (auto _ : state)
    benchmark::DoNotOptimize(s1.rfind(s2));
}

// Benchmark when the string matches somewhere from middle to the end.
template<class Str>
static void BM_RFindMatch2(benchmark::State &state) {
  Str s1 (state.range(0), '*');
  s1 += Str(state.range(0), '-'); 
  s1 += Str(MAX_STRING_LEN / 2, '*');
  Str s2(state.range(0), '-');
  for (auto _ : state)
    benchmark::DoNotOptimize(s1.rfind(s2));
}

////////////////////////////////////////////////////////////////////////////////////////
// Reserve Benchmark Definition
template<class Str>
static void BM_Reserve(benchmark::State& state)
{
    Str s1;
    for (auto _ : state)
    {
        s1.reserve(state.range(0));
        benchmark::DoNotOptimize(s1);
    }
}

////////////////////////////////////////////////////////////////////////////////////////
// Compare, Equality, Empty, C_str
template<class Str>
static void BM_Compare(benchmark::State& state)
{
    Str s1(state.range(0), '-');
    Str s2(state.range(0), '*');
    for (auto _ : state)
        benchmark::DoNotOptimize(s1.compare(s2));
}

template<class Str>
static void BM_Equality(benchmark::State& state)
{
    Str s1(state.range(0), '-');
    Str s2(state.range(0), '-');
    for (auto _ : state)
        benchmark::DoNotOptimize(s1 == s2);
}

template<class Str>
static void BM_CstrEquality(benchmark::State& state)
{
    Str s1(state.range(0), '-');
    for (auto _ : state)
        benchmark::DoNotOptimize(s1 == CONST_C_STR);
}

template<class Str>
static void BM_ConstCstrEquality(benchmark::State& state)
{
    Str s1(CONST_C_STR);
    for (auto _ : state)
        benchmark::DoNotOptimize(s1 == CONST_C_STR);
}

template<class Str>
static void BM_EmptyCstrEquality(benchmark::State& state)
{
    Str s1(CONST_C_STR);
    for (auto _ : state)
        benchmark::DoNotOptimize(s1 == "");
}

template<class Str>
static void BM_Empty(benchmark::State& state)
{
    Str s1(state.range(0), '-');
    for (auto _ : state)
        benchmark::DoNotOptimize(s1.empty());
}

template<class Str>
static void BM_Cstr(benchmark::State& state)
{
    Str s1(state.range(0), '-');
    for (auto _ : state)
        benchmark::DoNotOptimize(s1.c_str());
}

////////////////////////////////////////////////////////////////////////////////////////
// Access
template<class Str>
static void BM_ConstCStrAt(benchmark::State& state)
{
    Str s1(CONST_C_STR);
    for (auto _ : state)
        benchmark::DoNotOptimize(s1.at(CONST_C_STR_SIZE/2) = 'a');
}

////////////////////////////////////////////////////////////////////////////////////////
// IO
template<class Str>
static void BM_In(benchmark::State& state)
{
    std::string tmp (state.range(0), '-');
    std::istringstream iss(tmp);
    Str s1;
    for (auto _ : state)
        benchmark::DoNotOptimize(iss >> s1);
}

template<class Str>
static void BM_Getline(benchmark::State& state)
{
    std::string tmp (state.range(0), '-');
    std::istringstream iss(tmp);
    Str s1;
    for (auto _ : state)
        benchmark::DoNotOptimize(getline(iss, s1));
}

template<class Str>
static void BM_Out(benchmark::State& state)
{
    std::ostringstream oss;
    Str s1(state.range(0), '-');
    for (auto _ : state)
        benchmark::DoNotOptimize(oss << s1);
}

////////////////////////////////////////////////////////////////////////////////////////
// Swap
template<class Str>
static void BM_Swap(benchmark::State& state)
{
    Str s1(state.range(0), '*');
    Str s2(state.range(0), '-');
    for (auto _ : state) {
        benchmark::DoNotOptimize(s2);
        s2.swap(s1);
    }
}

////////////////////////////////////////////////////////////////////////////////////////
// This is where the benchmarks are programmatically registered .
void RegisterBenchmarks(const char* classname) {
    // buffer for formatting the benchmark name string into RegisterBenchmark
    char buffer[512];

    // This macro registers the benchmark and assigns a weight to the benchmark in weights
#   define REGISTER_BENCHMARK(fun) sprintf(buffer, "%s<%s>", #fun, classname);\
        benchmark::RegisterBenchmark(buffer, fun<Str>)\

    REGISTER_BENCHMARK(BM_CtorDefault);
    REGISTER_BENCHMARK(BM_Ctor)->Arg(0)->RangeMultiplier(4)->Range(1, 1024)->Arg(63)->Arg(MAX_STRING_LEN);
    REGISTER_BENCHMARK(BM_CstrConstruct)->Arg(0)->RangeMultiplier(4)->Range(1, 1024)->Arg(63)->Arg(MAX_STRING_LEN);
    REGISTER_BENCHMARK(BM_ConstCStrConstruct);
    REGISTER_BENCHMARK(BM_CopyConstruct)->Arg(0)->RangeMultiplier(4)->Range(1, 1024)->Arg(63)->Arg(MAX_STRING_LEN);
    REGISTER_BENCHMARK(BM_ConstCstrCopyConstruct);

    ////////////////////////////////////////////////////////////////////////////////////
    REGISTER_BENCHMARK(BM_Assign)->Arg(0)->RangeMultiplier(4)->Range(1, 1024)->Arg(63)->Arg(MAX_STRING_LEN);
    REGISTER_BENCHMARK(BM_AssignCstr)->Arg(0)->RangeMultiplier(4)->Range(1, 1024)->Arg(63)->Arg(MAX_STRING_LEN);
    REGISTER_BENCHMARK(BM_AssignConstCstr);

    ////////////////////////////////////////////////////////////////////////////////////
    REGISTER_BENCHMARK(BM_AppendToEmpty)->Arg(0)->RangeMultiplier(4)->Range(1, 1024)->Arg(MAX_STRING_LEN);
    REGISTER_BENCHMARK(BM_Append)->Arg(0)->RangeMultiplier(4)->Range(1, 1024)->Arg(MAX_STRING_LEN / 2);
    REGISTER_BENCHMARK(BM_AppendOperator)->Arg(0)->RangeMultiplier(4)->Range(1, 1024)->Arg(MAX_STRING_LEN / 2);
    REGISTER_BENCHMARK(BM_AppendOperatorTwice)->Arg(0)->RangeMultiplier(4)->Range(1, 1024)->Arg(MAX_STRING_LEN / 3);
    REGISTER_BENCHMARK(BM_ConstCstrAppend)->Arg(0)->RangeMultiplier(4)->Range(1, 1024)->Arg(MAX_STRING_LEN - CONST_C_STR_SIZE);
    REGISTER_BENCHMARK(BM_CstrAppend)->Arg(0)->RangeMultiplier(4)->Range(1, 1024)->Arg(MAX_STRING_LEN - CONST_C_STR_SIZE);
    REGISTER_BENCHMARK(BM_CstrAppendOperator)->Arg(0)->RangeMultiplier(4)->Range(1, 1024)->Arg(MAX_STRING_LEN - CONST_C_STR_SIZE);
    REGISTER_BENCHMARK(BM_CstrAppendOperatorTwice)->Arg(0)->RangeMultiplier(4)->Range(1, 1024)->Arg(MAX_STRING_LEN - (2 * CONST_C_STR_SIZE));

    ////////////////////////////////////////////////////////////////////////////////////
    REGISTER_BENCHMARK(BM_InsertToEmpty)->Arg(0)->RangeMultiplier(4)->Range(1, 1024)->Arg(MAX_STRING_LEN);
    REGISTER_BENCHMARK(BM_Insert)->Arg(0)->RangeMultiplier(4)->Range(1, 1024)->Arg(MAX_STRING_LEN / 2);
    REGISTER_BENCHMARK(BM_CstrInsert)->Arg(0)->RangeMultiplier(4)->Range(1, 1024)->Arg(MAX_STRING_LEN - CONST_C_STR_SIZE);
    REGISTER_BENCHMARK(BM_ConstCstrInsert)->Arg(0)->RangeMultiplier(4)->Range(1, 1024)->Arg(MAX_STRING_LEN - CONST_C_STR_SIZE);

    ////////////////////////////////////////////////////////////////////////////////////
    REGISTER_BENCHMARK(BM_ConcatToEmpty)->Arg(0)->RangeMultiplier(4)->Range(1, 1024)->Arg(MAX_STRING_LEN);
    REGISTER_BENCHMARK(BM_Concat)->Arg(0)->RangeMultiplier(4)->Range(1, 1024)->Arg(MAX_STRING_LEN / 2);
    REGISTER_BENCHMARK(BM_ConcatTwice)->Arg(0)->RangeMultiplier(4)->Range(1, 1024)->Arg(MAX_STRING_LEN / 3);
    REGISTER_BENCHMARK(BM_CstrConcat)->Arg(0)->RangeMultiplier(4)->Range(1, 1024)->Arg(MAX_STRING_LEN - CONST_C_STR_SIZE);
    REGISTER_BENCHMARK(BM_ConstCstrConcat)->Arg(0)->RangeMultiplier(4)->Range(1, 1024)->Arg(MAX_STRING_LEN - CONST_C_STR_SIZE);
    REGISTER_BENCHMARK(BM_CstrConcatTwice)->Arg(0)->RangeMultiplier(4)->Range(1, 1024)->Arg(MAX_STRING_LEN - (2 * CONST_C_STR_SIZE));

    ////////////////////////////////////////////////////////////////////////////////////
    REGISTER_BENCHMARK(BM_FindNoMatch)->Arg(0)->RangeMultiplier(4)->Range(1, 1024)->Arg(MAX_STRING_LEN);
    REGISTER_BENCHMARK(BM_FindAllMatch)->Arg(0)->RangeMultiplier(4)->Range(1, 1024)->Arg(MAX_STRING_LEN);
    REGISTER_BENCHMARK(BM_FindMatch1)->Arg(0)->RangeMultiplier(4)->Range(1, 1024)->Arg(MAX_STRING_LEN / 4);
    REGISTER_BENCHMARK(BM_FindMatch2)->Arg(0)->RangeMultiplier(4)->Range(1, 1024)->Arg(MAX_STRING_LEN / 4);

    ////////////////////////////////////////////////////////////////////////////////////
    REGISTER_BENCHMARK(BM_RFindNoMatch)->Arg(0)->RangeMultiplier(4)->Range(1, 1024)->Arg(MAX_STRING_LEN);
    REGISTER_BENCHMARK(BM_RFindAllMatch)->Arg(0)->RangeMultiplier(4)->Range(1, 1024)->Arg(MAX_STRING_LEN);
    REGISTER_BENCHMARK(BM_RFindMatch1)->Arg(0)->RangeMultiplier(4)->Range(1, 1024)->Arg(MAX_STRING_LEN / 4);
    REGISTER_BENCHMARK(BM_RFindMatch2)->Arg(0)->RangeMultiplier(4)->Range(1, 1024)->Arg(MAX_STRING_LEN / 4);

    ////////////////////////////////////////////////////////////////////////////////////
    REGISTER_BENCHMARK(BM_PushBack)->Arg(1)->Arg(MAX_STRING_LEN);
    REGISTER_BENCHMARK(BM_Reserve)->Arg(0)->Arg(MAX_STRING_LEN);

    ////////////////////////////////////////////////////////////////////////////////////
    REGISTER_BENCHMARK(BM_Compare)->Arg(0)->RangeMultiplier(4)->Range(1, 1024)->Arg(MAX_STRING_LEN);
    REGISTER_BENCHMARK(BM_Equality)->Arg(0)->RangeMultiplier(4)->Range(1, 1024)->Arg(MAX_STRING_LEN);
    REGISTER_BENCHMARK(BM_ConstCstrEquality);    
    REGISTER_BENCHMARK(BM_EmptyCstrEquality);
    REGISTER_BENCHMARK(BM_CstrEquality)->Arg(0)->Arg(MAX_STRING_LEN);

    ////////////////////////////////////////////////////////////////////////////////////
    REGISTER_BENCHMARK(BM_Empty)->Arg(0)->Arg(MAX_STRING_LEN);
    REGISTER_BENCHMARK(BM_Cstr)->Arg(0)->Arg(MAX_STRING_LEN);
    REGISTER_BENCHMARK(BM_ConstCStrAt);
    REGISTER_BENCHMARK(BM_Swap)->Arg(MAX_STRING_LEN);

    ////////////////////////////////////////////////////////////////////////////////////
    REGISTER_BENCHMARK(BM_In)->Arg(0)->Arg(MAX_STRING_LEN);
    REGISTER_BENCHMARK(BM_Getline)->Arg(0)->Arg(MAX_STRING_LEN);
    REGISTER_BENCHMARK(BM_Out)->Arg(0)->Arg(MAX_STRING_LEN);
    
#undef REGISTER_BENCHMARK
};

#endif