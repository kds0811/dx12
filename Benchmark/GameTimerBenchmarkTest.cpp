
//#include <benchmark/benchmark.h>
//#include "GameTimerW.h"
//#include "GameTimerCr.h"
//#include "Utils/Math/Vector.h"
//#include <vector>
//#include <limits>


//
//
// static void BM_VectorLength(benchmark::State& state)
//{
//    Vector v1[16];
//    float res[16];
//    for (size_t i = 0; i < 16; ++i)
//    {
//        v1[i] = Vector{(float)i + 1.0f, (float)i + 2.0f, (float)i + 3.0f};
//    }
//
//    for (auto _ : state)
//    {
//
//        for (size_t i = 0; i < 16; ++i)
//        {
//            res[i] = v1[i].Length();
//        }
//        benchmark::DoNotOptimize(res);
//        benchmark::ClobberMemory();
//    }
//}
// BENCHMARK(BM_VectorLength);
//
//
// static void BM_VectorLengthSq(benchmark::State& state)
//{
//    Vector v1[16];
//    float res[16];
//    for (size_t i = 0; i < 16; ++i)
//    {
//        v1[i] = Vector{(float)i + 1.0f, (float)i + 2.0f, (float)i + 3.0f};
//    }
//
//    for (auto _ : state)
//    {
//
//        for (size_t i = 0; i < 16; ++i)
//        {
//            res[i] = v1[i].LengthSq();
//        }
//        benchmark::DoNotOptimize(res);
//        benchmark::ClobberMemory();
//    }
//}
// BENCHMARK(BM_VectorLengthSq);
//
// static void BM_VectorLengthEst(benchmark::State& state)
//{
//    Vector v1[16];
//    float res[16];
//    for (size_t i = 0; i < 16; ++i)
//    {
//        v1[i] = Vector{(float)i + 1.0f, (float)i + 2.0f, (float)i + 3.0f};
//    }
//
//    for (auto _ : state)
//    {
//
//        for (size_t i = 0; i < 16; ++i)
//        {
//            res[i] = v1[i].LengthEst();
//        }
//        benchmark::DoNotOptimize(res);
//        benchmark::ClobberMemory();
//    }
//}
// BENCHMARK(BM_VectorLengthEst);

//
//// Benchmark ��� ��������� +=
// static void BM_VectorAddAssign_SIMD(benchmark::State& state)
//{
//     Vector v1{1.0f, 2.0f, 3.0f};
//     Vector v2{4.0f, 5.0f, 6.0f};
//
//     for (auto _ : state)
//     {
//         benchmark::DoNotOptimize(v1 += v2);
//         benchmark::ClobberMemory();
//     }
// }
// BENCHMARK(BM_VectorAddAssign_SIMD);
//
//
//// Benchmark ��� ��������� +
// static void BM_VectorAdd_SIMD(benchmark::State& state)
//{
//     Vector v1{1.0f, 2.0f, 3.0f};
//     Vector v2{4.0f, 5.0f, 6.0f};
//
//     for (auto _ : state)
//     {
//         Vector result = v1 + v2;
//         benchmark::DoNotOptimize(result);
//         benchmark::ClobberMemory();
//     }
// }
// BENCHMARK(BM_VectorAdd_SIMD);
//
//
//
//// Benchmark ��� ��������� -=
// static void BM_VectorSubtractAssign_SIMD(benchmark::State& state)
//{
//     Vector v1{1.0f, 2.0f, 3.0f};
//     Vector v2{4.0f, 5.0f, 6.0f};
//
//     for (auto _ : state)
//     {
//         benchmark::DoNotOptimize(v1 -= v2);
//         benchmark::ClobberMemory();
//     }
// }
// BENCHMARK(BM_VectorSubtractAssign_SIMD);
//
//
//
//
//// Benchmark ��� ��������� -
// static void BM_VectorSubtract_SIMD(benchmark::State& state)
//{
//     Vector v1{1.0f, 2.0f, 3.0f};
//     Vector v2{4.0f, 5.0f, 6.0f};
//
//     for (auto _ : state)
//     {
//         Vector result = v1 - v2;
//         benchmark::DoNotOptimize(result);
//         benchmark::ClobberMemory();
//     }
// }
// BENCHMARK(BM_VectorSubtract_SIMD);
//
//
//// Benchmark ��� ��������� -
// static void BM_VectorSubtractArray_SIMD(benchmark::State& state)
//{
//     Vector v1[16];
//     Vector v2[16];
//     Vector v3[16];
//     for (size_t i = 0; i < 16; ++i)
//     {
//         v1[i] = Vector{(float)i + (float)i, (float)i + (float)i, (float)i + (float)i};
//         v2[i] = Vector{(float)i, (float)i, (float)i};
//     }
//
//     for (auto _ : state)
//     {
//         for (size_t i = 0; i < 16; ++i)
//         {
//             v3[i] = v1[i] - v2[i];
//         }
//         benchmark::DoNotOptimize(v3);
//         benchmark::ClobberMemory();
//     }
// }
// BENCHMARK(BM_VectorSubtractArray_SIMD);

//
//// tick
// static void BM_GameTimerCr_Tick_Range(benchmark::State& state)
//{
//     for (auto _ : state)
//     {
//         GameTimerCr timer;
//         for (int i = 0; i < state.range(0); ++i)
//         {
//             timer.Tick();
//         }
//     }
// }
// BENCHMARK(BM_GameTimerCr_Tick_Range)->RangeMultiplier(4)->Range(2, 64)->Complexity();
//
// static void BM_GameTimerW_Tick_Range(benchmark::State& state)
//{
//     for (auto _ : state)
//     {
//         GameTimerW timer;
//         for (int i = 0; i < state.range(0); ++i)
//         {
//             timer.Tick();
//         }
//     }
// }
// BENCHMARK(BM_GameTimerW_Tick_Range)->RangeMultiplier(4)->Range(2, 64)->Complexity();
//
//
////Get Delta time
// static void BM_GameTimerCr_GetDeltaTime_Range(benchmark::State& state)
//{
//     for (auto _ : state)
//     {
//         GameTimerCr timer;
//         float dt = 0;
//         for (int i = 0; i < state.range(0); ++i)
//         {
//             dt += timer.GetDeltaTime();
//         }
//     }
// }
// BENCHMARK(BM_GameTimerCr_GetDeltaTime_Range)->RangeMultiplier(4)->Range(2, 64)->Complexity();
//
// static void BM_GameTimerW_GetDeltaTime_Range(benchmark::State& state)
//{
//     for (auto _ : state)
//     {
//         GameTimerW timer;
//         float dt = 0;
//         for (int i = 0; i < state.range(0); ++i)
//         {
//             dt += timer.GetDeltaTime();
//         }
//     }
// }
// BENCHMARK(BM_GameTimerW_GetDeltaTime_Range)->RangeMultiplier(4)->Range(2, 64)->Complexity();
//
//
//
//// ���� Start/Stop ��������
// template <typename TimerType>
// static void BM_Timer_StartStop(benchmark::State& state)
//{
//     for (auto _ : state)
//     {
//         TimerType timer;
//         for (int i = 0; i < state.range(0); ++i)
//         {
//             timer.Stop();
//             timer.Start();
//             timer.Tick();
//             benchmark::DoNotOptimize(timer.GetDeltaTime());
//         }
//     }
// }
//
// BENCHMARK_TEMPLATE(BM_Timer_StartStop, GameTimerCr)->RangeMultiplier(4)->Range(2, 64);
// BENCHMARK_TEMPLATE(BM_Timer_StartStop, GameTimerW)->RangeMultiplier(4)->Range(2, 64);