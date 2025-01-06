
#include <benchmark/benchmark.h>
#include "GameTimerW.h"
#include "GameTimerCr.h"

// tick
static void BM_GameTimerCr_Tick_Range(benchmark::State& state)
{
    for (auto _ : state)
    {
        GameTimerCr timer;
        for (int i = 0; i < state.range(0); ++i)
        {
            timer.Tick();
        }
    }
}
BENCHMARK(BM_GameTimerCr_Tick_Range)->RangeMultiplier(2)->Range(1, 64)->Complexity();

static void BM_GameTimerW_Tick_Range(benchmark::State& state)
{
    for (auto _ : state)
    {
        GameTimerW timer;
        for (int i = 0; i < state.range(0); ++i)
        {
            timer.Tick();
        }
    }
}
BENCHMARK(BM_GameTimerW_Tick_Range)->RangeMultiplier(2)->Range(1, 64)->Complexity();


//Get Delta time
static void BM_GameTimerCr_GetDeltaTime_Range(benchmark::State& state)
{
    for (auto _ : state)
    {
        GameTimerCr timer;
        float dt = 0;
        for (int i = 0; i < state.range(0); ++i)
        {
            dt += timer.GetDeltaTime();
        }
    }
}
BENCHMARK(BM_GameTimerCr_GetDeltaTime_Range)->RangeMultiplier(2)->Range(1, 64)->Complexity();

static void BM_GameTimerW_GetDeltaTime_Range(benchmark::State& state)
{
    for (auto _ : state)
    {
        GameTimerW timer;
        float dt = 0;
        for (int i = 0; i < state.range(0); ++i)
        {
            dt += timer.GetDeltaTime();
        }
    }
}
BENCHMARK(BM_GameTimerW_GetDeltaTime_Range)->RangeMultiplier(2)->Range(1, 64)->Complexity();



// Тест Start/Stop операций
template <typename TimerType>
static void BM_Timer_StartStop(benchmark::State& state)
{
    for (auto _ : state)
    {
        TimerType timer;
        for (int i = 0; i < state.range(0); ++i)
        {
            timer.Stop();
            timer.Start();
            timer.Tick();
            benchmark::DoNotOptimize(timer.GetDeltaTime());
        }
    }
}

BENCHMARK_TEMPLATE(BM_Timer_StartStop, GameTimerCr)->RangeMultiplier(2)->Range(1, 64);
BENCHMARK_TEMPLATE(BM_Timer_StartStop, GameTimerW)->RangeMultiplier(2)->Range(1, 64);