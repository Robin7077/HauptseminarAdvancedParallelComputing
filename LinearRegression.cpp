#include <benchmark/benchmark.h>
#include <math.h>
#include <vector>

void linearRegression(size_t size, float * __restrict__ x, float * __restrict__ y, float& m, float& c) {
    float avgX = 0;
    float avgY = 0;
    for (size_t i = 0; i < size; i++) {
        avgX += (x[i] / size);
        avgY += (y[i] / size);
    }
    float numerator = 0;
    float denominator = 0;
    for (size_t i = 0; i < size; i++) {
        numerator += ((x[i] - avgX) * (y[i] - avgY));
        float x2 = (x[i]-avgX);
        denominator += (x2 * x2);
    }
    m = -numerator / denominator;
    c = avgY;
}

struct LinearEquation{
    float m;
    float c;
};

static void LinearRegression(benchmark::State& state) {
    std::vector<float> x, y;
    x.reserve(state.range(0));
    y.reserve(state.range(0));
    std::vector<float> b(state.range(0)), m(state.range(0));
    for (int i = 0; i < state.range(0); i++)
    {
        x.push_back(0.5*i);
        y.push_back(4*i);
    }
    for (auto _ : state)
    {
        float m, c;
        linearRegression(state.range(0), x.data(), y.data(), m, c);
        benchmark::DoNotOptimize(m);
        benchmark::DoNotOptimize(c);
    }
}

BENCHMARK(LinearRegression)->Unit(benchmark::kMillisecond)->Arg(1)->Arg(10)->Arg(100)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000)->Arg(10000000)->Arg(100000000)->Arg(1000000000)->Arg(10000000000);

BENCHMARK_MAIN();
