#include <cmath>
#include <iostream>
#include <memory>
#include <vector>
#include <benchmark/benchmark.h>
#include <immintrin.h>

// Adapted from: https://github.com/skeeto/mandel-simd

struct spec {
    /* Image Specification */
    int width;
    int height;
    int depth;
    /* Fractal Specification */
    float xlim[2];
    float ylim[2];
    int iterations;
};

void
mandel_basic(unsigned char *image, const struct spec *s)
{
    float xdiff = s->xlim[1] - s->xlim[0];
    float ydiff = s->ylim[1] - s->ylim[0];
    float iter_scale = 1.0f / s->iterations;
    float depth_scale = s->depth - 1;
    for (int y = 0; y < s->height; y++) {
        for (int x = 0; x < s->width; x++) {
            float cr = x * xdiff / s->width  + s->xlim[0];
            float ci = y * ydiff / s->height + s->ylim[0];
            float zr = cr;
            float zi = ci;
            int k = 0;
            float mk = 0.0f;
            while (++k < s->iterations) {
                float zr1 = zr * zr - zi * zi + cr;
                float zi1 = zr * zi + zr * zi + ci;
                zr = zr1;
                zi = zi1;
                mk += 1.0f;
                if (zr * zr + zi * zi >= 4.0f)
                    break;
            }
            mk *= iter_scale;
            mk = sqrtf(mk);
            mk *= depth_scale;
            int pixel = mk;
            image[y * s->width * 3 + x * 3 + 0] = pixel;
            image[y * s->width * 3 + x * 3 + 1] = pixel;
            image[y * s->width * 3 + x * 3 + 2] = pixel;
        }
    }
}

void
mandel_basic_adapted_for_auto_vectorization(unsigned char *image, const struct spec *s)
{
    float xdiff = s->xlim[1] - s->xlim[0];
    float ydiff = s->ylim[1] - s->ylim[0];
    float iter_scale = 1.0f / s->iterations;
    float depth_scale = s->depth - 1;
    for (int y = 0; y < s->height; y++) {
        for (int x = 0; x < s->width; x++) {
            float cr = x * xdiff / s->width  + s->xlim[0];
            float ci = y * ydiff / s->height + s->ylim[0];
            float zr = cr;
            float zi = ci;
            int k = 0;
            float mk = 0.0f;
            while (++k < (s->iterations / 64) + 1) {
                for (size_t i = 0; i < 64; i++)
                {
                    float zr1 = zr * zr - zi * zi + cr;
                    float zi1 = zr * zi + zr * zi + ci;
                    zr = zr1;
                    zi = zi1;
                }
                mk += 64.0f;
                if (zr * zr + zi * zi >= 4.0f)
                    break;
            }
            mk *= iter_scale;
            mk = sqrtf(mk);
            mk *= depth_scale;
            int pixel = mk;
            image[y * s->width * 3 + x * 3 + 0] = pixel;
            image[y * s->width * 3 + x * 3 + 1] = pixel;
            image[y * s->width * 3 + x * 3 + 2] = pixel;
        }
    }
}

void
mandel_sse2(unsigned char *image, const struct spec *s)
{
    __m128 xmin = _mm_set_ps1(s->xlim[0]);
    __m128 ymin = _mm_set_ps1(s->ylim[0]);
    __m128 xscale = _mm_set_ps1((s->xlim[1] - s->xlim[0]) / s->width);
    __m128 yscale = _mm_set_ps1((s->ylim[1] - s->ylim[0]) / s->height);
    __m128 threshold = _mm_set_ps1(4);
    __m128 one = _mm_set_ps1(1);
    __m128 iter_scale = _mm_set_ps1(1.0f / s->iterations);
    __m128 depth_scale = _mm_set_ps1(s->depth - 1);

    for (int y = 0; y < s->height; y++) {
        for (int x = 0; x < s->width; x += 4) {
            __m128 mx = _mm_set_ps(x + 3, x + 2, x + 1, x + 0);
            __m128 my = _mm_set_ps1(y);
            __m128 cr = _mm_add_ps(_mm_mul_ps(mx, xscale), xmin);
            __m128 ci = _mm_add_ps(_mm_mul_ps(my, yscale), ymin);
            __m128 zr = cr;
            __m128 zi = ci;
            int k = 1;
            __m128 mk = _mm_set_ps1(k);
            while (++k < s->iterations) {
                /* Compute z1 from z0 */
                __m128 zr2 = _mm_mul_ps(zr, zr);
                __m128 zi2 = _mm_mul_ps(zi, zi);
                __m128 zrzi = _mm_mul_ps(zr, zi);
                /* zr1 = zr0 * zr0 - zi0 * zi0 + cr */
                /* zi1 = zr0 * zi0 + zr0 * zi0 + ci */
                zr = _mm_add_ps(_mm_sub_ps(zr2, zi2), cr);
                zi = _mm_add_ps(_mm_add_ps(zrzi, zrzi), ci);

                /* Increment k */
                zr2 = _mm_mul_ps(zr, zr);
                zi2 = _mm_mul_ps(zi, zi);
                __m128 mag2 = _mm_add_ps(zr2, zi2);
                __m128 mask = _mm_cmplt_ps(mag2, threshold);
                mk = _mm_add_ps(_mm_and_ps(mask, one), mk);

                /* Early bailout? */
                if (_mm_movemask_ps(mask) == 0)
                    break;
            }
            mk = _mm_mul_ps(mk, iter_scale);
            mk = _mm_sqrt_ps(mk);
            mk = _mm_mul_ps(mk, depth_scale);
            __m128i pixels = _mm_cvtps_epi32(mk);
            unsigned char *dst = image + y * s->width * 3 + x * 3;
            unsigned char *src = (unsigned char *)&pixels;
            for (int i = 0; i < 4; i++) {
                dst[i * 3 + 0] = src[i * 4];
                dst[i * 3 + 1] = src[i * 4];
                dst[i * 3 + 2] = src[i * 4];
            }
        }
    }
}

