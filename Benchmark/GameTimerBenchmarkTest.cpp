
#include <benchmark/benchmark.h>
#include "GameTimerW.h"
#include "GameTimerCr.h"

#define REPEAT2(x) x x
#define REPEAT4(x) REPEAT2(x) REPEAT2(x)
#define REPEAT8(x) REPEAT4(x) REPEAT4(x)
#define REPEAT16(x) REPEAT8(x) REPEAT8(x)
#define REPEAT32(x) REPEAT16(x) REPEAT16(x)
#define REPEAT(x) REPEAT32(x)

// virtual
class A
{
public:
    A() : i_(0) {}
    virtual ~A() {}
    virtual void f(int i) = 0;
    int get() const { return i_; }
protected:
    int i_;
};

class B : public A
{
public:
    void f(int i) override
    {
        i_ += i;  
    }
};  

// virtual Final
class X
{
public:
    X() : i_(0) {}
    virtual ~X() {}
    virtual void f(int i) = 0;
    int get() const { return i_; }

protected:
    int i_;
};

class Y final : public X 
{
public:
    void f(int i) override final { i_ += i; }
};  

// Non virtual
class C
{

public:
    C() : i_(0) {}
    void f(int i) { i_ += i; }
    int get() const { return i_; }
protected:
    int i_;
};

// CRTP
template <typename D>
class E
{
public:
    E() : i_(0) {}
    void f(int i) { static_cast<D*>(this)->f(i); }
    int get() const { return i_; }

protected:
    int i_;
};
class D : public E<D>
{
public:
    void f(int i) { i_ += i; }
};

// Test
void BM_none(benchmark::State& state)
{
    C* c = new C;
    int i = 0;
    for (auto _ : state)
    {
        REPEAT(c->f(++i);)
    }
    benchmark::DoNotOptimize(c->get());
    delete c;
}
void BM_dynamic(benchmark::State& state)
{
    A* a = new B;
    int i = 0;
    for (auto _ : state)
    {
        REPEAT(a->f(++i);)
    }
        
    benchmark::DoNotOptimize(a->get());
    delete a;
}

void BM_dynamicFinal(benchmark::State& state)
{
    X* x = new Y;
    int i = 0;
    for (auto _ : state)
    {
        REPEAT(x->f(++i);)
    }

    benchmark::DoNotOptimize(x->get());
    delete x;
}
void BM_CRTP(benchmark::State& state)
{
    E<D>* e = new D;
    int i = 0;
    for (auto _ : state)
    {
        REPEAT(e->f(++i);)
    }
    benchmark::DoNotOptimize(e->get());
}

BENCHMARK(BM_none)->Range(1, 16);
BENCHMARK(BM_dynamic)->Range(1, 16);
BENCHMARK(BM_dynamicFinal)->Range(1, 16);
BENCHMARK(BM_CRTP)->Range(1, 16); 

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
// BENCHMARK(BM_GameTimerCr_Tick_Range)->RangeMultiplier(2)->Range(1, 64)->Complexity();
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
// BENCHMARK(BM_GameTimerW_Tick_Range)->RangeMultiplier(2)->Range(1, 64)->Complexity();
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
// BENCHMARK(BM_GameTimerCr_GetDeltaTime_Range)->RangeMultiplier(2)->Range(1, 64)->Complexity();
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
// BENCHMARK(BM_GameTimerW_GetDeltaTime_Range)->RangeMultiplier(2)->Range(1, 64)->Complexity();
//
//
//
//// Тест Start/Stop операций
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
// BENCHMARK_TEMPLATE(BM_Timer_StartStop, GameTimerCr)->RangeMultiplier(2)->Range(1, 64);
// BENCHMARK_TEMPLATE(BM_Timer_StartStop, GameTimerW)->RangeMultiplier(2)->Range(1, 64);