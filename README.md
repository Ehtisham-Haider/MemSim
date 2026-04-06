# MemSim — Dynamic Memory Allocation Simulator

Desktop simulator for classic OS dynamic memory allocation strategies:
- First-Fit
- Next-Fit
- Best-Fit
- Worst-Fit

## Features
- Independent memory pools per algorithm
- Allocation/deallocation with split and coalescing rules
- Fragmentation reports and comparative dashboard
- Stress test engine (10 randomized requests)
- Exportable simulation log
- Qt 6 + QML modern UI

## Build
```bash
cmake --preset debug
cmake --build --preset build-debug
ctest --preset test-debug
```

## Run
```bash
./build/debug/MemSim
```