void mandel_avx(unsigned char* image, const struct spec *s)
{
    __m256 xmin = _mm256_set1_ps(s->xlim[0]);
    __m256 ymin = _mm256_set1_ps(s->ylim[0]);
    __m256 xscale = _mm256_set1_ps((s->xlim[1] - s->xlim[0]) / s->width);
    __m256 yscale = _mm256_set1_ps((s->ylim[1] - s->ylim[0]) / s->height);
    __m256 threshold = _mm256_set1_ps(4);
    __m256 one = _mm256_set1_ps(1);
    __m256 iter_scale = _mm256_set1_ps(1.0f / s->iterations);
    __m256 depth_scale = _mm256_set1_ps(s->depth - 1);

    for (int y = 0; y < s->height; y++) {
        for (int x = 0; x < s->width; x += 8) {
            __m256 mx = _mm256_set_ps(x + 7, x + 6, x + 5, x + 4,
                                      x + 3, x + 2, x + 1, x + 0);
            __m256 my = _mm256_set1_ps(y);
            __m256 cr = _mm256_add_ps(_mm256_mul_ps(mx, xscale), xmin);
            __m256 ci = _mm256_add_ps(_mm256_mul_ps(my, yscale), ymin);
            __m256 zr = cr;
            __m256 zi = ci;
            int k = 1;
            __m256 mk = _mm256_set1_ps(k);
            while (++k < s->iterations) {
                /* Compute z1 from z0 */
                __m256 zr2 = _mm256_mul_ps(zr, zr);
                __m256 zi2 = _mm256_mul_ps(zi, zi);
                __m256 zrzi = _mm256_mul_ps(zr, zi);
                /* zr1 = zr0 * zr0 - zi0 * zi0 + cr */
                /* zi1 = zr0 * zi0 + zr0 * zi0 + ci */
                zr = _mm256_add_ps(_mm256_sub_ps(zr2, zi2), cr);
                zi = _mm256_add_ps(_mm256_add_ps(zrzi, zrzi), ci);

                /* Increment k */
                zr2 = _mm256_mul_ps(zr, zr);
                zi2 = _mm256_mul_ps(zi, zi);
                __m256 mag2 = _mm256_add_ps(zr2, zi2);
                __m256 mask = _mm256_cmp_ps(mag2, threshold, _CMP_LT_OS);
                mk = _mm256_add_ps(_mm256_and_ps(mask, one), mk);

                /* Early bailout? */
                if (_mm256_testz_ps(mask, _mm256_set1_ps(-1)))
                    break;
            }
            mk = _mm256_mul_ps(mk, iter_scale);
            mk = _mm256_sqrt_ps(mk);
            mk = _mm256_mul_ps(mk, depth_scale);
            __m256i pixels = _mm256_cvtps_epi32(mk);
            unsigned char *dst = image + y * s->width * 3 + x * 3;
            unsigned char *src = (unsigned char *)&pixels;
            for (int i = 0; i < 8; i++) {
                dst[i * 3 + 0] = src[i * 4];
                dst[i * 3 + 1] = src[i * 4];
                dst[i * 3 + 2] = src[i * 4];
            }
        }
    }
}

