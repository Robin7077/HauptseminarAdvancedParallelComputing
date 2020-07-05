#include <benchmark/benchmark.h>
#include <math.h>
#include <vector>
#include <array>
#include <cstddef>

template<typename T>
void aos_algorithm(size_t size, T * __restrict__ a, float& geom, float& harm, float& quad, float& arith) {
    float n = static_cast<float>(size);
    float buffer1 = 1.0;
    float buffer2 = 0.0;
    float buffer3 = 0.0;
    float buffer4 = 0.0;
    for (size_t i = 0; i < size; i++)
    {
        buffer1 *= a[i].f;
        float b = 1 / a[i].f;
        buffer2 += b;
        float c = a[i].f*a[i].f;
        c /= n;
        buffer3 += c;
        buffer4 += a[i].f / n;
    }
    geom = pow(buffer1, 1.0f/n);
    harm = 1 / buffer2;
    quad = sqrt(buffer3);
    arith = buffer4;
}

void base_algorithm(size_t size, float * __restrict__ a, float& geom, float& harm, float& quad, float& arith) {
    float n = static_cast<float>(size);
    float buffer1 = 1.0;
    float buffer2 = 0.0;
    float buffer3 = 0.0;
    float buffer4 = 0.0;
    for (size_t i = 0; i < size; i++)
    {
        buffer1 *= a[i];
        float b = 1 / a[i];
        buffer2 += b;
        float c = a[i]*a[i];
        c /= n;
        buffer3 += c;
        buffer4 += a[i] / n;
    }
    geom = pow(buffer1, 1.0f/n);
    harm = 1 / buffer2;
    quad = sqrt(buffer3);
    arith = buffer4;
}

static void Base(benchmark::State& state) {
    std::vector<float> x;
    x.reserve(state.range(0));
    for (int64_t i = 0; i < state.range(0); i++)
    {
        float t = static_cast<float>(i) / static_cast<float>(i + 1);
        x.push_back(t);
    }
    for (auto _ : state)
    {
        float a, b, c, d;
        base_algorithm(state.range(0), x.data(), a, b, c, d);
        benchmark::DoNotOptimize(a);
        benchmark::DoNotOptimize(b);
        benchmark::DoNotOptimize(c);
        benchmark::DoNotOptimize(d);
    }
}

template<typename T>
static void AoSBenchmark(benchmark::State& state) {
    std::vector<T> x;
    x.reserve(state.range(0));
    for (int64_t i = 0; i < state.range(0); i++)
    {
        T t;
        t.f = static_cast<float>(i) / static_cast<float>(i + 1);
        x.push_back(t);
    }
    for (auto _ : state)
    {
        float a, b, c, d;
        aos_algorithm(state.range(0), x.data(), a, b, c, d);
        benchmark::DoNotOptimize(a);
        benchmark::DoNotOptimize(b);
        benchmark::DoNotOptimize(c);
        benchmark::DoNotOptimize(d);
    }
}

struct Test0 {
    float f;
};
static void Stat0(benchmark::State& state) {
    AoSBenchmark<Test0>(state);
}
template<size_t N>
struct Test {
    float f;
    std::array<std::byte, N> dump;
};
template<size_t N>
static void Stat(benchmark::State& state) {
    AoSBenchmark<Test<N>>(state);
}
static void Stat1(benchmark::State& state) {
    Stat<1>(state);
};
static void Stat2(benchmark::State& state) {
    Stat<2>(state);
};
static void Stat4(benchmark::State& state) {
    Stat<4>(state);
};
static void Stat8(benchmark::State& state) {
    Stat<8>(state);
};
static void Stat16(benchmark::State& state) {
    Stat<16>(state);
};
static void Stat32(benchmark::State& state) {
    Stat<32>(state);
};
static void Stat64(benchmark::State& state) {
    Stat<64>(state);
};
static void Stat128(benchmark::State& state) {
    Stat<128>(state);
};
BENCHMARK(Base)->Unit(benchmark::kMillisecond)->Arg(1)->Arg(10)->Arg(100)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000)->Arg(10000000)->Arg(100000000)->Arg(1000000000);
BENCHMARK(Stat0)->Unit(benchmark::kMillisecond)->Arg(1)->Arg(10)->Arg(100)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000)->Arg(10000000)->Arg(100000000)->Arg(1000000000);
BENCHMARK(Stat1)->Unit(benchmark::kMillisecond)->Arg(1)->Arg(10)->Arg(100)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000)->Arg(10000000)->Arg(100000000)->Arg(1000000000);
BENCHMARK(Stat2)->Unit(benchmark::kMillisecond)->Arg(1)->Arg(10)->Arg(100)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000)->Arg(10000000)->Arg(100000000)->Arg(1000000000);
BENCHMARK(Stat4)->Unit(benchmark::kMillisecond)->Arg(1)->Arg(10)->Arg(100)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000)->Arg(10000000)->Arg(100000000)->Arg(1000000000);
BENCHMARK(Stat8)->Unit(benchmark::kMillisecond)->Arg(1)->Arg(10)->Arg(100)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000)->Arg(10000000)->Arg(100000000)->Arg(1000000000);
BENCHMARK(Stat16)->Unit(benchmark::kMillisecond)->Arg(1)->Arg(10)->Arg(100)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000)->Arg(10000000)->Arg(100000000)->Arg(1000000000);
BENCHMARK(Stat32)->Unit(benchmark::kMillisecond)->Arg(1)->Arg(10)->Arg(100)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000)->Arg(10000000)->Arg(100000000)->Arg(1000000000);
BENCHMARK(Stat64)->Unit(benchmark::kMillisecond)->Arg(1)->Arg(10)->Arg(100)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000)->Arg(10000000)->Arg(100000000)->Arg(1000000000);
BENCHMARK(Stat128)->Unit(benchmark::kMillisecond)->Arg(1)->Arg(10)->Arg(100)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000)->Arg(10000000)->Arg(100000000)->Arg(1000000000);

BENCHMARK_MAIN();
