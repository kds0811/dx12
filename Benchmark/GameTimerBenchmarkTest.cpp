
#include <benchmark/benchmark.h>
#include "GameTimerW.h"
#include "GameTimerCr.h"
#include "GameTimerCr1.h"
#include "GameTimerCr2.h"
#include <thread>
#include <vector>

// ������� ��������� � ������ ����������� ��������
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
BENCHMARK(BM_GameTimerCr_Tick_Range)
    ->Range(8, 8 << 10)  // �� 8 �� 8192 ��������
    ->Complexity();      // ������ ���������

static void BM_GameTimerCr1_Tick_Range(benchmark::State& state)
{
    for (auto _ : state)
    {
        GameTimerCr1 timer;
        for (int i = 0; i < state.range(0); ++i)
        {
            timer.Tick();
        }
    }
}
BENCHMARK(BM_GameTimerCr1_Tick_Range)->Range(8, 8 << 10)->Complexity();

static void BM_GameTimerCr2_Tick_Range(benchmark::State& state)
{
    for (auto _ : state)
    {
        GameTimerCr2 timer;
        for (int i = 0; i < state.range(0); ++i)
        {
            timer.Tick();
        }
    }
}
BENCHMARK(BM_GameTimerCr2_Tick_Range)->Range(8, 8 << 10)->Complexity();

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
BENCHMARK(BM_GameTimerW_Tick_Range)->Range(8, 8 << 10)->Complexity();

// ���� Start/Stop ��������
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

BENCHMARK_TEMPLATE(BM_Timer_StartStop, GameTimerCr)->Range(8, 8 << 10);
BENCHMARK_TEMPLATE(BM_Timer_StartStop, GameTimerCr1)->Range(8, 8 << 10);
BENCHMARK_TEMPLATE(BM_Timer_StartStop, GameTimerCr2)->Range(8, 8 << 10);
BENCHMARK_TEMPLATE(BM_Timer_StartStop, GameTimerW)->Range(8, 8 << 10);