void mandel_avx512(unsigned char* image, const struct spec *s)
{

    __m512 xmin = _mm512_set1_ps(s->xlim[0]);
    __m512 ymin = _mm512_set1_ps(s->ylim[0]);
    __m512 xscale = _mm512_set1_ps((s->xlim[1] - s->xlim[0]) / s->width);
    __m512 yscale = _mm512_set1_ps((s->ylim[1] - s->ylim[0]) / s->height);
    __m512 threshold = _mm512_set1_ps(4);
    __m512 one = _mm512_set1_ps(1);
    __m512 iter_scale = _mm512_set1_ps(1.0f / s->iterations);
    __m512 depth_scale = _mm512_set1_ps(s->depth - 1);

    for (int y = 0; y < s->height; y++) {
        for (int x = 0; x < s->width; x += 16) {
            __m512 mx = _mm512_set_ps(x + 15, x + 14, x + 13, x + 12,
                                      x + 11, x + 10, x + 9, x + 8,
                                      x + 7, x + 6, x + 5, x + 4,
                                      x + 3, x + 2, x + 1, x + 0);
            __m512 my = _mm512_set1_ps(y);
            __m512 cr = _mm512_add_ps(_mm512_mul_ps(mx, xscale), xmin);
            __m512 ci = _mm512_add_ps(_mm512_mul_ps(my, yscale), ymin);
            __m512 zr = cr;
            __m512 zi = ci;
            int k = 1;
            __m512 mk = _mm512_set1_ps(k);
            while (++k < s->iterations) {
                /* Compute z1 from z0 */
                __m512 zr2 = _mm512_mul_ps(zr, zr);
                __m512 zi2 = _mm512_mul_ps(zi, zi);
                __m512 zrzi = _mm512_mul_ps(zr, zi);
                /* zr1 = zr0 * zr0 - zi0 * zi0 + cr */
                /* zi1 = zr0 * zi0 + zr0 * zi0 + ci */
                zr = _mm512_add_ps(_mm512_sub_ps(zr2, zi2), cr);
                zi = _mm512_add_ps(_mm512_add_ps(zrzi, zrzi), ci);

                /* Increment k */
                zr2 = _mm512_mul_ps(zr, zr);
                zi2 = _mm512_mul_ps(zi, zi);
                __m512 mag2 = _mm512_add_ps(zr2, zi2);
                __mmask16 mask = _mm512_cmp_ps_mask(mag2, threshold, _CMP_LT_OS);
                mk = _mm512_mask_add_ps(mk, mask, one, mk);

                /* Early bailout? */
                if (_cvtmask16_u32(mask) == 0)
                    break;
            }
            mk = _mm512_mul_ps(mk, iter_scale);
            mk = _mm512_sqrt_ps(mk);
            mk = _mm512_mul_ps(mk, depth_scale);
            __m512i pixels = _mm512_cvtps_epi32(mk);
            unsigned char *dst = image + y * s->width * 3 + x * 3;
            unsigned char *src = (unsigned char *)&pixels;
            for (int i = 0; i < 16; i++) {
                dst[i * 3 + 0] = src[i * 4];
                dst[i * 3 + 1] = src[i * 4];
                dst[i * 3 + 2] = src[i * 4];
            }
        }
    }
}

static void MandelbrotBase(benchmark::State& state) {
    auto width = 1000;
    auto height = 500;
    auto image = std::make_unique<unsigned char[]>(width*height*3+64);
    for (auto _ : state)
    {
        spec s = {width, height, 256, {-1.5, 0.5}, {-1.0, 1.0}, static_cast<int>(state.range(0))};
        mandel_basic(image.get(), &s);
        benchmark::DoNotOptimize(image);
        benchmark::DoNotOptimize(image.get());
    }
}

static void MandelbrotSSE(benchmark::State& state) {
    auto width = 1000;
    auto height = 500;
    auto image = std::make_unique<unsigned char[]>(width*height*3+64);
    for (auto _ : state)
    {
        spec s = {width, height, 256, {-1.5, 0.5}, {-1.0, 1.0}, static_cast<int>(state.range(0))};
        mandel_sse2(image.get(), &s);
        benchmark::DoNotOptimize(image);
        benchmark::DoNotOptimize(image.get());
    }
}

static void MandelbrotAvx2(benchmark::State& state) {
    auto width = 1000;
    auto height = 500;
    auto image = std::make_unique<unsigned char[]>(width*height*3+64);
    for (auto _ : state)
    {
        spec s = {width, height, 256, {-1.5, 0.5}, {-1.0, 1.0}, static_cast<int>(state.range(0))};
        mandel_avx(image.get(), &s);
        benchmark::DoNotOptimize(image);
        benchmark::DoNotOptimize(image.get());
    }
}

static void MandelbrotAvx512(benchmark::State& state) {
    auto width = 1000;
    auto height = 500;
    auto image = std::make_unique<unsigned char[]>(width*height*3+64);
    for (auto _ : state)
    {
        spec s = {width, height, 256, {-1.5, 0.5}, {-1.0, 1.0}, static_cast<int>(state.range(0))};
        mandel_avx512(image.get(), &s);
        benchmark::DoNotOptimize(image);
        benchmark::DoNotOptimize(image.get());
    }
}

BENCHMARK(MandelbrotBase)->Unit(benchmark::kMillisecond)->Arg(1)->Arg(10)->Arg(100)->Arg(1000)->Arg(10000)->Arg(100000);
BENCHMARK(MandelbrotSSE)->Unit(benchmark::kMillisecond)->Arg(1)->Arg(10)->Arg(100)->Arg(1000)->Arg(10000)->Arg(100000);
BENCHMARK(MandelbrotAvx2)->Unit(benchmark::kMillisecond)->Arg(1)->Arg(10)->Arg(100)->Arg(1000)->Arg(10000)->Arg(100000);
BENCHMARK(MandelbrotAvx512)->Unit(benchmark::kMillisecond)->Arg(1)->Arg(10)->Arg(100)->Arg(1000)->Arg(10000)->Arg(100000);

BENCHMARK_MAIN();
