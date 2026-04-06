# Product Requirements Document
## Dynamic Memory Allocation Simulator
### CS 330 — Operating Systems Open-Ended Lab | BESE-30 | Spring 2026

---

**Version:** 1.0  
**Tech Stack:** C++17 · Qt 6.x · QML · CMake  
**CLO Mapping:** CLO-2, CLO-4  
**Total Marks:** 15  

---

## Table of Contents

1. [Project Overview](#1-project-overview)
2. [Academic Requirements Mapping](#2-academic-requirements-mapping)
3. [System Architecture](#3-system-architecture)
4. [Module Specifications](#4-module-specifications)
5. [GUI Design — Complete Specification](#5-gui-design--complete-specification)
6. [Algorithm Specifications](#6-algorithm-specifications)
7. [Data Structures](#7-data-structures)
8. [Core Logic & Rules](#8-core-logic--rules)
9. [Stress Test Engine](#9-stress-test-engine)
10. [Comparative Analysis Engine](#10-comparative-analysis-engine)
11. [File Structure & Project Layout](#11-file-structure--project-layout)
12. [Build Configuration](#12-build-configuration)
13. [Coding Standards & Best Practices](#13-coding-standards--best-practices)
14. [Deliverables Checklist](#14-deliverables-checklist)

---

## 1. Project Overview

### 1.1 Purpose

This project implements a **Dynamic Memory Allocation Simulator** that visually demonstrates and compares four classical memory allocation algorithms:

- **First-Fit** — Allocates the first sufficiently large free block.
- **Next-Fit** — Like First-Fit but resumes from where the last allocation left off.
- **Best-Fit** — Allocates the smallest block that is large enough.
- **Worst-Fit** — Allocates the largest available block.

Each algorithm operates on its **own independent copy** of the same memory pool. The simulator provides a modern, animated Qt/QML GUI that displays real-time memory states, allocation/deallocation events, fragmentation metrics, and a side-by-side comparative dashboard.

### 1.2 Scope

The simulator is a **desktop application** built on Qt 6 with QML for the UI layer and C++ for all business logic and algorithms. It is not a real OS memory manager — it is a pedagogical simulation for academic study.

### 1.3 Key Outcomes

- Visual, step-by-step memory allocation and deallocation demonstrations.
- Automatic block splitting when size difference exceeds 20 KB.
- Automatic coalescing (merging) of adjacent free blocks on deallocation.
- Fragmentation reporting after every operation.
- Automated stress testing with 10 randomised allocation requests.
- A comparative summary table across all four algorithms.
- Exportable simulation log for inclusion in the lab report.

---

## 2. Academic Requirements Mapping

Each lab requirement is mapped to a specific module and feature.

| # | Lab Requirement | Module | Feature ID |
|---|---|---|---|
| R1 | 10+ free blocks, ≥1024 KB total | `MemoryPool` | POOL-01 |
| R2 | Memory layout diagram | QML `MemoryMapView` | GUI-01 |
| R3 | All 4 algorithms, independent pools | `AlgorithmEngine` | ALGO-01..04 |
| R4 | ≥5 allocations, ≥2 deallocations | `SimulationController` | SIM-01 |
| R5 | At least one failing allocation | `AlgorithmEngine::allocate()` | ALGO-05 |
| R6 | Print Block ID, Start, Size, Status, PID | `MemoryTableModel` | MDL-01 |
| R7 | After-every-operation display refresh | QML bindings + signals | GUI-02 |
| R8 | Fragmentation report per algorithm | `FragmentationAnalyser` | FRAG-01 |
| R9 | Comparative summary table | `ComparativeAnalyser` | COMP-01 |
| R10 | Block splitting (diff > 20 KB) | `AlgorithmEngine::split()` | ALGO-06 |
| R11 | Block coalescing | `AlgorithmEngine::coalesce()` | ALGO-07 |
| R12 | Merge notification message | QML `EventLog` | GUI-03 |
| R13 | Stress test (10 random allocs) | `StressTestEngine` | STRESS-01 |
| R14 | Comparative justification table | `ComparativeAnalyser` + Report | COMP-02 |

---

## 3. System Architecture

### 3.1 Layered Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                     QML / UI Layer                          │
│  DashboardPage · AlgorithmTabView · ComparativePage         │
│  MemoryMapView · StressTestPage · EventLogPanel             │
└─────────────────────────┬───────────────────────────────────┘
                          │  Q_PROPERTY / signals / slots
┌─────────────────────────▼───────────────────────────────────┐
│               Controller / ViewModel Layer (C++)            │
│  SimulationController · StressTestEngine                    │
│  ComparativeAnalyser  · ExportManager                       │
└──────┬──────────────────┬──────────────────┬────────────────┘
       │                  │                  │
┌──────▼──────┐  ┌────────▼────┐  ┌─────────▼──────┐
│ Algorithm   │  │MemoryPool   │  │Fragmentation   │
│ Engine      │  │Manager      │  │Analyser        │
│ (4 algos)   │  │(4 copies)   │  │                │
└─────────────┘  └─────────────┘  └────────────────┘
       │
┌──────▼──────────────────────────────────────────────────────┐
│                   Core Data Layer (C++)                     │
│  MemoryBlock · MemoryPool · AllocationRequest               │
│  AllocationResult · FragmentationReport · SimEvent          │
└─────────────────────────────────────────────────────────────┘
```

### 3.2 Design Patterns Used

| Pattern | Application |
|---|---|
| **MVC / MVVM** | QML = View, C++ ViewModels expose `Q_PROPERTY`, Models hold data |
| **Strategy Pattern** | Each algorithm is a concrete strategy implementing `IAllocationStrategy` |
| **Observer Pattern** | Qt signals/slots for model-to-view updates |
| **Factory Method** | `AlgorithmFactory::create(AlgorithmType)` produces strategy instances |
| **Command Pattern** | `AllocationRequest` objects encapsulate each user-issued memory command |
| **Singleton** | `SimulationController` (app-level coordinator) |
| **Repository Pattern** | `MemoryPoolManager` manages four independent pool instances |

### 3.3 Threading Model

- **Main thread:** Qt event loop, QML rendering, user interactions.
- **Worker thread:** `StressTestEngine` uses `QThread` + `QRunnable` so the UI never freezes during 10-request stress tests. Results posted back via `QMetaObject::invokeMethod` on the main thread.
- **No shared mutable state** between threads — each pool copy is owned by a single thread at a time.

---

## 4. Module Specifications

### 4.1 `MemoryBlock` (Core Data)

**File:** `src/core/MemoryBlock.h / .cpp`

```
Fields:
  int     blockId          // Unique sequential ID (BLK-0, BLK-1, ...)
  int     startAddress     // Byte offset from pool base (KB units)
  int     sizeKB           // Block size in KB
  bool    isFree           // Allocation status
  int     processId        // Owning PID (-1 if free)
  QString processLabel     // Human-readable process name (e.g. "P1")
```

**Methods:**
- `QString statusString() const` → `"FREE"` or `"ALLOCATED"`
- `QString summary() const` → formatted one-line block description

### 4.2 `MemoryPool` (Core Data)

**File:** `src/core/MemoryPool.h / .cpp`

```
Fields:
  QVector<MemoryBlock> blocks    // Ordered by startAddress
  QString              name      // Algorithm label ("First-Fit", etc.)
  int                  nextFitCursor  // Used only by Next-Fit

Methods:
  void           reset(const QVector<MemoryBlock>& initial)
  int            totalFreeKB() const
  int            totalAllocatedKB() const
  int            freeBlockCount() const
  int            largestFreeBlockKB() const
  bool           hasExternalFragmentation() const
  MemoryBlock*   blockAt(int index)
  int            blockCount() const
  void           insertBlock(int index, const MemoryBlock& b)
  void           removeBlock(int index)
```

### 4.3 `IAllocationStrategy` (Abstract Interface)

**File:** `src/algorithms/IAllocationStrategy.h`

```cpp
class IAllocationStrategy {
public:
    virtual ~IAllocationStrategy() = default;

    // Attempt to allocate sizeKB for processId.
    // Returns index of chosen block, or -1 on failure.
    virtual int findBlock(MemoryPool& pool, int sizeKB) = 0;

    // Algorithm name for display
    virtual QString name() const = 0;
    virtual QString shortName() const = 0;
};
```

### 4.4 `AlgorithmEngine`

**File:** `src/algorithms/AlgorithmEngine.h / .cpp`

Owns one `MemoryPool` instance and one `IAllocationStrategy` instance.

**Methods:**

```
AllocationResult allocate(int sizeKB, int processId, const QString& label)
  - Calls strategy->findBlock()
  - If no block found → returns FAILED result with reason
  - If found → calls split() if sizeKB difference > 20 KB
  - Marks block ALLOCATED, assigns processId
  - Appends SimEvent to event log

AllocationResult deallocate(int blockId)
  - Finds block by ID
  - Marks it FREE, clears processId
  - Calls coalesce() on adjacent blocks
  - Appends SimEvent to event log

void split(int blockIndex, int requestedSizeKB)
  - Computes remainder = block.sizeKB - requestedSizeKB
  - If remainder > 20 KB:
      - Resize original block to requestedSizeKB
      - Insert new FREE block at blockIndex+1 with remainder size
      - Emit splitOccurred(originalId, newBlockId)

void coalesce(int blockIndex)
  - Check block at blockIndex-1: if free → merge into current
  - Check block at blockIndex+1: if free → merge into current
  - For each merge: emit mergeOccurred(blockA_Id, blockB_Id)

FragmentationReport generateFragmentationReport() const
const QVector<SimEvent>& eventLog() const
void resetPool(const QVector<MemoryBlock>& initial)
```

### 4.5 Algorithm Implementations

**Files:** `src/algorithms/FirstFit.h`, `NextFit.h`, `BestFit.h`, `WorstFit.h`

Each is a concrete class implementing `IAllocationStrategy::findBlock()`.

| Class | Strategy |
|---|---|
| `FirstFit` | Scan from index 0; return index of first block with `sizeKB >= requested` |
| `NextFit` | Scan from `pool.nextFitCursor`; wrap around; update cursor on success |
| `BestFit` | Scan all; return index of smallest block with `sizeKB >= requested` |
| `WorstFit` | Scan all; return index of largest free block |

### 4.6 `FragmentationReport`

**File:** `src/core/FragmentationReport.h`

```
Fields:
  QString algorithmName
  int     totalFreeKB
  int     freeBlockCount
  int     largestFreeBlockKB
  bool    externalFragmentationPresent
  double  fragmentationRatio      // freeBlockCount / (totalBlocks)
  int     totalAllocatedKB
  int     failedAllocations
  QVector<int> freeBlockSizes     // for distribution display
```

### 4.7 `SimulationController`

**File:** `src/controller/SimulationController.h / .cpp`

Top-level QObject, registered with QML as a context property. Owns four `AlgorithmEngine` instances (one per algorithm). Exposes all operations via `Q_INVOKABLE` methods and all state via `Q_PROPERTY`.

**Q_PROPERTY list:**

```
Q_PROPERTY(QVariantList allAlgorithmStates  READ allAlgorithmStates  NOTIFY stateChanged)
Q_PROPERTY(QVariantList eventLog            READ eventLog            NOTIFY eventLogChanged)
Q_PROPERTY(bool         simulationRunning   READ simulationRunning   NOTIFY simulationRunningChanged)
Q_PROPERTY(QVariantMap  comparativeReport   READ comparativeReport   NOTIFY comparativeReportChanged)
```

**Q_INVOKABLE methods:**

```
void allocate(int sizeKB, int processId, const QString& label)
void deallocate(int blockId, const QString& algorithmName)
void resetAll()
void loadPreset(const QString& presetName)
QVariantList getFragmentationReports()
void exportLog(const QString& filePath)
```

### 4.8 `StressTestEngine`

**File:** `src/controller/StressTestEngine.h / .cpp`

Runs in a `QThread`. Generates 10 random allocation requests (size 10–150 KB, random process IDs) and applies them to each algorithm's engine sequentially, collecting results.

**Signals:**
```
void progressUpdated(int requestIndex, int total)
void requestCompleted(int algorithmIndex, int requestIndex, bool success, const QString& detail)
void stressTestFinished(QVariantList results)
```

### 4.9 `ComparativeAnalyser`

**File:** `src/controller/ComparativeAnalyser.h / .cpp`

Compares four `FragmentationReport` objects and produces structured comparison data.

**Output:** `QVariantMap` with keys:
```
"allocationSpeed"        → { firstFit: "Fast", nextFit: "Fast", bestFit: "Slow", worstFit: "Slow" }
"internalFragmentation"  → comparative ratings per algorithm
"externalFragmentation"  → comparative ratings per algorithm
"suitability"            → recommended use-case text per algorithm
"successRates"           → percentage of successful allocations per algorithm
"largestFreeBlocks"      → int values per algorithm
"fragmentationRatios"    → double values per algorithm
```

### 4.10 `ExportManager`

**File:** `src/controller/ExportManager.h / .cpp`

Generates a plain-text simulation log file containing:
- Memory pool initial layout
- Step-by-step allocation/deallocation events per algorithm
- Fragmentation report per algorithm
- Comparative summary table
- Stress test results

---

## 5. GUI Design — Complete Specification

### 5.1 Design Philosophy

The GUI follows a **dark-neutral, data-dense, OS-tool aesthetic** — similar to htop, profilers, or IDE debuggers. The design is NOT a generic business dashboard. It should feel like an operating system analysis tool.

**Visual language:**
- Dark background (`#1A1A2E`) with deep panel surfaces (`#16213E`, `#0F3460`)
- Accent colours per algorithm: First-Fit = Electric Blue `#4FC3F7`, Next-Fit = Amber `#FFB74D`, Best-Fit = Mint `#A5D6A7`, Worst-Fit = Rose `#F48FB1`
- Monospace font (`JetBrains Mono` or `Courier New`) in memory block labels and address fields
- `Inter` or system sans-serif for all other text
- Micro-animations on block state changes (colour fade, width transition)
- Subtle grid lines in memory visualisation areas
- Status chips with coloured dot indicators (green = FREE, coloured = ALLOCATED)

### 5.2 Main Window Structure

```
┌──────────────────────────────────────────────────────────────────┐
│  TITLE BAR: "MemSim — Dynamic Memory Allocation Simulator"       │
│  [  ← Back  ]  [Tabs: Simulation | Stress Test | Compare | Log ] │
├──────────────────────────────────────────────────────────────────┤
│  SIDEBAR (200px)           │   MAIN CONTENT AREA                 │
│  ─────────────────         │   (switches per active Tab)         │
│  Algorithm Selector        │                                     │
│  ○ First-Fit               │                                     │
│  ○ Next-Fit                │                                     │
│  ○ Best-Fit                │                                     │
│  ○ Worst-Fit               │                                     │
│  ○ All (side-by-side)      │                                     │
│  ─────────────────         │                                     │
│  Quick Controls            │                                     │
│  [+ Allocate]              │                                     │
│  [- Deallocate]            │                                     │
│  [⟳ Reset]                 │                                     │
│  [▶ Auto-Demo]             │                                     │
│  ─────────────────         │                                     │
│  Pool Stats                │                                     │
│  Total: 1024 KB            │                                     │
│  Free:  ---                │                                     │
│  Used:  ---                │                                     │
└──────────────────────────────────────────────────────────────────┘
```

**Main Window QML file:** `qml/MainWindow.qml`

- `ApplicationWindow` root with `minimumWidth: 1280`, `minimumHeight: 800`
- `SplitView` with `Sidebar.qml` (fixed 200 px) + `StackLayout` (main content)
- Window title: `"MemSim v1.0 — Memory Allocation Simulator"`

### 5.3 Tab 1: Simulation View

**QML file:** `qml/pages/SimulationPage.qml`

#### Layout

```
┌─────────────────────────────────────────────────────────────────┐
│  Algorithm: [First-Fit ▼]          [+ Allocate] [-Deallocate]   │
├─────────────────────────────────────────────────────────────────┤
│                    MEMORY MAP AREA                              │
│  ┌───────────────────────────────────────────────────────────┐  │
│  │ ADDRESS RULER: 0    100   300  350  650          1024 KB  │  │
│  │                                                           │  │
│  │ [BLK-0  100KB FREE ][BLK-1  200KB FREE ][BLK-2  50KB ...]│  │
│  │ [           BLK-3  300KB FREE          ][BLK-4  374KB  ] │  │
│  └───────────────────────────────────────────────────────────┘  │
├─────────────────────────────────────────────────────────────────┤
│                 BLOCK DETAIL TABLE                              │
│  Block ID │ Start Addr │ Size (KB) │ Status   │ Process         │
│  ──────────────────────────────────────────────────────────     │
│  BLK-0    │ 0          │ 100       │ ● FREE   │ —               │
│  BLK-1    │ 100        │ 200       │ ● FREE   │ —               │
│  ...                                                            │
├─────────────────────────────────────────────────────────────────┤
│  [Fragmentation Report Panel]                                   │
│  Total Free: xxx KB  │  Free Blocks: x  │  Largest: xxx KB     │
│  External Fragmentation: YES / NO       │  Ratio: x.xx         │
├─────────────────────────────────────────────────────────────────┤
│  [Event Log Panel — scrollable]                                 │
│  [12:04:01] ALLOC  P1 → BLK-2 (50 KB)  ✓ Success              │
│  [12:04:02] SPLIT  BLK-2 → BLK-2 (50 KB) + BLK-10 (150 KB)    │
│  [12:04:05] DEALLOC P1 ← BLK-2  ✓ Freed                       │
│  [12:04:05] MERGE  BLK-2 + BLK-10 → BLK-2 (200 KB)            │
└─────────────────────────────────────────────────────────────────┘
```

#### Memory Map Component — Detailed Spec

**QML file:** `qml/components/MemoryMapView.qml`

- A horizontal `Row` of `MemoryBlockRect` components inside a `ScrollView`.
- Each block's **width** is proportional to its `sizeKB` relative to total pool size.
- Minimum block width = 60 px (prevent tiny unreadable blocks); scrollable if needed.
- Block colour states:
  - FREE → `#2D3A4A` (dark blue-grey), border `#4FC3F7`
  - ALLOCATED → colour derived from `processId % colourPalette.length`, bright & saturated
  - JUST SPLIT (recently) → briefly flash `#FFD700` for 600 ms then settle
  - JUST MERGED → briefly flash `#98FB98` for 600 ms then settle
  - FAILED candidate → briefly flash `#FF4444` for 300 ms
- Each block shows:
  - Block ID (top-left, monospace, small)
  - Size in KB (centre, large)
  - Status chip: `● FREE` or `● P1` (bottom-right)
- Address ruler above the map: tick marks at every 100 KB, labels at every 200 KB.
- Arrow indicator for Next-Fit cursor position (a small `▼` above the active block).
- Smooth `Behavior on width { NumberAnimation { duration: 400 } }` for split/merge animations.
- Smooth `Behavior on color { ColorAnimation { duration: 300 } }` for state transitions.

#### Allocation Dialog

**QML file:** `qml/dialogs/AllocateDialog.qml`

A `Dialog` component with:
```
Fields:
  Process ID:    [SpinBox 1–99]
  Process Name:  [TextField "P1"]
  Size (KB):     [SpinBox 1–500]
  Algorithm:     [ComboBox — or "All Algorithms" option]
  
Buttons: [Cancel] [Allocate →]

Validation:
  - Size must be 1–500 KB
  - Process ID must not already be allocated
  - If "All Algorithms" selected, dispatches to all four engines
  - Shows inline warning if size > largest free block: 
    "⚠ Warning: this request may fail on some algorithms"
```

#### Deallocation Dialog

**QML file:** `qml/dialogs/DeallocateDialog.qml`

```
Fields:
  Select Block: [ComboBox populated with ALLOCATED blocks — "BLK-2 (P1, 50 KB)"]
  Algorithm:    [ComboBox]
  
Buttons: [Cancel] [Deallocate →]
```

### 5.4 Tab 2: All-Algorithms Side-by-Side View

**QML file:** `qml/pages/AllAlgorithmsPage.qml`

Four `MemoryMapView` panels displayed in a 2×2 grid (or 4×1 horizontally scrollable on smaller screens), each labelled with its algorithm name and accent colour.

```
┌──────────────────────────┬──────────────────────────┐
│  FIRST-FIT               │  NEXT-FIT                │
│  [memory map]            │  [memory map]            │
│  Free: 400KB │ Frags: 3  │  Free: 420KB │ Frags: 2  │
├──────────────────────────┼──────────────────────────┤
│  BEST-FIT                │  WORST-FIT               │
│  [memory map]            │  [memory map]            │
│  Free: 380KB │ Frags: 4  │  Free: 450KB │ Frags: 1  │
└──────────────────────────┴──────────────────────────┘

[Run Same Requests on All ▶]   [Reset All ⟳]
```

- "Run Same Requests on All" applies the same predefined allocation/deallocation sequence to all four pools simultaneously, with a 300 ms delay between steps for visual pacing.
- After each step, all four maps update in sync.

### 5.5 Tab 3: Stress Test View

**QML file:** `qml/pages/StressTestPage.qml`

```
┌─────────────────────────────────────────────────────────────────┐
│  STRESS TEST CONFIGURATION                                      │
│  Requests: [10 fixed per spec]   Size range: [10 KB] – [150 KB] │
│  Seed: [Random ▼]                [▶ Run Stress Test]            │
├─────────────────────────────────────────────────────────────────┤
│  PROGRESS BAR                                                   │
│  [████████████░░░░░░░░] Request 7 of 10                        │
├─────────────────────────────────────────────────────────────────┤
│  RESULTS TABLE (live update during test)                        │
│                                                                 │
│  Req# │ Size  │ First-Fit │ Next-Fit │ Best-Fit │ Worst-Fit     │
│  ─────────────────────────────────────────────────────────      │
│    1  │ 45 KB │ ✓ BLK-2   │ ✓ BLK-3  │ ✓ BLK-5  │ ✓ BLK-1     │
│    2  │ 120KB │ ✓ BLK-4   │ ✓ BLK-4  │ ✓ BLK-1  │ ✗ Failed    │
│    3  │ 80 KB │ ✗ Failed  │ ✓ BLK-6  │ ✓ BLK-3  │ ✗ Failed    │
│   ...                                                           │
├─────────────────────────────────────────────────────────────────┤
│  STRESS TEST SUMMARY                                            │
│  Algorithm  │ Successes │ Failures │ Success Rate │ Avg Frag    │
│  First-Fit  │     7     │    3     │    70%       │   3.2       │
│  Next-Fit   │     8     │    2     │    80%       │   2.8       │
│  Best-Fit   │     6     │    4     │    60%       │   4.1       │
│  Worst-Fit  │     5     │    5     │    50%       │   1.5       │
└─────────────────────────────────────────────────────────────────┘
```

- Progress bar updates via `StressTestEngine::progressUpdated` signal.
- Results table rows appear one by one as each request completes.
- Failed cells show `✗ Failed` in red; successful cells show `✓ BLK-n` in green.

### 5.6 Tab 4: Comparative Analysis Dashboard

**QML file:** `qml/pages/ComparativePage.qml`

```
┌─────────────────────────────────────────────────────────────────┐
│               COMPARATIVE ANALYSIS REPORT                       │
├─────────────────────────────────────────────────────────────────┤
│  METRIC COMPARISON TABLE                                        │
│                                                                 │
│  Metric              │ First-Fit│ Next-Fit │ Best-Fit │ Worst-Fit│
│  ──────────────────────────────────────────────────────────     │
│  Allocation Speed    │  Fast    │  Fast    │  Slow    │  Slow    │
│  Internal Frag       │  Medium  │  Medium  │  Low     │  High    │
│  External Frag       │  Medium  │  Low     │  High    │  Low     │
│  Success Rate        │  70%     │  80%     │  60%     │  50%     │
│  Largest Free Block  │  200 KB  │  220 KB  │  180 KB  │  300 KB  │
│  Best Use Case       │ General  │Streaming │ Fixed    │ Variable │
├─────────────────────────────────────────────────────────────────┤
│  FRAGMENTATION RATIO BAR CHART                                  │
│  First-Fit  [██████░░░░]  0.32                                  │
│  Next-Fit   [█████░░░░░]  0.28                                  │
│  Best-Fit   [████████░░]  0.41                                  │
│  Worst-Fit  [███░░░░░░░]  0.15                                  │
├─────────────────────────────────────────────────────────────────┤
│  SUCCESS RATE COMPARISON (horizontal bar chart)                 │
│  [visual bars rendered in QML Canvas or Rectangle elements]     │
├─────────────────────────────────────────────────────────────────┤
│  WRITTEN JUSTIFICATION SUMMARY (scrollable text area)          │
│  Rendered from ComparativeAnalyser output                       │
├─────────────────────────────────────────────────────────────────┤
│  [📄 Export Full Report as .txt]                                │
└─────────────────────────────────────────────────────────────────┘
```

- Charts are rendered using `Canvas` QML element (native Qt, no third-party chart library needed).
- Colour coding: each bar/column uses the algorithm's assigned accent colour.
- Best values in each row are highlighted with a subtle background.

### 5.7 Tab 5: Event Log View

**QML file:** `qml/pages/EventLogPage.qml`

```
┌─────────────────────────────────────────────────────────────────┐
│  EVENT LOG                              [Clear] [Export as .txt]│
│  Filter: [All ▼]  [First-Fit ▼]  [Search: ____________]        │
├─────────────────────────────────────────────────────────────────┤
│  Timestamp │ Algorithm  │ Event Type │ Detail                  │
│  ──────────────────────────────────────────────────────────     │
│  00:00.001  First-Fit   INIT         Pool loaded: 10 blocks     │
│  00:01.423  First-Fit   ALLOC ✓      P1 → BLK-2 (50 KB)        │
│  00:01.423  Next-Fit    ALLOC ✓      P1 → BLK-3 (50 KB)        │
│  00:01.424  First-Fit   SPLIT        BLK-2 → BLK-2+BLK-10      │
│  00:02.001  First-Fit   ALLOC ✗      P2 → FAILED (need 500KB)  │
│  00:03.001  First-Fit   DEALLOC ✓    P1 ← BLK-2 freed          │
│  00:03.002  First-Fit   MERGE        BLK-2 + BLK-10 → BLK-2    │
│  ...                                                            │
└─────────────────────────────────────────────────────────────────┘
```

- Row colours: ALLOC ✓ = green tint, ALLOC ✗ = red tint, SPLIT = amber tint, MERGE = teal tint, DEALLOC = blue tint.
- Filter by algorithm and event type via ComboBox.
- Realtime search filters rows as user types.
- Auto-scrolls to bottom on new events unless user has manually scrolled up.

### 5.8 Sidebar — Detailed Spec

**QML file:** `qml/components/Sidebar.qml`

```
┌──────────────────┐
│  ⬡ MemSim        │  ← Logo + app name
├──────────────────┤
│  ALGORITHMS      │
│  ● First-Fit     │  ← Radio buttons, accent colour dots
│  ○ Next-Fit      │
│  ○ Best-Fit      │
│  ○ Worst-Fit     │
│  ○ All           │
├──────────────────┤
│  ACTIONS         │
│  [+ Allocate]    │  ← Primary button (full width)
│  [− Deallocate]  │  ← Secondary button
│  [⟳ Reset All]   │  ← Destructive (red border on hover)
│  [▶ Auto Demo]   │  ← Runs the preset sequence automatically
├──────────────────┤
│  POOL STATS      │
│  Total  1024 KB  │
│  Free   ████     │  ← Mini progress bar (green = free)
│  Used   ████     │  ← Mini progress bar (colour = used)
│  Blocks    10    │
├──────────────────┤
│  PRESETS         │
│  [Load Demo 1]   │  ← Predefined sequence from spec
│  [Load Demo 2]   │  ← Second scenario
├──────────────────┤
│  v1.0 │ CS 330   │  ← Footer
└──────────────────┘
```

### 5.9 Animations & Transitions Summary

| Event | Animation |
|---|---|
| Block allocated | Colour fade from FREE grey to process colour (400 ms) |
| Block deallocated | Colour fade back to FREE grey (400 ms) |
| Block split | Original shrinks (width animation 400 ms), new block slides in from right (300 ms) |
| Block merged | Two blocks fade and expand into one (500 ms) |
| Allocation failure | Target blocks flash red (2× pulse, 150 ms each) |
| Next-Fit cursor move | Cursor arrow slides to new position (200 ms) |
| New event log entry | Slide-in from bottom (150 ms) |
| Tab switch | Cross-fade (150 ms) |
| Stress test row appear | Fade in top-to-bottom (100 ms stagger per row) |

All animations use `Behavior {}` blocks or `SequentialAnimation` in QML. All honour `Qt.application.supportsMultipleWindows` — animations can be disabled via a Settings toggle for accessibility.

### 5.10 Memory Layout Diagram (Initial State Display)

The initial pool layout matches the lab spec example (students define their own). The **default configuration** built into the app:

```
Address:  0      100    300   350        650              1024
          |─100─|─200─|─50─|────300────|──────374──────|
Block:   [BLK-0][BLK-1][BLK-2][  BLK-3  ][    BLK-4    ]
Status:  [FREE ][FREE ][FREE ][ FREE    ][   FREE      ]
```

Extended to 10 blocks (student must customise, this is the baseline):

```
BLK-0:   0 KB,   size 60 KB,   FREE
BLK-1:  60 KB,   size 100 KB,  FREE
BLK-2: 160 KB,   size 30 KB,   FREE
BLK-3: 190 KB,   size 80 KB,   FREE
BLK-4: 270 KB,   size 50 KB,   FREE
BLK-5: 320 KB,   size 200 KB,  FREE
BLK-6: 520 KB,   size 40 KB,   FREE
BLK-7: 560 KB,   size 150 KB,  FREE
BLK-8: 710 KB,   size 90 KB,   FREE
BLK-9: 800 KB,   size 224 KB,  FREE
Total: 1024 KB
```

---

## 6. Algorithm Specifications

### 6.1 First-Fit

```
findBlock(pool, sizeKB):
  for i = 0 to pool.blockCount() - 1:
    block = pool.blockAt(i)
    if block.isFree AND block.sizeKB >= sizeKB:
      return i
  return -1   // allocation failed
```

**Complexity:** O(n) where n = number of blocks.  
**Characteristic:** Fast. Tends to leave small fragments at the beginning of the pool.

### 6.2 Next-Fit

```
findBlock(pool, sizeKB):
  start = pool.nextFitCursor
  i = start
  do:
    block = pool.blockAt(i)
    if block.isFree AND block.sizeKB >= sizeKB:
      pool.nextFitCursor = i  // remember position
      return i
    i = (i + 1) % pool.blockCount()
  while i != start
  return -1
```

**Complexity:** O(n) amortised, avoids constant re-scanning of early blocks.  
**Characteristic:** Distributes allocations more evenly. Cursor wraps around.

### 6.3 Best-Fit

```
findBlock(pool, sizeKB):
  bestIndex = -1
  bestSize  = INT_MAX
  for i = 0 to pool.blockCount() - 1:
    block = pool.blockAt(i)
    if block.isFree AND block.sizeKB >= sizeKB:
      if block.sizeKB < bestSize:
        bestSize  = block.sizeKB
        bestIndex = i
  return bestIndex
```

**Complexity:** O(n) — must scan all blocks.  
**Characteristic:** Minimises wasted space per allocation but produces many tiny leftover fragments.

### 6.4 Worst-Fit

```
findBlock(pool, sizeKB):
  worstIndex = -1
  worstSize  = -1
  for i = 0 to pool.blockCount() - 1:
    block = pool.blockAt(i)
    if block.isFree AND block.sizeKB >= sizeKB:
      if block.sizeKB > worstSize:
        worstSize  = block.sizeKB
        worstIndex = i
  return worstIndex
```

**Complexity:** O(n) — must scan all blocks.  
**Characteristic:** Leaves large remainders after splitting. Counterintuitively reduces external fragmentation in some patterns.

---

## 7. Data Structures

### 7.1 `AllocationRequest`

```cpp
struct AllocationRequest {
    int     sizeKB;
    int     processId;
    QString processLabel;
    bool    applyToAll;         // true during side-by-side or stress test
};
```

### 7.2 `AllocationResult`

```cpp
struct AllocationResult {
    enum Status { SUCCESS, FAILED_NO_BLOCK, FAILED_POOL_FULL };

    Status  status;
    int     blockIndex;         // -1 if failed
    int     blockId;
    int     splitRemainder;     // 0 if no split occurred
    int     newBlockId;         // block created by split, -1 if none
    QString failureReason;
};
```

### 7.3 `SimEvent`

```cpp
struct SimEvent {
    enum Type { INIT, ALLOC_SUCCESS, ALLOC_FAIL, DEALLOC, SPLIT, MERGE };

    QDateTime timestamp;
    Type      type;
    QString   algorithmName;
    QString   detail;
    int       blockIdA;
    int       blockIdB;         // used for SPLIT and MERGE
    int       processId;
};
```

### 7.4 `StressTestResult`

```cpp
struct StressTestResult {
    int     requestIndex;
    int     sizeKB;
    bool    success;
    int     blockId;            // -1 if failed
    QString algorithmName;
};
```

---

## 8. Core Logic & Rules

### 8.1 Block Splitting Rule

Triggered whenever `allocate()` succeeds and `block.sizeKB - requestedSizeKB > 20`.

```
Before split: [BLK-3: 300 KB FREE]
Request: 50 KB
Remainder: 250 KB > 20 KB → SPLIT

After split:
  [BLK-3: 50 KB, ALLOCATED, P1]
  [BLK-10: 250 KB, FREE]        ← new block inserted at BLK-3's old position + 1

Start addresses:
  BLK-10.startAddress = BLK-3.startAddress + requestedSizeKB
  BLK-10.sizeKB = 300 - 50 = 250
```

New block IDs are assigned sequentially from a global `nextBlockId` counter (starts at pool size, e.g. 10 for a 10-block pool).

**Emit:** `splitOccurred(int originalBlockId, int newBlockId)` signal.

### 8.2 Block Coalescing Rule

Triggered on every `deallocate()` call.

```
Check left neighbour (index - 1): if exists AND isFree → merge
Check right neighbour (index + 1): if exists AND isFree → merge

Merge operation:
  survivor.sizeKB += absorbed.sizeKB
  pool.removeBlock(absorbed_index)
  Emit mergeOccurred(survivorId, absorbedId)
  Print: "MERGE: BLK-X and BLK-Y merged into BLK-X (newSize KB)"
```

Coalescing is applied **twice** (left then right) each time, and checks are repeated until no further merges are possible (handles three-way merges: left-freed-right all free).

### 8.3 Failed Allocation Handling

When `findBlock()` returns -1:
- `AllocationResult.status = FAILED_NO_BLOCK`
- `failureReason` = `"No free block found of sufficient size (requested X KB, largest free Y KB)"`
- Event logged as `ALLOC_FAIL`
- In GUI: request row shown in red, pool map briefly highlights all insufficiently-small blocks in red
- `FragmentationReport.failedAllocations` incremented

### 8.4 Preset Demo Sequence

The "Load Demo 1" preset executes this sequence in order:

```
1. Allocate P1 → 45 KB
2. Allocate P2 → 120 KB
3. Allocate P3 → 30 KB
4. Allocate P4 → 200 KB
5. Allocate P5 → 350 KB   ← expected to fail on some algorithms
6. Deallocate P1 (triggers coalesce check)
7. Deallocate P3 (triggers coalesce check, potential merge with adjacent)
8. Allocate P6 → 60 KB    ← fills previously freed space
```

This sequence is guaranteed to produce at least one failure and at least one coalescing event.

### 8.5 Address Bookkeeping

When a split produces a new block, all subsequent blocks' `startAddress` values are **not** recalculated — each block stores its own `startAddress` set at creation and does not change unless it is the block being modified. Merges update the survivor's `sizeKB` only; the `startAddress` of the lower-addressed block is preserved.

---

## 9. Stress Test Engine

### 9.1 Behaviour

1. Generates a `QVector<AllocationRequest>` of exactly 10 entries.
2. Each request: `sizeKB` = `QRandomGenerator::global()->bounded(10, 151)`, `processId` = auto-assigned (starting from 100 to avoid conflict with manual allocations).
3. Applies all 10 requests to all four algorithm engines (each on its own independent pool that is **reset to initial state** before the stress test begins).
4. After each request, collects `AllocationResult` for all four algorithms.
5. Runs in `QThread` — emits progress signals, never blocks UI.
6. On completion, emits `stressTestFinished(QVariantList)` with all results.

### 9.2 Randomness

- Default seed: system time (random per run).
- Optional fixed seed (entered in UI) for reproducible runs.
- Seed is displayed in the stress test results header for reproducibility in the lab report.

---

## 10. Comparative Analysis Engine

### 10.1 Metrics Collected

After the preset demo sequence and after the stress test, `ComparativeAnalyser` collects:

| Metric | Source |
|---|---|
| Allocation speed | Fixed (theoretical: First/Next = O(n) early exit; Best/Worst = O(n) full scan) |
| Internal fragmentation | Not applicable (no fixed partitions) — noted as "N/A" |
| External fragmentation | `FragmentationReport.externalFragmentationPresent` + `fragmentationRatio` |
| Success rate | Successful allocations / total attempted |
| Largest free block remaining | `FragmentationReport.largestFreeBlockKB` |
| Free block count | `FragmentationReport.freeBlockCount` |
| Average waste per allocation | Total wasted (split remainders) / successful allocations |

### 10.2 Written Justification (Generated)

The `ComparativeAnalyser::generateJustification()` method produces a structured text string covering:

1. **First-Fit**: Fastest search, creates fragmentation near the start of the pool over time. Good for general-purpose allocation where speed matters.

2. **Next-Fit**: Distributes allocations more evenly than First-Fit. Avoids re-scanning the beginning. Slightly better long-term fragmentation at the cost of slightly more bookkeeping.

3. **Best-Fit**: Minimises immediate wasted space per allocation. However, produces many tiny leftover fragments that are often too small to satisfy future requests — can lead to higher external fragmentation despite lower per-allocation waste.

4. **Worst-Fit**: Counterintuitive but often effective: allocating the largest available block leaves a larger remainder, which is more likely to satisfy future requests than tiny Best-Fit leftovers. Poor choice when pool contains one dominant large block.

5. **Stress test conclusion**: Tie-break the algorithms by success rate under random load. Present the actual numbers from the simulation run.

---

## 11. File Structure & Project Layout

```
MemSim/
├── CMakeLists.txt                  ← Top-level CMake
├── CMakePresets.json               ← Debug / Release presets
├── README.md
├── PRD.md                          ← This document
│
├── src/
│   ├── main.cpp                    ← QGuiApplication + QQmlEngine setup
│   │
│   ├── core/
│   │   ├── MemoryBlock.h
│   │   ├── MemoryBlock.cpp
│   │   ├── MemoryPool.h
│   │   ├── MemoryPool.cpp
│   │   ├── AllocationRequest.h
│   │   ├── AllocationResult.h
│   │   ├── SimEvent.h
│   │   └── FragmentationReport.h
│   │
│   ├── algorithms/
│   │   ├── IAllocationStrategy.h
│   │   ├── AlgorithmEngine.h
│   │   ├── AlgorithmEngine.cpp
│   │   ├── FirstFit.h
│   │   ├── FirstFit.cpp
│   │   ├── NextFit.h
│   │   ├── NextFit.cpp
│   │   ├── BestFit.h
│   │   ├── BestFit.cpp
│   │   ├── WorstFit.h
│   │   ├── WorstFit.cpp
│   │   └── AlgorithmFactory.h
│   │
│   ├── controller/
│   │   ├── SimulationController.h
│   │   ├── SimulationController.cpp
│   │   ├── StressTestEngine.h
│   │   ├── StressTestEngine.cpp
│   │   ├── ComparativeAnalyser.h
│   │   ├── ComparativeAnalyser.cpp
│   │   ├── ExportManager.h
│   │   └── ExportManager.cpp
│   │
│   └── models/
│       ├── MemoryTableModel.h      ← QAbstractTableModel for block table
│       ├── MemoryTableModel.cpp
│       ├── EventLogModel.h         ← QAbstractListModel for event log
│       └── EventLogModel.cpp
│
├── qml/
│   ├── main.qml                    ← Root QML entry point
│   ├── MainWindow.qml
│   │
│   ├── pages/
│   │   ├── SimulationPage.qml
│   │   ├── AllAlgorithmsPage.qml
│   │   ├── StressTestPage.qml
│   │   ├── ComparativePage.qml
│   │   └── EventLogPage.qml
│   │
│   ├── components/
│   │   ├── Sidebar.qml
│   │   ├── MemoryMapView.qml
│   │   ├── MemoryBlockRect.qml
│   │   ├── AddressRuler.qml
│   │   ├── FragmentationPanel.qml
│   │   ├── EventLogPanel.qml
│   │   ├── MetricBar.qml           ← Horizontal bar chart component
│   │   ├── ComparisonTable.qml
│   │   ├── StatsCard.qml
│   │   └── NextFitCursor.qml      ← The ▼ cursor indicator
│   │
│   ├── dialogs/
│   │   ├── AllocateDialog.qml
│   │   └── DeallocateDialog.qml
│   │
│   └── style/
│       ├── Theme.qml               ← Colour constants, font sizes
│       └── AppStyle.qml            ← Shared style object (singleton)
│
├── resources/
│   └── qml.qrc                     ← Qt resource file listing all QML files
│
└── tests/
    ├── CMakeLists.txt
    ├── test_firstfit.cpp
    ├── test_nextfit.cpp
    ├── test_bestfit.cpp
    ├── test_worstfit.cpp
    ├── test_splitting.cpp
    ├── test_coalescing.cpp
    └── test_fragmentation.cpp
```

---

## 12. Build Configuration

### 12.1 `CMakeLists.txt` (top-level)

```cmake
cmake_minimum_required(VERSION 3.21)
project(MemSim VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS
    Core Gui Quick QuickControls2 Widgets Concurrent)

qt_standard_project_setup()

qt_add_executable(MemSim
    src/main.cpp
    src/core/MemoryBlock.cpp
    src/core/MemoryPool.cpp
    src/algorithms/AlgorithmEngine.cpp
    src/algorithms/FirstFit.cpp
    src/algorithms/NextFit.cpp
    src/algorithms/BestFit.cpp
    src/algorithms/WorstFit.cpp
    src/controller/SimulationController.cpp
    src/controller/StressTestEngine.cpp
    src/controller/ComparativeAnalyser.cpp
    src/controller/ExportManager.cpp
    src/models/MemoryTableModel.cpp
    src/models/EventLogModel.cpp
)

qt_add_qml_module(MemSim
    URI MemSim
    VERSION 1.0
    QML_FILES
        qml/main.qml
        qml/MainWindow.qml
        qml/pages/SimulationPage.qml
        qml/pages/AllAlgorithmsPage.qml
        qml/pages/StressTestPage.qml
        qml/pages/ComparativePage.qml
        qml/pages/EventLogPage.qml
        qml/components/Sidebar.qml
        qml/components/MemoryMapView.qml
        qml/components/MemoryBlockRect.qml
        qml/components/AddressRuler.qml
        qml/components/FragmentationPanel.qml
        qml/components/EventLogPanel.qml
        qml/components/MetricBar.qml
        qml/components/ComparisonTable.qml
        qml/components/StatsCard.qml
        qml/components/NextFitCursor.qml
        qml/dialogs/AllocateDialog.qml
        qml/dialogs/DeallocateDialog.qml
        qml/style/Theme.qml
        qml/style/AppStyle.qml
)

target_link_libraries(MemSim PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Widgets
    Qt6::Concurrent
)
```

### 12.2 `main.cpp` Structure

```cpp
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "controller/SimulationController.h"
#include "models/MemoryTableModel.h"
#include "models/EventLogModel.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName("MemSim");
    app.setApplicationVersion("1.0");

    QQmlApplicationEngine engine;

    // Register C++ types for QML
    qmlRegisterType<MemoryTableModel>("MemSim", 1, 0, "MemoryTableModel");
    qmlRegisterType<EventLogModel>("MemSim", 1, 0, "EventLogModel");

    // Expose controller as context property
    SimulationController controller;
    engine.rootContext()->setContextProperty("simController", &controller);

    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
```

---

## 13. Coding Standards & Best Practices

### 13.1 C++ Standards

- **C++17** throughout — use structured bindings, `std::optional`, `if constexpr` where appropriate.
- No raw owning pointers — use `QScopedPointer`, `std::unique_ptr`, or value semantics.
- Prefer `const` and `const &` parameters wherever values are not modified.
- Every class that exposes data to QML must inherit `QObject` and use `Q_OBJECT` macro.
- Every public method intended for QML access must be marked `Q_INVOKABLE` or exposed via `Q_PROPERTY`.
- All `Q_PROPERTY` values that change must emit the appropriate `NOTIFY` signal.
- No business logic in QML — QML is view-only. All calculations and state transitions occur in C++.

### 13.2 Naming Conventions

| Element | Convention | Example |
|---|---|---|
| Classes | PascalCase | `AlgorithmEngine` |
| Methods | camelCase | `findBlock()` |
| Member variables | `m_` prefix | `m_blocks` |
| Constants | UPPER_SNAKE | `MAX_POOL_SIZE` |
| QML IDs | camelCase | `memoryMapView` |
| QML files | PascalCase | `MemoryMapView.qml` |
| Signals | camelCase verb | `splitOccurred` |

### 13.3 Error Handling

- `AlgorithmEngine::allocate()` always returns an `AllocationResult` — never throws for expected failures.
- QML dialogs validate inputs before calling `Q_INVOKABLE` methods.
- All `QFile` operations in `ExportManager` check `open()` return values and display `QMessageBox` on failure.
- Deallocation of a block not found by ID logs a warning via `qWarning()` and returns a FAILED result.

### 13.4 Memory Management

- `MemoryPool::blocks` is a `QVector<MemoryBlock>` (value-semantic, no heap allocation per block).
- Four `AlgorithmEngine` instances are owned by `SimulationController` as direct members (not heap-allocated).
- QML object lifetimes are managed by the QML engine.

### 13.5 Comment Standards

Every class header file must have:
```cpp
/**
 * @class AlgorithmEngine
 * @brief Owns one MemoryPool and one IAllocationStrategy.
 *        Implements allocate(), deallocate(), split(), and coalesce().
 *
 * @note Not thread-safe. All calls must be made from the owning thread.
 */
```

Every non-trivial method must have a brief `//` comment explaining its purpose at the point of definition.

---

## 14. Deliverables Checklist

### Code Deliverables

- [ ] All four allocation algorithms implemented and independently testable.
- [ ] Block splitting (difference > 20 KB) working with event emission.
- [ ] Block coalescing (merge on deallocation) with merge message printed to event log.
- [ ] At least one failing allocation handled gracefully (not a crash).
- [ ] Memory table displaying Block ID, Start Address, Size (KB), Status, Process ID.
- [ ] Fragmentation report (total free, free blocks, largest block, external fragmentation flag).
- [ ] Stress test with 10 random requests, shown in results table.
- [ ] Comparative summary table in UI.
- [ ] Export to `.txt` log file.

### GUI Deliverables

- [ ] `MainWindow.qml` — sidebar + tab navigation.
- [ ] `SimulationPage.qml` — memory map + block table + fragmentation panel + event log.
- [ ] `AllAlgorithmsPage.qml` — 4 independent pools shown simultaneously.
- [ ] `StressTestPage.qml` — progress bar + live results table + summary.
- [ ] `ComparativePage.qml` — comparison table + bar charts + justification text.
- [ ] `EventLogPage.qml` — filterable, searchable full event log.
- [ ] `AllocateDialog.qml` + `DeallocateDialog.qml`.
- [ ] All block state animations (split, merge, alloc, dealloc, fail).

### Report Deliverables (Hard Copy)

- [ ] Memory pool initial layout diagram (drawn to match defined configuration).
- [ ] Complete annotated output for all four algorithms (5 allocs + 2 deallocs + 1 failed).
- [ ] Fragmentation analysis per algorithm.
- [ ] Comparative justification table (matches in-app `ComparativePage`).
- [ ] Stress test results section.
- [ ] Source code printout (key modules).

---

## Appendix A — Initial Memory Pool (Default Configuration)

Students must define their own, but this is the minimum-compliant baseline:

| Block ID | Start (KB) | Size (KB) | Status | Total Accum. |
|---|---|---|---|---|
| BLK-0 | 0 | 60 | FREE | 60 |
| BLK-1 | 60 | 100 | FREE | 160 |
| BLK-2 | 160 | 30 | FREE | 190 |
| BLK-3 | 190 | 80 | FREE | 270 |
| BLK-4 | 270 | 50 | FREE | 320 |
| BLK-5 | 320 | 200 | FREE | 520 |
| BLK-6 | 520 | 40 | FREE | 560 |
| BLK-7 | 560 | 150 | FREE | 710 |
| BLK-8 | 710 | 90 | FREE | 800 |
| BLK-9 | 800 | 224 | FREE | 1024 |

**Total: 10 blocks, 1024 KB. ✓**

---

## Appendix B — Colour Palette Reference

| Algorithm | Primary | Secondary | Accent |
|---|---|---|---|
| First-Fit | `#4FC3F7` (Sky Blue) | `#0288D1` | `#B3E5FC` |
| Next-Fit | `#FFB74D` (Amber) | `#F57C00` | `#FFE0B2` |
| Best-Fit | `#A5D6A7` (Mint Green) | `#388E3C` | `#E8F5E9` |
| Worst-Fit | `#F48FB1` (Rose Pink) | `#C2185B` | `#FCE4EC` |
| FREE block | `#2D3A4A` | `#4FC3F7` border | — |
| App BG | `#1A1A2E` | `#16213E` panel | `#0F3460` deep |
| Success | `#66BB6A` | — | — |
| Failure | `#EF5350` | — | — |
| Split event | `#FFD54F` | — | — |
| Merge event | `#4DB6AC` | — | — |

---

*End of PRD — MemSim v1.0*  
*CS 330 Operating Systems · BESE-30 · Spring 2026*
